/* opt_crm.c - SR99 data aggregation and control
**
** WARNING!! NUM_CONTROLLER_VARS includes db_urms_status_t, urms_datafile_t, db_urms_t, db_urms_status2_t,
** db_urms_status3_t, and db_ramp_data_t.
** For the present purposes (i.e. 4/20/2016) we're using only db_urms_status_t, which contains the data 
** Cheng-Ju needs for data aggregation. We will eventually need all 28*6=168 database variables.
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "parameters.h"

#define NRANSI

#include "nrutil2.h"
#include "resource.h"
#include "rm_algo.h"
#include "clt_vars.h"

char str[len_str];

FILE *dbg_f, *dmd_f, *vsl_crm_f, *cal_opt_f, *pp, *st_file, *st_file_out, *dbg_st_file_out;
//static float delta_t=0.0;

//int optControl(float,float,float);
int Init();
int Init_sim_data_io();
//int optMeter(float,float,float);
//int save_data(float);
//int finish_data_saving();
int opt_metering(void);
int Set_Opt_Meter(float time,float time2,float timeSta);
float Mind(float a,float b);
int Finish_sim_data_io();

/*
int syst_init()
{		
	Init();  
	Init_sim_data_io();
	//init_data_saving();
	//init_metering();  // added on 08/30/13

	return 0;
}
*/

static int sig_list[] =
{
        SIGINT,
        SIGQUIT,
        SIGTERM,
        SIGALRM,
        ERROR,
};

static jmp_buf exit_env;

static void sig_hand(int code)
{
        if (code == SIGALRM)
                return;
        else
                longjmp(exit_env, code);
}

const char *usage = "-d (debug mode, i.e. use the two controllers in my office)";

#define NUM_ONRAMPS	16   // this variable is used by data base
#define NUM_OFFRAMPS 12  // this variable is used by data base
#define NUM_CYCLE_BUFFS       5

