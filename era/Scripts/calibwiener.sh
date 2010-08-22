
#!/bin/sh
# EA - Feb 09
# Generic script for specific heat calibration for EdwRootAna program at Lyon
# TO IMPROVE PROBABLY LATER
basedir=$1
datadir=$2
fmin=$3
fmax=$4

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# generate parameter file
paramfile=${basedir}/Paramfiles/params_calibwiener_${fmin}.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = CalibWiener" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRunListFile = "${basedir}"/runlist.txt" >> ${paramfile}
echo "gChannelListFile = "${basedir}"/channellist_calibwiener.txt" >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}
echo "gAnaDBFile = "${datadir}"/rootdbs/anadb_"${fmin}".root" >> ${paramfile}
echo "gAnaDBListFile = "${basedir}"/heattmpltdblist.txt" >> ${paramfile}
echo "gRootNtpFile = "${datadir}"/ntps/calibntp_"${fmin}".root" >> ${paramfile}
echo "gWienerWindowStart = "${fmin} >> ${paramfile}
echo "gWienerWindowStop = "${fmax} >> ${paramfile}

# Run EdwRootAna
${HOME}/EdwRootAna/bin/edwrootana.exe ${paramfile}

