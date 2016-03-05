/* ac_rm_algo.c - interconverts arterial signal and ramp meter controller data,
**	processes it with a linear optimizing algorithm, and writes the new
**	timing settings back to the database
*/
#include <db_include.h>
#include "msgs.h"
#include "rm_algo.h"
#include "atsc.h"
#include "urms.h"
#include "atsc_clt_vars.h"
#include <ab3418_lib.h>
#include "ab3418comm.h"
#include "meter_lib.h"
#include "wrfiles_rm.h"
#include "sys_ini.h"

#define _VARIABLES_H_DECLS

static jmp_buf exit_env;

#include "variables2.h"
#include "variables.h"
#include "temp_global.h"
#include "interface.h"
#include "init.h"
#include "read_data.h"
#include "control_compute.h"

long int count = 0;

static void sig_hand( int code )
{
        if( code == SIGALRM )
                return;
        else
                longjmp( exit_env, code );
}

static int sig_list[] =
{
        SIGINT,
        SIGQUIT,
        SIGTERM,
        SIGALRM,
        ERROR,
};

int db_trig_list_algo[] =  {
	DB_TSCP_STATUS_VAR,
	DB_SHORT_STATUS_VAR,
	DB_URMS_STATUS_VAR
};

int NUM_TRIG_VARS_ALGO = sizeof(db_trig_list_algo)/sizeof(int);

FILE *meter_fp;
FILE *signal_fp;
int test(db_ramp_data_t *db_ramp_data, unsigned char signal_flag, unsigned char ramp_flag);
int set_globals(phase_status_t *phase_status, get_long_status8_resp_mess_typ *status, db_urms_status_t *db_urms_status, urms_datafile_t *urms_datafile);

