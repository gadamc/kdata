//
// KSimpleKamper1.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KSIMPLEKAMPER1_H_
#define __KSIMPLEKAMPER1_H_

#include "KAmper.h"
#include "Rtypes.h"
#include "KLinearRemoval.h"
#include "KPatternRemoval.h"
#include "KPeakFinder.h"
#include <string>


class KSimpleKamper1 : KAmper {

public:
  KSimpleKamper1(void);
  virtual ~KSimpleKamper1(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * ampBolo, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}
  
private:
  std::string fName;

  KLinearRemoval fLinRemovalHeat;
  KLinearRemoval fLinRemovalIon;
  KPatternRemoval fPatRemoval;
  KPeakFinder fPeakFindHeat;
  KPeakFinder fPeakFindIon;

  Double_t* fHeatPulse;
  Double_t* fIonPulse;
  UInt_t fHeatPulseSize;
  UInt_t fIonPulseSize;
  
  void SetHeatPointers();
  void SetIonPointers();
  void CheckMemory(KRawBoloPulseRecord *pRec);
  
};


#endif // __KSIMPLEKAMPER1_H_
