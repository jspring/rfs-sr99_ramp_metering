#!/bin/bash

if [[ `date +%H` -lt 1 ]]
then 
	FILEDATE=`date -d "yesterday" +%y%m%d`
else 
	FILEDATE=`date +%y%m%d`
fi 

cd /big/data/olddata
x=1
while [[ -e "ramp_coord_data_"$FILEDATE"_"$x".tgz" ]]
do 
	x=$(($x+1))
done

FILENAME="sr99_ramp_coord_data_"$FILEDATE"_"$x".tgz"
OUT_DATA_DIRS=`ls -d /home/sr99_ramp_metering/src/Out_Data_$FILEDATE*`
TIMESTAMP=`ls -tr /big/data/ac_rm_1/*dat | tail -1| sed '{s/\./ /g}'|awk '{print $(NF-1)}'`

echo FILENAME $FILENAME OUT_DATA_DIRS $OUT_DATA_DIRS
cd /big/data/ac_rm_1
tar cvzf $FILENAME * $OUT_DATA_DIRS --exclude "*$TIMESTAMP*"
if [[ $? == 0 ]]
then
	echo tar OK
fi
scp $FILENAME 128.32.234.154:/linux2/big/data/ac_rm_1
mv  *tgz /big/data/olddata
for x in `ls | grep -v $TIMESTAMP` 
do 
	echo $x
	rm -f $x
done