int main( int argc, char *argv[]) {

	char hostname[MAXHOSTNAMELEN];
	char *domain = DEFAULT_SERVICE;
	int xport = COMM_OS_XPORT;
	posix_timer_typ *ptmr;

	db_urms_t db_urms;
//	db_signal_data_t db_signal_data;
	db_ramp_data_t db_ramp_data;
	get_long_status8_resp_mess_typ get_long_status8_resp_mess;
	get_short_status_resp_t short_status;
	urms_datafile_t urms_datafile;
	phase_status_t phase_status;

        int ipc_message_error_ctr = 0;
	phase_timing_t phase_timing;
	unsigned char ramp_flag = 0;
	unsigned char ramp_flag_sav = 0;
	unsigned char signal_flag = 0;
	unsigned char signal_flag_sav = 0;
	struct timespec tp;
	struct tm *ltime;
	unsigned char timeflag = 0;
	int dow;

	int interval = 50;
	int verbose = 0;
	char myflag = 0;

	int option;

#ifdef USE_CONFIG_FILE
	FILE *configfileptr = NULL;
	char *configfilename = NULL;
#endif

        /* Read and interpret any user switches. */
        while ((option = getopt(argc, argv, "vf:i:")) != EOF) {
                switch(option) {
                case 'v':
                        verbose = 1;
                        break;
                case 'i':
                        interval = atoi(optarg);
                        break;
                case 'f':
#ifdef USE_CONFIG_FILE
                        configfilename = strdup(optarg);
#endif
                        break;
                default:
                        printf("Usage: %s\n", argv[0]);
                        printf("    -d <data file> \n");
                        printf("    -v (verbose)\n");
                        exit(EXIT_FAILURE);
                }
        }


	get_local_name(hostname, MAXHOSTNAMELEN);
	if ( ((pclt = db_list_init(argv[0], hostname,
		domain, xport, NULL, 0, 
		NULL, 0)) == NULL))
//		db_trig_list_algo, NUM_TRIG_VARS_ALGO)) == NULL))
		exit(EXIT_FAILURE);
	if (setjmp(exit_env) != 0) {
		db_list_done(pclt, NULL, 0, NULL, 0);
		printf("%s: %d IPC message errors\n", argv[0], 
			ipc_message_error_ctr);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	if ((ptmr = timer_init( interval, ChannelCreate(0))) == NULL) {
		fprintf(stderr, "Unable to initialize delay timer\n");
		exit(EXIT_FAILURE);
	}

#ifdef USE_CONFIG_FILE
        /**********************************/
        /*                                */
        /*          Read from             */
        /*     parameters_tuning.cfg      */
        /*                                */
        /**********************************/

        /* Read from params section */
        if ((configfileptr = get_ini_section(configfilename, "params")) == NULL)
            {
                printf("ac_rm_algo:Cannot get ini file %s, section %s\n",
                   configfilename, "params");
                fflush(stdout);
                return 0;
            }

	MAX_GREEN_UPPERBOUND = (int)get_ini_long( configfileptr, "MAX_GREEN_UPPERBOUND", 24L );
	MAX_GREEN_LOWERBOUND = (int)get_ini_long( configfileptr, "MAX_GREEN_LOWERBOUND", 12L );
	RAMP_LIMIT = (float)get_ini_double( configfileptr, "RAMP_LIMIT", 76.0L );
	LT_OCC_THRESHOLD = (float)get_ini_double( configfileptr, "LT_OCC_THRESHOLD", 0.15 );
	MAX_GREEN_CHANGE = (int)get_ini_long( configfileptr, "MAX_GREEN_CHANGE", 2L );
	RT_RELEASE_OCC_THRESHOLD = (float)get_ini_double( configfileptr, "RT_RELEASE_OCC_THRESHOLD", 0.35 );
	MAX_STEPS = (int)get_ini_long( configfileptr, "MAX_STEPS", 100L );
	THRESHOLD_OCC_HEALTH = (float)get_ini_double( configfileptr, "THRESHOLD_OCC_HEALTH", 0.05 );
	THRESHOLD_VOL_HEALTH = (float)get_ini_double( configfileptr, "THRESHOLD_VOL_HEALTH", 0.1 );
	CYCLE_TO_ACTIVATE_OVERWRITE = (int)get_ini_long( configfileptr, "CYCLE_TO_ACTIVATE_OVERWRITE", 3L );
	CYCLE_TO_RELEASE = (int)get_ini_long( configfileptr, "CYCLE_TO_RELEASE", 3L );
	REGULAR_CYCLE = (int)get_ini_long( configfileptr, "REGULAR_CYCLE", 1L );
	OVERWRITE_CYCLE = (int)get_ini_long( configfileptr, "OVERWRITE_CYCLE", 1L );
	QUEUE_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION = (int)get_ini_long( configfileptr, "QUEUE_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION", 30L );
	LEFT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION = (float)get_ini_double( configfileptr, "LEFT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION", 0.3 );
	RIGHT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION = (float)get_ini_double( configfileptr, "RIGHT_OCC_THRESHOLD_TO_ACTIVATE_INTERSECTION", 0.3 );
	METHOD_FOR_RAMP_QUEUE = (int)get_ini_long( configfileptr, "METHOD_FOR_RAMP_QUEUE", 3L );
	SHOCH_WAVE_SPEED = (float)get_ini_double( configfileptr, "SHOCH_WAVE_SPEED", -15.0 );
	VEHICLE_EFFECTIVE_LENGTH = (float)get_ini_double( configfileptr, "VEHICLE_EFFECTIVE_LENGTH", 0.006 );
	ALINEA_GAMMA = (float)get_ini_double( configfileptr, "ALINEA_GAMMA", 0.2 );
	ALPHA = (float)get_ini_double( configfileptr, "ALPHA", 1.0 );
	ALINEA_LOWER_BOUND = (float)get_ini_double( configfileptr, "ALINEA_LOWER_BOUND", 600.0 );
	ALINEA_UPPER_BOUND = (float)get_ini_double( configfileptr, "ALINEA_UPPER_BOUND", 900.0 );
	QUEUE_RESET = get_ini_bool( configfileptr, "QUEUE_RESET", FALSE);
	QUEUE_RESET_INTERVAL = (int)get_ini_long( configfileptr, "QUEUE_RESET_INTERVAL", 300L );
	RAMP_EXIT_FLOW_ADJUST_FACTOR = (float)get_ini_double( configfileptr, "RAMP_EXIT_FLOW_ADJUST_FACTOR", 0.92 );
	RAMP_METER_CHANGE_THRESHOLD = (float)get_ini_double( configfileptr, "RAMP_METER_CHANGE_THRESHOLD", 0.1 );
	OCC_CRITICAL = (float)get_ini_double( configfileptr, "OCC_CRITICAL", 10.0 );
	IN_LANES = (float)get_ini_double( configfileptr, "IN_LANES", 2.0 );
	OUT_LANES = (float)get_ini_double( configfileptr, "OUT_LANES", 2.0 );
	ALINEA_KR = (float)get_ini_double( configfileptr, "ALINEA_KR", 70.0 );

	fclose(configfileptr);

#endif

	db_clt_read(pclt, DB_PHASE_3_TIMING_VAR, sizeof(phase_timing_t), &phase_timing);
	printf("Phase 3 min green %d max green 1 %d maximum green %d\n",
		phase_timing.min_green,
		phase_timing.max_green1,
		phase_timing.min_green + phase_timing.max_green1
	);
	signal_data.new_max_green = phase_timing.max_green1;
	db_clt_read(pclt, DB_PHASE_7_TIMING_VAR, sizeof(phase_timing_t), &phase_timing);
	printf("Phase 7 min green %d max green 1 %d maximum green %d\n",
		phase_timing.min_green,
		phase_timing.max_green1,
		phase_timing.min_green + phase_timing.max_green1
	);

        init_signal(&signal_data);
        init_realtimedata(&signal_data);
printf("signal_data.new_max_green %d\n", signal_data.new_max_green);
        init_ramp_data(&ramp_data);
	memset(&db_urms, 0, sizeof(db_urms_t));
	memset(&get_long_status8_resp_mess, 0, sizeof(get_long_status8_resp_mess_typ));
	memset(&short_status, 0, sizeof(get_short_status_resp_t));
	memset(&urms_datafile, 0, sizeof(urms_datafile_t));
	memset(&phase_status, 0, sizeof(phase_status_t));

	meter_fp = fopen("meter_result.txt", "w");
	signal_fp = fopen("signal_result.txt", "w");
		db_clt_read(pclt, DB_URMS_STATUS_VAR, sizeof(db_urms_status_t), &db_urms_status);
		if(db_urms_status.plan_base_lvl[0] == 0)
			myflag = 0;
		else
			myflag = 1; 
	while(1) {	
//		clt_ipc_receive(pclt, &trig_info, sizeof(trig_info));

//		db_clt_read(pclt, DB_URMS_DATAFILE_VAR, sizeof(urms_datafile_t), &urms_datafile);
//		db_clt_read(pclt, 0, sizeof(urms_datafile_t), &urms_datafile);
		db_clt_read(pclt, DB_URMS_STATUS_VAR, sizeof(db_urms_status_t), &db_urms_status);

		set_globals(&phase_status, &get_long_status8_resp_mess, &db_urms_status, &urms_datafile);

		ramp_flag = db_urms_status.computation_finished;
		if((ramp_flag != 0) && (ramp_flag_sav == 0)) {
			ramp_flag = 1;
			ramp_flag_sav = 1;
			printf("Got ramp flag!!!!!!\n");
		}
		else {
			if((ramp_flag != 0) && (ramp_flag_sav != 0)) {
				ramp_flag = 0;
			}
			else {
				ramp_flag = 0;
				ramp_flag_sav = 0;
			}
		}

//		if( (ramp_flag != 0) || (signal_flag != 0) )

		test(&db_ramp_data, signal_flag, ramp_flag);

	clock_gettime(CLOCK_REALTIME, &tp);
	ltime = localtime(&tp.tv_sec);
	dow = ltime->tm_wday;
//	printf("dow=%d dow%%6=%d hour %d\n", dow, dow % 6, db_urms_status.hour);
	if( ((dow % 6) != 0) && (db_urms_status.hour >= 15) && (db_urms_status.hour < 19)) { //check time
		if(timeflag == 0) {
			timeflag = 1;
			printf("Enabling control: %s", ctime(&tp.tv_sec));
		}
		//If HOV lane has transitioned from 1->0, set lanes 2 & 3 to REST_IN_GREEN
		if( (db_urms_status.plan_base_lvl[0] == 0) &&
			(myflag == 0))
			{
				myflag = 1;
				printf("%s : Setting lanes 2 & 3 to REST_IN_GREEN\n", ctime(&tp.tv_sec));
				db_urms.lane_1_action = URMS_ACTION_SKIP;
				db_urms.lane_1_release_rate = (db_urms_status.metered_lane_stat[0].metered_lane_rate_msb << 8) + (unsigned char) db_urms_status.metered_lane_stat[0].metered_lane_rate_lsb;
				db_urms.lane_1_plan = db_urms_status.plan[0];
				db_urms.lane_2_release_rate = ramp_data.new_meter_rate;
				db_urms.lane_3_release_rate = ramp_data.new_meter_rate;
				db_urms.lane_2_action = URMS_ACTION_REST_IN_GREEN;
				db_urms.lane_3_action = URMS_ACTION_REST_IN_GREEN;
				db_urms.lane_2_action = 2;
				db_urms.lane_3_action = 2;
				db_urms.lane_2_plan = db_urms_status.plan[1];
				db_urms.lane_3_plan = db_urms_status.plan[2];
				db_clt_write(pclt, DB_URMS_VAR, sizeof(db_urms_t), &db_urms);
			}
		else {
//		If HOV lane has transitioned from 1->1, but lanes 2 & 3 were REST_IN_GREEN 
//		(because of a previous HOV transition 1->0), skip the next block, which would
//		set a fixed rate.
			if( ((db_urms_status.plan_base_lvl[0] == 1) || (db_urms_status.plan_base_lvl[0] == 0)) &&
				(myflag == 1)) {
//				printf("HOV level %d\n", db_urms_status.plan_base_lvl[0]);
			; //Do nothing; HOV has not yet transitioned from level 1 to level 2
			}
		
			else {
			    if(ramp_flag != 0) {
				{
					myflag = 0;
					db_urms.lane_1_action = URMS_ACTION_SKIP;
					db_urms.lane_1_release_rate = (db_urms_status.metered_lane_stat[0].metered_lane_rate_msb << 8) + (unsigned char) db_urms_status.metered_lane_stat[0].metered_lane_rate_lsb;
					db_urms.lane_1_plan = db_urms_status.plan[0];
					db_urms.lane_2_plan = 2;
					db_urms.lane_3_plan = 1;
					if(urms_datafile.metering_rate[0] < ramp_data.new_meter_rate) {
						db_urms.lane_2_release_rate = urms_datafile.metering_rate[0];
						db_urms.lane_3_release_rate = urms_datafile.metering_rate[0];
					}
					else {
						db_urms.lane_2_release_rate = ramp_data.new_meter_rate;
						db_urms.lane_3_release_rate = ramp_data.new_meter_rate;
					}
					db_urms.lane_2_action = URMS_ACTION_FIXED_RATE;
					db_urms.lane_3_action = URMS_ACTION_FIXED_RATE;
					db_clt_write(pclt, DB_URMS_VAR, sizeof(db_urms_t), &db_urms);
					printf("ac_rm_algo: new meter rate %f\n", ramp_data.new_meter_rate);
				}
			    }
			}
//printf("myflag %d\n", myflag);
		}
	}
	else {
		if(timeflag == 1) {
			timeflag = 0;
			printf("Disabling control: %s", ctime(&tp.tv_sec));
		}

	}
	TIMER_WAIT(ptmr);
	}
	fclose(meter_fp);
	fclose(signal_fp);
	return 0;
}

//int test(db_signal_data_t *db_signal_data, db_ramp_data_t *db_ramp_data, unsigned char signal_flag, unsigned char ramp_flag)
int test(db_ramp_data_t *db_ramp_data, unsigned char signal_flag, unsigned char ramp_flag)
{
//	struct signal_variables *psignal_data = &signal_data;
	struct ramp_variables *pramp_data = &ramp_data;
	int flag;
	typedef unsigned char u8;
	typedef unsigned short u16;

	flag = read_ramp_data(pramp_data);
	//get new meter rate
	if(flag==0)
	{
		//Must wait until known good data before checking flag and doing rate calculation
		if( ramp_flag == 1 )
		{
			pramp_data->new_meter_rate = get_ALINEA_rate(pramp_data);
printf("get_ALINEA_rate: new meter rate %.1f\n", pramp_data->new_meter_rate);
		}
	}

	db_ramp_data->mainline_lead_occ[0] = pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_lead_occ[1] = pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_lead_occ[2] = pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][2];

	fprintf(meter_fp,"%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%d\n",
		get_current_time(),
		pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][1], 
		db_ramp_data->mainline_lead_occ[1],
		pramp_data->mainline_lead_occ[NUMBER_RAMP_DATA-1][2],
		pramp_data->new_meter_rate,
		(int)count);
