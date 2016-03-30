

#include "rm_table.h"
#include "wrfiles_rm.h"
#include "clt_vars.h"

static int sig_list[] =
{
        SIGINT,
        SIGQUIT,
        SIGTERM,
        SIGALRM,
        ERROR,
};

static jmp_buf exit_env;

static void sig_hand(int code)
{
        if (code == SIGALRM)
                return;
        else
                longjmp(exit_env, code);
}

db_id_t db_vars_list[] =  {
        {0, sizeof(db_urms_status_t)},
        {0, sizeof(urms_datafile_t)},
        {0, sizeof(db_urms_t)},
};
int num_db_vars = sizeof(db_vars_list)/sizeof(db_id_t);


int db_trig_list[] =  {
        DB_URMS_VAR
};
unsigned int num_trig_variables = sizeof(db_trig_list)/sizeof(int);

const char *usage = "-d <Database number (Modulo 4!)> -f <filename> -i (loop interval) -v (verbose)\n"; 

int main(int argc, char *argv[]) {
	char strbuf[500];
	FILE *filestream; 
	char *filename; 
	int use_db = 0;
        int option;
        int exitsig;
        db_clt_typ *pclt;
        char hostname[MAXHOSTNAMELEN+1];
        int interval = 1000;      /// Number of milliseconds between saves
        posix_timer_typ *ptimer;       /* Timing proxy */
        char *domain = DEFAULT_SERVICE; // usually no need to change this
        int xport = COMM_OS_XPORT;      // set correct for OS in sys_os.h
        int verbose = 0;
        int db_urms_status_var = 0;
        extern db_urms_status_t db_urms_status;
        extern urms_datafile_t urms_datafile;


        while ((option = getopt(argc, argv, "d:i:f:v")) != EOF) {
                switch(option) {
                case 'd':
                        db_urms_status_var = atoi(optarg);
			use_db = 1;
                        break;
                case 'i':
                        interval= atoi(optarg);
                        break;
		case 'f':
			filename = strdup(optarg);
			break;
                case 'v':
                        verbose = 1;
                        break;
               default:
                        printf("Usage: %s %s\n", argv[0], usage);
                        exit(EXIT_FAILURE);
                        break;
                }
        }

        db_vars_list[0].id = db_urms_status_var;
        db_vars_list[1].id = db_urms_status_var + 1;
        db_vars_list[2].id = db_urms_status_var + 2;
        db_trig_list[0] = db_urms_status_var + 2;

	if(use_db) {
		get_local_name(hostname, MAXHOSTNAMELEN);
		if ( (pclt = db_list_init(argv[0], hostname, domain, xport, db_vars_list, num_db_vars, db_trig_list, num_trig_variables)) == NULL) {
			printf("Database initialization error in %s.\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		/* Setup a timer for every 'interval' msec. */
		if ((ptimer = timer_init(interval, DB_CHANNEL(pclt) )) == NULL) {
			printf("Unable to initialize wrfiles timer\n");
			exit(EXIT_FAILURE);
		}
	}

	if(( exitsig = setjmp(exit_env)) != 0) {
		db_list_done(pclt, NULL, 0, NULL, 0);
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	memset(strbuf, 0, 500);
	filestream = fopen(filename, "r");
	while(fgets(strbuf, 500, filestream) != NULL) {
		get_data_log_line(strbuf, file_spec, num_file_columns);
		if(verbose) {
			printf("%hhx %.3f num_file_columns %d\n", 
				db_urms_status.mainline_stat[0].lead_stat,
				urms_datafile.mainline_lead_occ[0],
				num_file_columns
			);
			printf("strbuf %s\n", strbuf);
		}
		if(use_db) {
			db_clt_write(pclt, db_urms_status_var, sizeof(db_urms_status_t), &db_urms_status);
			db_clt_write(pclt, db_urms_status_var + 1, sizeof(urms_datafile_t), &urms_datafile);
//			db_clt_write(pclt, DB_RAMP_DATA_VAR, sizeof(db_ramp_data_t), &db_ramp_data);
//			timestamp_t timestamp;
		}
		TIMER_WAIT(ptimer);
	}

	return 0;
}