int main(int argc, char *argv[])
{
	timestamp_t ts;
	float time = 0, time2 = 0,timeSta = 0;
//	double tmp0, tmp1, tmp2, tmp3, tmp4;
	static int init_sw=1;
	int i;
//	db_urms_t urms_ctl[NUM_CONTROLLER_VARS/6];
	db_urms_status_t controller_data[NUM_CONTROLLER_VARS/6];  //See warning at top of file
	db_urms_status2_t controller_data2[NUM_CONTROLLER_VARS/6];  //See warning at top of file
	db_urms_status3_t controller_data3[NUM_CONTROLLER_VARS/6];  //See warning at top of file
	int option;
	int exitsig;
	db_clt_typ *pclt;
	char hostname[MAXHOSTNAMELEN+1];
	int interval = 1000;      /// Number of milliseconds between saves
	posix_timer_typ *ptimer;       /* Timing proxy */
	int cycle_index = 0;
	char *domain = DEFAULT_SERVICE; // usually no need to change this
	int xport = COMM_OS_XPORT;      // set correct for OS in sys_os.h
//	int verbose = 0;
	agg_data_t mainline_out[NUM_CYCLE_BUFFS][SecSize] =  {{{0}},{{0}}};      // data aggregated section by section
	agg_data_t onramp_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0}},{{0}}};      // data aggregated section by section
	agg_data_t offramp_out[NUM_CYCLE_BUFFS][NUM_OFFRAMPS] = {{{0}},{{0}}};  // data aggregated section by section
    
	agg_data_t mainline_out_f[SecSize] = {{0}};        // save filtered data to this array
	agg_data_t onramp_out_f[NumOnRamp] = {{0}};        // save filtered data to this array
	agg_data_t offramp_out_f[NUM_OFFRAMPS] = {{0}};    // save filtered data to this array

	agg_data_t controller_mainline_data[NUM_CONTROLLER_VARS/6] = {{0}};     // data aggregated controller by controller 
	agg_data_t controller_onramp_data[NUM_ONRAMPS] = {{0}};               // data aggregated controller by controller
	agg_data_t controller_offramp_data[NUM_OFFRAMPS] = {{0}};             // data aggregated controller by controller
	int debug = 0;
	int num_controller_vars = NUM_CONTROLLER_VARS/6; //See warning at top of file

	while ((option = getopt(argc, argv, "d")) != EOF) {
		switch(option) {
			case 'd':
				debug = 1;
				break;
			default:
				printf("\nUsage: %s %s\n", argv[0], usage);
				exit(EXIT_FAILURE);
				break;
		}
	}

	memset(controller_data, 0, NUM_CONTROLLER_VARS/6 * (sizeof(db_urms_status_t)));//See warning at top of file
	memset(controller_data2, 0, NUM_CONTROLLER_VARS/6 * (sizeof(db_urms_status2_t)));//See warning at top of file
	memset(controller_data3, 0, NUM_CONTROLLER_VARS/6 * (sizeof(db_urms_status3_t)));//See warning at top of file

	get_local_name(hostname, MAXHOSTNAMELEN);
	if ((pclt = db_list_init(argv[0], hostname, domain, xport,
		//db_vars_list, num_db_vars, NULL, 0)) == NULL) {
		NULL, 0, NULL, 0)) == NULL) {
		printf("Database initialization error in %s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Setup a timer for every 'interval' msec. */
	if ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL) {
		printf("Unable to initialize wrfiles timer\n");
		exit(EXIT_FAILURE);
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		db_list_done(pclt, NULL, 0, NULL, 0);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	if (init_sw==1)
	{
		Init();  
		Init_sim_data_io();
		init_sw=0;
	}
	if(debug)
		num_controller_vars = 2; //Use just the first two controllers in the list, i.e. 10.29.248.108 and 10.254.25.113.

	for (i = 0; i < num_controller_vars; i++){   //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+84].id, db_controller_list[i+84].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+112].id, db_controller_list[i+112].size, &controller_data3[i]);
	}

	for(;;)	
	{
	for (i = 0; i < num_controller_vars; i++){  //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+84].id, db_controller_list[i+84].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+112].id, db_controller_list[i+112].size, &controller_data3[i]);
	}

/*#################################################################################################################
###################################################################################################################*/

//** Cheng-Ju's code here **//
// 4 off-ramp is missing, total number of off-ramps is 9. After D3 fix those missing off-ramps, OffRampIndex table need to be updated. 
const char *controller_ip_strings[] = {
        "10.29.248.108",             //0, OR1
        "10.254.25.113",             //1
        "10.254.25.120",             //2, OR2, FR1
        "10.29.249.46",              //3, OR3,
        "10.29.248.42",              //4
        "10.29.248.20",              //5, OR4, FR2
        "10.29.248.128",             //6, OR5
        "10.254.27.67",              //7,
        "10.254.27.82",              //8, OR6, FR3
        "10.254.27.81",              //9, OR7
        "10.29.248.76",             //10     , FR4
        "10.254.28.213",            //11, OR8
        "10.254.28.212",            //12, OR9
        "10.254.28.211",            //13
        "10.29.248.118",            //14
        "10.29.248.52",             //15
        "10.254.24.156",            //16, OR10, FR5
        "10.254.24.157",            //17, OR11
        "10.29.248.185",            //18       
        "10.29.248.66",             //19, OR12
        "10.29.248.81",             //20, OR13
        "10.29.248.213",            //21      , FR9
        "10.29.248.155_PORT_1001",  //22, OR14
        "10.29.248.155_PORT_1002",  //23, OR15, FR10
        "10.29.248.124",            //24
        "10.29.248.67",             //25, OR16 
        "10.29.248.157",            //26
        "10.29.248.56",             //27      , FR12
}; //FR6, FR7, FR8, and FR11 are missing
//** This part aggregate data for each URMS2070 controller in the field   
	int OnRampIndex [NUM_CONTROLLER_VARS/6] =  { 0, -1, 2,  3, -1, 5,  6, -1, 8,  9, -1, 11, 12, -1, -1, -1, 16, 17, -1, 19, 20, -1, 22, 23, -1, 25, -1, -1}; 
	int OffRampIndex [NUM_CONTROLLER_VARS/6] = {-1, -1, 2, -1, -1, 5, -1, -1, 8, -1, 10, -1, -1, -1, -1, -1, 16, 17, -1, 19, 20, 21, -1, 23, -1, 25, -1, 27};  
	//int OffRampIndex [NUM_CONTROLLER_VARS/6] = {-1, -1, 2, -1, -1, 5, -1, -1, 8, -1, 10, -1, -1, -1, -1, -1, 16, -1, -1, -1, -1, 21, -1, 23, -1, -1, -1, 27};  
//	float float_temp = 0;
	for(i=0;i<NUM_CONTROLLER_VARS/6;i++){
		printf("IP %s controller is called by opt_crm.c \n", controller_ip_strings[i]);
		/*
		if( (float_temp = flow_aggregation_mainline(&controller_data[i]) ) >= 0)
			controller_mainline_data[i].agg_vol = Mind(12000.0, Maxd( 0, float_temp ) );
		else {
			//controller_mainline_data[i].flag = (int)float_temp;
			continue;
		}
		*/
		// min max function bound the data range and exclude nans.
        controller_mainline_data[i].agg_vol = Mind(12000.0, Maxd( 0, flow_aggregation_mainline(&controller_data[i]) ) );
		controller_mainline_data[i].agg_occ = Mind(100.0, Maxd( 0, occupancy_aggregation_mainline(&controller_data[i]) ) );
		controller_mainline_data[i].agg_speed = Mind(150.0, Maxd( 0, speed_aggregation_mainline(&controller_data[i]) ) );
		controller_mainline_data[i].agg_density = Mind(2000.0,Maxd( 0,  density_aggregation_mainline(&controller_data[i]) ) );
		controller_mainline_data[i].agg_mean_speed = Mind(150.0, Maxd( 0, mean_speed_aggregation_mainline(&controller_data[i]) ) );
        
        if(i==OffRampIndex[i]){
		//controller_offramp_data[i].agg_vol =  Mind(6000.0, Maxd( 0,flow_aggregation_offramp(&controller_data3[i]) ) );
        controller_offramp_data[i].agg_vol = flow_aggregation_offramp(&controller_data3[i]);
		    fprintf(dbg_st_file_out,"FR_%d_vol_%f ", i, controller_offramp_data[i].agg_vol);  
        //controller_offramp_data[i].agg_occ =  Mind(100.0, Maxd( 0,occupancy_aggregation_offramp(&controller_data3[i]) ) );
        controller_offramp_data[i].agg_occ = occupancy_aggregation_offramp(&controller_data3[i]);
            fprintf(dbg_st_file_out,"FR_%d_occ_%f ", i, controller_offramp_data[i].agg_occ); 
		//controller_offramp_data[i].turning_ratio = Mind(1, Maxd(0, controller_offramp_data[i].agg_vol/controller_mainline_data[i-1].agg_vol));
        controller_offramp_data[i].turning_ratio = controller_offramp_data[i].agg_vol/controller_mainline_data[i-1].agg_vol;  		
			fprintf(dbg_st_file_out,"FR_%d_sr_%f ", i, controller_offramp_data[i].turning_ratio); 
		}
        
		fprintf(dbg_st_file_out,"\n");

		if(i==OnRampIndex[i]){
		//controller_onramp_data[i].agg_vol = Mind(6000.0, Maxd( 0,flow_aggregation_onramp(&controller_data[i]) ) );
        controller_onramp_data[i].agg_vol = flow_aggregation_onramp(&controller_data[i]); 		
			fprintf(dbg_st_file_out,"OR_%d_vol_%f ", i, controller_onramp_data[i].agg_vol);  
		//controller_onramp_data[i].agg_occ = Mind(100.0, Maxd( 0,occupancy_aggregation_onramp(&controller_data[i], &controller_data2[i]) ) );
        controller_onramp_data[i].agg_occ = occupancy_aggregation_onramp(&controller_data[i], &controller_data2[i]) ; 		
			fprintf(dbg_st_file_out,"OR_%d_occ_%f ", i, controller_onramp_data[i].agg_occ);
		}
	}

//** This part aggregate data for each section
// controller index for each mainline section
int secCTidx [SecSize][4] =  {{7,  -1, -1, -1}, // controller in section 1 
                             {8,  -1, -1, -1}, // controller in section 2 
                             {9,  -1, -1, -1}, // controller in section 3
                             {10, 11, -1, -1}, // controller in section 4
                             {12, -1, -1, -1}, // controller in section 5    
                             {13, 14, 15, 16}, // controller in section 6 
                             {17, -1, -1, -1}, // controller in section 7 
							 {18, 19, -1, -1}, // controller in section 8 
                             {20, -1, -1, -1}, // controller in section 9   
                             {21, 22, -1, -1}, // controller in section 10
							 {23, -1, -1, -1}, // controller in section 11 
							 {24, 25, 26, -1}}; // controller in section 12 
int j; //
//This part aggregate mainline data for each section
 	for(i=0;i<SecSize;i++){
		// this loop aggregates all controller data in each section
		float temp_num_ct = 0.0; // number of controllers per section
		float temp_vol = 0.0;
		float temp_speed = 0.0;
		float temp_occ = 0.0;
		float temp_density = 0.0;
		float temp_mean_speed = 0.0;	
		for(j=0;j<4;j++){
			if(secCTidx[i][j]>0){
				temp_vol += controller_mainline_data[secCTidx[i][j]].agg_vol;
				temp_speed += controller_mainline_data[secCTidx[i][j]].agg_speed; 
			   	temp_occ += controller_mainline_data[secCTidx[i][j]].agg_occ;
				temp_density += controller_mainline_data[secCTidx[i][j]].agg_density;
				temp_mean_speed += controller_mainline_data[secCTidx[i][j]].agg_mean_speed;
				temp_num_ct ++;
			}
		}
		mainline_out[cycle_index][i].agg_vol = Mind(12000.0, Maxd(temp_vol/temp_num_ct,0));
		mainline_out[cycle_index][i].agg_speed = Mind(150.0, Maxd(temp_speed/temp_num_ct,0));
		mainline_out[cycle_index][i].agg_occ =  Mind(100.0, Maxd(temp_occ/temp_num_ct,0));
		mainline_out[cycle_index][i].agg_density = Mind(1200.0, Maxd(temp_density/temp_num_ct,0));
		mainline_out[cycle_index][i].agg_mean_speed =  Mind(150.0, Maxd(temp_mean_speed/temp_num_ct,0));

		// Initialize all temp variables
		temp_num_ct = 0.0; 
		temp_vol = 0.0;
		temp_speed = 0.0;
		temp_occ = 0.0;
		temp_density = 0.0;
		temp_mean_speed = 0.0;

	} 

//This part aggregate onramp data for each section
	int onrampCTidx[NumOnRamp] = {8, 9, 11, 12, 16, 17, 19, 20, 22, 23, 25}; 
	for(i=0;i<NumOnRamp;i++){
		onramp_out[cycle_index][i].agg_vol = Mind(6000.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_vol,0));
		onramp_out[cycle_index][i].agg_occ = Mind(100.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_occ,0));

	}

//This part aggregate onramp data for each section <--- match number of off-ramp by number of on-ramp 		 
	int offrampCTidx[NumOnRamp] = {8, -1, 10, -1, 16, 17, 19, 20, 21, 23,25}; // 4 off-ramp is missing, total number of off-ramps is 9 
	for(i=0;i<NumOnRamp;i++){ 
		if (offrampCTidx[i] != -1.0){//<-- impute data here
			offramp_out[cycle_index][i].agg_vol = Mind(6000.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_vol,0));
			offramp_out[cycle_index][i].agg_occ = Mind(100.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_occ,0));
		}else{
			offramp_out[cycle_index][i].agg_vol = 0;
			offramp_out[cycle_index][i].agg_occ = 0;
		}
	} 

