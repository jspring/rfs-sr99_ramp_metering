// main.cpp : main project file.
// This script will do the following task:
// 1. read data from all controllers.
// 2. check data bound.
// 3. remove invalid data.
// 4. impute data.
// 5. aggregate data lane by lane.

#include <stdio.h>
#include <string.h>
#include <math.h>
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <sstream>

#include "resource.h"
#include "rm_algo.h"
//using namespace std;

//#define CONTROLLER_DATA_COLUMN_MAX 3
//#define CONTROLLER_DATA_ROW_MAX 3
//#define TOTAL_NUNMBER_OF_CONTROLLERS 28

// declare function
int open_file(char* controller_data_file);

int main()
{

	db_agg_data_t agg_data;

	//char controller_1_data_file_name[] = "a_10.29.248.20_0224013.dat";
    //char controller_2_data_file_name[] = "a_10.29.248.42_0224013.dat";
    char controller_1_data_file_name[] = "vds_312422_flow.txt";
    char controller_2_data_file_name[] = "vds_312422_occ.txt";
    char controller_1_data_file_out [] = "vds_312422_flow_agg.txt";
    char controller_2_data_file_out [] = "vds_312422_occ_agg.txt";
	//read_file(controller_1_data_file_name); 
	//open_file(controller_1_data_file_name);
    //open_file(controller_2_data_file_name);
    read_file(controller_1_data_file_name,controller_1_data_file_out);
    read_file(controller_2_data_file_name,controller_2_data_file_out);

	// This structure save double or float data type,
	// but the date read from controller is all in character.
	struct all_controller_data{

	       // main line data, gp lane and HOV lane
		   float ML_lane_1_vol;
		   float ML_lane_1_occ;
		   float ML_lane_1_speed;
		   float ML_lane_2_vol;
		   float ML_lane_2_occ;
		   float ML_lane_2_speed;

           // on-ramp data
		   float on_ramp_1_demand_vol;
           float on_ramp_1_passage_vol;
           float on_ramp_1_queue_occ;
           float on_ramp_1_queue_vol;
           
		   // off-ramp data
		   float off_ramp_1_demand_vol;
           float off_ramp_1_passage_vol;
           float off_ramp_1_queue_occ;
           float off_ramp_1_queue_vol;
    
    };

/*
typedef struct {
        float mainline_agg_speed;
        float mainline_agg_occ;
        float mainline_agg_vol;
        char mainline_agg_stat;
        float offramp_agg_occ;
        float offramp_agg_vol;
        char offramp_agg_stat;
        float onramp_agg_vol;
        char onramp_agg_stat;
} IS_PACKED db_agg_data_t;
*/
	
    // variable of unit test 
	float occ_agg_3_test,flow_agg_3_test,speed_agg_3_test;
	float occ_agg_4_test,flow_agg_4_test,speed_agg_4_test;
    float occ_agg_5_test,flow_agg_5_test,speed_agg_5_test;
    float split_ratio_test;
	float get_queue_estimation_test;
    float get_density_estimation_test;
	float get_off_ramp_flow_by_flow_balance_test;
	float get_on_ramp_flow_by_flow_balance_test;

	// unit test of data aggregation
	occ_agg_3_test = occ_aggragation_3_lanes(0.5,0.6,0.7);
	flow_agg_3_test = flow_aggragation_3_lanes(10,3,5);
	speed_agg_3_test = speed_aggragation_3_lanes(30,40,45);
	
	occ_agg_4_test = occ_aggragation_4_lanes(0.5,0.6,0.7,10);
	flow_agg_4_test = flow_aggragation_4_lanes(10,3,5,6);
	speed_agg_4_test = speed_aggragation_4_lanes(30,40,45,55);
	
	occ_agg_5_test = occ_aggragation_5_lanes(0.5,0.6,0.7,10,20);
	flow_agg_5_test = flow_aggragation_5_lanes(10,3,5,6,5);
	speed_agg_5_test = speed_aggragation_5_lanes(30,40,45,55,100);
	
	split_ratio_test = get_split_ratio(50.0,10.0);
	
    get_queue_estimation_test = get_queue_estimation(50, 30, 10); 

    get_density_estimation_test = get_density_estimation(50, 30, 10,0.5,30);

	get_off_ramp_flow_by_flow_balance_test = get_off_ramp_flow_by_flow_balance(1200,300, 400);

	get_on_ramp_flow_by_flow_balance_test = get_on_ramp_flow_by_flow_balance(1200, 800, 100);

	printf("occ_agg_3_test: %f \n", occ_agg_3_test);
	printf("flow_agg_3_test: %f \n", flow_agg_3_test);
	printf("speed_agg_3_test: %f \n", speed_agg_3_test);
    
	printf("occ_agg_4_test: %f \n", occ_agg_4_test);
	printf("flow_agg_4_test: %f \n", flow_agg_4_test);
	printf("speed_agg_4_test: %f \n", speed_agg_4_test);

	printf("occ_agg_5_test: %f \n", occ_agg_5_test);
	printf("flow_agg_5_test: %f \n", flow_agg_5_test);
	printf("speed_agg_5_test: %f \n", speed_agg_5_test);

	printf("split_ratio_test: %f \n", split_ratio_test);
    
	printf("get_queue_estimation_test: %f \n", get_queue_estimation_test);
    
	printf("get_density_estimation_test: %f \n", get_density_estimation_test);
	
	printf("get_off_ramp_flow_by_flow_balance_test: %f \n", get_off_ramp_flow_by_flow_balance_test);
	
	printf("get_on_ramp_flow_by_flow_balance_test: %f \n", get_on_ramp_flow_by_flow_balance_test);
	getchar();
	return 0;
}
