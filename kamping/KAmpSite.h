//_____________________________________________
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
#include "KResult.h"
#include <map>
#include <string>

class KAmpEvent;
class KRawEvent;
class KAmpBolometerRecord;
class KAmpBoloPulseRecord;
class KRawBolometerRecord;
class KRawBoloPulseRecord;
class KPulseAnalysisRecord;

class KAmpSite  {

public:
  KAmpSite(void);
  virtual ~KAmpSite(void);
  
  virtual Bool_t RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee) = 0;      //this runs the data processing on each event. 
  virtual Bool_t PrepareKampSite(void){return true;}
  virtual Bool_t ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent *e) = 0;  //should call this method first. Use this to scan through data to estimate noise...etc..
  virtual Bool_t NeedScout(void) = 0;
  virtual void ReportStatus(void){}
  virtual void ReportResults(void){}
  virtual void SetTRefLinksForKAmpEvent(KPulseAnalysisRecord *rec, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp);
  const char * GetName(void) const {return fName.c_str();}
  virtual void SetName(const char* name){fName = name;}
  
  //helper method
  // this compiles, but it doesn't yet work with the build-system and how rootcint is used in Kdata
  // I leave this commented out here -- to be fixed in the future. 
  // static void fill( KPulseAnalysisRecord* rec,  void( KPulseAnalysisRecord:: *Method)(double), 
  //             std::map<string, KResult>& theResult, const char* theField );

protected:

  std::string fName;
};


#endif // __KAMPSITE_H_
