//
// KBlackForestKAmpSite.h
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBLACKFORESTKAMPSITE_H_
#define __KBLACKFORESTKAMPSITE_H_

#include "KAmpSite.h"
#include "KMultiTrapKamperProto.h"
#include <vector>
#include <string>
#include <map>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KBlackForestKAmpSite : public KAmpSite {

public:
  KBlackForestKAmpSite(void);
  virtual ~KBlackForestKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return false;}
  
  KMultiTrapKamperProto& GetMTKamper(void){return fMTKamp;}

private:
  void FillResults(KPulseAnalysisRecord* rec, map<string, KResult> &resMap);
  
  KMultiTrapKamperProto fMTKamp;

  
  
};


#endif // __KBLACKFORESTKAMPSITE_H_
