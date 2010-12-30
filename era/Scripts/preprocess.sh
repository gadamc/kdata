
#!/bin/sh
# EA - Feb 09
# Generic script to run preprocessing for EdwRootAna program at Lyon
basedir=$1
datadir=$2
runnum=$3 # Number of the run to be processed in the input run list

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# We must run in batch mode here!
if [ ! ${TMPBATCH} ] ; then
    echo "Warning, this script should be run in batch mode!"
fi

# run to deal with
runfile=${basedir}/tmprunlist_${runnum}.txt
awk -v a=${runnum} 'NR == a {print}' ${basedir}/runlist.txt > ${runfile}

# Copy of files to $TMPBATCH
cp ${datadir}/rootdbs/rawdb.root ${TMPBATCH}/.
mkdir ${TMPBATCH}/rootevts
therun=`awk -v a=${runnum} 'NR == a {print}' ${basedir}/runlist.txt`
echo "Run: "${therun}
cp -v ${datadir}/rootevts/${therun}_* ${TMPBATCH}/rootevts/.
cp ${HOME}/EdwRootAna/bin/edwrootana.exe ${TMPBATCH}/.

# generate parameter file
paramfile=${basedir}/Paramfiles/params_preprocess_${runnum}.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = Preprocess" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRunListFile = "${runfile} >> ${paramfile}
echo "gChannelListFile = "${basedir}"/channellist.txt" >> ${paramfile}
echo "gRawDBFile = ${TMPBATCH}/rawdb.root" >> ${paramfile}
echo "gRootEventDir = ${TMPBATCH}/rootevts" >> ${paramfile}
echo "gAnaDBFile = ${TMPBATCH}/anadb_"${runnum}".root" >> ${paramfile}
#echo "gPlotDir = "${basedir}"/Plots" >> ${paramfile}
echo "gTimeStepMinutes = 60" >> ${paramfile} # passage de 200 a 60 min a partir des runs d'oct09

# Run EdwRootAna
${TMPBATCH}/edwrootana.exe ${paramfile}

cp -f ${TMPBATCH}/anadb_${runnum}.root ${datadir}/rootdbs/.
rm -f ${runfile}
