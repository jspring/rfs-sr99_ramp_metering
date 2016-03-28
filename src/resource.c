#include <db_include.h>
#include "resource.h"
#include <urms.h>

const char *controller_strings[] = {
        "10.29.248.108", //0
        "10.254.25.113", //1
        "10.254.25.120", //2
        "10.29.249.46",  //3
        "10.29.248.42",  //4
        "10.29.248.20",  //5
        "10.29.248.128", //6
        "10.254.27.67",  //7
        "10.254.27.82",  //8
        "10.254.27.81",  //9
        "10.29.248.76",  //10
        "10.254.28.213", //11
        "10.254.28.212", //12
        "10.254.28.211", //13
        "10.29.248.118", //14
        "10.29.248.52",  //15
        "10.254.24.156", //16
        "10.254.24.157", //17
        "10.29.248.185", //18
        "10.29.248.66",  //19
        "10.29.248.81",  //20
        "10.29.248.213", //21
        "10.29.248.155_PORT_1001",  //22
        "10.29.248.155_PORT_1002",  //23
        "10.29.248.124",            //24
        "10.29.248.67",             //25
        "10.29.248.157",            //26
        "10.29.248.56",             //27
};

float flow_aggragation_3_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3);

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
        int j;
        float flow;

        for(j=0 ; j < controller_data->num_main; j++) {
            if(controller_data->mainline_stat[j].lead_stat == 2)
                flow += (float)controller_data->mainline_stat[j].lead_vol;
            else
                printf("Error %d controller %s detector %d\n",
                    controller_data->mainline_stat[j].lead_stat,
                    controller_strings[j],
                    j
                );
	}

            return flow;
}

float flow_aggregation_onramp(db_urms_status_t *controller_data){
        int j;
        float flow;

        for(j=0 ; j < controller_data->num_main; j++) {
            if(controller_data->metered_lane_stat[2].demand_stat == 2)
                flow += (float)controller_data->metered_lane_stat[j].demand_stat;
            else
                printf("Error %d controller %s detector %d\n",
                    controller_data->metered_lane_stat[j].demand_stat,
                    controller_strings[j],
                    j
                );

	}
            return flow;
}

float flow_aggregation_offramp(db_urms_status_t *controller_data){
        int j;
        float flow;

        for(j=0 ; j < controller_data->num_addl_det; j++) {
            if(controller_data->additional_det[j].volume)
                flow += (float)controller_data->additional_det[j].volume;
            else
                printf("Error %d controller %s detector %d\n",
                    controller_data->mainline_stat[j].lead_stat,
                    controller_strings[j],
                    j
                );

	}
            return flow;
}


float occupancy_aggregation_mainline(db_urms_status_t *controller_data){
        int j;
        float occupancy;

        for(j=0 ; j < controller_data->num_main; j++) {
            if(controller_data->metered_lane_stat[2].demand_stat == 2)
            occupancy += (float)((controller_data->mainline_stat[j].lead_occ_msb << 8) + controller_data->mainline_stat[j].lead_occ_lsb);
	}

        occupancy /= controller_data->num_main;

        return occupancy;
}

float flow_aggragation_3_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3)
{
    float flow_agg;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3)/3;
	return flow_agg;
}

float flow_aggragation_4_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3, float flow_lane_4)
{
    float flow_agg;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_lane_4 = mind(maxd(flow_lane_4,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3+flow_lane_4)/4;
	return flow_agg;
}

float flow_aggragation_5_lanes(float flow_lane_1,float flow_lane_2, float flow_lane_3, float flow_lane_4, float flow_lane_5)
{
    float flow_agg;
	int flow_upper_bound = 50; //the unit is per lane per 30 second
	flow_lane_1 = mind(maxd(flow_lane_1,0),flow_upper_bound);
    flow_lane_2 = mind(maxd(flow_lane_2,0),flow_upper_bound);
	flow_lane_3 = mind(maxd(flow_lane_3,0),flow_upper_bound);
	flow_lane_4 = mind(maxd(flow_lane_4,0),flow_upper_bound);
	flow_lane_5 = mind(maxd(flow_lane_5,0),flow_upper_bound);
	flow_agg = (flow_lane_1+flow_lane_2+flow_lane_3+flow_lane_4+flow_lane_5)/5;
	return flow_agg;
}

float occ_aggragation_3_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3)
{
    float occ_agg;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3)/3;
	return occ_agg;
}

float occ_aggragation_4_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3, float occ_lane_4)
{
    float occ_agg;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_lane_4 = mind(maxd(occ_lane_4,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3+occ_lane_4)/4;
	return occ_agg;
}

float occ_aggragation_5_lanes(float occ_lane_1,float occ_lane_2, float occ_lane_3, float occ_lane_4, float occ_lane_5)
{
    float occ_agg;
    occ_lane_1 = mind(maxd(occ_lane_1,0),1);
    occ_lane_2 = mind(maxd(occ_lane_2,0),1);
	occ_lane_3 = mind(maxd(occ_lane_3,0),1);
	occ_lane_4 = mind(maxd(occ_lane_4,0),1);
	occ_lane_5 = mind(maxd(occ_lane_5,0),1);
	occ_agg = (occ_lane_1+occ_lane_2+occ_lane_3+occ_lane_4+occ_lane_5)/5;
	return occ_agg;
}

float speed_aggragation_3_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3)
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

float speed_aggragation_4_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3, float speed_lane_4)
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

float speed_aggragation_5_lanes(float speed_lane_1,float speed_lane_2, float speed_lane_3, float speed_lane_4, float speed_lane_5)
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

int read_file(char input_file_name[], char output_file_name[])
{   
	float reg1, reg2, reg3;
	int counter=1;
     
     FILE *fp_r = fopen(input_file_name, "r");
     FILE *fp_w = fopen(output_file_name, "w");
     
     if (fp_r == NULL)
       return -1;
       
     if (fp_w == NULL)
       return -1;
     
     while(!feof(fp_r)) {
       fscanf(fp_r, "%f %f %f ", &reg1, &reg2, &reg3 );
	   printf("counter: %d_%f_%f_%f  \n", counter, reg1, reg2, reg3);
       fprintf(fp_w, "%f %f %f \n", reg1, reg2, reg3);
	   counter++;
     }
     
     fclose(fp_r);
     fclose(fp_w);

	 return 0;
}