//	printf("lead_occ_0 %.1f lead_occ_1 %.1f lead_occ_2 %.1f\n", lead_occ_0, lead_occ_1, lead_occ_2); 
	fflush(NULL);
	db_ramp_data->mainline_trail_occ[0] = pramp_data->mainline_trail_occ[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_trail_occ[1] = pramp_data->mainline_trail_occ[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_trail_occ[2] = pramp_data->mainline_trail_occ[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->queue_occ[0] = pramp_data->queue_occ[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->queue_occ[1] = pramp_data->queue_occ[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->queue_occ[2] = pramp_data->queue_occ[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->meter_rate[0] = pramp_data->meter_rate[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->meter_rate[1] = pramp_data->meter_rate[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->meter_rate[2] = pramp_data->meter_rate[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->data_time = pramp_data->data_time[NUMBER_RAMP_DATA-1];
	db_ramp_data->prev_update_data = pramp_data->prev_update_data;
	db_ramp_data->prev_occ_out = pramp_data->prev_occ_out;
	db_ramp_data->new_meter_rate = pramp_data->new_meter_rate;

	db_ramp_data->mainline_avg_occupancy[0] = pramp_data->mainline_avg_occupancy[0];
	db_ramp_data->mainline_avg_occupancy[1] = pramp_data->mainline_avg_occupancy[1];
	db_ramp_data->mainline_avg_occupancy[2] = pramp_data->mainline_avg_occupancy[2];

	db_ramp_data->mainline_avg_volume[0] = (u16)pramp_data->mainline_avg_volume[0];
	db_ramp_data->mainline_avg_volume[1] = (u16)pramp_data->mainline_avg_volume[1];
	db_ramp_data->mainline_avg_volume[2] = (u16)pramp_data->mainline_avg_volume[2];

	db_ramp_data->mainline_lead_status[0] = (u8)pramp_data->mainline_lead_status[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_lead_status[1] = (u8)pramp_data->mainline_lead_status[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_lead_status[2] = (u8)pramp_data->mainline_lead_status[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->mainline_trail_status[0] = (u8)pramp_data->mainline_trail_status[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_trail_status[1] = (u8)pramp_data->mainline_trail_status[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_trail_status[2] = (u8)pramp_data->mainline_trail_status[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->mainline_lead_vol[0] = (u8)pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_lead_vol[1] = (u8)pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_lead_vol[2] = (u8)pramp_data->mainline_lead_vol[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->mainline_trail_vol[0] = (u8)pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->mainline_trail_vol[1] = (u8)pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->mainline_trail_vol[2] = (u8)pramp_data->mainline_trail_vol[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->passage_vol[0] = (u8)pramp_data->passage_vol[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->passage_vol[1] = (u8)pramp_data->passage_vol[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->passage_vol[2] = (u8)pramp_data->passage_vol[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->demand_vol[0] = (u8)pramp_data->demand_vol[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->demand_vol[1] = (u8)pramp_data->demand_vol[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->demand_vol[2] = (u8)pramp_data->demand_vol[NUMBER_RAMP_DATA-1][2];

	db_ramp_data->queue_vol[0] = (u8)pramp_data->queue_vol[NUMBER_RAMP_DATA-1][0];
	db_ramp_data->queue_vol[1] = (u8)pramp_data->queue_vol[NUMBER_RAMP_DATA-1][1];
	db_ramp_data->queue_vol[2] = (u8)pramp_data->queue_vol[NUMBER_RAMP_DATA-1][2];
	db_clt_write(pclt, DB_RAMP_DATA_VAR, sizeof(db_ramp_data_t), db_ramp_data);


	//Read signal data
//	read_real_time_data(&signal_data);

//	if(QUEUE_RESET &&  get_current_time() - psignal_data->prev_queue_reset_time >=QUEUE_RESET_INTERVAL && METHOD_FOR_RAMP_QUEUE==1)
//	{
//		reset_ramp_queue(psignal_data);
//		psignal_data->prev_queue_reset_time = get_current_time();
//	}

	return 0;
}


int set_globals(phase_status_t *phase_status, get_long_status8_resp_mess_typ *status, db_urms_status_t *db_urms_status, urms_datafile_t *urms_datafile) {

	hr =  phase_status->ts.hour;
	sec =  phase_status->ts.sec;
	min =  phase_status->ts.min;
  
	greens = (int)(0xFF & phase_status->greens);
	yellows = (int)(0xFF & phase_status->yellows);
	reds = (int)(0xFF & phase_status->reds);
	green_yellow_overlap = (int)(0xFF & status->green_yellow_overlap);
	presence1 = (int)(0xFF & status->presence1);
	presence2 = (int)(0xFF & status->presence2);
	presence3 = (int)(0xFF & status->presence3);
	presence4 = (int)(0xFF & status->presence4);
	
//	printf("set_globals: greens %#hhx yellows %#hhx reds %#hhx\n", greens, yellows, reds);
//	printf("set_globals: green_yellow_overlap %#hhx presence1 %#hhx presence2 %#hhx presence3 %#hhx presence4 %#hhx\n", green_yellow_overlap, presence1, presence2, presence3, presence4);
//      printf("set_globals: urms_datafile->mainline_lead_occ[1] %.1f urms_datafile->mainline_lead_occ[2] %.1f\n", urms_datafile->mainline_lead_occ[1], urms_datafile->mainline_lead_occ[2]);
//      fflush(NULL);


	lead_stat_0 = (int)(0xFF & db_urms_status->mainline_stat[0].lead_stat);
	lead_vol_0 = (int)(0xFF & db_urms_status->mainline_stat[0].lead_vol);
	lead_occ_0 = urms_datafile->mainline_lead_occ[0];
	trail_stat_0 = (int)(0xFF & db_urms_status->mainline_stat[0].trail_stat);
	trail_vol_0 = (int)(0xFF & db_urms_status->mainline_stat[0].trail_vol);
	trail_occ_0 = urms_datafile->mainline_trail_occ[0];
	
	lead_stat_1 = (int)(0xFF & db_urms_status->mainline_stat[1].lead_stat);
	lead_vol_1 = (int)(0xFF & db_urms_status->mainline_stat[1].lead_vol);
	lead_occ_1 = urms_datafile->mainline_lead_occ[1];
	trail_stat_1 = (int)(0xFF & db_urms_status->mainline_stat[1].trail_stat);
	trail_vol_1 = (int)(0xFF & db_urms_status->mainline_stat[1].trail_vol);
	trail_occ_1 = urms_datafile->mainline_trail_occ[1];
	
	lead_stat_2 = (int)(0xFF & db_urms_status->mainline_stat[2].lead_stat);
	lead_vol_2 = (int)(0xFF & db_urms_status->mainline_stat[2].lead_vol);
	lead_occ_2 = urms_datafile->mainline_lead_occ[2];
	trail_stat_2 = (int)(0xFF & db_urms_status->mainline_stat[2].trail_stat);
	trail_vol_2 = (int)(0xFF & db_urms_status->mainline_stat[2].trail_vol);
	trail_occ_2 = urms_datafile->mainline_trail_occ[2];

	demand_vol_0 = (int)(0xFF & db_urms_status->metered_lane_stat[0].demand_vol);
	passage_vol_0 = (int)(0xFF & db_urms_status->metered_lane_stat[0].passage_vol);
	queue_occ_0 = urms_datafile->queue_occ[0];
	queue_vol_0 = (int)(0xFF & db_urms_status->queue_stat[0].vol);
	rate_0 = (int)(0xFF & urms_datafile->metering_rate[0]);

	demand_vol_1 = (int)(0xFF & db_urms_status->metered_lane_stat[1].demand_vol);
	passage_vol_1 = (int)(0xFF & db_urms_status->metered_lane_stat[1].passage_vol);
	queue_occ_1 = urms_datafile->queue_occ[1];
	queue_vol_1 = (int)(0xFF & db_urms_status->queue_stat[1].vol);
	rate_1 = (int)(0xFF & urms_datafile->metering_rate[1]);

	demand_vol_2 = (int)(0xFF & db_urms_status->metered_lane_stat[2].demand_vol);
	passage_vol_2 = (int)(0xFF & db_urms_status->metered_lane_stat[2].passage_vol);
	queue_occ_2 = urms_datafile->queue_occ[2];
	queue_vol_2 = (int)(0xFF & db_urms_status->queue_stat[2].vol);
	rate_2 = (int)(0xFF & urms_datafile->metering_rate[2]);

	return 0;

}
