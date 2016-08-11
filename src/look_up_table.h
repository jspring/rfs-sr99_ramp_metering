#define	NUM_5MIN_INTERVALS	288
#define	NUM_ONRAMPS_PLUS_1	17
// add On-ramp 5 minute historical data as a look up table, 288 is the time vector and 16 is the number of total on-ramp 
const float OR_HIS_FLOW_DATA[NUM_5MIN_INTERVALS][NUM_ONRAMPS_PLUS_1] = {
{	1.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	2.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	3.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	4.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	5.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	6.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	7.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	8.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	9.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	10.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	11.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	12.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	13.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	14.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	15.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	16.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	17.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	18.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	19.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	20.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	21.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	22.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	23.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	24.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	25.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	26.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	27.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	28.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	29.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	30.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	31.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	32.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	33.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	34.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	35.000	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	,	1	}	,
{	36.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	37.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	38.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	39.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	40.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	41.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	42.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	43.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	44.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	45.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	46.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	47.000	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	}	,
{	48.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	49.000	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	,	2	}	,
{	50.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	51.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	52.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	53.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	54.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	55.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	56.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	57.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	58.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	59.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	60.000	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	,	4	}	,
{	61.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	62.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	63.000	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	}	,
{	64.000	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	}	,
{	65.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	66.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	67.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	68.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	69.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	70.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	71.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	72.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	73.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	74.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	75.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	76.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	77.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	78.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	79.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	80.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	81.000	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	,	56	}	,
{	82.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	83.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	84.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	85.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	86.000	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	}	,
{	87.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	88.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	89.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	90.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	91.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	92.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	93.000	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	}	,
{	94.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	95.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	96.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	97.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	98.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	99.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	100.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	101.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	102.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	103.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	104.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	105.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	106.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	107.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	108.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	109.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	110.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	111.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	112.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	113.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	114.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	115.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	116.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	117.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	118.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	119.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	120.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	121.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	122.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	123.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	124.000	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	,	60	}	,
{	125.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	126.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	127.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	128.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	129.000	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	,	57	}	,
{	130.000	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	,	46	}	,
{	131.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	132.000	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	}	,
{	133.000	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	,	67	}	,
{	134.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	135.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	136.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	137.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	138.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	139.000	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	,	15	}	,
{	140.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	141.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	142.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	143.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	144.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	145.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	146.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	147.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	148.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	149.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	150.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	151.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	152.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	153.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	154.000	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	,	53	}	,
{	155.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	156.000	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	,	43	}	,
{	157.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	158.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	159.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	160.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	161.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	162.000	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	,	65	}	,
{	163.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	164.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	165.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	166.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	167.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	168.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	169.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	170.000	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	,	54	}	,
{	171.000	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	,	35	}	,
{	172.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	173.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	174.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	175.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	176.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	177.000	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	,	49	}	,
{	178.000	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	,	50	}	,
{	179.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	180.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	181.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	182.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	183.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	184.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	185.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	186.000	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	,	51	}	,
{	187.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	188.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	189.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	190.000	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	,	47	}	,
{	191.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	192.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	193.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	194.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	195.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	196.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	197.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	198.000	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	,	48	}	,
{	199.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	200.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	201.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	202.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	203.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	204.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	205.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	206.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	207.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	208.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	209.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	210.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	211.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	212.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	213.000	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	,	45	}	,
{	214.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	215.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	216.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	217.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	218.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	219.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	220.000	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	,	44	}	,
{	221.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	222.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	223.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	224.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	225.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	226.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	227.000	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	,	31	}	,
{	228.000	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	,	34	}	,
{	229.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	230.000	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	,	40	}	,
{	231.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	232.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	233.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	234.000	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	,	32	}	,
{	235.000	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	,	36	}	,
{	236.000	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	,	22	}	,
{	237.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	238.000	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	,	24	}	,
{	239.000	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	,	33	}	,
{	240.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	241.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	242.000	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	,	39	}	,
{	243.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	244.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	245.000	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	,	41	}	,
{	246.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	247.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	248.000	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	,	37	}	,
{	249.000	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	,	38	}	,
{	250.000	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	,	42	}	,
{	251.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	252.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	253.000	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	,	16	}	,
{	254.000	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	}	,
{	255.000	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	,	21	}	,
{	256.000	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	,	27	}	,
{	257.000	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	,	26	}	,
{	258.000	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	,	28	}	,
{	259.000	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	,	30	}	,
{	260.000	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	,	20	}	,
{	261.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	262.000	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	}	,
{	263.000	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	,	19	}	,
{	264.000	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	,	8	}	,
{	265.000	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	,	29	}	,
{	266.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	267.000	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	,	9	}	,
{	268.000	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	,	23	}	,
{	269.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	270.000	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	,	10	}	,
{	271.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	272.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	273.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	274.000	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	,	18	}	,
{	275.000	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	,	17	}	,
{	276.000	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	,	14	}	,
{	277.000	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	,	13	}	,
{	278.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	279.000	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	,	7	}	,
{	280.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	281.000	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	,	11	}	,
{	282.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	,
{	283.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	284.000	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	,	12	}	,
{	285.000	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	,	6	}	,
{	286.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	287.000	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	,	5	}	,
{	288.000	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	,	3	}	};
