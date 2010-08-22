
#!/bin/sh
# EA - Feb 09
# Generic script for channel-by-channel template building for EdwRootAna program at Lyon
basedir=$1
datadir=$2
channelnum=$3 # Number of the channel to be processed in the input list

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# Bolometer to deal with
channelfile=${basedir}/tmpchannellist_${channelnum}.txt
awk -v a=${channelnum} 'NR == a {print}' ${basedir}/channellist.txt > ${channelfile}

# generate parameter file
paramfile=${basedir}/Paramfiles/params_buildtemplate_${channelnum}.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = BuildTemplate" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRunListFile = "${basedir}"/runlist.txt" >> ${paramfile}
echo "gChannelListFile = "${channelfile} >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}
echo "gPlotDir = "${basedir}"/Plots" >> ${paramfile}
echo "gAnaDBFile = "${datadir}"/rootdbs/tmplts_dum_"${channelnum}".root" >> ${paramfile}
echo "gTemplateFromSinglePulse = 1" >> ${paramfile}
echo "gASCIITemplateDir = "${datadir}"/templates" >> ${paramfile}

# Run EdwRootAna
${HOME}/EdwRootAna/bin/edwrootana.exe ${paramfile}
rm -f ${channelfile}
rm -f ${datadir}/rootdbs/tmplts_dum_${channelnum}.root
