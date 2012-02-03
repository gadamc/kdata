//
// KBlackForestKAmpSite.h
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBLACKFORRESTKAMPSITE_H_
#define __KBLACKFORRESTKAMPSITE_H_

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
  
  KMultiTrapKamperProto fMTKamp;

  
  
};


#endif // __KBLACKFORRESTKAMPSITE_H_
