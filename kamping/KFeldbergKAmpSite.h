//
// KFeldbergKAmpSite.h
// KDataStructure
//
// Created by Michael Unrau
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KFELDBERGKAMPSITE_H_
#define __KFELDBERGKAMPSITE_H_

#include "KAmpSite.h"
#include "KFilterChainBestCorrelationKamper.h"
#include <vector>
#include <string>
#include <map>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KFeldbergKAmpSite : public KAmpSite {

public:
  KFeldbergKAmpSite(void);
  virtual ~KFeldbergKAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee); //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void){ return false;}
  
  KFilterChainBestCorrelationKamper& GetKamper(void){return fFCKamp;}
  
private:
  
  KFilterChainBestCorrelationKamper fFCKamp;

  
  
};


#endif // __KFELDBERGKAMPSITE_H_
