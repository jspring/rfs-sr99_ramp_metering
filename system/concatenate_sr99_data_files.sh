#!/bin/bash

DATE=$1

if [[ q$DATE == 'q' ]]
then
	echo "Usage: $0 <date, in MMDD format>"
	exit 1
fi

INDEX=0
LASTSTARTTIME=0
for x in `cat /home/jspring/bin/sr99_controller_ip_and_port_list.txt`
do
	OUTFILE=/big/data/ac_rm_1/"a_"$x".9999_"$DATE"_9999.dat"
	if [[ -e $OUTFILE ]]
	then
		echo $OUTFILE already exists. Either delete it or use it.
		exit 1
	fi
	for y in `ls /big/data/ac_rm_1/*"$x"*"$DATE"*".dat"`
	do
		echo $y
		FILENAME[INDEX]=/big/data/ac_rm_1/"a_"$x".9999_"$DATE"_9999.dat"
		cat $y >>${FILENAME[INDEX]}
	done
	STARTTIME[$INDEX]=`head -1 ${FILENAME[INDEX]} | awk '{print $1}' | sed '{s/:/ /g}' |sed '{s/\./ /1}'|awk '{print ($2*60) + $3}'`
	if [[ ${STARTTIME[$INDEX]} -gt $LASTSTARTTIME ]]
	then
		LASTSTARTTIME=${STARTTIME[$INDEX]}
	fi
	INDEX=$(($INDEX+1))
done 

for((z=0; z<28;z++))
do
	DELLINES=$((1+(($LASTSTARTTIME-${STARTTIME[$z]})/15)))
	more +$DELLINES ${FILENAME[$z]} >${FILENAME[$z]}.mod
	mv ${FILENAME[$z]}.mod ${FILENAME[$z]}
done
