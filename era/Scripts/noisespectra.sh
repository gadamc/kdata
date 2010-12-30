
#!/bin/sh
# EA - Sep 09
# Generic script to plot 2D noise spectra at Lyon
basedir=$1
datadir=$2

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# We must run in batch mode here!
if [ ! ${TMPBATCH} ] ; then
    echo "Warning, this script should be run in batch mode!"
fi

# Build list of DBs, so that the stupid "anadblist.txt" is not useful anymore
ls ${datadir}/rootdbs/anadb_* | awk -F "_" '{print $NF}' | sort -n | awk -v a="${datadir}/rootdbs/anadb_" '{print a$1}' > ${TMPBATCH}/anadblist.txt

# generate parameter file
paramfile=${basedir}/Paramfiles/params_noisespectra.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = PlotNoiseSpectra" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gChannelListFile = "${basedir}"/channellist.txt" >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gAnaDBFile = ${TMPBATCH}/anadb.root" >> ${paramfile}
echo "gAnaDBListFile = ${TMPBATCH}/anadblist.txt" >> ${paramfile}
echo "gPlotDir = "${basedir}"/Plots" >> ${paramfile}

# Run EdwRootAna
${HOME}/EdwRootAna/bin/edwrootana.exe ${paramfile}

