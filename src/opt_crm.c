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
#include "look_up_table.h"
#include "rm_algo.h"
#include "clt_vars.h"

char str[len_str];

FILE *dbg_f, *dmd_f, *local_rm_f, *cal_opt_f, *pp, *st_file, *st_file_out, *Ln_RM_rt_f, *dbg_st_file_out;


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

const char *usage = "-i <loop interval> -r (run in replay mode)";

#define NUM_CYCLE_BUFFS  5											               // XYLu: field data buffeed for 5 steps
//
const char *controller_strings[] = {												// XYLu: Check upstream 5 section data: mainline, onramps and off-ramps
	"10.253.28.96_PORT_10014",		//0, OR1	Stockton Blvd	
	"10.253.28.96_PORT_10012",		//1, OR2	Elk Grove Blvd
	"10.254.25.116",	//2		Red Fox
	"10.254.25.122",	//3, OR3, FR1	EB Laguna Blvd
	"10.254.25.124",	//4, OR4,	WB Laguna Blvd
	"10.253.28.102",	//5		Laguna Creek 
	"10.253.28.104",	//6, OR5, FR2	EB Sheldon Rd
	"10.253.28.103",	//7, OR6	WB Sheldon Rd
	"10.254.27.67",		//8		Jacinto Rd
	"10.254.27.82",		//9, OR7, FR3	EB Calvine Rd
	"10.254.27.81",		//10, OR8	WB Calvine Rd
	"10.253.28.105",	//11, FR4	Stockton Blvd
	"10.254.28.213",	//12, OR9	EB Mack Rd
	"10.254.28.212",	//13, OR10	WB Mack Rd
	"10.254.127.2",		//14		Tangerine Ave
	"10.253.28.107",	//15		Pomegranate Ave
	"10.253.28.106",	//16		Orange Ave
	"10.254.24.156",	//17, OR11, FR5	EB Florin Rd
	"10.254.24.157",	//18, OR12	FR6 WB Florin Rd
	"10.254.26.183",	//19		Turnbridge Dr
	"10.253.28.110",	//20, OR13	FR7 EB 47th Ave
	"10.29.248.81",		//21, OR14	FR8 WB 47th Ave
	"10.253.28.108",	//22, FR9	Martin Luther King Jr. 
	"10.253.28.128_PORT_1001", //23, OR15,  EB Fruitridge
	"10.253.28.128_PORT_1002", //24, OR16, FR10	WB Fruitridge
	"10.253.28.130",	//25		21st Ave UC
	"10.254.26.103",	//26, OR17	12th Ave
	"10.253.28.131",	//27, FR12	FR11, 8th Ave POC
	"10.253.28.134"		//28, 	99 NB to I50 Broadway

}; //FR6, FR7, FR8, and FR11 are missing


int main(int argc, char *argv[])
{
	timestamp_t ts;
	timestamp_t *pts = &ts;
	float time = 0;
//	float time = 0, time2 = 0,timeSta = 0, tmp=0.0;
	static int init_sw=1;
	int i, j, k;
	int min_index;
	db_urms_t urms_ctl[NumOnRamp] = {{0}};
	db_urms_status_t controller_data[NUM_CONTROLLERS];  //See warning at top of file
	db_urms_status2_t controller_data2[NUM_CONTROLLERS];  //See warning at top of file
	db_urms_status3_t controller_data3[NUM_CONTROLLERS];  //See warning at top of file

	int option;
	int exitsig;
	db_clt_typ *pclt;
	char hostname[MAXHOSTNAMELEN+1];
	posix_timer_typ *ptimer;       /* Timing proxy */
	int interval = 30000;      /// Number of milliseconds between saves
	int cycle_index = 0;
	char *domain = DEFAULT_SERVICE; // usually no need to change this
	int xport = COMM_OS_XPORT;      // set correct for OS in sys_os.h
//	int verbose = 0;
	static agg_data_t mainline_out[NUM_CYCLE_BUFFS][SecSize] =  {{{0},{0}}};      // data aggregated section by section
	static agg_data_t onramp_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0},{0}}};      // data aggregated section by section
	static agg_data_t onramp_queue_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0},{0}}};      // data aggregated section by section
	static agg_data_t offramp_out[NUM_CYCLE_BUFFS][NumOnRamp] = {{{0},{0}}};  // data aggregated section by section
	
	static agg_data_t mainline_out_f[SecSize] = {{0}};        // save filtered data to this array
	static agg_data_t onramp_out_f[NumOnRamp] = {{0}};        // save filtered data to this array
	static agg_data_t offramp_out_f[NumOnRamp] = {{0}};    // save filtered data to this array
	static agg_data_t onramp_queue_out_f[NumOnRamp] = {{0}};  // save filtered data queue detector data to this array
	 
	
	static agg_data_t controller_mainline_data[NUM_CONTROLLERS] = {{0.0}};     // data aggregated controller by controller 
	static agg_data_t controller_onramp_data[NumOnRamp] = {{0.0}};                 // data aggregated controller by controller
	static agg_data_t controller_onramp_queue_detector_data[NumOnRamp] = {{0.0}};
	static agg_data_t controller_offramp_data[NumOnRamp] = {{0.0}};               // data aggregated controller by controller
	static float hm_speed_prev [NUM_CONTROLLERS] = {1.0};               // this is the register of harmonic mean speed in previous time step
	static float mean_speed_prev [NUM_CONTROLLERS] = {1.0};             // this is the register of mean speed in previous time step
	static float density_prev [NUM_CONTROLLERS] = {0.0};             // this is the register of density in previous time step
	static float OR_flow_prev [NumOnRamp] = {0.0};               // this is the register of on-ramp flow in previous time step
	static float OR_occupancy_prev [NumOnRamp] = {0.0};               // this is the register of on-ramp occupancy in previous time step
	static float FR_flow_prev [NumOnRamp] = {0.0};               // this is the register of on-ramp flow in previous time step
	static float FR_occupancy_prev [NumOnRamp] = {0.0};               // this is the register of on-ramp occupancy in previous time step
	static float float_temp;
