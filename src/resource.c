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
	//printf("ML-flow_agg %4.2f num_main %d\n", flow, controller_data->num_main);
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

		confidence->num_total_vals = num_lane;
	    confidence->num_good_vals = num_lane;
		
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
	    flow = flow*120; // convert 30 second data into hour data
	}
	//printf("OR-flow_agg %4.2f num_meter %d\n",	flow, controller_data->num_meter);
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
	 	confidence->num_total_vals = num_lane;
	    confidence->num_good_vals = num_lane;

	if( (controller_data->num_addl_det > 0) && (controller_data->num_addl_det <= 16) ) {
	    for(i=0 ; i< controller_data->num_addl_det; i++){  
             if( (controller_data->additional_det[i].stat == 2) || (controller_data->additional_det[i].stat == 1) ){ // if the controller report the flow data is correct, then check the data is in the range or not
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
		flow = flow*120; // convert 30 second data into hour data
	}
	//printf("FR-flow_agg %4.2f num_addl_det %d\n", flow, controller_data->num_addl_det );
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

	//printf("Occ_agg %4.2f num_main %d\n", occupancy, controller_data->num_main);
	return  mind(100.0, occupancy);
}

float occupancy_aggregation_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2, struct confidence *confidence){ 
	int i;
	int j;
	int k=0;
	float occupancy = 0;
	float mean_occ = 0.0;
	float var_occ = 0.0;
    int num_lane = controller_data->num_main;
    float occ_temp [MAX_METERED_LANES * MAX_QUEUE_LOOPS];

	confidence->num_total_vals = MAX_METERED_LANES * MAX_QUEUE_LOOPS;
	confidence->num_good_vals = MAX_METERED_LANES * MAX_QUEUE_LOOPS;

	if( (controller_data->num_meter > 0) && (controller_data->num_meter <= 4) ) {
	    for(i=0 ; i < controller_data->num_meter; i++) {
	   	    for(j=0 ; j < MAX_QUEUE_LOOPS; j++) { 
 			    if( (controller_data2->queue_stat[i][j].stat == 2) || (controller_data2->queue_stat[i][j].stat == 1) ){
				    occupancy = 0.1 * ( ((controller_data2->queue_stat[i][j].occ_msb << 8) & 0xFF00) + ((controller_data2->queue_stat[i][j].occ_lsb) & 0xFF) );
					if(occupancy>=0 && occupancy<=100){
					    occ_temp[k] = occupancy;
                        			k++;
					}else{
					     confidence->num_good_vals--;
					}
 			    }else{
 				    confidence->num_good_vals--;
 				}
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

	//printf("Occ_agg %4.2f num_meter %d\n", occupancy, controller_data->num_meter);
	return  mind(100.0, occupancy);
}

float occupancy_aggregation_offramp(db_urms_status3_t *controller_data, struct confidence *confidence){
	int i;
	int j=0;
	float occupancy = 0;
	float mean_occ = 0.0;
	float var_occ = 0.0;
    int num_lane = controller_data->num_addl_det;
    float occ_temp [num_lane];

	confidence->num_total_vals = num_lane;
	confidence->num_good_vals = num_lane;

	if( (controller_data->num_addl_det > 0) && (controller_data->num_addl_det <= 16) ) {
	    for(i=0 ; i < controller_data->num_addl_det; i++) {
 		if( (controller_data->additional_det[i].stat == 2) || (controller_data->additional_det[i].stat == 1) ){
			occupancy = (float)((controller_data->additional_det[i].occ_msb << 8) + controller_data->additional_det[i].occ_lsb);
			occupancy = 0.1 * ( ((controller_data->additional_det[i].occ_msb << 8) & 0xFF00) + ((controller_data->additional_det[i].occ_lsb) & 0xFF) );
			   if(occupancy>=0 && occupancy<=100){
				   occ_temp[j] = occupancy;
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

	//printf("Occ_agg %4.2f num_addl_det %d\n", occupancy, controller_data->num_addl_det);
	return  mind(100.0, occupancy);
}


float hm_speed_aggregation_mainline(db_urms_status_t *controller_data, float hm_speed_prev, struct confidence *confidence){
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


    // speed change rate limiter 
	if( (speed - hm_speed_prev  >= -50) && (speed - hm_speed_prev  <= 60)  && (speed != -1) ){
	   speed = speed;
	}else{
	   speed = hm_speed_prev;
	}
	
	//printf("speed_agg %4.2f num_main %d\n", speed, controller_data->num_main);
	return mind(150.0, speed);
}

float mean_speed_aggregation_mainline(db_urms_status_t *controller_data, float mean_speed_prev, struct confidence *confidence){
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

	// speed change rate limiter 
	if( (speed - mean_speed_prev  >= -50) && (speed - mean_speed_prev  <= 60)  && (speed != -1) ){
	   speed = speed;
	}else{
	   speed = mean_speed_prev;
	}

	//printf("mean_speed_agg %4.2f num_main %d\n", speed,	controller_data->num_main);
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
//  		printf("queue_onramp: Error %d detector %d\n",
//				controller_data2->queue_stat[i][j].stat,
//				i
//			);
 		}
	    }
	    }
	}
	// check Nan 
	if(isnan(queue)){
		queue = FLOAT_ERROR;
	}
	//printf("queue_agg %4.2f num_meter %d\n", queue,controller_data->num_meter);
	return mind(500.0, maxd(queue,0));
}

float density_aggregation_mainline(float flow, float hm_speed, float density_prev){
	float density = 0.0;
	
	hm_speed =  mind(200,maxd(hm_speed,0));
	flow = mind(10000, maxd(flow,0));
	density = mind(200,maxd(flow/hm_speed,0));
    
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

	// density change rate limiter 
	if( (density - density_prev  >= -150) && (density - density_prev  <= 150)  && (density != -1) ){
	   density = density;
	}else{
	   density = density_prev;
	}
    
	/*
	printf("flow %4.2f speed %4.2f density_agg %4.2f \n",
			mind(10000, maxd(flow,0)),
			mind(150, maxd(hm_speed,0)),
			mind(200.0, maxd(density,0)) 
           );
    */
	return mind(200.0, density);
}

float turning_ratio_offramp(float FR_flow, float ML_flow){
float turning_ratio_offramp = 0.0;
	if(FR_flow>=0 && ML_flow>0){
		turning_ratio_offramp = (FR_flow/ML_flow)*100;
	}else{
	    turning_ratio_offramp = 0.0;
	}

	return mind(100,turning_ratio_offramp);
}

float butt_2(float in_dat)
{
   float x[2]={0.0,0.0}, out_dat=0.0;
   static float x_old[2]={0.0,0.0};
   
   x[0]=0.2779*x_old[0] - 0.4152*x_old[1] + 0.5872*in_dat;
   x[1]=0.4152*x_old[0] + 0.8651*x_old[1] + 0.1908*in_dat;  
   out_dat = 0.1468*x[0] + 0.6594*x[1] + 0.0675*in_dat;
   x_old[0]=x[0];
   x_old[1]=x[1];
   return out_dat;
}

float interp_OR_HIS_FLOW(timestamp_t *ts, int OR_idx ){
    
    // add On-ramp 5 minute historical data as a look up table, 288 is the time vector and 16 is the number of total on-ramp 
const float OR_HIS_FLOW_DATA[288][16+1] = {
{	1.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	2.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	3.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	4.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	5.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	6.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	7.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	8.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	9.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	10.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	11.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	12.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	13.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	14.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	15.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	16.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	17.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	18.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	19.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	20.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	21.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	22.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	23.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	24.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	25.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	26.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	27.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	28.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	29.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	30.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	31.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	32.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	33.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	34.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	35.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	36.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	37.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	38.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	39.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	40.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	41.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	42.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	43.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	44.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	45.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	46.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	47.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	48.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	49.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	50.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	51.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	52.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	53.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	54.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	55.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	56.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	57.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	58.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	59.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	60.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	61.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	62.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	63.000	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	}	,
{	64.000	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	}	,
{	65.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	66.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	67.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	68.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	69.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	70.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	71.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	72.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	73.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	74.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	75.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	76.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	77.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	78.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	79.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	80.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	81.000	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	}	,
{	82.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	83.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	84.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	85.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	86.000	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	}	,
{	87.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	88.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	89.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	90.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	91.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	92.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	93.000	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	}	,
{	94.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	95.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	96.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	97.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	98.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	99.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	100.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	101.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	102.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	103.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	104.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	105.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	106.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	107.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	108.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	109.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	110.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	111.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	112.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	113.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	114.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	115.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	116.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	117.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	118.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	119.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	120.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	121.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	122.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	123.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	124.000	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	}	,
{	125.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	126.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	127.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	128.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	129.000	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	}	,
{	130.000	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	}	,
{	131.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	132.000	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	}	,
{	133.000	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	}	,
{	134.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	135.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	136.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	137.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	138.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	139.000	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	}	,
{	140.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	141.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	142.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	143.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	144.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	145.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	146.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	147.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	148.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	149.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	150.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	151.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	152.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	153.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	154.000	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	}	,
{	155.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	156.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	157.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	158.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	159.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	160.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	161.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	162.000	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	}	,
{	163.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	164.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	165.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	166.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	167.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	168.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	169.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	170.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	171.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	172.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	173.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	174.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	175.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	176.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	177.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	178.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	179.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	180.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	181.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	182.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	183.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	184.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	185.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	186.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	187.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	188.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	189.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	190.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	191.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	192.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	193.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	194.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	195.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	196.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	197.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	198.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	199.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	200.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	201.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	202.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	203.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	204.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	205.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	206.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	207.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	208.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	209.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	210.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	211.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	212.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	213.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	214.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	215.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	216.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	217.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	218.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	219.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	220.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	221.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	222.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	223.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	224.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	225.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	226.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	227.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	228.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	229.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	230.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	231.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	232.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	233.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	234.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	235.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	236.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	237.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	238.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	239.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	240.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	241.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	242.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	243.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	244.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	245.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	246.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	247.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	248.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	249.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	250.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	251.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	252.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	253.000	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	}	,
{	254.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	255.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	256.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	257.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	258.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	259.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	260.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	261.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	262.000	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	}	,
{	263.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	264.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	265.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	266.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	267.000	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	}	,
{	268.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	269.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	270.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	271.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	272.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	273.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	274.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	275.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	276.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	277.000	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	}	,
{	278.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	279.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	280.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	281.000	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	}	,
{	282.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	283.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	284.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	285.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	286.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	287.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	288.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	};

    get_current_timestamp(&ts);

	int t_0 = 0;
	int t_1 = 0;
	float t_convert = 0.0; 
	float OR_flow = 0.0;
    
    t_convert = (12*ts.hour) + (ts.min/5);
	t_convert = mind(t_convert,288);
	t_convert = maxd(0,t_convert);
    
	t_0 = floor(t_convert);
    t_0 = mind(t_0,288);
	t_0 = maxd(0,t_0);
     
	t_1 = ceil(t_convert);
    t_1 = mind(t_1,288);
	t_1 = maxd(0,t_1);
    
    OR_flow = OR_HIS_FLOW_DATA[t_0][OR_idx+1] + (((t_convert - t_0)/(t_1-t_0))*(OR_HIS_FLOW_DATA[t_1][OR_idx+1] - OR_HIS_FLOW_DATA[t_0][OR_idx+1] ));

    return OR_flow;
}