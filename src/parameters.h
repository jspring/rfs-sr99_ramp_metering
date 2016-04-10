/*****************************************************************************************

    This files defines all the global variables and parameters
	For simulation: usinng Hybrid Metering

******************************************************************************************/
#pragma once

#include <stdio.h>

#define DYNAMIC_BOUNBDS
//#define USE_FIELD_BOUND
//#define DBG_COEFF

#define NUM_ONRAMPS     16
#define NUM_OFFRAMPS    12

#define OPT_SHORT
	
#ifndef OPT_SHORT                      // It was used for all lane coorinated; not used anymore 11_28_14;
	#define NumOnRamp           16	   // the number of onramp; SR99
	#define SecSize             17
	#define OPT_RM               3        //1: Opt RM; 2: Coord_ALINEA;  3: Hybrid: (upstream use default; downstream 11 onramps use CRM)
#endif

#ifdef OPT_SHORT	
	#define NumOnRamp           11	   // the number of onramp; SR99
	#define SecSize             12                // one more than NumOnRam
	#define OPT_RM               3        //1: Opt RM (original all onramps cordinated); 2: Coord_ALINEA;  3: Hybrid (upstream use default; downstream 11 onramps use CRM)
#endif

#define VSL_Update_Step		2           // twice of time length for detection; .g if detection is 60s; VSL will be updated every 120s
#define use_CRM				2           // 1: default; 2: Opt CRM; 3: Coord ALINEA;  4: Hybrid
#define Occ_thresh			20.0        // 04_20_10; 25: 05_12_12; 12.5 used for I-80 which was good; 15.0 used
#define rm_band				450.0		// default: 400
#define reduction_coeff		0.075
#define increase_coeff		0.15
//#define RELEASE_CYCLE_LEN   10.0
#define RELEASE_OCC         70.0
#define DEMAND_CHANGE       1           // 0: no change; 1: selective onramp dmd change; 2: all onramp dmd change // 02_20_14
//#define SWITCHING_METERING  1
#define Gain_Up				8.0
#define Gain_Dn				4.0

//#define CellSize 15
#define ALLvehType          0
#define ShortSecSize		30
#define NumLnDetector       27     //
#define NumCrossDetector    30
#define max_onramp_ln       3
#define max_mainline_ln		5
#define dmd_up_t            (30.0*60.0)     //[s]: 6:30am; this needs to be changed dynamically
#define dmd_dwn_t           (120.0*60.0)    //[s]: 8:00am
#define dmd_change			(0.5)			// demand of each onramp incresaed by 5%
#define dmd_slice_len       300             // 5[min]  or 300[s] per slice
#define max_RM_rt			1080             // 1080.0  // Field used Lane Max Rate value
#define min_RM_rt			100              // Field used value
#define N_interv			15              // number of RM rate based occ threshold intervals
#define RM_RELEASE_CYCLE	2

#define Np 2                // for 2-step MPC
const float T=30/3600.0;  // 30s RM control time interval in [hr]

#define Rou_J 210
#define Gama 0.004
#define QM 7200
static int ISUPDATE=3;         // for VSL update
static int ISUPDATE2=1;
#define Compliance_Level 0.3   // VSL compliance level: 1.0 is full level
#define	exp_flt 0.85           // sensor measure exp filter gain

#define eta 0.5
#define a_tts 1.0              // default: 1.0 OK on 01_01_13;
#define a_ttdM 2.0             //default: 2.0
#define a_ttd 2.0              // default: 2.0
//#define a_w 6.5			       // default: 6.5;   2.0 and 6.5 are the same in effect (1/7/14); not sure what is this? value=?
#define len_str 1280           // for using sprintf_s
#define Omega 20.11             // shockwave back-propagation speed 12.5 mph=20.1km/h
static float a_w[NumOnRamp]={0.0};

