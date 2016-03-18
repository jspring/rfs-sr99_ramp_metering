#include <db_include.h>
#include "read_data.h"

float hr,min,sec;
int green_yellow_overlap,presence1,presence2,presence3,presence4,greens,yellows,reds;
        //redudant
char c1,c2;
unsigned int a1,a2,a3,a4,a5,a6,a7,a8;
int lead_stat_0,lead_vol_0,trail_stat_0,trail_vol_0;
int lead_stat_1,lead_vol_1,trail_stat_1,trail_vol_1;
int lead_stat_2,lead_vol_2,trail_stat_2,trail_vol_2;
float lead_occ_0,trail_occ_0;
float lead_occ_1,trail_occ_1;
float lead_occ_2,trail_occ_2;

int demand_vol_2,passage_vol_2,queue_vol_2,b1_2,b2_2,b3_2;
float queue_occ_2;
int demand_vol_1,passage_vol_1,queue_vol_1,b1_1,b2_1,b3_1;
float queue_occ_1;
int demand_vol_0,passage_vol_0,queue_vol_0,b1_0,b2_0,b3_0;
float queue_occ_0;
int rate_0,rate_1,rate_2;

int get_new_data(char str[],struct signal_variables *psignal_data,struct ramp_variables *pramp_data)
{
	sscanf(str,"%f%c%f%c%f%hhu%hhu%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx%hhx %hhu%hhu%f%hhu%hhu%f %hhu%hhu%f%hhu%hhu%f %hhu%hhu%hhu%f%hhu%f %hhu%hhu%f%hhu%hu%hhu%hhu%hhu %hhu%hhu%f%hhu%hu%hhu%hhu%hhu %hhu%hhu%f%hhu%hu%hhu%hhu%hhu",
	&hr,&c1,&min,&c2,&sec,&a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8,&green_yellow_overlap,&presence1,&presence2,&presence3,&presence4,&greens,&yellows,&reds,
	&lead_stat_0,&lead_vol_0,&lead_occ_0,&trail_stat_0,&trail_vol_0,&trail_occ_0,
	&lead_stat_1,&lead_vol_1,&lead_occ_1,&trail_stat_1,&trail_vol_1,&trail_occ_1,
	&lead_stat_2,&lead_vol_2,&trail_stat_2,&lead_occ_2,&trail_vol_2,&trail_occ_2,
	&demand_vol_0,&passage_vol_0,&queue_occ_0,&queue_vol_0,&rate_0,&b1_0,&b2_0,&b3_0,
	&demand_vol_1,&passage_vol_1,&queue_occ_1,&queue_vol_1,&rate_1,&b1_1,&b2_1,&b3_1,
	&demand_vol_2,&passage_vol_2,&queue_occ_2,&queue_vol_2,&rate_2,&b1_2,&b2_2,&b3_2);

	if(lead_vol_0!=pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][0] || lead_vol_1!=pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][1] || lead_vol_2!=pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][2] || 
		trail_vol_0!=pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][0] || trail_vol_1!=pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][1] || trail_vol_2!=pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][2] )
	{
		if(lead_stat_0==2 || lead_stat_1==2 || lead_stat_2==2)
		;
//			ramp_flag = 1;
	}
//	if(get_phase6_signal_status()==2 && old_phase6_signal_status==1)
//		signal_flag = 1;
	else 
	{
//		signal_flag = 0;;
		if (get_phase6_signal_status()==1)
		;
//			old_phase6_signal_status=1;
	}

	return 0;
}

boolean check_mainline_occ_health_lead(struct ramp_variables* pramp_data,int lane_id)
{
        int i = NUMBER_RAMP_DATA-1;
        int j;
        int count;
        boolean flag;

        if(pramp_data->mainline_lead_status[i][lane_id]!=MAINLINE_DETECTOR_WORKING)
                return false;

        count = 0;
        flag = false;
        for(j=1;j<=i;j++)
        {
                if( pramp_data->data_time[j]< EPS )
                        continue;
                if( fabs(pramp_data->mainline_lead_occ[j][lane_id]-pramp_data->mainline_lead_occ[j-1][lane_id]) <= THRESHOLD_OCC_HEALTH*pramp_data->mainline_lead_occ[j-1][lane_id] )
                {
                        count++;
                        if(j==i)
                                flag = true;
                }
        }
        if( count> i/2 && flag )
                return false;
        else
                return true;
}