//	static float ML_flow_ratio = 0.0; // current most upstream flow to historical most upstream flow
//	float current_most_upstream_flow = 0.0;
	static struct confidence confidence[NUM_CONTROLLERS][3]; 

	float temp_ary_vol[NUM_CYCLE_BUFFS] = {0};    // temporary array of cyclic buffer
	float temp_ary_speed[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_occ[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_density[NUM_CYCLE_BUFFS] = {0};	
//	float temp_ary_OR_vol[NUM_CYCLE_BUFFS] = {0};
//	float temp_ary_OR_occ[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_OR_queue_detector_vol[NUM_CYCLE_BUFFS] = {0};
	float temp_ary_OR_queue_detector_occ[NUM_CYCLE_BUFFS] = {0}; 
//	float temp_ary_FR_vol[NUM_CYCLE_BUFFS] = {0};
//	float temp_ary_FR_occ[NUM_CYCLE_BUFFS] = {0};
	float OR_vol_tmp=0.0, FR_vol_tmp=0.0; 
//	float OR_vol_tmp=0.0, OR_occ_tmp=0.0, FR_vol_tmp=0.0, FR_occ_tmp=0.0; 

	//int num_zero_tolerant = 10;
	//int OR_flow_zero_counter[NumOnRamp] = {0};
	//int OR_occ_zero_counter[NumOnRamp] = {0};
	//int FR_flow_zero_counter[NumOnRamp] = {0};
	//int FR_occ_zero_counter[NumOnRamp] = {0};

//	Example: metering_rates[0...10] = Calvine_EB_Metering_Rate...12th_St_Metering_Rate;     // Add up 5
	const short metering_controller_db_vars[NumOnRamp] = {                                               // XYLu sequence of EB & WB changed for all onramps with 2-dirs
		2802,	//E Stockton
		3002,	//Elk Grove	
		3402,	//Laguna EB
		3602,	//Laguna WB
		4002,	//Sheldon EB
		4202,	//Sheldon WB
		4602,	//Calvine EB
		4802,	//Calvine WB
		5202,	//Mack Rd EB
		5402,	//Mack Rd WB
		6202,	//Florin Rd EB
		6402,	//Florin Rd WB
		6802,	//47th EB
		7002,	//47th WB
		7402,	//Fruitridge EB
		7602,	//Fruitridge WB
		8002,	//12th St
	};
//	static int counter = 0;

	// Initialization for urms_ctl
	// Set lane 4 (nonexistent) action
	// to SKIP and metering rate to 1100 VPH
	// Set regular lanes 2 & 3 to fixed rate and all plans to 0.
	for(i=0; i<NumOnRamp; i++) {                                                            // Add up 5., only need to change NumOnRamp
		urms_ctl[i].lane_1_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_1_plan = 1;
		urms_ctl[i].lane_2_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_2_plan = 1;
		urms_ctl[i].lane_3_action = URMS_ACTION_FIXED_RATE;
		urms_ctl[i].lane_3_plan = 1;
		urms_ctl[i].lane_4_action = URMS_ACTION_SKIP;
		urms_ctl[i].lane_4_release_rate = 1100;
		urms_ctl[i].lane_4_plan = 1;
	}

	while ((option = getopt(argc, argv, "i:r")) != EOF) {
		switch(option) {
			case 'i':
				interval = atoi(optarg);
				break;
			case 'r':
				pts = &controller_data2[20].ts;
				break;
			default:
				printf("\nUsage: %s %s\n", argv[0], usage);
				exit(EXIT_FAILURE);
				break;
		}
	}
	memset(controller_data, 0, NUM_CONTROLLERS * (sizeof(db_urms_status_t)));//See warning at top of file
	memset(controller_data2, 0, NUM_CONTROLLERS * (sizeof(db_urms_status2_t)));//See warning at top of file
	memset(controller_data3, 0, NUM_CONTROLLERS * (sizeof(db_urms_status3_t)));//See warning at top of file
	
	// XYLu: adde 5/21/2019
	
	memset(mainline_out, 0, NUM_CYCLE_BUFFS*SecSize*(sizeof(agg_data_t)));
	 memset(onramp_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t)));
	 memset(onramp_queue_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t)));
	 memset(offramp_out, 0, NUM_CYCLE_BUFFS*NumOnRamp*(sizeof(agg_data_t)));
	 memset(mainline_out_f, 0, SecSize*(sizeof(agg_data_t)));
	 memset(onramp_out_f, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(offramp_out_f, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(onramp_queue_out_f, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(controller_mainline_data, 0, NUM_CONTROLLERS*(sizeof(agg_data_t)));  
	 memset(controller_onramp_data, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(controller_onramp_queue_detector_data, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(controller_offramp_data, 0, NumOnRamp*(sizeof(agg_data_t)));
	 memset(confidence, 0, NUM_CONTROLLERS*3*(sizeof(struct confidence)));



	get_local_name(hostname, MAXHOSTNAMELEN);

	if ( (pclt = db_list_init(argv[0], hostname, domain, xport,
		NULL, 0, NULL, 0)) == NULL) {
		printf("Database initialization error in %s.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Setup a timer for every 'interval' msec. */
	if ( ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL)) {
		printf("Unable to initialize wrfiles timer\n");
		exit(EXIT_FAILURE);
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		urms_ctl[0].lane_1_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_2_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_3_action = URMS_ACTION_SKIP;
		urms_ctl[0].lane_4_action = URMS_ACTION_SKIP;
		for(i=0; i<11; i++)
			db_clt_write(pclt, metering_controller_db_vars[i], sizeof(db_urms_t), &urms_ctl[0]); 
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

	for (i = 0; i < NUM_CONTROLLERS; i++)
	{   //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+87].id, db_controller_list[i+87].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+116].id, db_controller_list[i+116].size, &controller_data3[i]);
        }

//BEGIN MAIN FOR LOOP HERE
for(;;)																			// XYLu:  why here within main()   ???
{

	cycle_index++;
	cycle_index = cycle_index % NUM_CYCLE_BUFFS;
	for (i = 0; i < NUM_CONTROLLERS; i++)
	{  //See warning at top of file
		db_clt_read(pclt, db_controller_list[i].id, db_controller_list[i].size, &controller_data[i]);
		db_clt_read(pclt, db_controller_list[i+87].id, db_controller_list[i+87].size, &controller_data2[i]);
		db_clt_read(pclt, db_controller_list[i+116].id, db_controller_list[i+116].size, &controller_data3[i]);
	}

/*#################################################################################################################
###################################################################################################################*/

// Cheng-Ju's code here

	//    *************  RT data read in with bound limit
	
	get_current_timestamp(&ts); // get current time step
	print_timestamp(dbg_st_file_out, pts); // #1 print out current time step to file
 
	for(i=0;i<NUM_CONTROLLERS;i++)
	{
		printf("opt_crm: IP %s onramp1 passage volume %d demand vol %d offramp volume %d\n", controller_strings[i], controller_data[i].metered_lane_stat[0].passage_vol, 
			controller_data[i].metered_lane_stat[0].demand_vol, controller_data3[i].additional_det[0].volume);
		
		// min max function bound the data range and exclude nans.
		controller_mainline_data[i].agg_vol = Mind(12000.0, Maxd( 1.0, flow_aggregation_mainline(&controller_data[i], &confidence[i][0]) ) );
		controller_mainline_data[i].agg_occ = Mind(100.0, Maxd( 1.0, occupancy_aggregation_mainline(&controller_data[i], &confidence[i][0]) ) );
		 
		float_temp = hm_speed_aggregation_mainline(&controller_data[i], hm_speed_prev[i], &confidence[i][0]);
		if(float_temp < 0)
		{
			printf("Error %f in calculating harmonic speed for controller %s\n", float_temp, controller_strings[i]);
			float_temp = hm_speed_prev[i];
		}
		controller_mainline_data[i].agg_speed = Mind(150.0, Maxd( 1.0, float_temp) );
		 
		float_temp = mean_speed_aggregation_mainline(&controller_data[i], mean_speed_prev[i], &confidence[i][0]);
		if(float_temp < 0)
		{
			printf("Error %f in calculating mean speed for controller %s\n", float_temp, controller_strings[i]);
			float_temp = mean_speed_prev[i];
		}
		controller_mainline_data[i].agg_mean_speed = Mind(150.0, Maxd( 1.0, float_temp) );

		if(confidence[i][0].num_total_vals > 0)
			printf("Confidence for controller %s mainline %f total_vals %f good vals %f\n", controller_strings[i], (float)confidence[i][0].num_good_vals/confidence[i][0].num_total_vals, (float)confidence[i][0].num_total_vals, (float)confidence[i][0].num_good_vals);
	
		controller_mainline_data[i].agg_density = Mind(125.0,Maxd( 1.0,  density_aggregation_mainline(controller_mainline_data[i].agg_vol, controller_mainline_data[i].agg_speed, density_prev[i]) ) );
		
		hm_speed_prev[i] = controller_mainline_data[i].agg_speed;
		mean_speed_prev[i] = controller_mainline_data[i].agg_mean_speed;
		density_prev[i] = controller_mainline_data[i].agg_density;

		//fprintf(dbg_st_file_out,"C%d ", i); //controller index 
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_vol); //2
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_occ); //3
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_speed); //4
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_density); //5
		fprintf(dbg_st_file_out,"%f ", controller_mainline_data[i].agg_mean_speed);//6
		//fprintf(dbg_st_file_out,"\n");
	
		// assign off-ramp data to array
		//if(i==OffRampIndex[i]){
		controller_offramp_data[i].agg_vol =  Mind(8000.0, Maxd( 0, flow_aggregation_offramp(&controller_data3[i], &confidence[i][2]) ) );
		controller_offramp_data[i].agg_occ =  Mind(100.0, Maxd( 0, occupancy_aggregation_offramp(&controller_data3[i], &confidence[i][2]) ) );            
		controller_offramp_data[i].turning_ratio = turning_ratio_offramp(controller_offramp_data[i].agg_vol,controller_mainline_data[i-1].agg_vol);
		if(confidence[i][2].num_total_vals > 0)
			printf("Confidence for controller %s offramp %f total_vals %f good vals %f\n", controller_strings[i], (float)confidence[i][2].num_good_vals/confidence[i][2].num_total_vals, (float)confidence[i][2].num_total_vals, (float)confidence[i][2].num_good_vals);
		
		//fprintf(dbg_st_file_out,"FR%d ", i); //controller index 
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].agg_vol); //7
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].agg_occ); //8
		fprintf(dbg_st_file_out,"%f ", controller_offramp_data[i].turning_ratio);//9
		    //}

		//fprintf(dbg_st_file_out,"\n");
	
		// assign on-ramp data to array
		//if(i==OnRampIndex[i])
		controller_onramp_data[i].agg_vol = Mind(8000.0, Maxd( 0, flow_aggregation_onramp(&controller_data[i], &confidence[i][1]) ) );
		if(confidence[i][1].num_total_vals > 0)
			printf("Confidence for controller %s onramp flow %f total_vals %f good vals %f\n", 
				controller_strings[i], 
				(float)confidence[i][1].num_good_vals/confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_total_vals, (float)confidence[i][1].num_good_vals);
		controller_onramp_data[i].agg_occ = Mind(100.0, Maxd( 0, occupancy_aggregation_onramp(&controller_data[i], &controller_data2[i], &confidence[i][1]) ) );
		// data from on-ramp queue detector
		controller_onramp_queue_detector_data[i].agg_vol = Mind(8000.0, Maxd( 0, flow_aggregation_onramp_queue(&controller_data[i], &controller_data2[i], &confidence[i][1]) ));
		controller_onramp_queue_detector_data[i].agg_occ = Mind(100.0, Maxd( 0, occupancy_aggregation_onramp_queue(&controller_data[i], &controller_data2[i], &confidence[i][1]) ));

		if(confidence[i][1].num_total_vals > 0)
			printf("Confidence for controller %s onramp occupancy (queue) %f total_vals %f good vals %f\n", 
				controller_strings[i], 
				(float)confidence[i][1].num_good_vals/confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_total_vals, 
				(float)confidence[i][1].num_good_vals);
 
		//fprintf(dbg_st_file_out,"OR%d ", i); //controller index 
		fprintf(dbg_st_file_out,"%f ", controller_onramp_data[i].agg_vol); //10
		fprintf(dbg_st_file_out,"%f ", controller_onramp_data[i].agg_occ);//11
		fprintf(dbg_st_file_out,"%f ", controller_onramp_queue_detector_data[i].agg_vol); //12
		fprintf(dbg_st_file_out,"%f ", controller_onramp_queue_detector_data[i].agg_occ);//13
	//:	fflush(dbg_st_file_out);
	
	}
	fprintf(dbg_st_file_out,"\n");

