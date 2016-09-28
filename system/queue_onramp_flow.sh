#!/bin/bash

A_FILE=$1

if [[ q$A_FILE == 'q' ]]
then
        A_FILE=`ls -t /big/data/ac_rm_1/a_10* | head  -1`
fi

MATCHSTR=`echo $A_FILE | sed '{s/\./ /g}' | awk '{print $(NF-1)}'`
MYPATH=`echo $A_FILE | sed '{s/a_10/ &/g}'| awk '{print $1}'`

echo $MATCHSTR

for THIS_PATH in `ls $MYPATH*$MATCHSTR.dat`
do
	echo `cat $THIS_PATH | awk '{print $47,$55,$63,$152,$157,$162,$167,$172,$177,$182,$187,$192,$197,$202,$207,$212,$217,$222,$227,$249}' | uniq -c | wc | awk '{print $1}'` `wc $THIS_PATH | awk '{print $1,$4}'` 
done
