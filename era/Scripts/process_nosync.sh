
#!/bin/sh
# EA - Feb 09
# Generic script for processing for EdwRootAna program at Lyon
basedir=$1
datadir=$2
nb=$3 # Number of events to process
i=$4 # "Parallelization index" (the processing will start at event nb*i)

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# Indices of events to deal with
start_evt=`echo "${nb}*${i}" | bc`
stop_evt=`echo " ${nb} * ( ${i} + 1 ) " | bc`
num_ntp=`echo " ${i} + 1 " | bc`

# generate parameter file
paramfile=${basedir}/Paramfiles/params_process_nosync_${num_ntp}.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = Process" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRunListFile = "${basedir}"/runlist.txt" >> ${paramfile}
echo "gChannelListFile = "${basedir}"/channellist.txt" >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}
echo "gRootNtpFile = "${datadir}"/ntps/nosyncntp_"${num_ntp}".root" >> ${paramfile}
echo "gRootNtpName = EdwRootNtp" >> ${paramfile}
echo "gIStart = "${start_evt} >> ${paramfile}
echo "gIStop = "${stop_evt} >> ${paramfile}
echo "gAnaDBFile = ${TMPBATCH}/anadb.root" >> ${paramfile}
echo "gAnaDBListFile = "${basedir}"/anadblist.txt" >> ${paramfile}
echo "gASCIITemplateDir = "${datadir}"/templates" >> ${paramfile}
echo "gWienerOnly = 1" >> ${paramfile}
#echo "gHeatIonSynchroFile = None" >> ${paramfile}

# Run EdwRootAna
${HOME}/EdwRootAna/bin/edwrootana.exe ${paramfile}
