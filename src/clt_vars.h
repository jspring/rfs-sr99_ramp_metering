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

#define DB_10_253_28_96_PORT_10012_TYPE	2800	
#define DB_10_253_28_96_PORT_10014_TYPE	3000	
#define DB_10_254_25_116_TYPE	3200	
#define DB_10_254_25_101_TYPE	3400	
#define DB_10_254_25_124_TYPE	3600	
#define DB_10_253_28_102_TYPE	3800	
#define DB_10_253_28_104_TYPE	4000	
#define DB_10_253_28_103_TYPE	4200 	
#define DB_10_254_27_67_TYPE	4400 	
#define DB_10_254_27_82_TYPE	4600 	
#define DB_10_254_27_81_TYPE	4800 	
#define DB_10_253_28_105_TYPE 	5000	
#define DB_10_254_28_213_TYPE 	5200	
#define DB_10_254_28_212_TYPE 	5400	
#define DB_10_254_127_2_TYPE 	5600	
#define DB_10_253_28_107_TYPE 	5800	
#define DB_10_253_28_106_TYPE 	6000	
#define DB_10_254_24_156_TYPE 	6200	
#define DB_10_254_24_157_TYPE 	6400	
#define DB_10_254_26_183_TYPE 	6600	
#define DB_10_253_28_110_TYPE 	6800	
#define DB_10_29_248_81_TYPE 	7000	
#define DB_10_253_28_108_TYPE 	7200	
#define DB_10_253_28_128_PORT_1001_TYPE 	7400
#define DB_10_253_28_128_PORT_1002_TYPE 	7600
#define DB_10_253_28_130_TYPE 	7800	
#define DB_10_254_26_103_TYPE 	8000	
#define DB_10_253_28_131_TYPE 	8200	
#define DB_10_253_28_134_TYPE 	8400	

#define DB_10_253_28_96_PORT_10012_VAR	2800	
#define DB_10_253_28_96_PORT_10014_VAR	3000	
#define DB_10_254_25_116_VAR	3200	
#define DB_10_254_25_101_VAR	3400	
#define DB_10_254_25_124_VAR	3600	
#define DB_10_253_28_102_VAR	3800	
#define DB_10_253_28_104_VAR	4000	
#define DB_10_253_28_103_VAR	4200 	
#define DB_10_254_27_67_VAR	4400 	
#define DB_10_254_27_82_VAR	4600 	
#define DB_10_254_27_81_VAR	4800 	
#define DB_10_253_28_105_VAR 	5000	
#define DB_10_254_28_213_VAR 	5200	
#define DB_10_254_28_212_VAR 	5400	
#define DB_10_254_127_2_VAR 	5600	
#define DB_10_253_28_107_VAR 	5800	
#define DB_10_253_28_106_VAR 	6000	
#define DB_10_254_24_156_VAR 	6200	
#define DB_10_254_24_157_VAR 	6400	
#define DB_10_254_26_183_VAR 	6600	
#define DB_10_253_28_110_VAR 	6800	
#define DB_10_29_248_81_VAR 	7000	
#define DB_10_253_28_108_VAR 	7200	
#define DB_10_253_28_128_PORT_1001_VAR 	7400
#define DB_10_253_28_128_PORT_1002_VAR 	7600
#define DB_10_253_28_130_VAR 	7800	
#define DB_10_254_26_103_VAR 	8000	
#define DB_10_253_28_131_VAR 	8200	
#define DB_10_253_28_134_VAR 	8400	

