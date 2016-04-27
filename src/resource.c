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

float maxd(float a,float b)
{
	if(a>=b){
		return a;
	}
	else{
		return b;
	}
}

float mind(float a,float b)
{
	if(a<=b){
		return a;
	}
	else{
		return b;
	}
}

float flow_aggregation_mainline(db_urms_status_t *controller_data){
        int i;
        float flow = 0.0;

	for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			flow += (float)controller_data->mainline_stat[i].lead_vol;
			printf("ML-flow %d of detector %d \n",controller_data->mainline_stat[i].lead_vol,i);
		}else{
			printf("flow_aggregation_mainline: Error %d detector %d\n",
				controller_data->mainline_stat[i].lead_stat, 
				i
			);
		}
	}
	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
	    flow = flow * 120;
	}
	printf("ML-flow_agg %4.2f num_main %d\n", flow, controller_data->num_main);
	return mind(12000.0, maxd(flow,0));
}

float flow_aggregation_onramp(db_urms_status_t *controller_data){
        int i;
        float flow = 0;

	for(i=0 ; i< controller_data->num_meter;i++) {
		if(controller_data->metered_lane_stat[i].demand_stat == 2){
			flow += (float)controller_data->metered_lane_stat[i].demand_vol;
			printf("OR-flow %d of detector %d \n", controller_data->metered_lane_stat[i].demand_vol, i);
			}else{
				printf("flow_aggregation_onramp: Error %d detector %d\n",
					controller_data->metered_lane_stat[i].demand_stat,
					i
				);
			}
	}
	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
	    flow = flow * 120;  
	}
	printf("OR-flow_agg %4.2f num_meter %d\n",	flow, controller_data->num_meter);
return mind(1000.0*controller_data->num_meter, maxd(flow,0));
}

float flow_aggregation_offramp(db_urms_status3_t *controller_data){
        int i;
        float flow = 0;

	for(i=0 ; i< controller_data->num_addl_det; i++) {
		if(controller_data->additional_det[i].stat == 2){
			flow += (float)controller_data->additional_det[i].volume;
			printf("FR-flow %d of detector %d \n", controller_data->additional_det[i].volume,i);
		}else{
			printf("flow_aggregation_offramp: Error %d detector %d\n",
				controller_data->additional_det[i].stat,
				i
			);
		}
	}
	if(isnan(flow)){
		flow = FLOAT_ERROR;
	}else{
		flow = flow *120 ; 
	}
	printf("FR-flow_agg %4.2f num_addl_det %d\n", flow, controller_data->num_addl_det );
	return mind(1000.0*controller_data->num_addl_det, maxd(flow,0));
}

float occupancy_aggregation_mainline(db_urms_status_t *controller_data){
	int i;
	float occupancy = 0;

	for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			occupancy += (float)((controller_data->mainline_stat[i].lead_occ_msb << 8) + controller_data->mainline_stat[i].lead_occ_lsb);
			printf("Occ %f of detector %d \n", 
				   (float)((controller_data->mainline_stat[i].lead_occ_msb << 8) + controller_data->mainline_stat[i].lead_occ_lsb),
				   i
		    );
		}else{
			printf("occupancy_aggregation_mainline: Error %d detector %d\n",
				controller_data->mainline_stat[i].lead_stat,
				i
			);
		}
	}

	occupancy /= controller_data->num_main;
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_main %d\n", occupancy, controller_data->num_main);
	return  mind(100.0, maxd(occupancy,0));
}

