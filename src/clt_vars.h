#include <db_include.h>
#include <db_utils.h>
#include <path_gps_lib.h>
#include <atsc.h>
#include <atsc_clt_vars.h>
#include <msgs.h>
#include <ab3418_lib.h>
#include <ab3418comm.h>
#include <urms.h>
#include "data_log.h"
#include "wrfiles_ac_rm.h"
#include "variables2.h"
#include "ac_rm_algo.h"

db_id_t db_vars_list[] =  {
        {DB_URMS_VAR, sizeof(db_urms_t)},
        {DB_URMS_STATUS_VAR, sizeof(db_urms_status_t)},
//	{DB_URMS_DATAFILE_VAR, sizeof(urms_datafile_t)},
        {DB_2070_TIMING_GET_VAR, sizeof(db_timing_get_2070_t)},
        {DB_2070_TIMING_SET_VAR, sizeof(db_timing_set_2070_t)},
        {DB_SHORT_STATUS_VAR, sizeof(get_short_status_resp_t)},
        {DB_PHASE_STATUS_VAR, sizeof(phase_status_t)},
        {DB_TSCP_STATUS_VAR, sizeof(get_long_status8_resp_mess_typ)},
        {DB_PHASE_1_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_2_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_3_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_4_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_5_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_6_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_7_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_PHASE_8_TIMING_VAR, sizeof(phase_timing_t)},
        {DB_RAMP_DATA_VAR, sizeof(db_ramp_data_t)},
        {DB_SIGNAL_DATA_VAR, sizeof(db_signal_data_t)},
};
int num_db_vars = sizeof(db_vars_list)/sizeof(db_id_t);
