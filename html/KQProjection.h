
//_____________________________________________
//
// KQProjection.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/10/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "Rtypes.h"
#include "KDataReader.h"
#include "KHLABolometerRecord.h"
#include "KHLAEvent.h"
#include "KQDataRecord.h"


#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#ifndef __KQPROJECTION_H__
#define __KQPROJECTION_H__

class KQProjection {
  protected:
    Double_t fIsDataRead;
    Double_t fVerbose;
    vector<KQDataRecord> fData;
    TH2D* fHistogram;
    Double_t fEnergyRecoilMin;
    Double_t fEnergyRecoilMax;
    Double_t fNumBinsEnergyRecoil;
    Int_t fEventCategory;
    Double_t fQMin;
    Double_t fQMax;
    Double_t fNumBinsQ;
    string fSourceFile;
    string fBoloName;
 
    
    

    void ResetHistogram()
    {
      if(fHistogram)
        fHistogram->SetBins(fNumBinsEnergyRecoil,
                            fEnergyRecoilMin,
                            fEnergyRecoilMax,
                            fNumBinsQ,
                            fQMin,
                            fQMax);
    }
    
  public:
    KQProjection();
    KQProjection(const KQProjection& anotherKQProjection);
    KQProjection(const Char_t* aSourceFile,
                 const Char_t* aBoloName,
                 const Char_t* aCategoryName = "fiducial",
                 Double_t aNumBinsEnergyRecoil = 1000,
                 Double_t anEnergyRecoilMin = 0,
                 Double_t anEnergyRecoilMax = 1000,
                 Double_t aNumBinsQ = 1000,
                 Double_t aQMin = 0,
                 Double_t aQMax = 2,
                 const Char_t* aHistogramName = "hist");
    virtual ~KQProjection();
    //getters
    Bool_t IsDataRead() const { return fIsDataRead; }
    Bool_t GetVerbose() const { return fVerbose; }
    TH2D* GetHistogram() { return (TH2D*)fHistogram->Clone(); }
    TH1D* GetProjection() { return (TH1D*)fHistogram->ProjectionY()->Clone(); }
    Int_t GetEntries() { return fHistogram->GetEntries(); }
    Double_t GetEnergyRecoilMin() const { return fEnergyRecoilMin; }
    Double_t GetEnergyRecoilMax() const { return fEnergyRecoilMax; }
    Double_t GetNumBinsEnergyRecoil() const { return fNumBinsEnergyRecoil; }
    Double_t GetNumBinsQ() const { return fNumBinsQ; }
    Double_t GetQMin() const { return fQMin; }
    Double_t GetQMax() const { return fQMax; }
    
    const Char_t* GetBoloName() const { return fBoloName.c_str(); }
    Int_t GetEventCategory() const { return fEventCategory; }
    const Char_t* GetEventCategoryName() const;
    
    //setters
    void SetVerbose(Double_t aVerbose) { fVerbose = aVerbose; }
    void SetHistogram(TH2D* aHistogram) { fHistogram = aHistogram; }
    void SetEnergyRecoilMin(Double_t anEnergyRecoilMin)
    { 
      fEnergyRecoilMin = anEnergyRecoilMin;
      ResetHistogram();
    }
    void SetEnergyRecoilMax(Double_t anEnergyRecoilMax)
    {
      fEnergyRecoilMax = anEnergyRecoilMax; 
      ResetHistogram();
    }
    void SetNumBinsEnergyRecoil(Double_t aNumBinsEnergyRecoil) 
    {
      fNumBinsEnergyRecoil = aNumBinsEnergyRecoil;
      ResetHistogram();
    }
    void SetNumBinsQ(Double_t aNumBinsQ) 
    { 
      fNumBinsQ = aNumBinsQ;
      ResetHistogram();
    }
    void SetQMin(Double_t aQMin)
    { 
      fQMin = aQMin;
      ResetHistogram();
    }
    void SetQMax(Double_t aQMax) 
    { 
      fQMax = aQMax; 
      ResetHistogram();
    }
    void SetBoloName(const Char_t* aBoloName) { fBoloName = aBoloName; }
    void SetEventCategory(Int_t anEventCategory) { fEventCategory = anEventCategory; }
    void SetEventCategory(const Char_t* anEventCategory);
    
    Int_t Fill(Double_t x,Double_t y,Double_t w = 1);
    
    Bool_t ReadData(Double_t anEnergyRecoilMin,
                    Double_t anEnergyRecoilMax);
    virtual Bool_t MakeHistogram();
                    
    void Fit(TF1* aFunction);
    friend class KMultiQProjection;
    ClassDef(KQProjection,0);
                    
    
};

#endif //__KQPROJECTION_H__
