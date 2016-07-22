#include <db_include.h>
#include "resource.h"
#include <urms.h>
#include <math.h>
#define FLOAT_ERROR -1.0 
// units
// flow is vehicle per hour
// speed is miles per hour
// occupancy is 0 to 100

//
const char *controller_strings[] = {
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

//float flow_aggregation_3_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3);

float maxd(float a,float b){
	if(a>=b){
		return a;
	}
	else{
		return b;
	}
}

float mind(float a,float b){
	if(a<=b){
		return a;
	}
	else{
		return b;
	}
}

float sum_array(float a[], int num_elements){
   int i;
   float sum=0.0;
   for (i=0; i<num_elements; i++)
   {
	 sum = sum + a[i];
   }
   return(sum);
}

float mean_array(float a[], int num_elements){   
	float temp = 0.0;
	float mean = 0.0;
    temp = sum_array(a , num_elements);
	temp /= (float)num_elements;
    return mean = temp;
}

float var_array(float a[], int num_elements){
	float sum=0.0;
	float mean=0.0;
	float var=0.0;
	int i;

    mean = mean_array(a , num_elements);
	  
    for(i=0;i<num_elements;i++)
	{
		sum=sum+(a[i]-mean)*(a[i]-mean);
		var=sum/(float)num_elements;
	}
	return var;
}
/* Function to calculate factorial */
long int factorial(int x){
    int i,f=1;
    for (i=2 ; i<=x ; i++)
        f = f * i;
    return (f);
}

long int nCr(int n, int r){   
    int ncr=0;
	int npr=0;
    npr = factorial(n)/factorial(n-r);
    ncr = npr / factorial(r);
	return ncr;
}

// Def minumum lane flow 
float flow_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence){
        int i;
        int j = 0;
        float flow = 0.0;
	    float mean_flow = 0.0;
	    float var_flow = 0.0;
	    int num_lane = controller_data->num_main;
	    float flow_temp [num_lane];

	confidence->num_total_vals = num_lane;
	confidence->num_good_vals = num_lane;

    // this loop get data from data base
	if( (controller_data->num_main > 0) && (controller_data->num_main <= 8) ) {
	    for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){ // if the controller report the flow data is correct, then check the data is in the range or not
			if((float)controller_data->mainline_stat[i].lead_vol >= 0 && (float)controller_data->mainline_stat[i].lead_vol <= 2000){ // if flow is in the range
			    flow_temp[j]=(float)controller_data->mainline_stat[i].lead_vol;  
			    j++;
			}else{  // replce the flow measurement if it is not in the range
				confidence->num_good_vals--;
			}
		}else if(controller_data->mainline_stat[i].trail_stat == 2){
			if((float)controller_data->mainline_stat[i].trail_vol >= 0 && (float)controller_data->mainline_stat[i].trail_vol <= 2000){
			     flow_temp[j]=(float)controller_data->mainline_stat[i].trail_vol;  
			     j++;
			}else{
                 confidence->num_good_vals--;
			}
		}else{
			confidence->num_good_vals--;
		}
	    }
	}
	else
		return FLOAT_ERROR; // <--handle the float error here

	mean_flow = mean_array(flow_temp, confidence->num_good_vals);
	var_flow = var_array(flow_temp, confidence->num_good_vals);

	// this loop replace data with large variance
	for(i=0 ; i < confidence->num_good_vals; i++) {
	    if (abs(flow_temp[i]-mean_flow)>5*var_flow)
            flow_temp[i] = mean_flow;
	}
	
	// average the cleaned data
	flow = mean_array(flow_temp, confidence->num_good_vals);
    
	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
	    flow = flow * 120; // convert 30 second data into hour data
	}
	printf("ML-flow_agg %4.2f num_main %d\n", flow, controller_data->num_main);
	return mind(12000.0, flow);
}

