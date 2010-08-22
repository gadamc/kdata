#!/bin/sh
# EA - March 2009
# count the lines of ntps to get the number of events
# from a given run list
# format aa00aaa
listruns='/afs/in2p3.fr/home/a/armengau/ID_Run10/runs4v_gamma.txt'
# Directory where the events (and ntps) are:
destination='/sps/edelweis/RawData/'

nb=0
for i in `more ${listruns}` ; do
    nbloc=`wc -l ${destination}/${i}/${i}_ntp | awk '{print $1}'`
    echo ${i}" : "${nbloc}
    nb=`expr "${nbloc}" '+' "${nb}"`
done
echo "Total : "${nb}
