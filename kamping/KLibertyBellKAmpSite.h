//
// KLibertyBellKAmpSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KLIBERTYBELLKAMPSITE_H_
#define __KLIBERTYBELLKAMPSITE_H_

#include "KAmpSite.h"
#include "KBBv2TimeDomainFitKamper.h"
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

class KLibertyBellKAmpSite : public KAmpSite {

public:
  KLibertyBellKAmpSite(void);
  virtual ~KLibertyBellKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return false;}
  
  KTrapKamperProto& GetTrapKamperProto(void){return fTrapKamp;}
  KBBv2TimeDomainFitKamper& GetBBv2TimeDomainFitKamper(void){return fBBv2TimeFitter;}
  
private:

  void FillTrapResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap);
  void FillTimeFitterResults(KPulseAnalysisRecord *rec, map<string, KResult> &resMap);

  
  KTrapKamperProto fTrapKamp;
  KBBv2TimeDomainFitKamper fBBv2TimeFitter;
  
  
};


#endif // __KLIBERTYBELLKAMPSITE_H_
