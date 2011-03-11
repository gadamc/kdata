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

#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"

#include <iostream>
#include <string>
using namespace std;

#ifndef __KQPROJECTION_H__
#define __KQPROJECTION_H__

class KQProjection {
  private:
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
  public:
    KQProjection();
    KQProjection(const KQProjection& anotherKQProjection);
    KQProjection(const Char_t* aSourceFile,
                 const Char_t* aBoloName,
                 Double_t anEnergyRecoilMin = 0,
                 Double_t anEnergyRecoilMax = 1000,
                 const Char_t* aCategoryName = "fiducial",
                 const Char_t* aHistogramName = "hist");
    //getters
    TH2D* GetHistogram() { return fHistogram; }
    TH1D* GetProjection() { return fHistogram->ProjectionY(); }
    Double_t GetEnergyRecoilMin() const { return fEnergyRecoilMin; }
    Double_t GetEnergyRecoilMax() const { return fEnergyRecoilMax; }
    Double_t GetNumBinsEnergyRecoil() const { return fNumBinsEnergyRecoil; }
    Double_t GetNumBinsQ() const { return fNumBinsQ; }
    Double_t GetQMin() const { return fQMin; }
    Double_t GetQMax() const { return fQMax; }
    
    const Char_t* GetBoloName() const { return fBoloName.c_str(); }
    const Char_t* GetEventCategory() const;
    
    //setters
    void SetEnergyRecoilMin(Double_t anEnergyRecoilMin) { fEnergyRecoilMin = anEnergyRecoilMin; }
    void SetEnergyRecoilMax(Double_t anEnergyRecoilMax) { fEnergyRecoilMax = anEnergyRecoilMax; }
    void SetNumBinsEnergyRecoil(Double_t aNumBinsEnergyRecoil) { fNumBinsEnergyRecoil = aNumBinsEnergyRecoil; }
    void SetNumBinsQ(Double_t aNumBinsQ) { fNumBinsQ = aNumBinsQ; }
    void SetQMin(Double_t aQMin) { fQMin = aQMin; }
    void SetQMax(Double_t aQMax) { fQMax = aQMax; }
    void SetBoloName(const Char_t* aBoloName) { fBoloName = aBoloName; }
    void SetEventCategory(const Char_t* anEventCategory);
    
    Bool_t ReadData(const Char_t* aSourceFile,
                    const Char_t* aBoloName,
                    Double_t anEnergyRecoilMin,
                    Double_t anEnergyRecoilMax,
                    const Char_t* aCategoryName);
                    
    void Fit(TF1* aFunction);
    ClassDef(KQProjection,0);
                    
    
};

#endif //__KQPROJECTION_H__
