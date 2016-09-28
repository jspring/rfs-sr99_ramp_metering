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
	echo `cat $THIS_PATH | awk '{print $44,$45,$52,$53,$60,$61,$246,$247,$254,$255,$256,$257,$258,$259,$260,$261}' | uniq -c | wc | awk '{print $1}'` `wc $THIS_PATH | awk '{print $1,$4}'` 
done
