#include <db_include.h>
#include <db_utils.h>
#include <path_gps_lib.h>
#include <atsc.h>
#include <atsc_clt_vars.h>
#include <urms.h>
#include "data_log.h"
//#include "wrfiles_rm.h"
#include "variables2.h"
#include "rm_algo.h"

#define DB_10_29_248_108_TYPE	3000	
#define DB_10_254_25_113_TYPE	3200	
#define DB_10_254_25_120_TYPE	3400	
#define DB_10_29_249_46_TYPE	3600	
#define DB_10_29_248_42_TYPE	3800	
#define DB_10_29_248_20_TYPE	4000	
#define DB_10_29_248_128_TYPE	4200 	
#define DB_10_254_27_67_TYPE	4400 	
#define DB_10_254_27_82_TYPE	4600 	
#define DB_10_254_27_81_TYPE	4800 	
#define DB_10_29_248_76_TYPE 	5000	
#define DB_10_254_28_213_TYPE 	5200	
#define DB_10_254_28_212_TYPE 	5400	
#define DB_10_254_28_211_TYPE 	5600	
#define DB_10_29_248_118_TYPE 	5800	
#define DB_10_29_248_52_TYPE 	6000	
#define DB_10_254_24_156_TYPE 	6200	
#define DB_10_254_24_157_TYPE 	6400	
#define DB_10_29_248_185_TYPE 	6600	
#define DB_10_29_248_66_TYPE 	6800	
#define DB_10_29_248_81_TYPE 	7000	
#define DB_10_29_248_213_TYPE 	7200	
#define DB_10_29_248_155_PORT_1002_TYPE 	7400
#define DB_10_29_248_155_PORT_1001_TYPE 	7600
#define DB_10_29_248_124_TYPE 	7800	
#define DB_10_29_248_67_TYPE 	8000	
#define DB_10_29_248_157_TYPE 	8200	
#define DB_10_29_248_56_TYPE 	8400	

#define DB_10_29_248_108_VAR	3000	
#define DB_10_254_25_113_VAR	3200	
#define DB_10_254_25_120_VAR	3400	
#define DB_10_29_249_46_VAR	3600	
#define DB_10_29_248_42_VAR	3800	
#define DB_10_29_248_20_VAR	4000	
#define DB_10_29_248_128_VAR	4200 	
#define DB_10_254_27_67_VAR	4400 	
#define DB_10_254_27_82_VAR	4600 	
#define DB_10_254_27_81_VAR	4800 	
#define DB_10_29_248_76_VAR 	5000	
#define DB_10_254_28_213_VAR 	5200	
#define DB_10_254_28_212_VAR 	5400	
#define DB_10_254_28_211_VAR 	5600	
#define DB_10_29_248_118_VAR 	5800	
#define DB_10_29_248_52_VAR 	6000	
#define DB_10_254_24_156_VAR 	6200	
#define DB_10_254_24_157_VAR 	6400	
#define DB_10_29_248_185_VAR 	6600	
#define DB_10_29_248_66_VAR 	6800	
#define DB_10_29_248_81_VAR 	7000	
#define DB_10_29_248_213_VAR 	7200	
#define DB_10_29_248_155_PORT_1002_VAR 	7400
#define DB_10_29_248_155_PORT_1001_VAR 	7600
#define DB_10_29_248_124_VAR 	7800	
#define DB_10_29_248_67_VAR 	8000	
#define DB_10_29_248_157_VAR 	8200	
#define DB_10_29_248_56_VAR 	8400	

db_id_t db_controller_list[] =  {
	{DB_10_29_248_108_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_25_113_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_25_120_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_249_46_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_42_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_20_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_128_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_67_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_82_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_81_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_76_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_28_213_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_28_212_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_28_211_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_118_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_52_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_24_156_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_24_157_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_185_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_66_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_81_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_213_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_155_PORT_1001_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_155_PORT_1002_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_124_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_67_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_157_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_56_VAR, sizeof(db_urms_status_t)},


	{DB_10_29_248_108_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_25_113_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_25_120_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_249_46_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_42_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_20_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_128_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_67_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_82_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_81_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_76_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_28_213_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_28_212_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_28_211_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_118_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_52_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_24_156_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_24_157_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_185_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_66_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_81_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_213_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_155_PORT_1001_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_155_PORT_1002_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_124_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_67_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_157_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_56_VAR + 1, sizeof(urms_datafile_t)},

	{DB_10_29_248_108_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_25_113_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_25_120_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_249_46_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_42_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_20_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_128_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_67_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_82_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_81_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_76_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_28_213_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_28_212_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_28_211_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_118_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_52_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_24_156_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_24_157_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_185_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_66_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_81_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_213_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_155_PORT_1001_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_155_PORT_1002_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_124_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_67_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_157_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_56_VAR + 2, sizeof(db_urms_t)},

	{DB_10_29_248_108_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_25_113_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_25_120_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_249_46_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_42_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_20_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_128_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_67_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_82_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_81_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_76_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_28_213_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_28_212_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_28_211_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_118_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_52_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_24_156_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_24_157_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_185_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_66_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_81_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_213_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_155_PORT_1001_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_155_PORT_1002_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_124_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_67_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_157_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_56_VAR + 3, sizeof(db_urms_status2_t)},

	{DB_10_29_248_108_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_25_113_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_25_120_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_249_46_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_42_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_20_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_128_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_67_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_82_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_81_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_76_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_28_213_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_28_212_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_28_211_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_118_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_52_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_24_156_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_24_157_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_185_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_66_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_81_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_213_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_155_PORT_1001_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_155_PORT_1002_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_124_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_67_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_157_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_56_VAR + 4, sizeof(db_urms_status3_t)},

	{DB_10_29_248_108_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_25_113_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_25_120_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_249_46_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_42_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_20_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_128_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_67_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_82_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_81_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_76_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_28_213_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_28_212_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_28_211_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_118_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_52_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_24_156_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_24_157_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_185_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_66_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_81_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_213_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_155_PORT_1001_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_155_PORT_1002_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_124_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_67_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_157_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_56_VAR + 5, sizeof(db_ramp_data_t)},
};
#define NUM_CONTROLLER_VARS (sizeof(db_controller_list)/sizeof(db_id_t))


int db_trig_list[] =  {
	DB_10_29_248_108_VAR,
	DB_10_254_25_113_VAR,
	DB_10_254_25_120_VAR,
	DB_10_29_249_46_VAR,
	DB_10_29_248_42_VAR,
	DB_10_29_248_20_VAR,
	DB_10_29_248_128_VAR,
	DB_10_254_27_67_VAR,
	DB_10_254_27_82_VAR,
	DB_10_254_27_81_VAR,
	DB_10_29_248_76_VAR,
	DB_10_254_28_213_VAR,
	DB_10_254_28_212_VAR,
	DB_10_254_28_211_VAR,
	DB_10_29_248_118_VAR,
	DB_10_29_248_52_VAR,
	DB_10_254_24_156_VAR,
	DB_10_254_24_157_VAR,
	DB_10_29_248_185_VAR,
	DB_10_29_248_66_VAR,
	DB_10_29_248_81_VAR,
	DB_10_29_248_213_VAR,
	DB_10_29_248_155_PORT_1001_VAR,
	DB_10_29_248_155_PORT_1002_VAR,
	DB_10_29_248_124_VAR,
	DB_10_29_248_67_VAR,
	DB_10_29_248_157_VAR,
	DB_10_29_248_56_VAR,
};
#define NUM_TRIG_VARS (sizeof(db_trig_list)/sizeof(int))