////     RT data read END 
	
	   

////    *************** RT data aggregation over loops

	// This part aggregate data for each section; each section may have different number of loop detectors
	// Loop detector index for each mainline section

#undef ORIGINAL_IMPL
#ifdef ORIGINAL_IMPL
	int secCTidx [SecSize][4] =  {{7,  -1, -1, -1}, // controller in section 1                                    // Add up 5 sections
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


		const int secCTidx [SecSize][4] = {{0,  -1,  -1, -1}, // controller add up section 1            // this only affect mainline data, not onramp nor off-ramp
							 {0,  1,  -1, -1}, // controller add up section 2                          // the -1 could appear in any of the 4 locations
							 {1,  2,  -1, -1}, // controller add up section 3
							 {2,  3,  -1, -1}, // controller add up section 4
							 {4,  5,  -1, -1}, // controller add up section 5
							 {5,  6, -1, -1}, // controller in section 6                                // Add up 5 sections
	                     {7,  8, -1, -1}, // controller in section 7                                    // XYLu: may use both 7 &8; prevously: 8 only
	                     {8,  9, -1, -1}, // controller in section 8
	                     {10, 11, -1, -1}, // controller in section 9
	                     {11, 12, -1, -1}, // controller in section 10                       
	                     {13, 14, 15, 16}, // controller in section 11                                  // DBG only; Problem one for flow only, not for occ, speed and density
	                     {16, 17, -1, -1}, // controller in section 12 
							 {18, 19, -1, -1}, // controller in section 13 
	                     {19, 20, -1, -1}, // controller in section 14   
	                     {20, 23, -1, -1}, // controller in section 15
							 {23, 25, -1, -1}, // controller in section 16 
							 {25, 26, -1, -1}}; // controller in section 17 	
#endif

		const int secCTidx [SecSize][4] = {{0,  -1,  -1, -1}, //controller add up section 1 //SecSize=18
							 {0,  1,  -1, -1}, // controller add up section 2                  // this only affect mainline data, not onramp nor off-ramp
							 {2,  -1,  -1, -1}, // controller add up section 3                          // the -1 could appear in any of the 4 locations
							 {4,  -1,  -1, -1}, // controller add up section 4
							 {5,  6,  -1, -1}, // controller add up section 5
							 {6,  7,  -1, -1}, // controller add up section 6
							 {8,  9, -1, -1}, // controller in section 7                                // Add up 5 sections
	                     	{9,  10, -1, -1}, // controller in section 8                                    // XYLu: may use both 7 &8; prevously: 8 only
	                     	{10,  12, -1, -1}, // controller in section 9
	                     	{12,  13, -1, -1}, // controller in section 10
	                     	{15, 16, 17, -1}, // controller in section 11
	                     	{17, 18, -1, -1}, // controller in section 12                       
	                     	{19, 20, -1, -1}, // controller in section 13                                  // DBG only; Problem one for flow only, not for occ, speed and density
	                     	{20, 21, -1, -1}, // controller in section 14 
							{21, -1, -1, -1}, // controller in section 15 
	                     	{24, -1, -1, -1}, // controller in section 16   
	                     	{24, 26, -1, -1}, // controller in section 17
							{26, 28, -1, -1}}; // controller in section 18 
							 
                           // N. B.   7/21/19: data not available: 3, 14, 22, 23, 25

							 
		float temp_num_ct = 0.0; // number of controllers per section
		float temp_vol = 0.0;
		float temp_speed = 0.0;
		float temp_occ = 0.0;
		float temp_density = 0.0;
		float temp_mean_speed = 0.0;
		
		
		
//    *****************    Data aggregation for mainline data for each section over-distance or adjacnet loops
 	for(i=0;i<SecSize;i++)
 	{
		// this loop aggregates all controller data in each section
	
		for(j=0;j<4;j++)
		{
			if(secCTidx[i][j] > -1)
			{
				temp_vol += controller_mainline_data[secCTidx[i][j]].agg_vol;
				temp_speed += controller_mainline_data[secCTidx[i][j]].agg_speed; 
			   	temp_occ += controller_mainline_data[secCTidx[i][j]].agg_occ;
				temp_density += controller_mainline_data[secCTidx[i][j]].agg_density;
				temp_mean_speed += controller_mainline_data[secCTidx[i][j]].agg_mean_speed;
				temp_num_ct ++;
			}
		}		//j-loop END
		mainline_out[cycle_index][i].agg_vol = Mind(12000.0, Maxd(temp_vol/temp_num_ct,1));
		mainline_out[cycle_index][i].agg_speed = Mind(100.0, Maxd(temp_speed/temp_num_ct,1));
		mainline_out[cycle_index][i].agg_occ =  Mind(90.0, Maxd(temp_occ/temp_num_ct,1));
		mainline_out[cycle_index][i].agg_density = Mind(250.0, Maxd(temp_density/temp_num_ct,1));   

		// Initialize all temp variables
		temp_num_ct = 0.0; 
		temp_vol = 0.0;
		temp_speed = 0.0;
		temp_occ = 0.0;
		temp_density = 0.0;
		temp_mean_speed = 0.0;

	} 			// i-loop  END

                                         // JS: Print out: mainline_out[10].agg_vol
                                         
//This part aggregate onramp data for each section												// XYLu: this is RT Onramp data;   Add Up 5
#ifdef ORIGINAL_IMPL
	int onrampCTidx[NumOnRamp] = {8, 9, 11, 12, 16, 17, 19, 20, 22, 23, 25}; 

	static int onrampCTidx[NumOnRamp] = {0, 2, 3, 5, 6, 8, 9, 11, 12, 16, 17, 19, 20, 22, 23, 25}; 	// XYLu: Added up 5
#endif
	static int onrampCTidx[NumOnRamp] = {0, 1, 3, 4, 6, 7, 9, 10, 12, 13, 17, 18, 20, 21, 23, 24, 26}; 	// XYLu: Added up 6//NumOnRamp=17
	
	for(i=0;i<NumOnRamp;i++)
	{
		onramp_out[cycle_index][i].agg_vol = Mind(8000.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_vol,0));
		onramp_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_onramp_data[onrampCTidx[i]].agg_occ,0 ));
	onramp_queue_out[cycle_index][i].agg_vol = Mind(8000.0, Maxd( controller_onramp_queue_detector_data[onrampCTidx[i]].agg_vol ,0));
		onramp_queue_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_onramp_queue_detector_data[onrampCTidx[i]].agg_occ,0 ));
	}

	