boolean check_mainline_occ_health_trail(struct ramp_variables* pramp_data,int lane_id)
{
        int i = NUMBER_RAMP_DATA-1;
        int j;
        int count;
        boolean flag;
        float tabs;

        if(pramp_data->mainline_trail_status[i][lane_id]!=MAINLINE_DETECTOR_DISABLE)
                return false;

        count = 0;
        flag = false;
        for(j=1;j<=i;j++)
        {
                if( pramp_data->data_time[j]< EPS )
                        continue;
                if( fabs(pramp_data->mainline_trail_occ[j][lane_id]-pramp_data->mainline_trail_occ[j-1][lane_id])<=THRESHOLD_OCC_HEALTH*pramp_data->mainline_trail_occ[j-1][lane_id] )
                {
                        tabs=fabs(pramp_data->mainline_trail_occ[j][lane_id]-pramp_data->mainline_trail_occ[j-1][lane_id]);
                        count++;
                        if(j==i)
                                flag = true;
                }
        }
        if( count> i/2 && flag )
                return false;
        else
                return true;
}

boolean check_mainline_vol_health_lead(struct ramp_variables* pramp_data,int lane_id)
{
        int i = NUMBER_RAMP_DATA-1;
        int j;
        int count;
        boolean flag;

        if(pramp_data->mainline_lead_status[i][lane_id]!=MAINLINE_DETECTOR_WORKING)
                return false;

        count = 0;
        flag = false;
        for(j=1;j<=i;j++)
        {
                if( pramp_data->data_time[j]< EPS )
                        continue;
                if( fabs(pramp_data->mainline_lead_vol[j][lane_id]-pramp_data->mainline_lead_vol[j-1][lane_id])<=THRESHOLD_VOL_HEALTH*pramp_data->mainline_lead_vol[j-1][lane_id] )
                {
                        count++;
                        if(j==i)
                                flag = true;
                }
        }
        if( count> i/2 && flag )
                return false;
        else
                return true;
}

boolean check_mainline_vol_health_trail(struct ramp_variables* pramp_data,int lane_id)
{
        int i = NUMBER_RAMP_DATA-1;
        int j;
        int count;
        boolean flag;

        if(pramp_data->mainline_trail_status[i][lane_id]!=MAINLINE_DETECTOR_DISABLE)
                return false;

        count = 0;
        flag = false;
        for(j=1;j<=i;j++)
        {
                if( pramp_data->data_time[j]< EPS )
                        continue;
                if( fabs(pramp_data->mainline_trail_vol[j][lane_id]-pramp_data->mainline_trail_vol[j-1][lane_id])<=THRESHOLD_VOL_HEALTH*pramp_data->mainline_trail_vol[j-1][lane_id] )
                {
                        count++;
                        if(j==i)
                                flag = true;
                }
        }
        if( count> i/2 && flag )
                return false;
        else
                return true;
}


float get_mainline_average_occupancy(struct ramp_variables* pramp_data)
{
        int i = NUMBER_RAMP_DATA-1;
        int lane_id;
        int lead_health,trail_health;
        for(lane_id=0;lane_id<NUMBER_MAINLINE_LANE;lane_id++)
        {
                lead_health = check_mainline_occ_health_lead(pramp_data, lane_id);
                trail_health = check_mainline_occ_health_trail(pramp_data,lane_id);
                if ( lead_health && trail_health )
                        pramp_data->mainline_avg_occupancy[lane_id] = 0.5*(pramp_data->mainline_lead_occ[i][lane_id]+pramp_data->mainline_trail_occ[i][lane_id]);
                else if (lead_health)
                        pramp_data->mainline_avg_occupancy[lane_id] = pramp_data->mainline_lead_occ[i][lane_id];
                else if (trail_health)
                        pramp_data->mainline_avg_occupancy[lane_id] = pramp_data->mainline_trail_occ[i][lane_id];
        }
        return 0;
}

float get_mainline_average_volume(struct ramp_variables* pramp_data)
{
        int i = NUMBER_RAMP_DATA-1;
        int lane_id;
        int lead_health,trail_health;
        for(lane_id=0;lane_id<NUMBER_MAINLINE_LANE;lane_id++)
        {
                lead_health = check_mainline_vol_health_lead(pramp_data, lane_id);
                trail_health = check_mainline_vol_health_trail(pramp_data,lane_id);
                if ( lead_health && trail_health )
                        pramp_data->mainline_avg_volume[lane_id] = (int) (0.5*(pramp_data->mainline_lead_vol[i][lane_id]+pramp_data->mainline_trail_vol[i][lane_id]));
                else if (lead_health)
                        pramp_data->mainline_avg_volume[lane_id] = pramp_data->mainline_lead_vol[i][lane_id];
                else if (trail_health)
                        pramp_data->mainline_avg_volume[lane_id] = pramp_data->mainline_trail_vol[i][lane_id];
        }
        return 0;
}

