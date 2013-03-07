//_____________________________________________
// KAmpKounselor.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KAMPKOUNSELOR_H_
#define __KAMPKOUNSELOR_H_

#include "Rtypes.h"
#include <vector>

class KAmpSite;
class KDataReader;
class KDataWriter;

class KAmpKounselor {

public:
  
  KAmpKounselor(void);
  virtual ~KAmpKounselor(void);
  virtual void AddKAmpSite(KAmpSite *aSite);
  virtual void SetFiles(const char* inputRawKDataFile, const char* outputAmpKDataFile);
  virtual Bool_t RunKamp(const char* inputRawKDataFile, const char* outputAmpKDataFile, int maxNumEvents = -1);
  virtual Bool_t Scout(int maxNumEvents = -1);
  virtual Bool_t Prepare();
  virtual Int_t Run(int maxNumEvents = -1);
  virtual void ReportResults();
  virtual void WriteKampSiteData(void);
  //virtual std::vector<KAmpSite *> GetListOfKAmpSites(void){ return fKampSites;}
  virtual void SetStatusCheckPercentage(Float_t val){if (val > 0 && val < 1.0) fStatusCheckPercentage = val;}
  virtual Float_t GetStatusCheckPercentage(void){return fStatusCheckPercentage;}

  virtual void SetReport(UInt_t val){fReport = val;}
  virtual UInt_t GetReport(void){return fReport;}

  KDataReader* GetKDataReader(void){return fInput;}
  KDataWriter* GetKDataWriter(void){return fOutput;}
  
private:
  std::vector<KAmpSite *> fKampSites;
  KDataReader *fInput;
  KDataWriter *fOutput;
  
  Float_t fStatusCheckPercentage; //between 0 and 1, represents how often ReportStatus is called for each KAmpSite
  UInt_t fReport; //if = 0, no reports, if = 1 report results only, if >= 2, reportstatus + results. default = 2
  Bool_t CheckSetup();

};


#endif // __KAMPKOUNSELOR_H_
