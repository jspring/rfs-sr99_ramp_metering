#ifndef _READ_DATA_H_
#define _READ_DATA_H_

#include "temp_global.h"
#include "variables2.h"
#include "variables.h"
#include "interface.h"

extern	float hr,min,sec;
extern	int green_yellow_overlap,presence1,presence2,presence3,presence4,greens,yellows,reds;
//redudant
extern	char c1,c2;
extern	unsigned int a1,a2,a3,a4,a5,a6,a7,a8;
extern	int lead_stat_0,lead_vol_0,trail_stat_0,trail_vol_0;
extern	int lead_stat_1,lead_vol_1,trail_stat_1,trail_vol_1;
extern	int lead_stat_2,lead_vol_2,trail_stat_2,trail_vol_2;
extern	float lead_occ_0,trail_occ_0;
extern	float lead_occ_1,trail_occ_1;
extern	float lead_occ_2,trail_occ_2;
extern	int demand_vol_2,passage_vol_2,queue_vol_2,b1_2,b2_2,b3_2;
extern	float queue_occ_2;
extern	int demand_vol_1,passage_vol_1,queue_vol_1,b1_1,b2_1,b3_1;
extern	float queue_occ_1;
extern	int demand_vol_0,passage_vol_0,queue_vol_0,b1_0,b2_0,b3_0;
extern	float queue_occ_0;
extern	int rate_0,rate_1,rate_2;

int get_new_data(char str[],struct signal_variables *psignal_data,struct ramp_variables *pramp_data);

boolean check_mainline_occ_health_lead(struct ramp_variables* pramp_data,int lane_id);

boolean check_mainline_occ_health_trail(struct ramp_variables* pramp_data,int lane_id);

boolean check_mainline_vol_health_lead(struct ramp_variables* pramp_data,int lane_id);

boolean check_mainline_vol_health_trail(struct ramp_variables* pramp_data,int lane_id);

float get_mainline_average_occupancy(struct ramp_variables* pramp_data);

float get_mainline_average_volume(struct ramp_variables* pramp_data);

int move_ramp_data_forward(struct ramp_variables* pramp_data);

int get_new_ramp_data(struct ramp_variables* pramp_data);

int check_new_ramp_data(struct ramp_variables* pramp_data);

int replace_ramp_data(struct ramp_variables* pramp_data);

int is_value_negative();

int read_ramp_data(struct ramp_variables* pramp_data);

int read_real_time_data(struct signal_variables* psignal_data);

#endif
