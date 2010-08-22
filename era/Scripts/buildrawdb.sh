
#!/bin/sh
# EA - Feb 09
# Generic script to build a rawdb for EdwRootAna program at Lyon
# Two input parameters
basedir=$1
datadir=$2

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# generate parameter file
paramfile=${basedir}/Paramfiles/params_buildrawdb.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = BuildRawDB" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gSambaDir = "${datadir}"/events" >> ${paramfile}
echo "gRunListFile = "${basedir}"/runlist.txt" >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}

# Run EdwRootAna
${HOME}/EdwRootAna/bin/edwrootana.exe ${paramfile}

