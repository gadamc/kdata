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
// Additionally, the KAmpSite MUST manage the TRef links between the Bolometer/BoloPulse/PulseAnalysis records.
// 
// A KAmpSite will be used in a routine or program that's main goal is to take a RAW-level KData file and
// convert it into an AMP-level KData file that is filled with pulse characteristic estimates (amplitudes, start times,
// pulse widths, chi^2, etc...). 
//
// As a first example, see the KBackyardKampSite and code inside RunKampSite
//

#include "KAmpSite.h"

#include "KAmpEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"

KAmpSite::KAmpSite(void)
{

}

KAmpSite::~KAmpSite(void)
{

}

void KAmpSite::SetTRefLinksForKAmpEvent(KPulseAnalysisRecord *rec, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp)
{
  //as a KampSite - you want to create a valid KAmpEvent, so you MUST set the TRef links.
  //
  rec->SetBolometerRecord(boloAmp);  
  rec->SetBoloPulseRecord(pAmp); 
  pAmp->AddPulseAnalysisRecord(rec);
}