//This part aggregate off-ramp data for each section <--- match number of off-ramp by number of off-ramp 	   // XYLu: this is RT Off-Ramp Data; Add up stream off-ramp
	//int offrampCTidx[NumOnRamp] = {8, -1, 10, -1, 16, 17, 19, 20, 21, 23, 25}; // 4 off-ramp is missing, total number of off-ramps is 9

#ifdef ORIGINAL_IMPL
	int offrampCTidx[NumOnRamp] = {8, -1, 9, -1, 16, 17, 19, 20, 22, 23, 26}; // 4 off-ramp is missing, total number of off-ramps is 9

	int offrampCTidx[NumOnRamp] = {-1, 2, -1, 5, -1, 8, -1, 10, -1, 16, 17, 19, 20, 21, 23, 25};    // XYLu, Added Upstream; Total 16; but only 11 off-ramps
#endif
	int offrampCTidx[NumOnRamp] = {-1, -1, 3, -1, 6, -1, 9, -1, 11, -1, -1, 17, 18, 20, 21, 22, 24};    // XYLu, Added Upstream; Total 16; but only 11 off-ramps
	
	for(i=0;i<NumOnRamp;i++)
	{ 
		if (offrampCTidx[i] != -1.0)
		{//<-- impute data here
			offramp_out[cycle_index][i].agg_vol = Mind(12000.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_vol,0));
			offramp_out[cycle_index][i].agg_occ = Mind(90.0, Maxd(controller_offramp_data[offrampCTidx[i]].agg_occ,0 ));
		}
		else
		{
			offramp_out[cycle_index][i].agg_vol = 0.0;
			offramp_out[cycle_index][i].agg_occ = 0.0;
		}
	} 

	
////    *************** RT data aggregation over loops END

	
	
/*
	// flow balance of mainline by using filtered data
	for(i=0;i<SecSize;i++){
		  if(mainline_out[cycle_index][i].agg_vol < 100.0){
			  if (i==0){
	          mainline_out[cycle_index][i].agg_vol = mainline_out[cycle_index][i+1].agg_vol+offramp_out[cycle_index][1].agg_vol;
			  }else if (i==11){
	          mainline_out[cycle_index][i].agg_vol = offramp_out[cycle_index][i].agg_vol-onramp_out[cycle_index][i-1].agg_vol;
			  }else{
	          mainline_out[cycle_index][i].agg_vol = mainline_out[cycle_index][i+1].agg_vol+offramp_out[cycle_index][i-1].agg_vol-onramp_out[cycle_index][i-1].agg_vol;
			  }
		  }
	  }
*/


