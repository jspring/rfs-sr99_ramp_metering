#!/bin/bash

TIMESTAMP=`ls -tr /big/data/ac_rm_1 | tail -1| sed '{s/\./ /g}'|awk '{print $(NF-1)}'`
cd /big/data/ac_rm_1
tar cvzf /big/data/"ac_rm_"$TIMESTAMP".tgz" /big/data/ac_rm_1/* --exclude "*$TIMESTAMP*"
scp /big/data/"ac_rm_"$TIMESTAMP".tgz" caltrans@128.32.234.154:.
#for x in `ls | grep -v $TIMESTAMP` 
#do 
#	echo $x
#	rm -f $x
#done
#mv /big/data/*tgz /big/data/olddata
