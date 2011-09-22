//
// KAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// A KAmpSite is responsible for taking Raw Pulse traces, passing those pulses to its KAmpers to 
// estimate the pulse amplitudes and peak positions, then to fill the KAmpEvents with the appropriate
// results. The appropriate results can be any number of KPulseAnalysisRecords added to the KAmpEvent.
// A KAmpSite will be used in a routine or program that's main goal is to take a RAW-level KData file and
// convert it into an AMP-level KData file that is filled with pulse characteristic estimates (amplitudes, start times,
// pulse widths, chi^2, etc...). 

#include "KAmpSite.h"


KAmpSite::KAmpSite(void)
{

}

KAmpSite::~KAmpSite(void)
{

}