// replace bad flow data by upstream data
   //for(i=0;i<SecSize;i++){
	  // if( (i==0) && (mainline_out[cycle_index][i].agg_vol==-1.0)){
	  //     mainline_out[cycle_index][i].agg_vol = 1000;
	  // }else if( (i!=0) && (mainline_out[cycle_index][i].agg_vol==-1.0) &&  (mainline_out[cycle_index][i-1].agg_vol!=-1.0) && (mainline_out[cycle_index][i+1].agg_vol=-1.0))
	  // {
	  //     mainline_out[cycle_index][i].agg_vol = mainline_out[cycle_index][i-1].agg_vol;
	  // }else if ( (i!=0) && (mainline_out[cycle_index][i].agg_vol==-1.0) &&  (mainline_out[cycle_index][i].agg_vol!=-1.0) &&  (mainline_out[cycle_index][i+1].agg_vol!=-1.0) && (i!=(SecSize-1)))
	  // {
   //        mainline_out[cycle_index][i].agg_vol = 0.5*(mainline_out[cycle_index][i-1].agg_vol+mainline_out[cycle_index][i+1].agg_vol);
	  // }else if ( (i==(SecSize-1)) &&  (mainline_out[cycle_index][SecSize-1].agg_vol==-1.0))
	  // {
   //        mainline_out[cycle_index][SecSize-1].agg_vol = 1000; 
	  // }
   //}

// average the historical data from data buffer

   for(i=0;i<SecSize;i++){

        float temp_ary_vol[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_speed[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_occ[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_density[NUM_CYCLE_BUFFS] = {{0}};	
       
		for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
         temp_ary_vol[j]= mainline_out[j][i].agg_vol;
		 temp_ary_speed[j]= mainline_out[j][i].agg_speed;
		 temp_ary_occ[j]= mainline_out[j][i].agg_occ;
		 temp_ary_density[j]= mainline_out[j][i].agg_density;

	  }

	   mainline_out_f[i].agg_vol = mean_array(temp_ary_vol,NUM_CYCLE_BUFFS);
	   mainline_out_f[i].agg_speed = mean_array(temp_ary_speed,NUM_CYCLE_BUFFS);
       mainline_out_f[i].agg_occ = mean_array(temp_ary_occ,NUM_CYCLE_BUFFS);
	   mainline_out_f[i].agg_density = mean_array(temp_ary_density,NUM_CYCLE_BUFFS);
   }

   for(i=0;i<NumOnRamp;i++){
		float temp_ary_OR_vol[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_OR_occ[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_FR_vol[NUM_CYCLE_BUFFS] = {{0}};
		float temp_ary_FR_occ[NUM_CYCLE_BUFFS] = {{0}};

	  for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
	     temp_ary_OR_vol[j] = onramp_out[j][i].agg_vol; 
		 temp_ary_OR_occ[j] = onramp_out[j][i].agg_occ;   
		 temp_ary_FR_vol[j] = offramp_out[j][i].agg_vol;   
		 temp_ary_FR_occ[j] = offramp_out[j][i].agg_occ;   
	  }
	  onramp_out_f[i].agg_vol =  mean_array(temp_ary_OR_vol,NUM_CYCLE_BUFFS);
	  onramp_out_f[i].agg_occ = mean_array(temp_ary_OR_occ,NUM_CYCLE_BUFFS);
      offramp_out_f[i].agg_vol = mean_array(temp_ary_FR_vol,NUM_CYCLE_BUFFS);
	  offramp_out_f[i].agg_occ = mean_array(temp_ary_FR_occ,NUM_CYCLE_BUFFS);
   }

/*###################################################################################################################
###################################################################################################################*/

		get_current_timestamp(&ts);
		print_timestamp(st_file_out, &ts);
		for(i=0;i<SecSize;i++)
		{
			    detection_s[i]->data[Np-1].flow=Mind(12000.0, Maxd(mainline_out_f[i].agg_vol, 200.0*(1.0+0.5*rand()/RAND_MAX)));
			    detection_s[i]->data[Np-1].speed=Mind(100.0, Maxd(mainline_out_f[i].agg_speed, 5.0*(1.0+0.5*rand()/RAND_MAX)));
			    detection_s[i]->data[Np-1].occupancy=Mind(100.0, Maxd((mainline_out_f[i].agg_occ), 5.0*(1.0+0.5*rand()/RAND_MAX)));
			    detection_s[i]->data[Np-1].density=Mind(1200.0, Maxd(mainline_out_f[i].agg_density, 10.0*(1.0+0.5*rand()/RAND_MAX)));  
			    fprintf(st_file_out,"%.6f ", mainline_out_f[i].agg_vol); 
                fprintf(st_file_out,"%.6f ", mainline_out_f[i].agg_speed); 
				fprintf(st_file_out,"%.6f ", mainline_out_f[i].agg_occ); 
				fprintf(st_file_out,"%.6f ", mainline_out_f[i].agg_density); 


		} 

		fprintf(st_file_out,"\n");

		for(i=0;i<NumOnRamp;i++)
		{	
				detection_onramp[i]->data[Np-1].flow=Mind(6000.0, Maxd(onramp_out_f[i].agg_vol, 100.0*(1.0+0.5*rand()/RAND_MAX)));
				detection_onramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd((onramp_out_f[i].agg_occ), 5.0*(1.0+0.5*rand()/RAND_MAX))); 
				detection_offramp[i]->data[Np-1].flow=Mind(6000.0, Maxd(offramp_out_f[i].agg_vol, 100.0*(1.0+0.5*rand()/RAND_MAX)));
				detection_offramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd((offramp_out_f[i].agg_occ), 5.0*(1.0+0.5*rand()/RAND_MAX))); 	
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_vol);  			
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_occ);  			
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_vol);
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_occ);
		}
		fprintf(st_file_out,"\n");
		
		
		det_data_4_contr(time);		
		get_meas(time);				
		update_q_R();
		opt_metering();
		
		fprintf(cal_opt_f,"%lf ", time);
		for (i=0;i<NumOnRamp;i++)
		{
			if (i<NumOnRamp-1)
				fprintf(cal_opt_f,"%lf ",opt_r[i][0]);
			else
				fprintf(cal_opt_f,"%lf\n", opt_r[i][1]);
		}
		
		ln_rm_distrib();
	
		if (use_CRM == 1)
			Set_Default_Meter(time,time2,timeSta); 
		else if (use_CRM == 2)	
			Set_Opt_Meter(time,time2,timeSta);     // original Opt RM strategy fro all 16 onramps
		else if (use_CRM == 3)
			Set_Coord_ALINEA(time,time2,timeSta);
		else if (use_CRM == 4)
			Set_Hybrid_Meter(time,time2,timeSta);  // upstream use default; downstream 11 onramps use CRM
		else;

		cycle_index++;
		cycle_index %= NUM_CYCLE_BUFFS;
	
		TIMER_WAIT(ptimer);	

	} 
	
	Finish_sim_data_io();
	      	
	return 0;
}

int Init_sim_data_io()
{	
	//st_file=fopen("In_Data/state_var.txt","r");	
	
	dbg_f=fopen("Out_Data/dbg_file.txt","w");
	
	//dmd_f=fopen("Out_Data/dmd_file.txt","w");
	
    vsl_crm_f=fopen("Out_Data/crm_RT_rate.txt","w");
    
    cal_opt_f=fopen("Out_Data/cal_opt_RT_rt.txt","w");
    
    st_file_out=fopen("Out_Data/state_var_out.txt","w");

    dbg_st_file_out=fopen("Out_Data/dbg_state_var_out.txt","w");

    if(st_file_out == NULL) {
	perror("st_file_out fopen");
	exit(1);
    }
    
    if(dbg_st_file_out == NULL) {
	perror("dbg_st_file_out fopen");
	exit(1);
    }

	//sec_outfile=fopen("Out_Data/section.txt","w");
	
	pp=fopen("Out_Data/coeff.txt","w");
		
	
	return 1;
}


