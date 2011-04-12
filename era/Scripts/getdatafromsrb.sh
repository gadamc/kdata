#!/bin/sh
# EA - Jan 09
# Small script to get EDW raw data from HPSS.
# File containing the list of runs you want to get (one line per run):
# format aa00aaa
listruns='/afs/in2p3.fr/home/a/armengau/ID_Run10/runs4v_bckgd.txt'
# SRB directory where the runs are located (at the "year" level!):
srbdir='/edw/edw2rawdata/2008/'
# Directory where you want to get the runs:
destination='/sps/edelweis/RawData/'

source /usr/local/shared/bin/srb_env.sh
Sinit
Scd ${srbdir}
cd ${destination}
for run in `more ${listruns}` ; do
  cd ${destination}
  for i in `Sls` ; do # find the correct month
    month=`echo $i | awk -F "/" ' NF == 5 {print $NF}'`
    if [ $month ] ; then
      file=`Sls ${srbdir}/${month}/events | grep $run`
      echo $month" "$run
      if [ $file ] ; then
	echo "..found"
	Scd ${month}/events
	Sget ${run}.tar
	tar xvf ${run}.tar
	rm -f ${run}.tar
	Scd ../../
      fi
    fi
  done
done
Sexit
