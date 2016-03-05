#include <db_include.h>
#include <db_utils.h>
#include <path_gps_lib.h>
#include <atsc.h>
#include <atsc_clt_vars.h>
#include <msgs.h>
#include <urms.h>
#include "data_log.h"
#include "wrfiles_rm.h"
#include "variables2.h"
#include "rm_algo.h"

db_id_t db_vars_list[] =  {
        {DB_URMS_VAR, sizeof(db_urms_t)},
        {DB_URMS_STATUS_VAR, sizeof(db_urms_status_t)},
        {DB_RAMP_DATA_VAR, sizeof(db_ramp_data_t)},
        {DB_AGG_DATA_VAR, sizeof(db_agg_data_t)},
};
int num_db_vars = sizeof(db_vars_list)/sizeof(db_id_t);