////    *************************  Mainline data imputation

// replace bad flow data by upstream data
//if flow < 100 do upstream downstrean interpolation flow data


////    *************************  Mainline data imputation END

	
	
// average the historical data from data buffer                        // XYLu: the historical data is defined in look_up_table.h



// moving average filter for mainline RT data
	for(i=0; i<SecSize; i++)
	{
	  for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
	 	temp_ary_vol[j]= mainline_out[j][i].agg_vol;
		 temp_ary_speed[j]= mainline_out[j][i].agg_speed;
		 temp_ary_occ[j]= mainline_out[j][i].agg_occ;
		 temp_ary_density[j]= mainline_out[j][i].agg_density;
	  }
	   mainline_out_f[i].agg_vol = mean_array(temp_ary_vol,NUM_CYCLE_BUFFS);                // XYLu: mean_arraay(), defined in resource.c
	   mainline_out_f[i].agg_speed = mean_array(temp_ary_speed,NUM_CYCLE_BUFFS);
		mainline_out_f[i].agg_occ = mean_array(temp_ary_occ,NUM_CYCLE_BUFFS);
	   mainline_out_f[i].agg_density = mean_array(temp_ary_density,NUM_CYCLE_BUFFS);	   // JS: Print out: mainline_out_f[10].agg_vol
	}
	
// The followimg code has been aded by XYLu 2019, 05/13


	for(i=0; i<NumOnRamp; i++)
	{
		for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
	 	temp_ary_vol[j]= onramp_out[j][i].agg_vol;		 
		temp_ary_occ[j]= onramp_out[j][i].agg_occ;		 
	  }
	   onramp_out_f[i].agg_vol = mean_array(temp_ary_vol,NUM_CYCLE_BUFFS);                // XYLu: moving average over-time; mean_arraay(), defined in resource.c	   
		onramp_out_f[i].agg_occ = mean_array(temp_ary_occ,NUM_CYCLE_BUFFS);	  
	}
	
	for(i=0; i<NumOnRamp; i++)//NumOnRamp=17
	{
		if (offrampCTidx[i] > 0)
		{
			for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  		{
	 	 		temp_ary_vol[j]= offramp_out[j][i].agg_vol;		 
		 		temp_ary_occ[j]= offramp_out[j][i].agg_occ;		 
	  		}
	    	offramp_out_f[i].agg_vol = mean_array(temp_ary_vol,NUM_CYCLE_BUFFS);                // XYLu: moving average over-time, mean_arraay(), defined in resource.c	   
			offramp_out_f[i].agg_occ = mean_array(temp_ary_occ,NUM_CYCLE_BUFFS);	 
		} 
 	}

 
// moving average filter for on-ramp off-ramp                         // XYLu:   Here the historical data is used from this point;   defined in look_up_table.h
	for(i=0; i<NumOnRamp; i++)
	{
	  
	  //current_most_upstream_flow = mainline_out_f[1].agg_vol;

#ifdef ORIGINAL_IMPL
	// Use historical data only
	ML_flow_ratio = ratio_ML_HIS_FLOW(current_most_upstream_flow, MOST_UPSTREAM_MAINLINE_FLOW_DATA, pts);								  
	onramp_out_f[i].agg_vol = Mind(1500.0*N_OnRamp_Ln[i], Maxd(interp_OR_HIS_FLOW(i+1+5, OR_flow_prev[i] , OR_HIS_FLOW_DATA, pts),50)); // interpolate missing value from table    
	onramp_out_f[i].agg_occ = Mind(90.0, Maxd(interp_OR_HIS_OCC(i+1+5, OR_occupancy_prev[i], OR_HIS_OCC_DATA, pts),5)); // interpolate missing value from table
	offramp_out_f[i].agg_vol = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(i+1,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),50)); // interpolate missing value from table
	offramp_out_f[i].agg_occ = Mind(90.0, Maxd(interp_FR_HIS_OCC(i+1, FR_occupancy_prev[i], FR_HIS_OCC_DATA, pts),5)); // interpolate missing value from table 