int move_ramp_data_forward(struct ramp_variables* pramp_data)
{
	int i,j;
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_status[i][j]=pramp_data->mainline_lead_status[i+1][j];

	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_status[i][j]=pramp_data->mainline_trail_status[i+1][j];
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_vol[i][j]=pramp_data->mainline_lead_vol[i+1][j];			
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_vol[i][j]=pramp_data->mainline_trail_vol[i+1][j];			
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_lead_occ[i][j]=pramp_data->mainline_lead_occ[i+1][j];			
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_MAINLINE_LANE;j++)
			pramp_data->mainline_trail_occ[i][j]=pramp_data->mainline_trail_occ[i+1][j];


	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->passage_vol[i][j]=pramp_data->passage_vol[i+1][j];
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->demand_vol[i][j]=pramp_data->demand_vol[i+1][j];		
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->queue_vol[i][j]=pramp_data->queue_vol[i+1][j];
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->queue_occ[i][j]=pramp_data->queue_occ[i+1][j];			
			
	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		for(j=0;j<NUMBER_METERED_LANE;j++)
			pramp_data->meter_rate[i][j]=pramp_data->meter_rate[i+1][j];	

	for(i=0;i<NUMBER_RAMP_DATA-1;i++)
		pramp_data->data_time[i]=pramp_data->data_time[i+1];


	get_update_time_ramp(pramp_data);





	return 0;
}

int get_new_ramp_data(struct ramp_variables* pramp_data)
{
	int i = NUMBER_RAMP_DATA-1;
	
	pramp_data->mainline_lead_status[i][0] = get_status_main_lead_1();
	pramp_data->mainline_lead_status[i][1] = get_status_main_lead_2();
	pramp_data->mainline_lead_status[i][2] = get_status_main_lead_3();

	pramp_data->mainline_trail_status[i][0] = get_status_main_trail_1();
	pramp_data->mainline_trail_status[i][1] = get_status_main_trail_2();
	pramp_data->mainline_trail_status[i][2] = get_status_main_trail_3();

	pramp_data->mainline_lead_vol[i][0] = get_vol_main_lead_1();
	pramp_data->mainline_lead_vol[i][1] = get_vol_main_lead_2();
	pramp_data->mainline_lead_vol[i][2] = get_vol_main_lead_3();

	pramp_data->mainline_trail_vol[i][0] = get_vol_main_trail_1();
	pramp_data->mainline_trail_vol[i][1] = get_vol_main_trail_2();
	pramp_data->mainline_trail_vol[i][2] = get_vol_main_trail_3();

	pramp_data->mainline_lead_occ[i][0] = get_occ_main_lead_1();
	pramp_data->mainline_lead_occ[i][1] = get_occ_main_lead_2();
	pramp_data->mainline_lead_occ[i][2] = get_occ_main_lead_3();

	pramp_data->mainline_trail_occ[i][0] = get_occ_main_trail_1();
	pramp_data->mainline_trail_occ[i][1] = get_occ_main_trail_2();
	pramp_data->mainline_trail_occ[i][2] = get_occ_main_trail_3();

	pramp_data->passage_vol[i][0] = get_vol_metered_passage_1();
	pramp_data->passage_vol[i][1] = get_vol_metered_passage_2();
	pramp_data->passage_vol[i][2] = get_vol_metered_passage_3();

	pramp_data->demand_vol[i][0] = get_vol_metered_demand_1();
	pramp_data->demand_vol[i][1] = get_vol_metered_demand_2();
	pramp_data->demand_vol[i][2] = get_vol_metered_demand_3();

	pramp_data->queue_vol[i][0] = get_vol_metered_queue_1();
	pramp_data->queue_vol[i][1] = get_vol_metered_queue_2();
	pramp_data->queue_vol[i][2] = get_vol_metered_queue_3();

	pramp_data->queue_occ[i][0] = get_occ_metered_queue_1();
	pramp_data->queue_occ[i][1] = get_occ_metered_queue_2();
	pramp_data->queue_occ[i][2] = get_occ_metered_queue_3();

	pramp_data->meter_rate[i][0] = get_rate_metered_1();
	pramp_data->meter_rate[i][1] = get_rate_metered_2();
	pramp_data->meter_rate[i][2] = get_rate_metered_3();

	pramp_data->data_time[i] = get_data_time();

	return 0;
}