#define vsl_gain   15.0        // 20.0 fro I-80;    17=>10; 15.0
#define vsl_p_gain 2.0         // 2.0
#define V_free    112.63         // fre flow speed on I-66
#define Q_ln      2050.0         // lane capacity; 1950 used for I-80
#define	VSL_max	  104.6
#define	VSL_min	  55.0         //10  // 20 & 30 did not work; 15 seems to be good; better than 10
#define gama_3    15.0         // gain of VSL_1 at Sycamore
#define gama_6    15.0         // Lee Highway, Tr 3
#define	alpha6_1  0.618
#define	alpha6_2  0.382

// for OptSolver
#define N (NumOnRamp)*Np
#define M1 (NumOnRamp)*6        /* M1+M2+M3 = M */
#define M2 (NumOnRamp)*2 
#define M3 0
#define NM1M2 (N+M1+M2)
#define M (M1+M2+M3)
#define NP N+1        /* NP >= N+1 */
#define MP M+2        /* MP >= M+2 */
//#define alpha_TTD 65
//#define alpha_TTD_M alpha_TTD*5
// for OptSolver End

static int ControlOn;
static int StateOn=0;
static int StateOff=0;
static int count=1;

static float u[SecSize]={0};    // For all cells
float alpha[SecSize]={0};
float beta_c[SecSize]={0};
static float q[SecSize]={0.0};   // composite mainline flow
static float v[SecSize]={0.0};   // composite speed for each cell
static float o[SecSize]={0.0};   // composite occupancy for each cell
static float qc[SecSize]={7200};      // mainline capacity; assigned in Init()
static float queue[SecSize]={0.0};      // updated with update_queue();
//float s[SecSize]={0.0};          // off-ramp flow  // changed on 03/04/14
static float s[NumOnRamp]={0.0};          // off-ramp flow
static float R[NumOnRamp]={0.0};   // composite onramp flow;  changed on 01/03/13
static float dmd[NumOnRamp]={0.0};          // onramp demand flow;     changed on 01/03/13
static float Q_o[NumOnRamp]={0.0};  // Onramp capacity; total onramp max RM rate
static float Q_min[NumOnRamp]={0.0};  // Total Onramp minimum RM rate
static float ss[NumOnRamp][Np]={0};
static float dd[NumOnRamp][Np]={0};     // onramp dmd
static float pre_w[NumOnRamp]={0};

static float q_main[SecSize]={0.0};     // mainline flow of all cells   
static float u2[SecSize]={0};           // speed to feed into the model

static float pre_rho[SecSize]={0};
static float opt_r[SecSize][Np]={0};
static float metering_rate_change_time=0.0;
static float up_rho[Np]={0};


unsigned int replication=0;
static int detectorNum=0,sectionNum=0,nodeNum=0,centroidNum=0;  // moved from data_save.h and readdetectordata.h; 05_29_13


//const int secIDS[ShortSecSize]={17570,17556,2383,2384,2385,2386,2387,17234,17240,17248,17266,17276,17298,17338,17374,17382,17398,17508,16687,16695,16711,16733,16743,16783,16786,16573,16823,16847,16873,16883}; // Lane-wise detectin for I66
//const float Sec_L_0[ShortSecSize]={1012.9,895.839,796.97,536.87,498.223,533.309,661.85,555.169,1007.45,554.588,455.433,720.441,1251.4,440.151,2108.5,293.6,34.3,393.3,1138.42,371.43,110.26,264.1,458.1,916.8,45,332.92,1168.96,694.9,585.6,367.1};   
//unsigned int PolylineId[SecSize]={620,621,622,623,624,625,626,628};
//const int N_Ln[ShortSecSize]={3,3,3,3,3,3,3,3,3,3,4,4,3,4,4,4,4,4,4,4,5,4,4,4,5,4,5,5,5,4};
//const int CellDetectionId_0[ShortSecSize]={19775,19776,19778,19779,19781,19786,19788,19791,19794,19795,19798,19799,19800,19803,19809,19810,19813,19814,19817,19818,19821,19822,19825,19826,19829,19830,19833,19834,18153,19837};  // 07_09_13

