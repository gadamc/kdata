
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

# We must run in batch mode here!
if [ ! ${TMPBATCH} ] ; then
    echo "Warning, this script should be run in batch mode!"
fi

# Indices of events to deal with
start_evt=`echo "${nb}*${i}" | bc`
stop_evt=`echo " ${nb} * ( ${i} + 1 ) " | bc`
num_ntp=`echo " ${i} + 1 " | bc`

# Copy of files to $TMPBATCH
cp ${datadir}/rootdbs/rawdb.root ${TMPBATCH}/.
cp ${HOME}/EdwRootAna/bin/edwrootana.exe ${TMPBATCH}/.
cp ${basedir}/heationsync.txt ${TMPBATCH}/.
mkdir ${TMPBATCH}/rootevts

# Build list of DBs, so that the stupid "anadblist.txt" is not useful anymore
ls ${datadir}/rootdbs/anadb_* | awk -F "_" '{print $NF}' | sort -n | awk -v a="${datadir}/rootdbs/anadb_" '{print a$1}' > ${TMPBATCH}/anadblist.txt

# generate parameter file
paramfile=${basedir}/Paramfiles/params_process_${num_ntp}.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = Process" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRunListFile = "${basedir}"/runlist.txt" >> ${paramfile}
echo "gChannelListFile = "${basedir}"/channellist.txt" >> ${paramfile}
echo "gRawDBFile = ${TMPBATCH}/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}
echo "gScratchDir = ${TMPBATCH}/rootevts/" >> ${paramfile}
echo "gRootNtpFile = ${TMPBATCH}/ntp_"${num_ntp}".root" >> ${paramfile}
echo "gRootNtpName = EdwRootNtp" >> ${paramfile}
echo "gIStart = "${start_evt} >> ${paramfile}
echo "gIStop = "${stop_evt} >> ${paramfile}
echo "gAnaDBFile = ${TMPBATCH}/anadb.root" >> ${paramfile}
echo "gAnaDBListFile = ${TMPBATCH}/anadblist.txt" >> ${paramfile}
echo "gASCIITemplateDir = "${datadir}"/templates" >> ${paramfile}
echo "gWienerOnly = 1" >> ${paramfile}
echo "gHeatIonSynchroFile = ${TMPBATCH}/heationsync.txt" >> ${paramfile}

# Run EdwRootAna
${TMPBATCH}/edwrootana.exe ${paramfile}

# Copy of files from $TMPBATCH
cp ${TMPBATCH}/ntp_${num_ntp}.root ${datadir}/ntps/.