#endif

	if (NumOnRamp == 0)
		OR_vol_tmp = onramp_out_f[i].agg_vol; // interpolate missing value from table  
	else
		OR_vol_tmp = Mind(1000.0*N_OnRamp_Ln[i], Maxd(interp_OR_HIS_FLOW(i, OR_flow_prev[i] , OR_HIS_FLOW_DATA, pts),50)); // interpolate missing value from table  
	  
	//OR_occ_tmp = Mind(90.0, Maxd(interp_OR_HIS_OCC(i+1, OR_occupancy_prev[i], OR_HIS_OCC_DATA, pts),5)); // interpolate missing value from table
	onramp_out_f[i].agg_vol = 1.1*Maxd(OR_vol_tmp, onramp_out_f[i].agg_vol);
	onramp_out_f[i].agg_vol = Mind(1500.0*N_OnRamp_Ln[i], Maxd(onramp_out_f[i].agg_vol,50)); // interpolate missing value from table  
	onramp_out_f[i].agg_occ = Mind(95.0, Maxd(onramp_out_f[i].agg_occ,5)); // interpolate missing value from table
	//onramp_out_f[i].agg_occ = 1.1*Maxd(OR_occ_tmp, onramp_out_f[i].agg_occ);
	
	if (offrampCTidx[i] > 0)                                 //   XYLu, Added on 2019 05 18; to avoid over bound
	{                             // int offrampCTidx[NumOnRamp] = {-1, -1, 3, -1, 6, -1, 9, -1, 11, -1, -1, 17, 18, 20, 21, 22, 24};    
		if (offrampCTidx[i] == 3) //                         
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(1,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 6)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(2,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 9)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(3,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 11)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(4,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 17)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(5,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 18)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(6,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 20)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(7,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 21)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(8,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 22)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(9,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		if (offrampCTidx[i] == 24)
			FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(10,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
		//if (offrampCTidx[i] == 25)
		//	FR_vol_tmp = Mind(2000.0*N_OffRamp_Ln[i], Maxd(interp_FR_HIS_FLOW(11,  FR_flow_prev[i] ,FR_HIS_FLOW_DATA, pts),20)); // interpolate missing value from table
			
		offramp_out_f[i].agg_vol = Maxd(FR_vol_tmp, offramp_out_f[i].agg_vol); 			
		//offramp_out_f[i].agg_vol = Mind(1800.0*N_OffRamp_Ln[i], Maxd(offramp_out_f[i].agg_vol,50)); // interpolate missing value from table
		offramp_out_f[i].agg_occ = Mind(95.0, Maxd(offramp_out_f[i].agg_occ,5)); // interpolate missing value from table 		
	} 


	// changed to use of RT data
    //ML_flow_ratio = ratio_ML_HIS_FLOW(current_most_upstream_flow, MOST_UPSTREAM_MAINLINE_FLOW_DATA, pts);		//  XYLu, it is not used.						  
	
	
	
	  
	  for(j=0; j<NUM_CYCLE_BUFFS; j++)
	  {
	     //temp_ary_OR_vol[j] = onramp_out[j][i].agg_vol; 
		 //temp_ary_OR_occ[j] = onramp_out[j][i].agg_occ;   
		 temp_ary_OR_queue_detector_vol[j] = onramp_queue_out[j][i].agg_vol;
		 temp_ary_OR_queue_detector_occ[j] = onramp_queue_out[j][i].agg_occ;
		 //temp_ary_FR_vol[j] = offramp_out[j][i].agg_vol;   
		 //temp_ary_FR_occ[j] = offramp_out[j][i].agg_occ;	
	  }

	  onramp_queue_out_f[i].agg_vol = mean_array(temp_ary_OR_queue_detector_vol,NUM_CYCLE_BUFFS); 
	  onramp_queue_out_f[i].agg_occ = mean_array(temp_ary_OR_queue_detector_occ,NUM_CYCLE_BUFFS);

	  // register of on-ramp and off-ramp data in previous time step
	  OR_flow_prev[i] = onramp_out_f[i].agg_vol;   
	  OR_occupancy_prev[i] = onramp_out_f[i].agg_occ;
	  FR_flow_prev[i] =  offramp_out_f[i].agg_vol;
	  FR_occupancy_prev[i] = offramp_out_f[i].agg_occ;
 
	}

//#ifdef ORIGINAL_IMP
// Butterworth filter for mainline
	for(i=0; i<SecSize; i++)
	{
	   mainline_out_f[i].agg_vol = butt_2_ML_flow(mainline_out_f[i].agg_vol, i);
	   mainline_out_f[i].agg_speed = butt_2_ML_speed(mainline_out_f[i].agg_speed, i);
	   mainline_out_f[i].agg_occ = butt_2_ML_occupancy(mainline_out_f[i].agg_occ, i);
	   mainline_out_f[i].agg_density = butt_2_ML_density(mainline_out_f[i].agg_density, i);
	}
//#endif

/*###################################################################################################################
###################################################################################################################*/

//// *********************   Here the mainline data get into the data buffer used later in MPC                                            //   XYLu

		print_timestamp(st_file_out, pts);//1
		for(i=0;i<SecSize;i++)
		{			    			    
			    detection_s[i]->data[Np-1].flow=Mind(12000.0, Maxd(mainline_out_f[i].agg_vol, 200.0));
			    detection_s[i]->data[Np-1].speed=Mind(100.0, Maxd(mainline_out_f[i].agg_speed, 5.0));
			    detection_s[i]->data[Np-1].occupancy=Mind(100.0, Maxd((mainline_out_f[i].agg_occ), 5.0));
			    detection_s[i]->data[Np-1].density=Mind(1200.0, Maxd(mainline_out_f[i].agg_density, 10.0));
			    
	        //fprintf(st_file_out,"Sec %d ", i); 
			    fprintf(st_file_out,"%f ", mainline_out_f[i].agg_vol);		//2,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,66,70
	        	fprintf(st_file_out,"%f ", mainline_out_f[i].agg_speed);		//3,7,11,15,19,23,27,31,35,39,43,47,51,55,59,63,67,71
				fprintf(st_file_out,"%f ", mainline_out_f[i].agg_occ);		//4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72
				fprintf(st_file_out,"%f ", mainline_out_f[i].agg_density);	//5,9,13,17,21,25,29,33,37,41,45,49,53,57,61,65,69,73
		} 
	
		//fprintf(st_file_out,"\n");

		for(i=0;i<NumOnRamp;i++)                                                 // XYLu: rand()  is used only for simulation; should be removed
		{				
				
				detection_onramp[i]->data[Np-1].flow=Mind(6000.0, Maxd(onramp_out_f[i].agg_vol, 100.0));
				detection_onramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd((onramp_out_f[i].agg_occ), 5.0)); 
				detection_offramp[i]->data[Np-1].flow=0.0; //Mind(6000.0, Maxd(offramp_out_f[i].agg_vol, 100.0));   // no need for off-ramp flow.
				detection_offramp[i]->data[Np-1].occupancy=Mind(100.0, Maxd((offramp_out_f[i].agg_occ), 5.0)); 	
				
				//fprintf(st_file_out,"OR %d ", i);//
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_vol); 		//74,80,86,92,98,104,110,116,122,128,134,140,146,152,158,164,170
				fprintf(st_file_out,"%f ", onramp_out_f[i].agg_occ); 		//75,81,87,93,99,105,111,117,123,129,135,141,147,153,159,165,171
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_vol); 	//76,82,88,94,100,106,112,118,124,130,136,142,148,154,160,166,172
				fprintf(st_file_out,"%f ", offramp_out_f[i].agg_occ); 	//77,83,89,95,101,107,113,119,125,131,137,143,149,155,161,167,173
				fprintf(st_file_out,"%f ", onramp_queue_out_f[i].agg_vol); 	//78,84,90,96,102,108,114,120,126,132,138,144,150,156,162,168,174
				fprintf(st_file_out,"%f ", onramp_queue_out_f[i].agg_occ); 	//79,85,91,97,103,109,115,121,127,133,139,145,151,157,163,169,175
				//fprintf(st_file_out,"\n");//
					
				max_occ_2_dwn[i]=detection_s[i]->data[Np-1].occupancy;	
				max_occ_all_dwn[i]=detection_s[i]->data[Np-1].occupancy;																//XYLu:  We may not need the following anymore  5/21/2019
				
				/*for (j=i+1;j<=i+2;j++)
				{					
					if (j < NumOnRamp+1)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}*/
				if (i < NumOnRamp -1)
				{
					for (j=i+1;j<i+2;j++)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}
				else if (i < NumOnRamp)
				{
					for (j=i+1;j<i+1;j++)
						max_occ_2_dwn[i]=Maxd(max_occ_2_dwn[i], (detection_s[j]->data[Np-1].occupancy));
				}
				else
					max_occ_2_dwn[i]=detection_s[NumOnRamp]->data[Np-1].occupancy;//NumOnRamp=17
				
				for (j=i+1;j<NumOnRamp+1;j++)				//NumOnRamp=17
					max_occ_all_dwn[i]=Maxd(max_occ_all_dwn[i], (detection_s[j]->data[Np-1].occupancy));			
			
		}
		
		fprintf(st_file_out,"\n");
		
		
		
		/*************************************************
		   
		      XYLu code start from here
		
		**************************************************/

		
		det_data_4_contr(time);	
		get_meas(time);		
		update_q_R();
		opt_metering();
		
		//fprintf(cal_opt_f,"%lf ", time);   // Output calculated Opt RM rt; not really print time
		for (i=0;i<NumOnRamp;i++)
		{				
			total_rt[i]=opt_r[i][0];							
			fprintf(cal_opt_f,"%10.2f ", total_rt[i]);				
		}
		fprintf(cal_opt_f,"\n");
		

		
