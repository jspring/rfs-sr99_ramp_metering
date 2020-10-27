#!/bin/bash

YEAR=
STARTTIME=$1
STOPTIME=$2
MINRATE=$3
MAXRATE=$4
OCC=$5

echo "Usage $0 <start time> <stop time> <min rate> <max rate> <min occupancy>, returns epoch time in seconds of public holidays in that year"

#Martin_Luther_King_Jr_Day	third_monday_in_january		Jan 15-21
DELTA=$((`date -d "Jan 21 $YEAR" +%w`-1))
DATE=$((21-$DELTA))
MLKSTART=`date -d "Jan $DATE $YEAR $STARTTIME" +%s`
MLKSTOP=`date -d "Jan $DATE $YEAR $STOPTIME" +%s`
echo $MLKSTART $MLKSTOP $MINRATE $MAXRATE $OCC >holidayfile.tmp

#Presidents_Day			third_monday_in_february	Feb 15-21
DELTA=$((`date -d "Feb 21 $YEAR" +%w`-1))
DATE=$((21-$DELTA))
PRESIDENTSDAYSTART=`date -d "Feb $DATE $YEAR $STARTTIME" +%s`
PRESIDENTSDAYSTOP=`date -d "Feb $DATE $YEAR $STOPTIME" +%s`
echo $PRESIDENTSDAYSTART $PRESIDENTSDAYSTOP $MINRATE $MAXRATE $OCC >>holidayfile.tmp

#Memorial_Day			last_monday_in_may		May 24-31
DELTA=$((`date -d "May 31 $YEAR" +%w`-1))
DELTA=$((`date -d "May 31 $YEAR" +%w`))
DATE=$((31-$DELTA))
MEMORIALDAYSTART=`date -d "May $DATE $YEAR $STARTTIME" +%s`
MEMORIALDAYSTOP=`date -d "May $DATE $YEAR $STOPTIME" +%s`
echo $MEMORIALDAYSTART $MEMORIALDAYSTOP $MINRATE $MAXRATE $OCC >>holidayfile.tmp

#Labor_Day			first_monday_in_september	Sept 1-7
DELTA=$((`date -d "Sep 7 $YEAR" +%w`))
DATE=$((7-$DELTA))
LABORDAYSTART=`date -d "Sep $DATE $YEAR $STARTTIME" +%s`
LABORDAYSTOP=`date -d "Sep $DATE $YEAR $STOPTIME" +%s`
echo $LABORDAYSTART $LABORDAYSTOP $MINRATE $MAXRATE $OCC >>holidayfile.tmp

#Thanksgiving			fourth_thursday_in_november+friday Nov 22-28
DOW=$((`date -d "Nov 28 $YEAR" +%w`))
if [[ $DOW -lt 4 ]]
then
	DATE=$((21+(4-$DOW)))
else
	DELTA=$(($DOW-4))
	DATE=$((28-$DELTA))
fi
THANKSGIVINGSTART=`date -d "Nov $DATE $YEAR $STARTTIME" +%s`
THANKSGIVINGSTOP=`date -d "Nov $DATE $YEAR $STOPTTIME" +%s`
echo $THANKSGIVINGSTART $THANKSGIVINGSTOP $MINRATE $MAXRATE $OCC >>holidayfile.tmp





#New_Years_Day			January 1 (January 3 if January 4 is Saturday, January 2 if January 1 is Sunday)
#Cesar_Chavez_Day		March 31
#Independence_Day		July 4 (July 3 if July 4 is Saturday, July 5 if July 4 is Sunday)
#Veterans_Day			November 11 (November 10 if November 11 is Saturday, November 12 if November 11 is Sunday)
#Christmas			December 25 (December 24 if December 25 is Saturday, December 26 if Dec 25 is Sunday)

