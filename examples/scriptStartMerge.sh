#!/bin/bash
# sleep 7

# root -q -l rootScript_concatBoloEra.c
# Erics has two sets of files for Run12, and within a set there are a few files per bolometer,  but we process the complete data in one step

root -q -l rootScript_fillEraToKEdsEvents.c
# fill Erics dst-, cuts-, eIon- and eHeat- Files into our data structure one file per bolometer 

root -q -l processMuonVetoToKEDSOnKalinka.root.C
# filling the Muondata into out datastructure, 1File per muonVetoRun

root -q -l rootScript_concatMuonKEdwDS.c
# concatenating the muon Events into one single File

root -q -l rootScript_mergeKEdsTrees.c
#merging always 2files together until at last also the uVeto data is included

#make sure these following programms have been compiled
/kalinka/home/edelweiss/EdwSoftware/Kdata/dev/bin/allPassOne_GSEventNumbers /kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Kds_AllBolosVeto.root /kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Kds_Run12_v2.0.root 
# setting a new global EventNumber

# filling of SystemRecord information
# is boloSystemOn/isMuonSystemOn will probably rely on my routine findMuonRates.c which produces another small rootFile with online and offline perios, probably I will add it to this directory,

#skimming the file/ removing noise evts to get a nice file with reduced file size