float flow_aggregation_onramp(db_urms_status_t *controller_data, struct confidence *confidence){
        int i;
		int j = 0;
        float flow = 0;
		float mean_flow = 0.0;
		float var_flow = 0.0;
        int num_lane = controller_data->num_meter;
		float flow_temp [num_lane];
    // this loop get data from data base

	if( (controller_data->num_meter > 0) && (controller_data->num_meter <= 4) ) {
	    for(i=0 ; i< controller_data->num_meter;i++) {
		if(controller_data->metered_lane_stat[i].passage_stat == 2){ 
			if((float)controller_data->metered_lane_stat[i].passage_vol>= 0 && (float)controller_data->metered_lane_stat[i].passage_vol <= 2000){ // if flow is in the range
			    flow_temp[j]=(float)controller_data->metered_lane_stat[i].passage_vol;
				j++;
			}else{  // replce the flow measurement if it is not in the range
                confidence->num_good_vals--;
			}
		}else if(controller_data->metered_lane_stat[i].demand_stat == 2){
			if((float)controller_data->metered_lane_stat[i].demand_vol>= 0 && (float)controller_data->metered_lane_stat[i].demand_vol <= 2000){ 
			    flow_temp[j]=(float)controller_data->metered_lane_stat[i].demand_vol;
				j++;
			}else{  // replce the flow measurement if it is not in the range
                confidence->num_good_vals--;
			}
		}else{
			confidence->num_good_vals--;
		}
	    }
	}
	else
		return FLOAT_ERROR;

	mean_flow = mean_array(flow_temp, confidence->num_good_vals);
    var_flow = var_array(flow_temp, confidence->num_good_vals);

	// this loop replace data with large variance
    for(i=0 ; i <confidence->num_good_vals; i++) {
	    if (abs(flow_temp[i]-mean_flow)>5*var_flow)
            flow_temp[i] = mean_flow;
	}
    
	// average the cleaned data
	flow = mean_array(flow_temp, confidence->num_good_vals);

	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
	    flow = flow*120;
	}
	printf("OR-flow_agg %4.2f num_meter %d\n",	flow, controller_data->num_meter);
return  mind(12000.0, flow); 
}

float flow_aggregation_offramp(db_urms_status3_t *controller_data, struct confidence *confidence){
        int i;
		int j = 0;
        float flow = 0.0;
		float mean_flow = 0.0;
		float var_flow = 0.0;
		int num_lane = 1;
        num_lane = controller_data->num_addl_det;
		float flow_temp [num_lane];

	if( (controller_data->num_addl_det > 0) && (controller_data->num_addl_det <= 16) ) {
	    for(i=0 ; i< controller_data->num_addl_det; i++){  
            if(controller_data->additional_det[i].stat == 2){ // if the controller report the flow data is correct, then check the data is in the range or not
			    if((float)controller_data->additional_det[i].volume>= 0 && (float)controller_data->additional_det[i].volume <= 2000){ // if flow is in the range
			        flow_temp[j]=(float)controller_data->additional_det[i].volume;
					j++;
				}else{  // replce the flow measurement if it is not in the range
					confidence->num_good_vals--;
				}
			}else{
				confidence->num_good_vals--;
		    }
	    }
	}
	else
		return FLOAT_ERROR;

	mean_flow = mean_array(flow_temp, confidence->num_good_vals);
    var_flow = var_array(flow_temp, confidence->num_good_vals);

	// this loop replace data with large variance
    for(i=0 ; i < confidence->num_good_vals; i++) {
	    if (abs(flow_temp[i]-mean_flow)>5*var_flow)
            flow_temp[i] = mean_flow;
	}

	// average the cleaned data
	flow = mean_array(flow_temp, confidence->num_good_vals);
    
	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
		flow = flow*120; 
	}
	printf("FR-flow_agg %4.2f num_addl_det %d\n", flow, controller_data->num_addl_det );
	return mind(12000.0, flow);
}