int Init()  // A major function; Called by AAPI.cxx: the top function for intialization of the whole system
{
	int i; //j;
//	int id;
//	int ori=0, dest=0;	
//	float tmp_buff=0.0;
//	errno_t err;

	// Memory set for variables
	for(i=0;i<SecSize;i++)
	{
		u[i]=89.0; 		
	}
	for(i=0;i<SecSize;i++)
	{
		v[i]=0.0;
		q[i]=0.0;
		o[i]=0.0;
	}

	
	memset(&opt_r,0,sizeof(opt_r));
	memset(&dd,0,sizeof(dd));
	memset(&ss,0,sizeof(ss));
	memset(&pre_w,0,sizeof(pre_w));
	memset(&pre_rho,0,sizeof(pre_rho));
	memset(&up_rho,0,sizeof(up_rho));
	memset(&ln_metering_rate,0,sizeof(ln_metering_rate));
	memset(&dmd,0,sizeof(dmd));
	memset(&s,0,sizeof(s));
	memset(&qc,0,sizeof(qc));
	memset(&q_main,0,sizeof(q_main));
	memset(&dyna_min_r,0,sizeof(dyna_min_r));
	memset(&dyna_max_r,0,sizeof(dyna_max_r));
	memset(&release_cycle,0,sizeof(release_cycle));
	memset(&RM_occ,0,sizeof(RM_occ));
	memset(&Ramp_rt,0,sizeof(Ramp_rt));
	memset(&Q_o,0,sizeof(Q_o));
	memset(&Q_min,0,sizeof(Q_min));
	memset(&(detection_s_0[0]),0,sizeof(detection_s_0[SecSize]));
	memset(&(detection_onramp_0[0]),0,sizeof(detection_onramp_0[NumOnRamp]));
	memset(&(detection_offramp_0[0]),0,sizeof(detection_offramp_0[NumOnRamp]));
	memset(&a_w,0,sizeof(a_w));
	
	for(i=0; i<SecSize;i++)
		detection_s[i] = &(detection_s_0[i]);	
	for(i=0; i<NumOnRamp;i++)
	{
		detection_onramp[i] = &(detection_onramp_0[i]);	
		detection_offramp[i] = &(detection_offramp_0[i]);	
	}
	
	for(i=0;i<NumOnRamp;i++)
	{
		dmd[i]=0.0;
		dyna_min_r[i]=200.0;
		dyna_max_r[i]=dyna_min_r[i]+rm_band;	
		Ramp_rt[i]=max_RM_rt;   // only used in Set_Coord_ALINEA
		Q_o[i]=max_RM_rt*N_OnRamp_Ln[i];
		Q_min[i]=min_RM_rt*N_OnRamp_Ln[i];
		//opt_r[i][0]=max_RM_rt*N_OnRamp_Ln[i];
		a_w[i]=6.5;
		onrampL[i]=onrampL[i]/1609.0;		
	}
	//T=30/120.0;
	
	for(i=0; i<SecSize;i++)
		L[i]=L[i]/1609.0;
	// read system parameters

	
	//InitRealtimeDetection();	// mempry allocation; for detection measure
	//InitRealtimeDetection_s();	// memory allocation; for control detection, almost the same as InitRealtimeDetection(), just has less detector and not save data
    //InitDataProfile_s();	//for control history data
	//InitRealTimeSection();	//for section measure
	//Init_L();               // read Aimsun solid line length (similar to section length)
	//Init_lambda();        // use constant composite one
	//get_onramp_length();
	
	
	u[NumOnRamp]=104;   // Dim Ok	
	ControlOn=0;
	StateOn=0;
	StateOff=0;
	

	//init qc
	for(i=0;i<SecSize;i++)
		qc[i]=Q_ln*lambda[i];
	

	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int moveData(detData* detection)
{
	int k;
	for(k=0;k<Np-1;k++)
	{
		//detection->data[k]=detection->data[k+1];
		detection->data[k].DataTime=detection->data[k+1].DataTime;
		detection->data[k].density=detection->data[k+1].density;
		detection->data[k].flow=detection->data[k+1].flow;
		detection->data[k].instant_density=detection->data[k+1].instant_density;
		detection->data[k].instant_flow=detection->data[k+1].instant_flow;
		detection->data[k].instant_speed=detection->data[k+1].instant_speed;
		detection->data[k].occupancy=detection->data[k+1].occupancy;
		detection->data[k].speed=detection->data[k+1].speed;
		detection->data[k].vehType=detection->data[k+1].vehType;
	}
	return 1;
}


int update_queue(float time)  // This will not be used since we have onramp length available;
{
	int i,j;
	float w;
//	StructAkiEstadSection onramp_info;

	//for(i=0;i<SecSize;i++)
	for(i=0;i<NumOnRamp;i++)
	{
		w=0;
		for(j=0;j<max_onramp_ln;j++)
		{
			
			/*if(OnRamp_Ln_Id[i][j]>0)
			{
				onramp_info=AKIEstGetParcialStatisticsSection(OnRamp_Ln_Id[i][j],time,0);
				if(onramp_info.report!=0)
					AKIPrintString("read section stat error");
				else
					w=Maxd(w,onramp_info.LongQueueAvg);  // use the max queue of the lane as onramp queue
			}
			*/
		}
		queue[i]=w;
	}

	return 1;
}


int det_data_4_contr(float time) // not used anymore
{
	int i,j;
	
	//update_queue(time);
	
	for(i=0;i<NumOnRamp;i++)
	{
		pre_w[i]=(float)queue[i];
	}

	for(i=0;i<NumOnRamp;i++) // est demnd, odd-ramp flow and queue
	{
		for(j=0;j<Np;j++)
		{				
			dd[i][j]=(float)(detection_onramp[i]->data[Np-1].flow);
			//if (i==0 || i==5 || i==6 || i==7 || i==8)						
			dd[i][j]=(dd[i][j])*(1.0+dmd_change);			
		}
		dmd[i]=(float)(detection_onramp[i]->data[Np-1].flow);
		
		for(j=0;j<Np;j++)	
			ss[i][j]=detection_offramp[i]->data[Np-1].flow; // use current step
			
		pre_w[i]=pre_w[i]+T*(dd[i][0]-opt_r[i][0]);
	}
	

	for(j=0;j<Np;j++)		
		up_rho[j]=(float)((detection_s[0]->data[Np-1].density));		
	for(i=0;i<NumOnRamp;i++)		
		pre_rho[i]=(float)((detection_s[i+1]->data[Np-1].density));  // exclude the most upstream
	
	return 1;
}

int update_q_R()       // update flow for each cell
{
	int i;
	
	for(i=1;i<=NumOnRamp;i++)
	{		
		R[i-1]=get_min(dmd[i-1],get_min(Q_o[i-1],qc[i]-q_main[i-1]));                                     // use filtered onramp flow
		q[i]=q_main[i]+R[i-1]-ss[i-1][Np-1];                       // flow of currect cell          // index of R changed;  01/03/13		
	}
	q[0]=q_main[0];
	
	return 1;
}



int get_meas(float T)
{
	int i;


	for(i=0;i<SecSize;i++)
	{
		
		v[i]=exp_flt*(detection_s[i]->data[Np-1].speed)+(1.0-exp_flt)*v[i];
		o[i]=exp_flt*(detection_s[i]->data[Np-1].occupancy)+(1.0-exp_flt)*o[i];
		q_main[i]=exp_flt*(detection_s[i]->data[Np-1].flow)+(1.0-exp_flt)*q_main[i]; // use upastream section flow for MPC
		u2[i]=(detection_s[i]->data[Np-1].speed)*1609.0/3600.0;
	}
	
	for(i=0;i<NumOnRamp;i++)
	{
		if (detection_offramp[i]->detId > 0)					
			s[i]=exp_flt*(detection_offramp[i]->data[Np-1].flow)+(1-exp_flt)*s[i]; // changed on 03/03/14
			//s[i]=detection_offramp[i]->data[Np-1].flow;
		else
			s[i]=0.0;
	}
	
	return 1;
}



int ln_rm_distrib()
{
	int i,j;
	float tt_flw;
//	char str[len_str];

	for (i=0;i<NumOnRamp;i++)
	{
		tt_flw=0.0;
		for(j=0;j<max_onramp_ln;j++)
		{
			if (detection_onramp[i]->detId_ln[j]>0)
				tt_flw+=detection_onramp[i]->ln_flow[j];
		}
		for(j=0;j<max_onramp_ln;j++)
		{
			if (detection_onramp[i]->detId_ln[j]>0)
			{
				ln_metering_rate[i][j]=((detection_onramp[i]->ln_flow[j])/Maxd(tt_flw,1.0))*opt_r[i][0];
				
			}
			else
				ln_metering_rate[i][j]=0.0;			
			
		}
		
	}
	fprintf(vsl_crm_f, "\n");	

	return 1;
}

/**************************************************

	Should apply to all 16 onramps

***************************************************/

/*******************************************************
	Upstream 5 meters using default local responsive;
	downstream 11 meters are coordinated
********************************************************/


int Set_Hybrid_Meter(float time,float time2,float timeSta)
{
//	AKIPrintString("metering");
	int i,j;
//	int k, tmp_err, tmp_err1, tmp_err2, tmp_err3, tmp_err4;
//	float flow=0.0;
//	float amax=0.0;
//	float amin=0.0;
	float new_rate=0.0;
//	char str[len_str];

//	tmp_err=0; tmp_err1=0; tmp_err2=0; tmp_err3=0; tmp_err4=0; 
	
	
	
	if(ISUPDATE2>=0)  // set every step
	{
	
		////////////////////////////////////////////////////////
		// Coorfdinated Opt Metering for downstream 11 Onramps
		////////////////////////////////////////////////////////

		for(i=0;i<NumOnRamp;i++)  
		{
			#ifdef DYNAMIC_BOUNBDS		
			if (detection_s[i+1]->data[Np-1].density < 30)  
				dyna_min_r[i]=700.0;
			else if (detection_s[i+1]->data[Np-1].density < 45)
				dyna_min_r[i]=675.0;
			else if (detection_s[i+1]->data[Np-1].density < 60)
				dyna_min_r[i]=650.0;
			else if (detection_s[i+1]->data[Np-1].density < 75)
				dyna_min_r[i]=625.0;
			else if (detection_s[i+1]->data[Np-1].density < 90)
				dyna_min_r[i]=600.0;
			else if (detection_s[i+1]->data[Np-1].density < 105)
				dyna_min_r[i]=575.0;
			else if (detection_s[i+1]->data[Np-1].density < 120)
				dyna_min_r[i]=550.0;
			else if (detection_s[i+1]->data[Np-1].density < 135)
				dyna_min_r[i]=525.0;
			else 
				dyna_min_r[i]=500.0;
			/*if (detection_s[i+1]->data[Np-1].density < 20) 
				dyna_min_r[i]=900.0;
			else if (detection_s[i+1]->data[Np-1].density < 30)  
				dyna_min_r[i]=850.0;
			else if (detection_s[i+1]->data[Np-1].density < 45)
				dyna_min_r[i]=800.0;
			else if (detection_s[i+1]->data[Np-1].density < 60)
				dyna_min_r[i]=750.0;
			else if (detection_s[i+1]->data[Np-1].density < 75)
				dyna_min_r[i]=600.0;
			else if (detection_s[i+1]->data[Np-1].density < 90)
				dyna_min_r[i]=500.0;
			else if (detection_s[i+1]->data[Np-1].density < 105)
				dyna_min_r[i]=400.0;
			else if (detection_s[i+1]->data[Np-1].density < 120)
				dyna_min_r[i]=350.0;
			else if (detection_s[i+1]->data[Np-1].density < 135)
				dyna_min_r[i]=325.0;
			else 
				dyna_min_r[i]=300.0;
			*/
		
			//dyna_min_r[i]=dyna_min_r[i]*1.25;
			dyna_min_r[i]=dyna_min_r[i]*1.1;	
			dyna_max_r[i]=dyna_min_r[i]+rm_band;
			if (dyna_max_r[i]>max_RM_rt)
				dyna_max_r[i]=max_RM_rt;
			#endif			
			fprintf(dbg_f,"ds=%10.2f; Occ=%10.2f ", detection_s[i]->data[Np-1].density, (detection_s[i]->data[Np-1].occupancy));
			
			for (j=0;j<max_onramp_ln;j++)
			{				
				if(ln_meteringId[i+5][j]>0)    //Shuld be   "i+5"
				{
						//tmp_err=ECIGetParametersFlowMeteringById(ln_meteringId[i+5][j],timeSta,&amax,&flow,&amin);  // removed on 12_03_15
						new_rate=ln_metering_rate[i][j];

						// further modify RM rate
											
						/*if (i==(5-5) || i==(7-5)) // 
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0-0.35);
							dyn2_max_r[i]=dyna_max_r[i]*(1.0-0.35);
							new_rate=new_rate*(1.0-0.35);
						}
						if (i==(6-5) || i==(8-5) || i==(11-5)) // 7: Calvine WB: 
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0+0.25);
							dyna_max_r[i]=dyna_max_r[i]*(1.0+0.25);
							new_rate=new_rate*(1.0+0.25); 
						}	*/		

						if (dyna_max_r[i]>max_RM_rt)                            // added on 01_08_14
							dyna_max_r[i]=max_RM_rt;						
										
						if (new_rate <= dyna_min_r[i])
						{
															
							if (j<2)
								actual_r[i+5][j]=dyna_min_r[i];	
							else
								actual_r[i+5][j]=max_RM_rt;  //HOV
						}
						else if(new_rate>dyna_min_r[i]  && new_rate<=dyna_max_r[i])	
						{							
							
							if (j<2)
								actual_r[i+5][j]=new_rate;
							else
								actual_r[i+5][j]=max_RM_rt;  //HOV
						}
						else
						{										
							
							if (j<2)
								actual_r[i+5][j]=dyna_max_r[i];
							else
								actual_r[i+5][j]=max_RM_rt;  //HOV
						}
																				
				} //ln_meteringId
				else
					actual_r[i+5][j]=0.0;
				
				
			} // for j-loop end

			fprintf(dbg_f,"\n");							
		}  // i-llop end downstream

		

		// Activate downstream metering only with queue over-write
		for(i=0;i<NumOnRamp;i++) 
		{
			for (j=0;j<max_onramp_ln;j++)
			{				
				if(ln_meteringId[i+5][j]>0)
				{
					if (actual_r[i+5][j]>max_RM_rt)
						actual_r[i+5][j]=max_RM_rt;
				}
				else
					actual_r[i+5][j]=0.0;

				if(ln_meteringId[i+5][j]>0)
				{
				//	tmp_err1=ECIChangeParametersFlowMeteringById(ln_meteringId[i+5][j],timeSta,max_RM_rt,actual_r[i+5][j],dyna_min_r[i]); // removed on 12_03_15				 

					// Queue flush for all onramps				
				//	if( (AKIDetGetTimeOccupedAggregatedbyId(OnRamp_Ln_AdvanceDetEndId[i][j],0)> RELEASE_OCC)  )  // 80.0 is too high							
				//		release_cycle[i+5][j]=0;	 // removed on_12_03_15		
					if (release_cycle[i+5][j]<= RM_RELEASE_CYCLE)
					{															
						//tmp_err4=ECIChangeParametersFlowMeteringById(ln_meteringId[i+5][j],timeSta,max_RM_rt,max_RM_rt,dyna_min_r[i]);	 // removed on 12_03_15														
						actual_r[i+5][j]=max_RM_rt;
						release_cycle[i+5][j]++;
					}
				}
			}  // for j-loop
			ISUPDATE2=0;
		}  // for i-loop end

		// Output time and RM rate
		fprintf(vsl_crm_f,"%10.2f\t", timeSta);
		for(i=0;i<NumOnRamp+5;i++)
			fprintf(vsl_crm_f,"%10.2f\t", (float)(actual_r[i][0]+actual_r[i][1]+actual_r[i][2]));
		fprintf(vsl_crm_f,"\n");
			
	} // if ISUPDATE2 loop 
	else
		ISUPDATE2++;	
	return 1;
}


// This is not used anymore, 11_27_14
int Set_Opt_Meter(float time,float time2,float timeSta)
{
//	AKIPrintString("metering");
	int i,j; 
//	int tmp_err, tmp_err1, tmp_err2, tmp_err3, tmp_err4;
//	float flow=0.0;
//	float amax=0.0;
//	float amin=0.0;
//	float tmp_occ=0.0;
	static float new_rate=0.0;	
//	char str[len_str];
	//static int release_cycle=0;

//	tmp_err=0; tmp_err1=0; tmp_err2=0; tmp_err3=0; tmp_err4=0;

	if(ISUPDATE2>=0)  // set every step
	{
		for(i=0;i<NumOnRamp;i++)
		{
			//for (j=0;j<max_onramp_ln;j++)
			//	Green[i][j]=0;

			#ifdef DYNAMIC_BOUNBDS
			#ifndef USE_FIELD_BOUND
			if (detection_s[i+1]->data[Np-1].density < 30)  
				dyna_min_r[i]=700.0;
			else if (detection_s[i+1]->data[Np-1].density < 45)
				dyna_min_r[i]=675.0;
			else if (detection_s[i+1]->data[Np-1].density < 60)
				dyna_min_r[i]=650.0;
			else if (detection_s[i+1]->data[Np-1].density < 75)
				dyna_min_r[i]=625.0;
			else if (detection_s[i+1]->data[Np-1].density < 90)
				dyna_min_r[i]=600.0;
			else if (detection_s[i+1]->data[Np-1].density < 105)
				dyna_min_r[i]=575.0;
			else if (detection_s[i+1]->data[Np-1].density < 120)
				dyna_min_r[i]=550.0;
			else if (detection_s[i+1]->data[Np-1].density < 135)
				dyna_min_r[i]=525.0;
			else //if (detection_s[i+1]->data[Np-1].density < 150)
				dyna_min_r[i]=500.0;
			//else
			//	dyna_min_r[i]=100.0;

			dyna_min_r[i]=dyna_min_r[i]*1.25;
			#endif
			#ifdef USE_FIELD_BOUND
				tmp_err=ECIGetParametersFlowMeteringById(ln_meteringId[i][0],timeSta,&amax,&flow,&amin);
				dyna_min_r[i]=amin;
			#endif

			
			dyna_max_r[i]=dyna_min_r[i]+rm_band;
			//if (dyna_max_r[i]>max_RM_rt)                            // added on 01_08_14
			//	dyna_max_r[i]=max_RM_rt;

			#endif
			
			//fprintf(dbg_f,"%10.2f %10.2f ", detection_s[i]->data[Np-1].density, (detection_s[i]->data[Np-1].occupancy));
			//fprintf(dbg_f,"%10.2f ", detection_onramp[i]->data[Np-1].occupancy);
		}
		//fprintf(dbg_f,"\n");

		for(i=0;i<NumOnRamp;i++)
		{	
			for (j=0;j<max_onramp_ln;j++)
			{
				
				if(ln_meteringId[i][j]>0)
				{
					//	tmp_err=ECIGetParametersFlowMeteringById(ln_meteringId[i][j],timeSta,&amax,&flow,&amin);  // removed o n 12_03_15
						new_rate=ln_metering_rate[i][j];

						// further modify RM rate
						/*if (i==5 || i==7) // run multi-day and reps on 2/11~2/12
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0-0.25);
							dyna_max_r[i]=dyna_max_r[i]*(1.0-0.25);
							new_rate=new_rate*(1.0-0.25);
						}
						if (i==6) // 7: Calvine WB: 
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0+0.1);
							dyna_max_r[i]=dyna_max_r[i]*(1.0+0.1);
							new_rate=new_rate*(1.0+0.1); 
						}*/
					#ifndef USE_FIELD_BOUND
						if (i==5 || i==7) // 
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0-0.35);
							dyna_max_r[i]=dyna_max_r[i]*(1.0-0.35);
							new_rate=new_rate*(1.0-0.35);
						}
						if (i==6 || i==8 || i==11) // 7: Calvine WB: 
						{
							dyna_min_r[i]=dyna_min_r[i]*(1.0+0.25);
							dyna_max_r[i]=dyna_max_r[i]*(1.0+0.25);
							new_rate=new_rate*(1.0+0.25); 
						}				

						if (dyna_max_r[i]>max_RM_rt)                            // added on 01_08_14
							dyna_max_r[i]=max_RM_rt;
					#endif	
					
					//#ifdef DYNAMIC_BOUNBDS
						if (new_rate < dyna_min_r[i])
						{
						//	tmp_err3=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,dyna_max_r[i],dyna_min_r[i],dyna_min_r[i]);	// removed on 12_03_15							
							if (j<2)
								actual_r[i][j]=dyna_min_r[i];	
							else
								actual_r[i][j]=max_RM_rt;  //HOV
						}
						else if(new_rate<=dyna_max_r[i] && new_rate>=dyna_min_r[i])	
						{							
							//tmp_err1=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,dyna_max_r[i],new_rate,dyna_min_r[i]);	 // removed on 12_03_15
							if (j<2)
								actual_r[i][j]=new_rate;
							else
								actual_r[i][j]=max_RM_rt;  //HOV
						}
						else
						{										
							//tmp_err2=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,dyna_max_r[i],dyna_max_r[i],dyna_min_r[i]);	// removed on 12_03_15
							if (j<2)
								actual_r[i][j]=dyna_max_r[i];
							else
								actual_r[i][j]=max_RM_rt;  //HOV
						}
						// Queue flush				
					//	if( (AKIDetGetTimeOccupedAggregatedbyId(OnRamp_Ln_AdvanceDetEndId[i][j],0)> RELEASE_OCC)  )  // 80.0 is too high							
					//		release_cycle[i][j]=0;		 //// removed on 12_03_15	
						if (release_cycle[i][j]<= 2)
							{															
							//	tmp_err4=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,max_RM_rt,max_RM_rt,dyna_min_r[i]);	// removed on 12_03_15										
				
								actual_r[i][j]=max_RM_rt;
								release_cycle[i][j]++;
							}														
																											
				} //ln_meteringId
				else
					actual_r[i][j]=0.0;
			} // for j-loop end

			//opt_r[i][0]=(float)(actual_r[i][0]+actual_r[i][1]+actual_r[i][2]);
		}// for i-loop end	
		fprintf(vsl_crm_f, "\n");
		ISUPDATE2=0;

		fprintf(vsl_crm_f,"%10.2f\t", timeSta);
		for(i=0;i<NumOnRamp;i++)
			 fprintf(vsl_crm_f,"%10.2f\t", (float)(actual_r[i][0]+actual_r[i][1]+actual_r[i][2]));
		fprintf(vsl_crm_f,"\n");
	} // if ISUPDATE2 loop 
	else
		ISUPDATE2++;
	
	return 1;
}

