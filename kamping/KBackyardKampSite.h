//
// KBackyardKampSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KBACKYARDKAMPSITE_H_
#define __KBACKYARDKAMPSITE_H_

#include "KAmpSite.h"
#include "KSimpleKamper1.h"

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;

class KBackyardKampSite : public KAmpSite {

public:
  KBackyardKampSite(void);
  virtual ~KBackyardKampSite(void);
  
  virtual Bool_t RunKampSite(KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, 
                        KRawBoloPulseRecord* pRaw);      //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e);  //should call this method first. Use this to scan through data to estimate noise...etc..

private:
  KSimpleKamper1 fSimpKamp1;
  
};


#endif // __KAMPSITE_H_