float occupancy_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence){
	int i;
    int j = 0;
	float lead_occ = 0.0;
    float trail_occ = 0.0;
	float occupancy = 0.0;
	float mean_occ = 0.0;
	float var_occ = 0.0;
    int num_lane = controller_data->num_main;
    float occ_temp [num_lane];

	confidence->num_total_vals = num_lane;
	confidence->num_good_vals = num_lane;

	if( (controller_data->num_main > 0) && (controller_data->num_main <= 8) ) {
	    for(i=0 ; i < controller_data->num_main; i++) {
			lead_occ = 0.1 * ( ((controller_data->mainline_stat[i].lead_occ_msb << 8) & 0xFF00) + ((controller_data->mainline_stat[i].lead_occ_lsb) & 0xFF) );
            trail_occ = 0.1 * ( ((controller_data->mainline_stat[i].trail_occ_msb << 8) & 0xFF00) + ((controller_data->mainline_stat[i].trail_occ_lsb) & 0xFF) );
		if(controller_data->mainline_stat[i].lead_stat == 2){
			if(lead_occ>=0 && lead_occ <=100){
			    occ_temp[j] = lead_occ;
			    j++;
			}else {
				confidence->num_good_vals--;
			}
		}else if(controller_data->mainline_stat[i].trail_stat == 2){
		    if(trail_occ>=0 && trail_occ <=100){
			    occ_temp[j] = trail_occ;
			    j++;
			}else {
				confidence->num_good_vals--;
			}
		}else{
		    confidence->num_good_vals--;
		}
	    }
	}
	else
		return FLOAT_ERROR;

	mean_occ = mean_array(occ_temp, confidence->num_good_vals);
	var_occ = var_array(occ_temp, confidence->num_good_vals);

	// this loop replace data with large variance
	for(i=0 ; i < confidence->num_good_vals; i++) {
	    if (abs(occ_temp[i]-mean_occ)>5*var_occ)
            occ_temp[i] = mean_occ;
	}
	
	// average the cleaned data
	occupancy = mean_array(occ_temp, confidence->num_good_vals);
    
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_main %d\n", occupancy, controller_data->num_main);
	return  mind(100.0, occupancy);
}

float occupancy_aggregation_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence){ 
	int i;
	int j;
	float occupancy = 0;

	if( (controller_data->num_meter > 0) && (controller_data->num_meter <= 4) ) {
	    for(i=0 ; i < controller_data->num_meter; i++) {
	   	for(j=0 ; j < MAX_QUEUE_LOOPS; j++) { 
			if(controller_data2->queue_stat[i][j].stat == 2){
			occupancy += 0.1 * ( ((controller_data2->queue_stat[i][j].occ_msb << 8) & 0xFF00) + ((controller_data2->queue_stat[i][j].occ_lsb) & 0xFF) );
				printf("occupancy_aggregation_onramp: Occ %f of detector %d \n", 
					0.1 * ( ((controller_data2->queue_stat[i][j].occ_msb << 8) & 0xFF00) + ((controller_data2->queue_stat[i][j].occ_lsb) & 0xFF) ),
					   i
			    );
			}else{
				printf("occupancy_aggregation_onramp: Error %d detector %d\n",
					controller_data2->queue_stat[i][j].stat,
					i
				);
			}
	        }
	   }
	}
	else
		return FLOAT_ERROR;

	occupancy /= controller_data->num_meter; // this is average onramp occupancy
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_meter %d\n", occupancy, controller_data->num_meter);
	return  mind(100.0, maxd(occupancy,0));
}

float occupancy_aggregation_offramp(db_urms_status3_t *controller_data, struct confidence *confidence){
	int i;
	float occupancy = 0;

	if( (controller_data->num_addl_det > 0) && (controller_data->num_addl_det <= 16) ) {
	    for(i=0 ; i < controller_data->num_addl_det; i++) {
		if(controller_data->additional_det[i].stat == 2){
			occupancy += (float)((controller_data->additional_det[i].occ_msb << 8) + controller_data->additional_det[i].occ_lsb);
			occupancy += 0.1 * ( ((controller_data->additional_det[i].occ_msb << 8) & 0xFF00) + ((controller_data->additional_det[i].occ_lsb) & 0xFF) );
			printf("occupancy_aggregation_offramp: Occ %f of detector %d \n", 
				   0.1 * ( ((controller_data->additional_det[i].occ_msb << 8) & 0xFF00) + ((controller_data->additional_det[i].occ_lsb) & 0xFF) ),
				   i
		    );
		}else{
			printf("occupancy_aggregation_offramp: Error %d detector %d\n",
				controller_data->additional_det[i].stat,
				i
			);
		}
	    }
	}
	else
		return FLOAT_ERROR;

	occupancy /= controller_data->num_addl_det; // this is average offramp occupancy
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_addl_det %d\n", occupancy, controller_data->num_addl_det);
	return  mind(100.0, maxd(occupancy,0));
}


