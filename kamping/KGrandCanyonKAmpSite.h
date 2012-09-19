//_____________________________________________
// KGrandCanyonKAmpSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KGRANDCANYONKAMPSITE_H_
#define __KGRANDCANYONKAMPSITE_H_

#include "KAmpSite.h"
#include "KTrapKamperProto.h"
//#include "KOptimalKamper.h"
#include <vector>
#include <string>
#include <map>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KGrandCanyonKAmpSite : public KAmpSite {

public:
  KGrandCanyonKAmpSite(void);
  virtual ~KGrandCanyonKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return false;}
  
  KTrapKamperProto& GetTrapKamperProto(void){return fTrapKamp;}
  
private:
  void FillResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap);

  
  KTrapKamperProto fTrapKamp;
  
  //KTrapKamperProto* fTrapKampHeat;
  //std::map<std::string, KTrapKamperProto *> fIonTraps;
  
  //KOptimalKamper fOptKamp;
  
};


#endif // __KGRANDCANYONKAMPSITE_H_