static float dyna_min_r[NumOnRamp]={0.0};
static float dyna_max_r[NumOnRamp]={0.0};
static float Ramp_rt[NumOnRamp]={0.0,0.0,0.0};
static float RM_occ[NumOnRamp]={0.0,0.0,0.0};
static float RM_occ_All[NumOnRamp+5]={0.0,0.0,0.0};
static float ln_metering_rate[NumOnRamp][max_onramp_ln];
const int ln_meteringId[16][max_onramp_ln]={{21044,21045,21046},{19780,0,0},{19782,0,0},{19789,0,0},{21063,21062,21060},
                       {19892,0,19796},{21080,21081,21082},{19924,0,19801},{19936,0,19804},{19811,0,0},{19968,0,19815},
                       {19959,0,19819},{19948,0,19823},{19827,0,0},{19831,0,0},{19836,0,0}};



	static int release_cycle[16][max_onramp_ln]={{0,0,0}};
	static float actual_r[16][max_onramp_ln]={{0.0,0.0,0.0}};
	static float L[SecSize]={555.2,1562.0,455.4,1971.8,440.2,2402.1,427.6,1509.9,374.4,1374.9,377.9,1863.9};  // composite length; including most downstream sec; 1st section has no meter; 10_21_13
	const float Q[SecSize]={2050.0,2000.0,2050.0,2050.0,2050.0,2000.0,2050.0,2050.0,2050.0,2000.0,2000.0,2000.0};	 //onramp flow capacity

	// for downstream 11 onramps only
	const int N_OnRamp_Ln[NumOnRamp]={2,3,2,2,1,2,2,2,1,1,1};  // from upstream to downstream
	const int OnRamp_Ln_Id[NumOnRamp][max_onramp_ln]={{19884,0,17256},{19904,19896,17264},{19916,0,17324},{19928,0,17336},
	                     {17392,0,0},{19964,0,17510},{19955,0,16705},{19940,0,16731},{16785,0,0},{16571,0,0},{16853,0,0}};
	//const int HOV_Ln_Id[NumOnRamp]={3,3,3,3,0,3,3,3,0,0,0};
	const int CellDetectionId[SecSize]={19791,19795,19798,19800,19803,19810,19814,19818,19822,19826,19830,19834};  // 10_24_13; most downstream not detected
	//const int N_Mainline_Ln_RM[NumOnRamp]={3,3,3,3,3,3,4,3,4,4,4};
	//const int Mainline_Ln_Det_Id[NumOnRamp][max_mainline_ln]={{18392,18394,0,0,18390},{18378,18380,0,0,18376},{18371,18373,0,0,18369},{18357,18359,0,0,18355},
	//			{18667,18669,0,0,18665},{18336,18338,0,0,18334},{18327,18329,18331,0,18325},{18311,18309,0,0,18313},
	//			{18302,18304,18306,0,18300},{18275,18277,18279,0,18273},{18266,18268,18270,0,18264}};     // Upstream 11 sections
	
	//for all 16 onramps
	const int N_Mainline_Ln_RM_All[(NumOnRamp+5)]={3,3,3,3,3,3,4,3,4,4,4,4,4,4,4,5};
	const int N_OnRamp_Ln_All[(NumOnRamp+5)]={3,1,1,2,3,2,3,2,2,1,2,2,2,1,1,1};
	const int OnRamp_Ln_Id_All[(NumOnRamp+5)][max_onramp_ln]={{19852,19842,17550},{338,0,0},{336,0,0},{19978,0,2218},{19875,19871,17232},{19884,0,17256},{19904,19896,17264},
                         {19916,0,17324},{19928,0,17336},{17392,0,0},{19964,0,17510},{19955,0,16705},{19940,0,16731},{16785,0,0},{16571,0,0},{16853,0,0}};	                                                                                                                                     
	const int Mainline_Ln_Det_Id_All[(NumOnRamp+5)][max_mainline_ln]={{18392,18394,0,0,18390},{18378,18380,0,0,18376},{18371,18373,0,0,18369},{18357,18359,0,0,18355},
				{18667,18669,0,0,18665},{18336,18338,0,0,18334},{18327,18329,18331,0,18325},{18311,18309,0,0,18313},
				{18302,18304,18306,0,18300},{18275,18277,18279,0,18273},{18266,18268,18270,0,18264},{18512,18514,18516,0,18510},
				{18228,18230,18232,0,18226},{18192,18194,18196,0,18190},{18201,18203,18205,0,18199},{18535,18537,18539,18541,18533}}; // all 16 Sections
	
	//const int OnRamp_Ln_AdvanceDetEndId_All[(NumOnRamp+5)][max_onramp_ln]={{19860,19850,18679},{18418,0,0},{18419,0,0},{19980,0,0},
    //                   {21067,21068,21069},{19886,0,18424},{19906,19898,18425},{19922,0,18689},{19934,0,18691},{18434,0,0},
    //                   {19966,0,18436},{19957,0,18438},{19944,0,18693},{18442,0,0},{18444,0,0},{18446,0,0}};

