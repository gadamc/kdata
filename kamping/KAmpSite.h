//
// KAmpSite.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KAMPSITE_H_
#define __KAMPSITE_H_

#include "Rtypes.h"

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBoloPulseRecord;
class KPulseAnalysisRecord;

class KAmpSite {

public:
  KAmpSite(void);
  virtual ~KAmpSite(void);
  
  virtual Bool_t RunKampSite(KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, 
                        KRawBoloPulseRecord* pRaw) = 0;      //this runs the data processing on each event. 
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e) = 0;  //should call this method first. Use this to scan through data to estimate noise...etc..

  virtual void SetTRefLinksForKAmpEvent(KPulseAnalysisRecord *rec, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp);
private:

};


#endif // __KAMPSITE_H_
