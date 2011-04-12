
#!/bin/sh
# EA - Feb 09
# Generic script to build Root event files from samba for EdwRootAna program at Lyon
# Two input parameters
basedir=$1
datadir=$2

# ROOT configuration
if [ "${ROOT_ENV_SET}" != "yes" ] ; then
    export ROOTSYS=/usr/local/root/new/root
    . /usr/local/shared/bin/root_env.sh
fi

# generate parameter file
paramfile=${basedir}/Paramfiles/params_readsamba.txt
rm -f ${paramfile}
touch ${paramfile}
echo "gProcessingStep = ReadSamba" >> ${paramfile}
echo "gBaseDir = "${basedir} >> ${paramfile}
echo "gSambaDir = "${datadir}"/events" >> ${paramfile}
echo "gRawDBFile = "${datadir}"/rootdbs/rawdb.root" >> ${paramfile}
echo "gRootEventDir = "${datadir}"/rootevts" >> ${paramfile}

# Run EdwRootAna
${KDATA_ROOT}/bin/edwrootana ${paramfile}