//for measuremnt only
//const int onramp_secId[NumOnRamp]={17256,17677,17324,17336,17392,17510,16705,16731,16785,16571,16853};   // 16 onramps; not used anymore; divided to lanes
static float onrampL[NumOnRamp]={379.2,453.6,587.6,831.7,284.8,615.9,334.6,470.7,402.7,522.7,272.3};
const int OnRampDetectionEndId[NumOnRamp]={19797,20139,19802,19806,19812,19816,20140,19824,19828,19832,19835}; // single detector after meter
//const int OnRampLnDetB4MeterId[NumOnRamp][max_onramp_ln]={{21072,0,21073},{21077,21078,21079},{21085,0,21086},{21089,0,21090},
//                             {21091,0,0},{21094,0,21095},{21096,0,21097},{21100,0,21101},{21102,0,0},{21103,0,0},{21104,0,0}};
//const int OnRampLnDetAfterMeterId[NumOnRamp][max_onramp_ln]={{21071,0,2107},{21074,21075,21076},{21083,0,21084},{21087,0,21088},
//                       {19812,0,0},{21092,0,21093},{19961,0,19820},{21098,0,21099},{19828,0,0},{19832,0,0},{19835,0,0}};

const int OnRamp_Ln_AdvanceDetEndId[NumOnRamp][max_onramp_ln]={{19886,0,18424},{19906,19898,18425},{19922,0,18689},{19934,0,18691},
                                  {18434,0,0},{19966,0,18436},{19957,0,18438},{19944,0,18693},{18442,0,0},{18444,0,0},{18446,0,0}};

int Green[(NumOnRamp+5)][max_onramp_ln]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},
                                      {0,0,0},{0,0,0},{0,0,0},{0,0,0},
									  {0,0,0},{0,0,0},{0,0,0},{0,0,0},
									  {0,0,0},{0,0,0},{0,0,0},{0,0,0}};



const int OffRampDetectionId[NumOnRamp]={18423, 0, 18426, 0, 18433, 18435, 18437, 18439,18441,18443,18445}; // OffRamp Matched with OnRamp
const int offramp_secId[NumOnRamp]=     {17246, 0, 17286, 0, 17384, 17500, 16697, 16727,16751,16565,16833}; // OffRamp Matched with OnRamp

const float lambda[SecSize]={3.0000,4.0000,3.3654,4.0000,4.0000, 4.0000,4.0000,4.2945,4.0000,4.1191,5.0000,4.6147}; // composite ln number

/*// 3 loop stations downstream of the critical VSL for I-80 W model
int free_sec_detId1[5]={665,666,667,668,669};  // most downstream 5 Lns of 
int free_sec_detId2[5]={660,661,662,663,664};  // downstream a little 5 Lns
int free_sec_detId3[5]={659,658,657,656,655};  // immediate downstream 5 Lns
*/

