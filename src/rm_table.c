#include <db_include.h>
#include <path_gps_lib.h>
#include "data_log.h"
#include <urms.h>
#include "wrfiles_rm.h"
#include "variables2.h"
#include "meter_lib.h"
#include "rm_algo.h"

//Ramp meter inputs
db_urms_status_t db_urms_status;
urms_datafile_t urms_datafile;
db_ramp_data_t db_ramp_data;

timestamp_t timestamp;

/**
 *      This array is used to specify how to read in the DB vars.
 */
db_var_spec_t db_vars_ac_rm[] =
{
        {1, sizeof(db_urms_status_t), &db_urms_status},
        {2, sizeof(urms_datafile_t), &urms_datafile},
        {3, sizeof(db_ramp_data_t), &db_ramp_data},
};
int num_ac_rm_vars = (sizeof(db_vars_ac_rm)/sizeof(db_var_spec_t));

/** This array is used to specify the output format of the data file.
 */
data_log_column_spec_t file_spec[] = 
{
        {"HH:MM:SS.SSS ", &timestamp, BASE_TIMESTAMP, REPLAY_TIME},		//###1
//get_long_status8_resp_mess_typ
        {"%hhu ",   &db_urms_status.mainline_stat[0].lead_stat, BASE_CHAR, REPLAY_USE},	//###2
        {"%hhu ",   &db_urms_status.mainline_stat[0].lead_vol, BASE_CHAR, REPLAY_USE},	//###3
        {"%.3f ",   &urms_datafile.mainline_lead_occ[0], BASE_FLOAT, REPLAY_USE},	//###4
        {"%hhu ",   &db_urms_status.mainline_stat[0].trail_stat, BASE_CHAR, REPLAY_USE},//###5
        {"%hhu ",   &db_urms_status.mainline_stat[0].trail_vol, BASE_CHAR, REPLAY_USE},	//###6
        {"%.3f ",   &urms_datafile.mainline_trail_occ[0], BASE_FLOAT, REPLAY_USE},	//###7
        {"%hhu ",   &db_urms_status.mainline_stat[0].speed, BASE_CHAR, REPLAY_USE},	//###8

        {"%hhu ",   &db_urms_status.mainline_stat[1].lead_stat, BASE_CHAR, REPLAY_USE},	//###9
        {"%hhu ",   &db_urms_status.mainline_stat[1].lead_vol, BASE_CHAR, REPLAY_USE},	//###10
        {"%.3f ",   &urms_datafile.mainline_lead_occ[1], BASE_FLOAT, REPLAY_USE},	//###11
        {"%hhu ",   &db_urms_status.mainline_stat[1].trail_stat, BASE_CHAR, REPLAY_USE},//###12
        {"%hhu ",   &db_urms_status.mainline_stat[1].trail_vol, BASE_CHAR, REPLAY_USE},	//###13
        {"%.3f ",   &urms_datafile.mainline_trail_occ[1], BASE_FLOAT, REPLAY_USE},	//###14
        {"%hhu ",   &db_urms_status.mainline_stat[1].speed, BASE_CHAR, REPLAY_USE},	//###15

        {"%hhu ",   &db_urms_status.mainline_stat[2].lead_stat, BASE_CHAR, REPLAY_USE},	//###16
        {"%hhu ",   &db_urms_status.mainline_stat[2].lead_vol, BASE_CHAR, REPLAY_USE},	//###17
        {"%.3f ",   &urms_datafile.mainline_lead_occ[2], BASE_FLOAT, REPLAY_USE},	//###18
        {"%hhu ",   &db_urms_status.mainline_stat[2].trail_stat, BASE_CHAR, REPLAY_USE},//###19
        {"%hhu ",   &db_urms_status.mainline_stat[2].trail_vol, BASE_CHAR, REPLAY_USE},	//###20
        {"%.3f ",   &urms_datafile.mainline_trail_occ[2], BASE_FLOAT, REPLAY_USE},	//###21
        {"%hhu ",   &db_urms_status.mainline_stat[2].speed, BASE_CHAR, REPLAY_USE},	//###22

        {"%hhu ",   &db_urms_status.mainline_stat[3].lead_stat, BASE_CHAR, REPLAY_USE},	//###23
        {"%hhu ",   &db_urms_status.mainline_stat[3].lead_vol, BASE_CHAR, REPLAY_USE},	//###24
        {"%.3f ",   &urms_datafile.mainline_lead_occ[3], BASE_FLOAT, REPLAY_USE},	//###25
        {"%hhu ",   &db_urms_status.mainline_stat[3].trail_stat, BASE_CHAR, REPLAY_USE},//###26
        {"%hhu ",   &db_urms_status.mainline_stat[3].trail_vol, BASE_CHAR, REPLAY_USE},	//###27
        {"%.3f ",   &urms_datafile.mainline_trail_occ[3], BASE_FLOAT, REPLAY_USE},	//###28
        {"%hhu ",   &db_urms_status.mainline_stat[3].speed, BASE_CHAR, REPLAY_USE},	//###29

        {"%hhu ",   &db_urms_status.mainline_stat[4].lead_stat, BASE_CHAR, REPLAY_USE},	//###30
        {"%hhu ",   &db_urms_status.mainline_stat[4].lead_vol, BASE_CHAR, REPLAY_USE},	//###31
        {"%.3f ",   &urms_datafile.mainline_lead_occ[4], BASE_FLOAT, REPLAY_USE},	//###32
        {"%hhu ",   &db_urms_status.mainline_stat[4].trail_stat, BASE_CHAR, REPLAY_USE},//###33
        {"%hhu ",   &db_urms_status.mainline_stat[4].trail_vol, BASE_CHAR, REPLAY_USE},	//###34
        {"%.3f ",   &urms_datafile.mainline_trail_occ[4], BASE_FLOAT, REPLAY_USE},	//###35
        {"%hhu ",   &db_urms_status.mainline_stat[4].speed, BASE_CHAR, REPLAY_USE},	//###36

        {"%hhu ",   &db_urms_status.mainline_stat[5].lead_stat, BASE_CHAR, REPLAY_USE},	//###37
        {"%hhu ",   &db_urms_status.mainline_stat[5].lead_vol, BASE_CHAR, REPLAY_USE},	//###38
        {"%.3f ",   &urms_datafile.mainline_lead_occ[5], BASE_FLOAT, REPLAY_USE},	//###39
        {"%hhu ",   &db_urms_status.mainline_stat[5].trail_stat, BASE_CHAR, REPLAY_USE},//###40
        {"%hhu ",   &db_urms_status.mainline_stat[5].trail_vol, BASE_CHAR, REPLAY_USE},	//###41
        {"%.3f ",   &urms_datafile.mainline_trail_occ[5], BASE_FLOAT, REPLAY_USE},	//###42
        {"%hhu ",   &db_urms_status.mainline_stat[5].speed, BASE_CHAR, REPLAY_USE},	//###43

        {"%hhu ",   &db_urms_status.metered_lane_stat[0].demand_vol, BASE_CHAR, REPLAY_USE},	//###44
        {"%hhu ",   &db_urms_status.metered_lane_stat[0].passage_vol, BASE_CHAR, REPLAY_USE},	//###45
        {"%.3f ",   &urms_datafile.queue_occ[0], BASE_FLOAT, REPLAY_USE},	//###46
        {"%hhu ",   &db_urms_status.queue_stat[0].vol, BASE_CHAR, REPLAY_USE},	//###47
        {"%hu ",   &urms_datafile.metering_rate[0], BASE_SHORT, REPLAY_USE},	//###48
        {"%hhu ",   &db_urms_status.cmd_src[0], BASE_CHAR, REPLAY_USE},		//###49
        {"%hhu ",   &db_urms_status.action[0], BASE_CHAR, REPLAY_USE},		//###50
        {"%hhu ",   &db_urms_status.plan[0], BASE_CHAR, REPLAY_USE},		//###51

        {"%hhu ",   &db_urms_status.metered_lane_stat[1].demand_vol, BASE_CHAR, REPLAY_USE},	//###52
        {"%hhu ",   &db_urms_status.metered_lane_stat[1].passage_vol, BASE_CHAR, REPLAY_USE},	//###53
        {"%.3f ",   &urms_datafile.queue_occ[1], BASE_FLOAT, REPLAY_USE},	//###54
        {"%hhu ",   &db_urms_status.queue_stat[1].vol, BASE_CHAR, REPLAY_USE},	//###55
        {"%hu ",    &urms_datafile.metering_rate[1], BASE_SHORT, REPLAY_USE},	//###56
        {"%hhu ",   &db_urms_status.cmd_src[1], BASE_CHAR, REPLAY_USE},		//###57
        {"%hhu ",   &db_urms_status.action[1], BASE_CHAR, REPLAY_USE},		//###58
        {"%hhu ",   &db_urms_status.plan[1], BASE_CHAR, REPLAY_USE},		//###59

        {"%hhu ",   &db_urms_status.metered_lane_stat[2].demand_vol, BASE_CHAR, REPLAY_USE},	//###60
        {"%hhu ",   &db_urms_status.metered_lane_stat[2].passage_vol, BASE_CHAR, REPLAY_USE},	//###61
        {"%.3f ",   &urms_datafile.queue_occ[2], BASE_FLOAT, REPLAY_USE},	//###62
        {"%hhu ",   &db_urms_status.queue_stat[2].vol, BASE_CHAR, REPLAY_USE},	//###63
        {"%hu ",    &urms_datafile.metering_rate[2], BASE_SHORT, REPLAY_USE},	//###64
        {"%hhu ",   &db_urms_status.cmd_src[2], BASE_CHAR, REPLAY_USE},		//###65
        {"%hhu ",   &db_urms_status.action[2], BASE_CHAR, REPLAY_USE},		//###66
        {"%hhu ",   &db_urms_status.plan[2], BASE_CHAR, REPLAY_USE},		//###67

        {"%hhu ",   &db_urms_status.additional_det[0].volume, BASE_CHAR, REPLAY_USE},	//###68
        {"%hhu ",   &db_urms_status.additional_det[0].occ_msb, BASE_CHAR, REPLAY_USE},	//###69
        {"%hhu ",   &db_urms_status.additional_det[0].occ_lsb, BASE_CHAR, REPLAY_USE},	//###70
        {"%hhu ",   &db_urms_status.additional_det[0].stat, BASE_CHAR, REPLAY_USE},	//###71

        {"%hhu ",   &db_urms_status.additional_det[1].volume, BASE_CHAR, REPLAY_USE},	//###72
        {"%hhu ",   &db_urms_status.additional_det[1].occ_msb, BASE_CHAR, REPLAY_USE},	//###73
        {"%hhu ",   &db_urms_status.additional_det[1].occ_lsb, BASE_CHAR, REPLAY_USE},	//###74
        {"%hhu ",   &db_urms_status.additional_det[1].stat, BASE_CHAR, REPLAY_USE},	//###75


        {"%hhu ",   &db_urms_status.rm2rmc_ctr, BASE_CHAR, REPLAY_USE},		//###76
	{"%hhu ",   &db_urms_status.is_metering, BASE_CHAR, REPLAY_USE},	//###77
	{"%hhu ",   &db_urms_status.computation_finished, BASE_CHAR, REPLAY_USE}, //###78
	{"%hx ",    &db_urms_status.checksum, BASE_CHAR, REPLAY_USE}, 		//###89
        {"%hu ",    &urms_datafile.metering_rate[2], BASE_SHORT, REPLAY_USE},	//###80
	{"%.1f ",   &db_ramp_data.new_meter_rate, BASE_FLOAT, REPLAY_USE}, 	//###81
	{"%.1f ",   &db_ramp_data.data_time, BASE_FLOAT, REPLAY_USE}, 		//###82
	{"%.1f ",   &db_ramp_data.prev_update_data, BASE_FLOAT, REPLAY_USE}, 	//###83
	{"%.3f ",   &db_ramp_data.prev_occ_out, BASE_FLOAT, REPLAY_USE}, 	//###84
	{"%hhu ",   &db_ramp_data.passage_vol[2], BASE_CHAR, REPLAY_USE}, 	//###85
	{"%hhu ",   &db_urms_status.plan_base_lvl[0], BASE_CHAR, REPLAY_USE},	//###86
	{"%hhu ",   &db_urms_status.plan_base_lvl[1], BASE_CHAR, REPLAY_USE},	//###87
	{"%hhu ",   &db_urms_status.plan_base_lvl[2], BASE_CHAR, REPLAY_USE},	//###88
	{"%hhu ",   &db_urms_status.hour, BASE_CHAR, REPLAY_USE},		//###89
	{"%hhu ",   &db_urms_status.minute, BASE_CHAR, REPLAY_USE},		//###90
	{"%hhu ",   &db_urms_status.second, BASE_CHAR, REPLAY_USE},		//###91
	{"%hhu ",   &db_urms_status.num_main, BASE_CHAR, REPLAY_USE},		//###92
	{"%hhu ",   &db_urms_status.num_meter, BASE_CHAR, REPLAY_USE},		//###93
	{"%hhu ",   &db_urms_status.num_addl_det, BASE_CHAR, REPLAY_USE},	//###94
	{"%hhu ",   &db_urms_status.num_opp, BASE_CHAR, REPLAY_USE},		//###95
};

#define NUM_FILE_COLUMNS sizeof(file_spec)/sizeof(data_log_column_spec_t)
int num_file_columns = sizeof(file_spec)/sizeof(data_log_column_spec_t);