float hm_speed_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence){
	// compute harmonic mean of speed
	int i; //  lane number index
	int j = 0; 
	float tmp = 0.0;
	float speed = 0.0;
	float mean_speed = 0.0;
	float var_speed = 0.0;
	int num_lane = controller_data->num_main;
	float speed_temp [num_lane];

    confidence->num_total_vals = num_lane;
	confidence->num_good_vals = num_lane;

	if( (controller_data->num_main > 0) && (controller_data->num_main <= 8) ) {
	    for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			if((float)controller_data->mainline_stat[i].speed >= 0 && (float)controller_data->mainline_stat[i].speed <= 150){ // if flow is in the range
			    speed_temp[j]=(float)controller_data->mainline_stat[i].speed;
				j++;
			}else{  // replce the flow measurement if it is not in the range
                confidence->num_good_vals--; 
			}
		}else if(controller_data->mainline_stat[i].trail_stat == 2){
			if((float)controller_data->mainline_stat[i].speed >= 0 && (float)controller_data->mainline_stat[i].speed <= 150){
			     speed_temp[j]=(float)controller_data->mainline_stat[i].speed;  
			     j++;
			}else{
                 confidence->num_good_vals--;
			}
		}else{
			confidence->num_good_vals--;
			
		}
	    }
	}
	else
		return FLOAT_ERROR;

	mean_speed = mean_array(speed_temp, confidence->num_good_vals);
	var_speed = var_array(speed_temp, confidence->num_good_vals);
    
    // this loop replace data with large variance
	for(i=0 ; i < confidence->num_good_vals; i++) {
	    if (abs(speed_temp[i]-mean_speed)>5*var_speed)
            speed_temp[i] = mean_speed;
	}

    // compute harmonic mean
	for(i=0 ; i < confidence->num_good_vals; i++) {
	    tmp += (1.0/speed_temp[i]);
	}

	speed = max((confidence->num_good_vals)/tmp,0);
	
	// check Nan 
	if(isnan(speed)){
		speed = FLOAT_ERROR;
	}else{
	    speed = speed*1.6;
	}
	printf("speed_agg %4.2f num_main %d\n", speed, controller_data->num_main);
	return mind(150.0, speed);
}

float mean_speed_aggregation_mainline(db_urms_status_t *controller_data, struct confidence *confidence){
	// compute mean of speed
	int i; //  lane number index
	int j = 0;
	float speed = 0.0;
    float mean_speed = 0.0;
	float var_speed = 0.0;
	int num_lane = controller_data->num_main;
	float speed_temp [num_lane];

	confidence->num_total_vals = num_lane;
	confidence->num_good_vals = num_lane;
	
	if( (controller_data->num_main > 0) && (controller_data->num_main <= 8) ) {
	    for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			if((float)controller_data->mainline_stat[i].speed >= 0 && (float)controller_data->mainline_stat[i].speed <= 150){ // if flow is in the range
			    speed_temp[j]=(float)controller_data->mainline_stat[i].speed;
				j++;
			}else{  // replce the flow measurement if it is not in the range
                confidence->num_good_vals--; 
			}
		}else if(controller_data->mainline_stat[i].trail_stat == 2){
			if((float)controller_data->mainline_stat[i].speed >= 0 && (float)controller_data->mainline_stat[i].speed <= 150){
			     speed_temp[j]=(float)controller_data->mainline_stat[i].speed;  
			     j++;
			}else{
                 confidence->num_good_vals--;
			}
		}else{
			confidence->num_good_vals--;
			
		}
	    }
	}
	else
		return FLOAT_ERROR;
    
	mean_speed = mean_array(speed_temp, confidence->num_good_vals);
    var_speed = var_array(speed_temp, confidence->num_good_vals);

	// this loop replace data with large variance
    for(i=0 ; i < confidence->num_good_vals; i++) {
	    if (abs(speed_temp[i]-mean_speed)>5*var_speed)
            speed_temp[i] = mean_speed;
	}

    speed = mean_array(speed_temp, confidence->num_good_vals);
	
	// check Nan 
	if(isnan(speed)){
		speed = FLOAT_ERROR;
	}else{
		speed = speed * 1.6;
	}
	printf("mean_speed_agg %4.2f num_main %d\n", speed,	controller_data->num_main);
	return mind(150.0, speed);
}