int origin[20]={0};
int destination[20]={0};


int InitRealtimeDetection(void);
int InitRealtimeDetection_s(void);	// memory allocation; for control detection, almost the same as InitRealtimeDetection(), just has less detector and not save data
int InitRealTimeSection(void);	//for section measure

// from data_save.h
int open_detector(char* data_saving, unsigned int replic);
int open_section(char* data_saving, unsigned int replic);
int open_system(char* data_saving, unsigned int replic);
int open_detector_instant(char* data_saving, unsigned int replic);
int open_section_instant(char* data_saving, unsigned int replic);
int open_network(char* data_saving, unsigned int replic);
int open_signal(char* data_saving, unsigned int replic);
int open_meter(char* data_saving, unsigned int replic);
int init_data_saving(unsigned int replica);
int get_detIds(void);
int get_sectIds(void);
int get_nodeIds(void);
int read_detector(float);
int read_detector_instant(float);
int read_section(float);
int read_system(float);
int read_section_instant(float);
int read_meter_state(float);
int read_signal_state(float);
int save_networkinfo(char * data_saving, unsigned replic);
//int data_dir(char* data_saving_dir, unsigned int contr_sw);

float detInterval,detInstantInterval,last_det_readtime,last_sect_readtime,sectStatInterval,last_syst_readtime,systStatInterval,last_det_inst_readtime;
float last_sect_inst_readtime,sectInstantInterval;
int N_emission;
//int detectorNum,sectionNum,nodeNum;


// from readdetector.h

typedef struct{
	int vehType;
	float DataTime;
	float flow;	
	float speed;
	float occupancy;
	float density;
//	float section_speed;
//	float section_harmonic_speed;
	float instant_flow;
	float instant_speed;
	float instant_density;
}detectorData;

typedef struct{
	int detId;
	float practical_flow;  // used for Onramp only
	int detId_ln[max_onramp_ln];
	float ln_flow[max_onramp_ln];
	//float detectionInterval;
	//FILE *outfile;
	int sectionId;
	detectorData data[Np]; //
}detData;

typedef struct{
	int vehType;
	float DataTime;
	float flow;
	float speed;
	float density;
	float Harmonic_speed;
}sectionData;

typedef struct{
	int sectionId;
	//FILE *outfile;
	sectionData data[Np];
}secData;

typedef struct {
	int detId;
	//FILE *outfile;
	detectorData data[Np];
}data_profile;

detData detection_s_0[SecSize];  
//detData **detection[];	// the realtime data for each cell from detector
detData detection_onramp_0[NumOnRamp];	//the realtime data for onramp from detector
detData detection_offramp_0[NumOnRamp];	//the realtime data for offramp from detector

detData *detection_s[SecSize]; 
detData *detection_onramp[NumOnRamp];
detData *detection_offramp[NumOnRamp];

//detData **detection[];	// the realtime data for each cell from detector
//detData **detection_onramp=&(detection_onramp_0[0]);	//the realtime data for onramp from detector
//detData **detection_offramp=&(detection_offramp_0[0]);	//the realtime data for offramp from detector


//data_profile *dataProfileUp
//data_profile *dataProfileDown;	// the predictive(from history) data for the most upstream and downstream cell as the boundary condition
//data_profile **dataProfileOnRamp
//data_profile **dataProfileOffRamp;	// the predictive(from history) data for the onramp and offramp as the boundary condition
//secData **section;
//data_profile *dataProfile580;




int Init();
int get_u_for_opt();
int det_data_4_contr(float);
int get_s();
int get_q_main();
int update_q_R();
int get_meas(float);
int get_state(float);  
//int set_coef(float**,int*,int*,float);
int ln_rm_distrib();
int Set_Hybrid_Meter(float,float,float);
int Set_Opt_Meter(float,float,float);
int Set_Coord_ALINEA(float,float,float);
int Finish_sim_data_out();
float Mins(float,float);
float Maxs(float,float);
float get_min(float, float);
float Maxd(float,float);
