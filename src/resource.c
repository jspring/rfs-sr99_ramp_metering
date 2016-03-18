#include <db_include.h>
#include "resource.h"

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

int open_file(char controller_data_file[])
{
    int column_max;
	column_max = 3;
	double data[2600][3];
	int count = 0;
    int i = 0;
    int j =0;

    FILE *file;
    file = fopen(controller_data_file,"r");
    if(!file)
    {
        perror("Error opening file");
        return -1;
    }

	while (!feof(file)           /* Check for the end of file*/
        &&(count < column_max))  /* To avoid memory corruption */
    {
        fscanf(file, "%lf", &(data[count++]));
    }
    fclose(file);
 

    /* Print data */
    printf("Data count: %s\n", controller_data_file);
    for(i = 0; i < 100; i++)
    {
		for(j=0; j<count; j++){
        printf("Data[%d][%d] = %lf\n", i,j, data[i][j]);
		}
    }
	return 0;
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