int check_new_ramp_data(struct ramp_variables* pramp_data)
{
	int i,j;
	i = NUMBER_RAMP_DATA-1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_lead_status[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_trail_status[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_lead_vol[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_trail_vol[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_lead_occ[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		if(pramp_data->mainline_trail_occ[i][j]<0)
			return 1;

	for(j=0;j<NUMBER_METERED_LANE;j++)
		if(pramp_data->passage_vol[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_METERED_LANE;j++)
		if(pramp_data->demand_vol[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_METERED_LANE;j++)
		if(pramp_data->queue_vol[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_METERED_LANE;j++)
		if(pramp_data->queue_occ[i][j]<0)
			return 1;
	for(j=0;j<NUMBER_METERED_LANE;j++)
		if(pramp_data->meter_rate[i][j]<0)
			return 1;

	return 0;
}

int replace_ramp_data(struct ramp_variables* pramp_data)
{
	int i,j;
	i = NUMBER_RAMP_DATA-1;
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_lead_status[i][j] = pramp_data->mainline_lead_status[i-1][j];
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_trail_status[i][j] = pramp_data->mainline_trail_status[i-1][j];
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_lead_vol[i][j] = pramp_data->mainline_lead_vol[i-1][j];
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_trail_vol[i][j] = pramp_data->mainline_trail_vol[i-1][j];
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_lead_occ[i][j] = pramp_data->mainline_lead_occ[i-1][j];
	for(j=0;j<NUMBER_MAINLINE_LANE;j++)
		pramp_data->mainline_trail_occ[i][j] = pramp_data->mainline_trail_occ[i-1][j];

	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->passage_vol[i][j] = pramp_data->passage_vol[i-1][j];
	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->demand_vol[i][j] = pramp_data->demand_vol[i-1][j];
	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->queue_vol[i][j] = pramp_data->queue_vol[i-1][j];
	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->queue_occ[i][j] = pramp_data->queue_occ[i-1][j];
	for(j=0;j<NUMBER_METERED_LANE;j++)
		pramp_data->meter_rate[i][j] = pramp_data->meter_rate[i-1][j];

	return 0;
}

int is_value_negative()
{
	if(get_occ_main_lead_1()<0)
		return 1;
	if(get_occ_main_lead_2()<0)
		return 1;
	if(get_occ_main_lead_3()<0)
		return 1;
	if(get_occ_main_trail_1()<0)
		return 1;
	if(get_occ_main_trail_2()<0)
		return 1;
	if(get_occ_main_trail_3()<0)
		return 1;

	if(get_vol_main_lead_1()<0)
		return 1;
	if(get_vol_main_lead_2()<0)
		return 1;
	if(get_vol_main_lead_3()<0)
		return 1;
	if(get_vol_main_trail_1()<0)
		return 1;
	if(get_vol_main_trail_2()<0)
		return 1;
	if(get_vol_main_trail_3()<0)
		return 1;

	if(get_vol_metered_queue_1()<0)
		return 1;
	if(get_vol_metered_queue_2()<0)
		return 1;
	if(get_vol_metered_queue_3()<0)
		return 1;
	if(get_occ_metered_queue_1()<0)
		return 1;
	if(get_occ_metered_queue_2()<0)
		return 1;
	if(get_occ_metered_queue_3()<0)
		return 1;

	if(get_vol_metered_passage_1()<0)
		return 1;
	if(get_vol_metered_passage_2()<0)
		return 1;
	if(get_vol_metered_passage_3()<0)
		return 1;

	if(get_vol_metered_demand_1()<0)
		return 1;
	if(get_vol_metered_demand_2()<0)
		return 1;
	if(get_vol_metered_demand_3()<0)
		return 1;

	return 0;
}




int read_ramp_data(struct ramp_variables* pramp_data)
{
	move_ramp_data_forward(pramp_data);
	if(is_value_negative())
		return 1;
	get_new_ramp_data(pramp_data);
	if(check_new_ramp_data(pramp_data))
		replace_ramp_data(pramp_data);
	get_mainline_average_occupancy(pramp_data);
	get_mainline_average_volume(pramp_data);
	return 0;
}


int read_real_time_data(struct signal_variables* psignal_data)
{
	psignal_data->data_row++;
	psignal_data->realtime_data[psignal_data->data_row][PHASE5SIGNAL]=get_phase5_signal_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE5APPROACH1]=get_phase5_approach1_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE5APPROACH2]=get_phase5_approach2_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE5STOPBAR]=get_phase5_stopbar_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE8SIGNAL]=get_phase8_signal_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE8APPROACH]=get_phase8_approach_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE8STOPBAR]=get_phase8_stopbar_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE6SIGNAL]=get_phase6_signal_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE7SIGNAL]=get_phase7_signal_status();
	psignal_data->realtime_data[psignal_data->data_row][PHASE3SIGNAL]=get_phase3_signal_status();
	return 0;
}
