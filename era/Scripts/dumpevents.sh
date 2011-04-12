
#!/bin/sh
# EA - May 09
# Generic script to dump traces to ASCII files
# 2 input parameters
# Used for template fabrication
basedir=$1
datadir=$2

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# generate parameter file
paramfile=${basedir}/Paramfiles/params_dumpevents.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = DumpEvent" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}
echo "gEvtListFile = "${basedir}"/evtlist.txt" >> ${paramfile}
echo "gASCIIOutputDir = "${datadir}"/traces" >> ${paramfile}

# Run EdwRootAna
${KDATA_ROOT}/bin/edwrootana ${paramfile}