db_id_t db_controller_list[] =  {
	{DB_10_253_28_96_PORT_10012_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_96_PORT_10014_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_25_116_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_25_101_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_25_124_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_102_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_104_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_103_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_67_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_82_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_27_81_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_105_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_28_213_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_28_212_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_127_2_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_107_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_106_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_24_156_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_24_157_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_26_183_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_110_VAR, sizeof(db_urms_status_t)},
	{DB_10_29_248_81_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_108_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_128_PORT_1002_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_128_PORT_1001_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_130_VAR, sizeof(db_urms_status_t)},
	{DB_10_254_26_103_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_131_VAR, sizeof(db_urms_status_t)},
	{DB_10_253_28_134_VAR, sizeof(db_urms_status_t)},


	{DB_10_253_28_96_PORT_10012_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_96_PORT_10014_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_25_116_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_25_101_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_25_124_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_102_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_104_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_103_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_67_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_82_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_27_81_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_105_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_28_213_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_28_212_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_127_2_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_107_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_106_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_24_156_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_24_157_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_26_183_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_110_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_29_248_81_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_108_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_128_PORT_1002_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_128_PORT_1001_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_130_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_254_26_103_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_131_VAR + 1, sizeof(urms_datafile_t)},
	{DB_10_253_28_134_VAR + 1, sizeof(urms_datafile_t)},

	{DB_10_253_28_96_PORT_10012_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_96_PORT_10014_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_25_116_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_25_101_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_25_124_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_102_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_104_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_103_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_67_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_82_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_27_81_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_105_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_28_213_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_28_212_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_127_2_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_107_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_106_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_24_156_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_24_157_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_26_183_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_110_VAR + 2, sizeof(db_urms_t)},
	{DB_10_29_248_81_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_108_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_128_PORT_1002_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_128_PORT_1001_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_130_VAR + 2, sizeof(db_urms_t)},
	{DB_10_254_26_103_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_131_VAR + 2, sizeof(db_urms_t)},
	{DB_10_253_28_134_VAR + 2, sizeof(db_urms_t)},

	{DB_10_253_28_96_PORT_10012_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_96_PORT_10014_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_25_116_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_25_101_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_25_124_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_102_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_104_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_103_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_67_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_82_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_27_81_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_105_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_28_213_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_28_212_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_127_2_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_107_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_106_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_24_156_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_24_157_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_26_183_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_110_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_29_248_81_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_108_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_128_PORT_1002_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_128_PORT_1001_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_130_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_254_26_103_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_131_VAR + 3, sizeof(db_urms_status2_t)},
	{DB_10_253_28_134_VAR + 3, sizeof(db_urms_status2_t)},

	{DB_10_253_28_96_PORT_10012_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_96_PORT_10014_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_25_116_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_25_101_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_25_124_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_102_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_104_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_103_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_67_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_82_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_27_81_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_105_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_28_213_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_28_212_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_127_2_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_107_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_106_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_24_156_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_24_157_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_26_183_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_110_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_29_248_81_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_108_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_128_PORT_1002_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_128_PORT_1001_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_130_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_254_26_103_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_131_VAR + 4, sizeof(db_urms_status3_t)},
	{DB_10_253_28_134_VAR + 4, sizeof(db_urms_status3_t)},

	{DB_10_253_28_96_PORT_10012_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_96_PORT_10014_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_25_116_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_25_101_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_25_124_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_102_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_104_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_103_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_67_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_82_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_27_81_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_105_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_28_213_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_28_212_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_127_2_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_107_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_106_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_24_156_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_24_157_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_26_183_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_110_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_29_248_81_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_108_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_128_PORT_1002_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_128_PORT_1001_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_130_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_254_26_103_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_131_VAR + 5, sizeof(db_ramp_data_t)},
	{DB_10_253_28_134_VAR + 5, sizeof(db_ramp_data_t)},
};
#define NUM_CONTROLLER_VARS (sizeof(db_controller_list)/sizeof(db_id_t))
#define NUM_CONTROLLERS (NUM_CONTROLLER_VARS / 6)


int db_trig_list[] =  {
	DB_10_253_28_96_PORT_10012_VAR,
	DB_10_253_28_96_PORT_10014_VAR,
	DB_10_254_25_116_VAR,
	DB_10_254_25_101_VAR,
	DB_10_254_25_124_VAR,
	DB_10_253_28_104_VAR,
	DB_10_253_28_104_VAR,
	DB_10_253_28_103_VAR,
	DB_10_254_27_67_VAR,
	DB_10_254_27_82_VAR,
	DB_10_254_27_81_VAR,
	DB_10_253_28_105_VAR,
	DB_10_254_28_213_VAR,
	DB_10_254_28_212_VAR,
	DB_10_254_127_2_VAR,
	DB_10_253_28_107_VAR,
	DB_10_253_28_106_VAR,
	DB_10_254_24_156_VAR,
	DB_10_254_24_157_VAR,
	DB_10_254_26_183_VAR,
	DB_10_253_28_110_VAR,
	DB_10_29_248_81_VAR,
	DB_10_253_28_108_VAR,
	DB_10_253_28_128_PORT_1002_VAR,
	DB_10_253_28_128_PORT_1001_VAR,
	DB_10_253_28_130_VAR,
	DB_10_254_26_103_VAR,
	DB_10_253_28_131_VAR,
	DB_10_253_28_134_VAR,
};
#define NUM_TRIG_VARS (sizeof(db_trig_list)/sizeof(int))
