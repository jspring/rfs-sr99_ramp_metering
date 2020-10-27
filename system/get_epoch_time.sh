#!/bin/bash

FILE=$1

rm eventfile.tmp weeklyfile.tmp
rm eventfile.txt weeklyfile.txt

for x in `cat $1`
do
	LINE=`echo $x | sed '{s/days/day/g}' | sed '{s/,/ /g}'`
	echo $LINE | grep Events >/dev/null 2>&1
	if [[ $? -eq 0 ]]
	then
		STARTTIME=`echo $LINE | awk '{print $2,$3}'`
		STOPTIME=`echo $LINE | awk '{print $2,$4}'`
		RESTOFLINE=`echo $LINE | awk '{print $5,$6,$7}'`
		EPOCHSTART=`date -d "$STARTTIME" +%s 2>/dev/null`
		EPOCHSTOP=`date -d "$STOPTIME" +%s 2>/dev/null`
		echo $EPOCHSTART $EPOCHSTOP $RESTOFLINE >>eventfile.tmp
	else
		echo $LINE | grep Holiday >/dev/null 2>&1
		if [[ $? -eq 0 ]]
		then
			STARTTIME=`echo $LINE | awk '{print $4}'`
			STOPTIME=`echo $LINE | awk '{print $5}'`
			RESTOFLINE=`echo $LINE | awk '{print $6,$7,$8}'`
			./get_mlk_day.sh $STARTTIME $STOPTIME $RESTOFLINE
		else
			STARTTIME=`echo $LINE | awk '{print $3,$4}'`
			STOPTIME=`echo $LINE | awk '{print $3,$5}'`
			RESTOFLINE=`echo $LINE | awk '{print $6,$7,$8}'`
			EPOCHSTART=`date -d "$STARTTIME" +%s 2>/dev/null`
			EPOCHSTOP=`date -d "$STOPTIME" +%s 2>/dev/null`
			echo $EPOCHSTART $EPOCHSTOP $RESTOFLINE >>weeklyfile.tmp
		fi
	fi
done

sort -n weeklyfile.tmp >weeklyfile.txt
sort -n eventfile.tmp >eventfile.txt
sort -n holidayfile.tmp >holidayfile.txt
mv eventfile.txt $FILE.epoch
cat holidayfile.txt >>$FILE.epoch
cat weeklyfile.txt >>$FILE.epoch