int Set_Coord_ALINEA(float time,float time2,float timeSta)
{
//	AKIPrintString("metering");
	int i,j;
//	int tmp_err, tmp_err1; // tmp_err2;
//	float flow=0.0;
//	float amax=0.0;
	float amin=0.0;
	float corridor_mean_occ=0.0, tmp_occ=0.0;
	//int tmp_deno=0;
//	char str[len_str];
	//static int release_cycle=0;

//	tmp_err=0; tmp_err1=0; 
	if(ISUPDATE2>=0)  // set every step
	{
		for(i=0;i<NumOnRamp+5;i++)
		{	
			
			for (j=0;j<max_mainline_ln;j++)
			{
				if (Mainline_Ln_Det_Id_All[i][j]>0)
				{					
				//	RM_occ[i]+=AKIDetGetTimeOccupedAggregatedbyId(Mainline_Ln_Det_Id_All[i][j],0);  // removed on 12_03_15																
				}
			}
			RM_occ[i]=RM_occ[i]/N_Mainline_Ln_RM_All[i];
			//fprintf(dbg_f,"%10.2f ", RM_occ[i]);
			//fprintf(dbg_f,"%10.2f %10.2f ", SR99_RM_rate_tbl[0][i], SR99_RM_occ_tbl[0][i]);
		}
		corridor_mean_occ=0.0;
		for(i=0;i<NumOnRamp;i++)
			corridor_mean_occ+=RM_occ[i];
		corridor_mean_occ=Maxd(corridor_mean_occ/NumOnRamp, 30.0);
		
		for(i=0;i<NumOnRamp+5;i++)
		{	
			Ramp_rt[i]=0.0;
			for (j=0;j<max_onramp_ln;j++)
			{
				if (ln_meteringId[i][j]>0)
				{
				//	tmp_err=ECIGetParametersFlowMeteringById(ln_meteringId[i][j],timeSta,&amax,&flow,&amin);   // removed on 12_03_15
					if (j<2)
					{
						tmp_occ=corridor_mean_occ*(1+(8.0-i)*0.025);  // Upstream operate at higher OCC
						if (RM_occ[i] <= tmp_occ)
							Ramp_rt[i]+=Gain_Up*(tmp_occ-RM_occ[i]);
						if (RM_occ[i] > tmp_occ)
							Ramp_rt[i]+=Gain_Dn*(tmp_occ-RM_occ[i]);

						//actual_r[i][j]=(AKIDetGetTimeOccupedAggregatedbyId(Mainline_Ln_Det_Id[i][j],0)/Maxd(RM_occ[i],1.0))*Ramp_rt[i];
						actual_r[i][j]=Ramp_rt[i]*1.35;
						
					}
					else  // HOV lane														
						actual_r[i][j]=max_RM_rt;							
				}
				else
					actual_r[i][j]=0.0;

				
			} // for j-loop
			
			if (actual_r[i][j]>max_RM_rt)
				actual_r[i][j]=max_RM_rt;
			if (actual_r[i][j]<amin)
				actual_r[i][j]=amin;
		} // for i-loop
		//fprintf(dbg_f,"\n");

		for(i=0;i<NumOnRamp;i++)
		{	
			for (j=0;j<max_onramp_ln;j++)
			{				
				if(ln_meteringId[i][j]>0)
				{
				//	tmp_err1=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,max_RM_rt,actual_r[i][j],amin); // removed on 12_03_15

				// Queue flush				
				//	if( (AKIDetGetTimeOccupedAggregatedbyId(OnRamp_Ln_AdvanceDetEndId[i][j],0)> RELEASE_OCC)  )  // 80.0 is too high	// Removed on 12_03_15						
						release_cycle[i][j]=0;			
					if (release_cycle[i][j]<= 2)
					{															
					//	tmp_err2=ECIChangeParametersFlowMeteringById(ln_meteringId[i][j],timeSta,max_RM_rt,max_RM_rt,dyna_min_r[i]);	// removed on 12_03_15														
						actual_r[i][j]=max_RM_rt;
						release_cycle[i][j]++;
					}	
				}
			} // for j-loop end
			//opt_r[i][0]=(float)(actual_r[i][0]+actual_r[i][1]+actual_r[i][2]);
		}// for i-loop end	
		fprintf(vsl_crm_f, "\n");
		ISUPDATE2=0;

		fprintf(vsl_crm_f,"%10.2f\t", timeSta);
		for(i=0;i<NumOnRamp;i++)
			 fprintf(vsl_crm_f,"%10.2f\t", (float)(actual_r[i][0]+actual_r[i][1]+actual_r[i][2]));
		fprintf(vsl_crm_f,"\n");

	} // if ISUPDATE2 loop 
	else
		ISUPDATE2++;	
	return 1;
}