/******************************************************************
		             Set CRM Rate
******************************************************************/			


for (k=0;k<NumOnRamp;k++)
	{
		if (N_OnRamp_Ln[k] == 1)
		{
			ln_CRM_rt[k][0]=total_rt[k];
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
		}
		if (N_OnRamp_Ln[k] == 2)
		{
			if (k != 11)			
				ln_CRM_rt[k][0]=total_rt[k]-Onramp_HOV_Util*max_RM_rt;  
			else		// No HOV lane this case
				ln_CRM_rt[k][0]=0.5*total_rt[k];  	
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
			
		}
		if (N_OnRamp_Ln[k] == 3)
		{				
			ln_CRM_rt[k][0]=0.5*(total_rt[k]-Onramp_HOV_Util*max_RM_rt);  
			ln_CRM_rt[k][1]=ln_CRM_rt[k][0];                                   //0.5*(total_rt[k]-Onramp_HOV_Util*max_RM_rt);  
			if (ln_CRM_rt[k][0] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][0] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][0] = min_Ln_RM_rt[k];
			if (ln_CRM_rt[k][1] > max_Ln_RM_rt[k])
				ln_CRM_rt[k][1] = max_Ln_RM_rt[k];
			if (ln_CRM_rt[k][1] < min_Ln_RM_rt[k])
				ln_CRM_rt[k][1] = min_Ln_RM_rt[k];
		}	
	

			if (max_occ_2_dwn[k] < Occ_Cr)
				ln_CRM_rt[k][0]=0.85*max_Ln_RM_rt[k];
			if (max_occ_all_dwn[k] < Occ_Cr)
				ln_CRM_rt[k][0]=max_Ln_RM_rt[k];
				
			if (onramp_out_f[k].agg_occ > OnRamp_Occ_Cr)				
				(ln_CRM_rt[k][0])=(ln_CRM_rt[k][0])*(1.0+ 0.25*(onramp_out_f[k].agg_occ - OnRamp_Occ_Cr)/(100.0-OnRamp_Occ_Cr));
			if (ln_CRM_rt[k][0]>max_Ln_RM_rt[k])
				ln_CRM_rt[k][0]=max_Ln_RM_rt[k];

				
		fprintf(Ln_RM_rt_f,"%10.2f ",ln_CRM_rt[k][0]);			
	}
	fprintf(Ln_RM_rt_f,"\n");	
		
/******************************************************************
		             Set LRRM Rate
******************************************************************/		

   
for(k=0;k<NumOnRamp;k++)
	{	
		for (j=0;j<N_interv-1;j++)
		{
				if (o[k]<=SR99_RM_occ_tbl[0][k])				
					total_LRRM_rt[k]=SR99_RM_rate_tbl[0][k];														
				else if (o[k]>SR99_RM_occ_tbl[j][k] && o[k]<=SR99_RM_occ_tbl[j+1][k])					
					total_LRRM_rt[k]=SR99_RM_rate_tbl[j-1][k];																
				else				
					total_LRRM_rt[k]=SR99_RM_rate_tbl[j+1][k];							
		} // for j-loop
										
		if (total_LRRM_rt[k]>max_RM_rt)
			total_LRRM_rt[k]=max_RM_rt;	
		if (total_LRRM_rt[k]<min_RM_rt)
			total_LRRM_rt[k]=min_RM_rt;	
	
		ln_LRRM_rt[k][0]=total_LRRM_rt[k];

		if (ln_LRRM_rt[k][0] > max_Ln_RM_rt[k])
			ln_LRRM_rt[k][0] = max_Ln_RM_rt[k];
		if (ln_LRRM_rt[k][0] < min_Ln_RM_rt[k])
			ln_LRRM_rt[k][0] = min_Ln_RM_rt[k];
		fprintf(local_rm_f,"%10.2f\t", (float)(total_LRRM_rt[k]));

	}
	fprintf(local_rm_f,"\n");
		

//FOR TEST PURPOSES ONLY###############################
//counter++;
//FOR TEST PURPOSES ONLY###############################
		if( (pts->hour >= 15) && (pts->hour < 18) )
			min_index = 2;
		else
			min_index = 0;

		for (i=min_index;i<NumOnRamp;i++)               // Lane-wise RM rate
		{
			urms_ctl[i].lane_1_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[i].lane_2_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[i].lane_3_action = URMS_ACTION_FIXED_RATE;
			urms_ctl[i].lane_4_action = URMS_ACTION_SKIP;

			urms_ctl[i].lane_1_release_rate = ln_CRM_rt[i][0];
			urms_ctl[i].lane_2_release_rate = ln_CRM_rt[i][0];
			urms_ctl[i].lane_3_release_rate = ln_CRM_rt[i][0];
			urms_ctl[i].lane_4_release_rate = ln_CRM_rt[i][0];
			urms_ctl[i].no_control = 0;
//FOR TEST PURPOSES ONLY###############################
//			urms_ctl[i].lane_1_release_rate = 450+i;
//			urms_ctl[i].lane_2_release_rate = 400+i;
//			urms_ctl[i].lane_3_release_rate = 350+i;
//			urms_ctl[i].lane_4_release_rate = 300+i;
//			urms_ctl[i].lane_1_action = URMS_ACTION_SKIP;
//			urms_ctl[i].lane_2_action = URMS_ACTION_SKIP;
//			urms_ctl[i].lane_3_action = URMS_ACTION_SKIP;
//			urms_ctl[i].lane_4_action = URMS_ACTION_SKIP;
//			urms_ctl[i].lane_1_plan = 1;
//			urms_ctl[i].lane_2_plan = 1;
//			urms_ctl[i].lane_3_plan = 1;
//			urms_ctl[i].lane_4_plan = 1;
			printf("Controller db var %d lane 1 rate %d action %d plan %d lane 2 rate %d action %d plan %d lane 3 rate %d action %d plan %d lane 4 rate %d action %d plan %d\n",
				metering_controller_db_vars[i],
				urms_ctl[i].lane_1_release_rate,
				urms_ctl[i].lane_1_action,
				urms_ctl[i].lane_1_plan,
				urms_ctl[i].lane_2_release_rate,
				urms_ctl[i].lane_2_action,
				urms_ctl[i].lane_2_plan,
				urms_ctl[i].lane_3_release_rate,
				urms_ctl[i].lane_3_action,
				urms_ctl[i].lane_3_plan,
				urms_ctl[i].lane_4_release_rate,
				urms_ctl[i].lane_4_action,
				urms_ctl[i].lane_4_plan
			);
			db_clt_write(pclt, metering_controller_db_vars[i], sizeof(db_urms_t), &urms_ctl[i]); 
//FOR TEST PURPOSES ONLY###############################


		}                                                             // XYLu: the end of for (;;)
		//cycle_index++;
		//cycle_index %= NUM_CYCLE_BUFFS;
	
			TIMER_WAIT(ptimer);	
	} 
	
	Finish_sim_data_io();
	      	
	return 0;
}                                               // XYLu:    END of main()