float queue_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence){
	float average_vehicle_length = 4.5; // average vehicle length 4.5 meters
	float queue = 0;
	float sum_inflow = 0;
	float sum_outflow = 0;
	int i; //  lane number index
	int j; //  queue loop number index

	if( (controller_data->num_meter > 0) && (controller_data->num_meter <= 4) ) {
	for(i=0 ;i < controller_data->num_meter; i++) {
	    for(j=0 ;j < MAX_QUEUE_LOOPS; j++) {
		if(controller_data2->queue_stat[i][j].stat == 2){
			sum_inflow += (float)controller_data2->queue_stat[i][j].vol;  //Advance detector
			sum_outflow +=  (float)controller_data->metered_lane_stat[i].passage_vol; // Passage detector
			printf("OR-inflow %d OR-outflow %d of detector %d \n", 
				controller_data2->queue_stat[i][j].vol,
				controller_data->metered_lane_stat[i].passage_vol,
				i
			);
			queue = (sum_inflow-sum_outflow)*average_vehicle_length;
			queue /= controller_data->num_meter; // average queue length
		}
		else if(controller_data2->queue_stat[i][j].stat == 5){
			queue = FLOAT_ERROR; // queue attained max queue length  
		}
		else{ 
			printf("queue_onramp: Error %d detector %d\n",
				controller_data2->queue_stat[i][j].stat,
				i
			);
		}
	    }
	    }
	}
	// check Nan 
	if(isnan(queue)){
		queue = FLOAT_ERROR;
	}
	printf("queue_agg %4.2f num_meter %d\n", queue,controller_data->num_meter);
	return mind(500.0, maxd(queue,0));
}

float density_aggregation_mainline(float flow, float hm_speed ){
	float density = 0.0;
	
	hm_speed =  mind(200,maxd(hm_speed,0));
	flow = mind(10000, maxd(flow,0));
	density = mind(2000,maxd(flow/hm_speed,0));
    
	// check Nan 
	if(isnan(density)){
		density = FLOAT_ERROR;
	}

	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}

	if(isnan(hm_speed)){
	    hm_speed = FLOAT_ERROR;
	}

	printf("flow %4.2f speed %4.2f density_agg %4.2f \n",
			mind(10000, maxd(flow,0)),
			mind(150, maxd(hm_speed,0)),
			mind(1200.0, maxd(density,0)) 
           );
	return mind(1200.0, density);
}


/*
float density_aggregation_mainline(db_urms_status_t *controller_data){
	float density = 0.0;
	float flow = 0.0;
	float speed = 0.0;
	
	speed = mean_speed_aggregation_mainline(controller_data);
	flow = flow_aggregation_mainline(controller_data);
	speed =  mind(200,maxd(speed * 1.6,0));
	//flow = mind(10000, maxd(temp_flow,0));
	flow = mind(10000, maxd(flow * 120,0));
	density = mind(2000,maxd(flow/speed,0));
    
	// check Nan 
	if(isnan(density)){
		density = FLOAT_ERROR;
	}

	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}

	if(isnan(speed)){
	    speed = FLOAT_ERROR;
	}

	printf("flow %4.2f speed %4.2f density_agg %4.2f \n",
			mind(10000, maxd(flow,0)),
			mind(150, maxd(speed,0)),
			mind(1200.0, maxd(density,0)) 
           );
	return mind(1200.0, maxd(density,0));
}
*/