/*******************************************
               OptSolver
********************************************/

int set_coef(float c[MP][NP],float Qm)
{
	char str[len_str];

	float w_const[SecSize][Np]={{0.0}}; 					// Used to construct b_u; 
	float p_const[SecSize][Np]={{0.0}};					// Used to construct b_u; 	
	float f[(NumOnRamp)*Np]={0.0};
	
	float b_u[M1]={0.0};
	float b_l[M2]={0.0};   // upper bound of r

	int m = 0,i = 0,j = 0;
	static unsigned short memset_sw=1;
	
	if (memset_sw==1)
	{
	memset(&w_const,0,sizeof(w_const));
	memset(&p_const,0,sizeof(p_const));
	memset(&f,0,sizeof(f));	
	memset(&b_u,0,sizeof(b_u));
	memset(&b_l,0,sizeof(b_l));	
	memset_sw=0;
	}

	


	for(m=0;m<NumOnRamp;m++)
	{
		if (m==0)
		{
			p_const[m][0]=(1-T*u2[0]/L[m])*up_rho[1]+T/(L[m]*lambda[m+1])*opt_r[m][0];
			p_const[m][1]=(1-T*u2[0]/L[m])*p_const[m][0]+(T*u2[m]/L[m])*0;
			w_const[m][0]=pre_w[m]+T*dd[m][0];
			w_const[m][1]=w_const[m][0]+T*dd[m][1];	
		}
		else
		{
			p_const[m][0]=(1-T*u2[m+1]/L[m])*pre_rho[m]+T/(L[m]*lambda[m+1])*opt_r[m][0];
			p_const[m][1]=(1-T*u2[m+1]/L[m])*p_const[m][0]+(T*u2[m+1]/L[m])*p_const[m-1][0]-T/(L[m]*lambda[m+1])*ss[m][Np-1];
			w_const[m][0]=pre_w[m]+T*dd[m][0];
			w_const[m][1]=w_const[m][0]+T*dd[m][Np-1];				
		}
	}
	

//obj fnc
	for(m=0;m<(NumOnRamp);m++)
	{
		if(m==0)
		{			
			f[m]=(float)( (2-T*u2[m+1]/L[m])*T  -2*a_w[m]*T +a_ttd*(2-T*u2[m+1]/L[m])*T*u2[m+1]);
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);			
		}
		if (m<NumOnRamp-1)
		{
			f[m]=(float)( (2-T*u2[m+1]/L[m])*T + (lambda[m+1]*T*T*u2[m]/(lambda[m]*L[m])) + (lambda[m+1]*T*T*u2[m]*u2[m+1])/(lambda[m]*L[m])-2*a_w[m]*T +a_ttd*(2-T*u2[m+1]/L[m])*T*u2[m+1]);
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);
		}
		if (m==NumOnRamp-1)
		{
			f[m]=(float)( (lambda[m+1]*T*T*u2[m]/(lambda[m]*L[m])) - 2*a_w[m]*T );
			f[NumOnRamp+m]=(float)(T-T*a_w[m]+a_ttd*T*u2[m+1]);
		}
	}

	// lower and upper bounds
	
	for(m=0;m<NumOnRamp;m++)
	{		
		b_u[m]=Rou_J-p_const[m][0];
		b_u[NumOnRamp+m]=Rou_J-p_const[m][1];
		b_u[2*NumOnRamp+m]=pre_w[m]+dd[m][0];
		b_u[3*NumOnRamp+m]=pre_w[m]+T*dd[m][0]+T*dd[m][1];
		//b_u[4*NumOnRamp+m]=Mins(dd[m][1],Q_o[m]);
		b_u[4*NumOnRamp+m]=Mins(dd[m][1],R[m]);
		b_u[5*NumOnRamp+m]=Mins(Q_o[m],dd[m][0]);
		//b_u[5*NumOnRamp+m]=R[m];
	
		
		b_l[m]=pre_w[m]+T*dd[m][0]-onrampL[m]*Rou_J;
		b_l[NumOnRamp+m]=pre_w[m]+T*dd[m][0]+T*dd[m][1]-onrampL[m]*Rou_J;
	}
	
	for(m=0;m<M1;m++)	
	{
		//if (b_u[m] < 0.0)
		if (b_u[m] < Q_min[m])
			b_u[m]=Maxs(b_u[m],Mins(Maxs(dd[m][1],800),Q_o[m]));
			//b_u[m]=Maxs(b_u[m],500.0);
	}	
			
	for(m=0;m<NumOnRamp;m++)
	{	
		b_l[m]=Maxs(b_l[m],Q_min[m]);
		b_l[NumOnRamp+m]=Maxs(b_l[NumOnRamp+m],Q_min[m]);
		//b_l[m]=Maxs(b_l[m],0.0);
		//b_l[NumOnRamp+m]=Maxs(b_l[NumOnRamp+m],0.0);
	}

	// Assign f to Matrix c
	for(j=0;j<2*NumOnRamp;j++)	
		c[0][j+1]=f[i];			
	
	// Assign Upper &  lower Bounds to Matrix c
	for(i=1;i<=M1;i++)
		c[i][0]=b_u[i-1];		
	for(i=1;i<=M2;i++)
		c[10*NumOnRamp+i][0]=b_l[i-1];		

	// Matrix c
	for(i=1;i<=NumOnRamp;i++)
	{
		for(j=1;j<=NumOnRamp;j++)	
			c[i][j]=T/(L[j]*lambda[j]);
			
		if (i==1)
		{
			for(j=1;j<=NumOnRamp;j++)
			{		
				c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
				c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
			}		
		}
		else
		{
			for (j=1;j<=NumOnRamp;j++)
			{
				if (j==1)
				{					
					c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
					c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
				}
				else
				{
					c[NumOnRamp+i][j-1]=T*T*u2[j]/(L[j]*L[j-1]*lambda[j-1]);
					c[NumOnRamp+i][j]=(1-T*u2[j]/L[j])*T/(L[j]*lambda[j]);
					c[NumOnRamp+i][NumOnRamp+j]=T/(L[j]*lambda[j]);
				}				
			}
		}
		
		for(j=1;j<=NumOnRamp;j++)	
			c[2*NumOnRamp+i][j]=T;
		
		for(j=1;j<=NumOnRamp;j++)	
		{
			c[3*NumOnRamp+i][j]=T;
			c[3*NumOnRamp+i][NumOnRamp+j]=T;
		}
		
		for(j=1;j<=NumOnRamp;j++)	
			c[4*NumOnRamp+i][j]=1;
	
		for(j=1;j<=NumOnRamp;j++)	
			c[5*NumOnRamp+i][NumOnRamp+j]=1;
	}	// Matrix c: i-loop end	

	// The sign of the coefficient matrix sould be reversed; ecept the coeff of objective function (1st row) and b_u, b_l (1st column)
	for (i=1;i<MP;i++)
	{
		for (m=1;m<NP;m++)
			c[i][m]=-c[i][m];
	}

	sprintf(str,"up_rho:");
	fprintf(pp,"%s\n",str);	
	for(m=0;m<Np;m++)
	{
		fprintf(pp,"%lf ",up_rho[m]);		
	}
	fprintf(pp,"\n");	

	sprintf(str,"pre_rho:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		fprintf(pp,"%lf ",pre_rho[m]);		
	}
	fprintf(pp,"\n");
	
	
	sprintf(str,"u2:");
	fprintf(pp,"%s\n",str);
	
	for(m=0;m<SecSize;m++)
	{
		fprintf(pp,"%lf ",u2[m]);		
	}	
	fprintf(pp,"\n");
	

	sprintf(str,"pre_w:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)
	{
		fprintf(pp,"%lf ",pre_w[m]);		
	}
	fprintf(pp,"\n");

	//sprintf(str,"Q_o:");
	/*fprintf(pp,"Q_o=:\n");
	for(m=0;m<NumOnRamp;m++)
	{
		fprintf(pp,"%lf ",Q_o[m]);		
	}
	fprintf(pp,"\n") */
	

	sprintf(str,"Onramp Length:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)
	{
		fprintf(pp,"%lf ",onrampL[m]);		
	}
	fprintf(pp,"\n");	

	
	sprintf(str,"dd:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",dd[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
	sprintf(str,"ss:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<NumOnRamp;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",ss[m][j]);			
		}
		fprintf(pp,"\n");		
	}

	sprintf(str,"w_const:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",w_const[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
		
	sprintf(str,"p_const:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<SecSize;m++)
	{
		for(j=0;j<Np;j++)
		{
			fprintf(pp,"%lf ",p_const[m][j]);			
		}
		fprintf(pp,"\n");		
	}
	
	
	sprintf(str,"f:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<(NumOnRamp)*Np;m++)
	{
		fprintf(pp,"%f ",f[m]);		
	}
	fprintf(pp,"\n");	
	

	sprintf(str,"b_u:");
	fprintf(pp,"%s\n",str);
	//for(m=0;m<M1;m++)
	//{
	//	fprintf(pp,"%f ",b_u[m]);		
	//}
	for(i=0;i<10;i++)
	{
		for(m=0;m<NumOnRamp;m++)
			fprintf(pp,"%f ",b_u[NumOnRamp*i+m]);	
		fprintf(pp,"\n");	
	}
	

	sprintf(str,"b_l:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<M2;m++)
	{
		fprintf(pp,"%f ",b_l[m]);		
	}
	fprintf(pp,"\n");
	


/*sprintf(str,"M1 & M2:");
	fprintf(pp,"%s\n",str);
	fprintf(pp,"%i %i ",M1, M2); //b_l[m]);		
	fprintf(pp,"\n");	
*/
	sprintf(str,"c:");
	fprintf(pp,"%s\n",str);
	for(m=0;m<MP;m++)
	{
		for(j=0;j<NP;j++)
		{
			fprintf(pp,"%lf ",c[m][j]);			
		}
		fprintf(pp,"\n");		
	}

	
	return 0;
}

int opt_metering(void)
{
	int i,icase,*izrov,*iposv;

	float cc[MP][NP]={{0.0}};
	
	float **a;

	memset(&cc,0,sizeof(cc));
		
	set_coef(cc, Q_ln);   // previous Q_ln=1800
	izrov=ivector(1,N);
	iposv=ivector(1,M);
	a=convert_matrix(&cc[0][0],1,MP,1,NP);
	simplx(a,M,N,M1,M2,M3,&icase,izrov,iposv);
	
	/*for (i=1;i<=M;i++)
		fprintf(dbg_f,"%d ",iposv[i]);	
	fprintf(dbg_f,"\n");
	*/
	for (i=1;i<=N;i++)
		fprintf(dbg_f,"%d ",izrov[i]);	
	fprintf(dbg_f,"\n");
	
	if (icase == 0)
	{	
	
			for(i=1;i<=NumOnRamp;i++)
				{	
					opt_r[i-1][0]=a[iposv[i+1]][1];							
					//fprintf(dbg_f,"i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i][0]);	
				}	
			for(i=NumOnRamp+1;i<=2*NumOnRamp;i++)	
				{
					opt_r[i-1-NumOnRamp][1]=a[iposv[i+1]][1];								
					//fprintf(dbg_f,"i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i-1-NumOnRamp][1]);					
				}	
	}
	else
		fprintf(dbg_f,"i=%d, icase=%d\n",i,icase);		
	
	free_convert_matrix(a,1,MP,1,NP);
	free_ivector(iposv,1,M);
	free_ivector(izrov,1,N);

	return 0;
}
#undef NRANSI

/*******************************************
               OptSolver End
********************************************/


int Finish_sim_data_io()
{
	fflush(dbg_f);
	fclose(dbg_f);
	//fflush(dmd_f);
	//fclose(dmd_f);
	fflush(vsl_crm_f);
	fclose(vsl_crm_f);
	fflush(cal_opt_f);
	fclose(cal_opt_f);
	//fflush(det_outfile);
	//fclose(det_outfile);
	//fflush(sec_outfile);
	//fclose(sec_outfile);
	if (use_CRM == 2)
	{
		fflush(pp);
		fclose(pp);
	}
	//fclose(st_file);
	fflush(st_file_out);
	fclose(st_file_out);
    
	fflush(dbg_st_file_out);
	fclose(dbg_st_file_out);

	return 1;
}


//			    detection_s[i]->data[Np-1].flow=Mind(12000.0, Maxd(mainline_out[i].agg_vol, 200.0*(1.0+0.5*rand()/RAND_MAX)));
float Mind(float a,float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Maxd(float a,float b)
{
	if(a>=b)
		return a;
	else
		return b;
}
float get_min(float a, float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Mins(float a,float b)
{
	if(a<=b)
		return a;
	else
		return b;
}
float Maxs(float a,float b)
{
	if(a>=b)
		return a;
	else
		return b;
}