int Init_sim_data_io()
{	
	st_file=fopen("In_Data/state_var.txt","r");	
	
	dbg_f=fopen("Out_Data/dbg_file.txt","w");
	
	local_rm_f=fopen("Out_Data/local_rm_rate.txt","w");
	
	cal_opt_f=fopen("Out_Data/cal_opt_RT_rt.txt","w");
	
	st_file_out=fopen("Out_Data/state_var_out.txt","w");	
	dbg_st_file_out =fopen("Out_Data/dbg_state_var_out.txt","w");	
	
	Ln_RM_rt_f=fopen("Out_Data/lanewise_rt.txt","w");	
	
	pp=fopen("Out_Data/coeff.txt","w");
		
	
	return 1;
}

/***************************************************

	Subrutines of XYLu needs updating  from here;   09_15_16
	
****************************************************/

int Init()  // A major function; Called by AAPI.cxx: the top function for intialization of the whole system
{
	int i; 
	
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
	memset(u,0,SecSize*(sizeof(float))); 
	memset(v,0,SecSize*(sizeof(float))); 
	memset(q,0,SecSize*(sizeof(float))); 
	memset(o,0,SecSize*(sizeof(float))); 
	memset(qc,0,SecSize*(sizeof(float)));
	memset(queue,0,SecSize*(sizeof(float))); 
	memset(s,0,NumOnRamp*(sizeof(float)));
	memset(R,0,NumOnRamp*(sizeof(float)));
	memset(dmd,0,NumOnRamp*(sizeof(float)));
	memset(Q_o,0,NumOnRamp*(sizeof(float)));
	memset(Q_min,0,NumOnRamp*(sizeof(float)));
	memset(ss,0,Np*NumOnRamp*(sizeof(float)));
	memset(dd,0,Np*NumOnRamp*(sizeof(float)));
	memset(pre_w,0,NumOnRamp*(sizeof(float)));
	memset(max_occ_all_dwn,0,NumOnRamp*(sizeof(float)));
	memset(max_occ_2_dwn,0,NumOnRamp*(sizeof(float)));
	memset(q_main,0,SecSize*(sizeof(float)));
	memset(u2,0,SecSize*(sizeof(float))); 
	memset(pre_rho,0,SecSize*(sizeof(float)));
	memset(opt_r,0,SecSize*(sizeof(float)));
	memset(up_rho,0,Np*sizeof(float));
	memset(dyna_min_r, 0, NumOnRamp*(sizeof(float))); 
	memset(dyna_max_r, 0, NumOnRamp*(sizeof(float))); 
	memset(Ramp_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(RM_occ, 0, NumOnRamp*(sizeof(float))); 
	memset(ln_CRM_rt, 0, NumOnRamp*max_onramp_ln*(sizeof(float))); 
	memset(ln_LRRM_rt, 0, NumOnRamp*max_onramp_ln*(sizeof(float))); 
	memset(release_cycle, 0, NumOnRamp*max_onramp_ln*(sizeof(int))); 
	memset(total_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(total_LRRM_rt, 0, NumOnRamp*(sizeof(float))); 
	memset(detection_s_0,0,SecSize*sizeof(detData));
	memset(detection_onramp_0,0,NumOnRamp*sizeof(detData));
	memset(detection_offramp_0,0,NumOnRamp*sizeof(detData));
	memset(a_w,0,NumOnRamp*sizeof(float));
	
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
		dyna_min_r[i]=300.0*N_OnRamp_Ln[i];
		dyna_max_r[i]=dyna_min_r[i]+rm_band;	
		Ramp_rt[i]=max_RM_rt;   // only used in Set_Coord_ALINEA
		//Q_o[i]=max_RM_rt*N_OnRamp_Ln[i];   // changed on 09_15_16
		
		if(N_OnRamp_Ln[i] == 1)
			Q_o[i]=max_RM_rt;
		else if (N_OnRamp_Ln[i] == 2)
			Q_o[i]=max_RM_rt*(1.0+Onramp_HOV_Util);
		else
			Q_o[i]=max_RM_rt*(N_OnRamp_Ln[i]-1+Onramp_HOV_Util);
		Q_min[i]=min_RM_rt*N_OnRamp_Ln[i];		
		a_w[i]=6.5;
		onrampL[i]=onrampL[i]/1609.0;		
	}	
	
	for(i=0; i<SecSize;i++)
		L[i]=L[i]/1609.0;
	
	u[NumOnRamp]=104;   // Dim Ok	
	ControlOn=0;
	StateOn=0;
	StateOff=0;
	
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
printf("det_data_4_contr: NumOnRamp %d Np %d\n",NumOnRamp, Np);                                   
	
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


/*******************************************
	       OptSolver
********************************************/

int set_coef(float c[MP][NP],float Qm)
{
	

	float w_const[SecSize][Np]={{0.0}}; 					// Used to construct b_u; 
	float p_const[SecSize][Np]={{0.0}};					// Used to construct b_u; 	
	float f[(NumOnRamp)*Np]={0.0};
	
	float b_u[M1]={0.0};
	float b_l[M2]={0.0};   // upper bound of r

	int m,i,j;
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
		c[0][j+1]=f[j];			
	
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
	
#ifdef COEFF_DATA
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
#endif
	
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
//printf("opt_metering6: M %d N %d M1 %d M2 %d M3 %d icase %d izrov %d iposv %d\n", M,N,M1,M2,M3,icase,(int)izrov,(int)iposv);

	
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
//					printf("i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i][0]);	
				}	
			for(i=NumOnRamp+1;i<=2*NumOnRamp;i++)	
				{
					opt_r[i-1-NumOnRamp][1]=a[iposv[i+1]][1];								
					//fprintf(dbg_f,"i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i-1-NumOnRamp][1]);					
//					printf("i=%d, icase=%d iposv=%d, r=%10.2f\n",i,icase, iposv[i],opt_r[i-1-NumOnRamp][1]);					
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
	fflush(local_rm_f);
	fclose(local_rm_f);	
	fflush(cal_opt_f);
	fclose(cal_opt_f);
	fflush(Ln_RM_rt_f);
	fclose(Ln_RM_rt_f);
	//fflush(sec_outfile);
	//fclose(sec_outfile);
	if (use_CRM == 2)
	{
		fflush(pp);
		fclose(pp);
	}
	fclose(st_file);
	fflush(st_file_out);
	fclose(st_file_out);


	return 1;
}


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
