//_____________________________________________
//
// KQHistogramManager.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/10/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class fills TH2D Q-ERecoil histograms with data retrieved from a
// KQDataReader object

#include "Rtypes.h"
#include "TDirectory.h"
#include "TMath.h"
#include "KQDataReader.h"
#include "KQHistogramRecord.h"
#include "KQDataReader.h"
#include "KQUncertainty.h"


#include <iomanip>
#include <iostream>
using namespace std;

#ifndef __KQHISTOGRAMMANAGER_H__
#define __KQHISTOGRAMMANAGER_H__

class KQHistogramManager {
  private:
    
    Int_t fNumBinsEnergyRecoil;
    Int_t fNumBinsQ;
    Double_t fQMin;
    Double_t fQMax;
    Double_t fEnergyRecoilMin;
    Double_t fEnergyRecoilMax;
    Double_t fConfidenceInSigmas;
    KQDataReader* fQDataReader;
    KBoloConfig* fBoloConfig;
    vector<KQHistogramRecord> fHistogramRecords;
  public:
    KQHistogramManager(KQDataReader* aKQDataReader = 0);
    virtual ~KQHistogramManager(void){}

    void SetNumBinsEnergyRecoil(Int_t aNumBinsEnergyRecoil)
    { fNumBinsEnergyRecoil = aNumBinsEnergyRecoil; }
    void SetNumBinsQ(Int_t aNumBinsQ)
    { fNumBinsQ = aNumBinsQ; }
    void SetQMin(Double_t aQMin) { fQMin = aQMin; }
    void SetQMax(Double_t aQMax) { fQMax = aQMax; }
    void SetEnergyRecoilMin(Double_t anEnergyRecoilMin)
    { fEnergyRecoilMin = anEnergyRecoilMin; }
    void SetEnergyRecoilMax(Double_t anEnergyRecoilMax)
    { fEnergyRecoilMax = anEnergyRecoilMax; }  
    void SetQDataReader(KQDataReader* aKQDataReader = 0)
    { fQDataReader = aKQDataReader; }
    void SetConfidenceLevel(Double_t aQuantil = 0.9);
  
    
    Int_t GetNumBinsEnergyRecoil() const { return fNumBinsEnergyRecoil; }
    Int_t GetNumBinsQ() const { return fNumBinsQ; }
    Double_t GetQMin() const { return fQMin; }
    Double_t GetQMax() const { return fQMax; }
    Double_t GetEnergyRecoilMin() const { return fEnergyRecoilMin; }
    Double_t GetEnergyRecoilMax() const { return fEnergyRecoilMax; }
    Double_t GetConfidenceInSigmas() const { return fConfidenceInSigmas; }
    Double_t GetConfidenceLevel();
    
    KQDataReader* GetQDataReader() const { return fQDataReader; }
    
    void CalculateTheoErrors();
    void PrintErrors(ostream& os = cout);
    void Fill(Int_t aNumProjections);
    
    
    

  ClassDef(KQHistogramManager,0);

};

#endif //__KQDHISTOGRAMMANAGER_H__