float occupancy_aggregation_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2){ 
	int i;
	int j;
	float occupancy = 0;

	for(i=0 ; i < controller_data->num_meter; i++) {
	   for(j=0 ; j < MAX_QUEUE_LOOPS; j++) { 
		if(controller_data2->queue_stat[i][j].stat == 2){
			occupancy += (float)((controller_data2->queue_stat[i][j].occ_msb << 8) + controller_data2->queue_stat[i][j].occ_lsb);
			printf("Occ %f of detector %d \n", 
				   (float)((controller_data2->queue_stat[i][j].occ_msb << 8) + controller_data2->queue_stat[i][j].occ_lsb),
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

	occupancy /= controller_data->num_meter; // this is average onramp occupancy
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_meter %d\n", occupancy, controller_data->num_meter);
	return  mind(100.0, maxd(occupancy,0));
}

float occupancy_aggregation_offramp(db_urms_status3_t *controller_data){
	int i;
	float occupancy = 0;

	for(i=0 ; i < controller_data->num_addl_det; i++) {
		if(controller_data->additional_det[i].stat == 2){
			occupancy += (float)((controller_data->additional_det[i].occ_msb << 8) + controller_data->additional_det[i].occ_lsb);
			printf("Occ %f of detector %d \n", 
				   (float)((controller_data->additional_det[i].occ_msb << 8) + controller_data->additional_det[i].occ_lsb),
				   i
		    );
		}else{
			printf("occupancy_aggregation_offramp: Error %d detector %d\n",
				controller_data->additional_det[i].stat,
				i
			);
		}
	}

	occupancy /= controller_data->num_addl_det; // this is average offramp occupancy
    // check Nan 
	if(isnan(occupancy)){
		occupancy = FLOAT_ERROR;
	}

	printf("Occ_agg %4.2f num_addl_det %d\n", occupancy, controller_data->num_addl_det);
	return  mind(100.0, maxd(occupancy,0));
}


float speed_aggregation_mainline(db_urms_status_t *controller_data){
	// compute harmonic mean of speed
	float tmp = 0.0;
	float speed = 0.0;
	int i; //  lane number index

	for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			tmp += (1.0/((float)controller_data->mainline_stat[i].speed));
			printf("speed %d of detector %d \n", controller_data->mainline_stat[i].speed, i);
		}else{
			printf("speed_aggregation_mainline: Error %d detector %d\n",
				controller_data->mainline_stat[i].lead_stat,
				i
			);
		}
	} 
	speed = max((controller_data->num_main)/tmp,0);
	// check Nan 
	if(isnan(speed)){
		speed = FLOAT_ERROR;
	}else{
	    speed = speed*1.6;
	}
	printf("speed_agg %4.2f num_main %d\n", speed, controller_data->num_main);
	return mind(150.0, maxd(speed,0));
}

float mean_speed_aggregation_mainline(db_urms_status_t *controller_data){
	// compute mean of speed
	float speed = 0.0;
	int i; //  lane number index

	for(i=0 ; i < controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
			speed += (float)controller_data->mainline_stat[i].speed;
			printf("speed %d of detector %d \n", controller_data->mainline_stat[i].speed,i);
		}else{
			printf("mean_speed_aggregation_mainline: Error %d detector %d\n",
				controller_data->mainline_stat[i].lead_stat,
				i
			);
		}
	} 
	speed /= controller_data->num_main;
	// check Nan 
	if(isnan(speed)){
		speed = FLOAT_ERROR;
	}else{
		speed = speed * 1.6;
	}
	printf("mean_speed_agg %4.2f num_main %d\n", speed,	controller_data->num_main);
	return mind(150.0, maxd(speed,0));
}


float queue_onramp(db_urms_status_t *controller_data, db_urms_status2_t *controller_data2){
	float average_vehicle_length = 4.5; // average vehicle length 4.5 meters
	float queue = 0;
	float sum_inflow = 0;
	float sum_outflow = 0;
	int i; //  lane number index
	int j; //  queue loop number index

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
	// check Nan 
	if(isnan(queue)){
		queue = FLOAT_ERROR;
	}
	printf("queue_agg %4.2f num_meter %d\n", queue,controller_data->num_meter);
	return mind(500.0, maxd(queue,0));
}

float density_aggregation_mainline(db_urms_status_t *controller_data){
	float density = 0.0;
	float flow = 0.0;
	float speed = 0.0;
	float temp_flow = 0.0;
    float temp_speed = 0.0;
	
	int i;
	for(i=0 ; i< controller_data->num_main; i++) {
		if(controller_data->mainline_stat[i].lead_stat == 2){
                temp_flow += (float)controller_data->mainline_stat[i].lead_vol;     // total flow
			    temp_speed += (1./(float)controller_data->mainline_stat[i].speed); // harmonic mean speed
				
				printf("flow %4.2f speed %4.2f of detector %d \n",
					(float)controller_data->mainline_stat[i].lead_vol, 
					(float)controller_data->mainline_stat[i].speed, 
					 i );
		}else
		{       printf("density_aggregation_mainline: Error %d detector %d\n",
                    controller_data->mainline_stat[i].lead_stat,
                    i
					);
		}
	}
	
	speed = mind(200,maxd(controller_data->num_main/temp_speed,0));
	speed =  mind(200,maxd(speed * 1.6,0));
	flow = mind(10000, maxd(temp_flow,0));
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

//float data[3] = {0}; // add data bound here

//float add_cyclic_buffer_contents(float *data[]) {
//    // static 
//	float output = 0;
//	int i;
//
//	for( i = 1; i >= 0; i--)
//		data[i + 1] = data[i];
//
//	data[0] = datum;
//
//	for i = 0; i < 3; i++)
//		output += data[i];
//	
//	return output;
//}
	
/*
float flow_aggregation_3_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3)
{
    float flow_agg = 0;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3)/3;
	return flow_agg;
}

float flow_aggregation_4_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3, float flow_lane_4)
{
    float flow_agg = 0;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_lane_4 = mind(maxd(flow_lane_4,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3+flow_lane_4)/4;
	return flow_agg;
}

float flow_aggregation_5_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3, float flow_lane_4, float flow_lane_5)
{
    float flow_agg = 0;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_lane_4 = mind(maxd(flow_lane_4,0),flow_upper_bound);
	flow_lane_5 = mind(maxd(flow_lane_5,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3+flow_lane_4+flow_lane_5)/5;
	return flow_agg;
}

float occ_aggregation_3_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3)
{
    float occ_agg = 0;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3)/3;
	return occ_agg;
}

float occ_aggregation_4_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3, float occ_lane_4)
{
    float occ_agg = 0;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_lane_4 = mind(maxd(occ_lane_4,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3+occ_lane_4)/4;
	return occ_agg;
}

float occ_aggregation_5_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3, float occ_lane_4, float occ_lane_5)
{
    float occ_agg = 0;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_lane_4 = mind(maxd(occ_lane_4,0),1);
	occ_lane_5 = mind(maxd(occ_lane_5,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3+occ_lane_4+occ_lane_5)/5;
	return occ_agg;
}

float speed_aggregation_3_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3)
{
    float speed_agg,speed_lane_1_t,speed_lane_2_t,speed_lane_3_t;
	int speed_upper_bound = 200; // the unit is mile per hour 
    speed_lane_1_t = mind(maxd(speed_lane_1,1.0), speed_upper_bound);
    speed_lane_2_t = mind(maxd(speed_lane_2,1.0), speed_upper_bound);
    speed_lane_3_t = mind(maxd(speed_lane_3,1.0), speed_upper_bound);
    speed_lane_1_t = 1/speed_lane_1;
    speed_lane_2_t = 1/speed_lane_2;
    speed_lane_3_t = 1/speed_lane_3;
	speed_agg = 3/(speed_lane_1_t+speed_lane_2_t+speed_lane_3_t);
	
	return speed_agg;
}

float speed_aggregation_4_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3, float speed_lane_4)
{
    float speed_agg,speed_lane_1_t,speed_lane_2_t,speed_lane_3_t,speed_lane_4_t;
	int speed_upper_bound = 200; // the unit is mile per hour 
    speed_lane_1_t = mind(maxd(speed_lane_1,1.0), speed_upper_bound);
    speed_lane_2_t = mind(maxd(speed_lane_2,1.0), speed_upper_bound);
    speed_lane_3_t = mind(maxd(speed_lane_3,1.0), speed_upper_bound);
    speed_lane_4_t = mind(maxd(speed_lane_4,1.0), speed_upper_bound);
    speed_lane_1_t = 1/speed_lane_1;
    speed_lane_2_t = 1/speed_lane_2;
    speed_lane_3_t = 1/speed_lane_3;
    speed_lane_4_t = 1/speed_lane_4;
	speed_agg = 4/(speed_lane_1_t+speed_lane_2_t+speed_lane_3_t+speed_lane_4_t);
	
	return speed_agg;
}

float speed_aggregation_5_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3, float speed_lane_4, float speed_lane_5)
{
    float speed_agg,speed_lane_1_t,speed_lane_2_t,speed_lane_3_t,speed_lane_4_t,speed_lane_5_t;
	int speed_upper_bound = 200; // the unit is mile per hour 
    speed_lane_1_t = mind(maxd(speed_lane_1,1.0), speed_upper_bound);
    speed_lane_2_t = mind(maxd(speed_lane_2,1.0), speed_upper_bound);
    speed_lane_3_t = mind(maxd(speed_lane_3,1.0), speed_upper_bound);
    speed_lane_4_t = mind(maxd(speed_lane_4,1.0), speed_upper_bound);
    speed_lane_5_t = mind(maxd(speed_lane_5,1.0), speed_upper_bound);
	speed_lane_1_t = 1/speed_lane_1;
    speed_lane_2_t = 1/speed_lane_2;
    speed_lane_3_t = 1/speed_lane_3;
    speed_lane_4_t = 1/speed_lane_4;
	speed_lane_5_t = 1/speed_lane_5;
	speed_agg = 5/(speed_lane_1_t+speed_lane_2_t+speed_lane_3_t+speed_lane_4_t+speed_lane_5_t);
	
	return speed_agg;
}
*/

/*
float get_split_ratio(float upstream_total_flow, float off_ramp_flow)
{   
	float split_ratio;
	split_ratio = off_ramp_flow/upstream_total_flow;    
	return split_ratio;
}

float get_queue_estimation(float in_flow, float out_flow, float previous_queue)
{
	float sampling_time = 30;
	float current_queue;
	float queue_upper_bound=50;
	current_queue = previous_queue + sampling_time*(in_flow-out_flow);
	current_queue = mind(maxd(current_queue,0),queue_upper_bound);
	return current_queue;
}

float get_density_estimation(float in_flow, float out_flow, float section_length, float previous_density, float speed)
{
    float sampling_time = 30;
	float current_density;
	float density_upper_bound=200;
	current_density = previous_density + (sampling_time/section_length)*(in_flow-out_flow);
	current_density = mind(maxd(current_density,0),density_upper_bound);
	return current_density;
}

float get_off_ramp_flow_by_flow_balance(float upstream_total_flow, float on_ramp_flow, float downstream_total_flow)
{
	float off_ramp_flow;
	float off_ramp_flow_upper_bound = 500;
    off_ramp_flow = upstream_total_flow+on_ramp_flow-downstream_total_flow;
	off_ramp_flow = mind(maxd(off_ramp_flow,0),off_ramp_flow_upper_bound);
	return off_ramp_flow; 
}

float get_on_ramp_flow_by_flow_balance(float upstream_total_flow, float downstream_total_flow, float off_ramp_flow)
{
    float on_ramp_flow;
    float on_ramp_flow_upper_bound = 500;
    on_ramp_flow = off_ramp_flow+downstream_total_flow-upstream_total_flow;
	on_ramp_flow = mind(maxd(off_ramp_flow,0),on_ramp_flow_upper_bound);
	return on_ramp_flow; 
}
*/
