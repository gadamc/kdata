//_____________________________________________
//
// KMultiQProjection.h
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

#ifndef __KMULTIQPROJECTION_H__
#define __KMULTIQPROJECTION_H__

class KMultiQProjection {
  private:
    string fSourceFile;
    string fBoloName;
    Int_t fEventCategory;
    Int_t fNumProjections;
    Int_t fNumBinsEnergyRecoil;
    Int_t fNumBinsQ;
    Double_t fQMin;
    Double_t fQMax;
    vector<TH2D*> fHistograms;
  public:
    KMultiQProjection(const Char_t* aSourceFile,
                      const Char_t* aBoloName,
                      Int_t aNumProjections,
                      Double_t* theEnergyRecoilMins,
                      Double_t* theEnergyRecoilMaxs,
                      const Char_t* anEventCategory);
    ~KMultiQProjection();
                      
    //getters
    Double_t GetNumProjections() const { return fNumProjections; }
    Double_t GetNumBinsEnergyRecoil() const { return fNumBinsEnergyRecoil; }
    Double_t GetNumBinsQ() const { return fNumBinsQ; }
    Double_t GetQMin() const { return fQMin; }
    Double_t GetQMax() const { return fQMax; }
    const Char_t* GetEventCategory() const;
    
    TH2D* GetHistogram(UInt_t anIndex) { 
      if(anIndex>=fHistograms.size()) {
        cout << " KMultiQProjection::Get(UInt_t anIndex): invalid index, must be in [0,"
        << fHistograms.size()-1 << "]" << endl;
        return 0;
      }
      else
        return (TH2D*)fHistograms[anIndex]->Clone();
    }
    
    TH1D* GetProjection(UInt_t anIndex) {
      if(anIndex>=fHistograms.size()) {
        cout << "KMultiQProjection::Get(UInt_t anIndex): invalid index, must be in [0,"
        << fHistograms.size()-1 << "]" << endl;
        return 0;
      }
      else
        return ((TH2D*)fHistograms[anIndex]->Clone())->ProjectionY();
    }
    
    //setters
    void SetNumBinsEnergyRecoil(Int_t aNumBinsEnergyRecoil)
    { fNumBinsEnergyRecoil = aNumBinsEnergyRecoil; }
    void SetNumBinsQ(Int_t aNumBinsQ)
    { fNumBinsQ = aNumBinsQ; }
    void SetQMin(Double_t aQMin) { fQMin = aQMin; }
    void SetQMax(Double_t aQMax) { fQMax = aQMax; } 
    void SetEventCategory(const Char_t* anEventCategory);
    
    Bool_t ReadData(const Char_t* aSourceFile,
             const Char_t* aBoloName,
             Int_t aNumProjections,
             Double_t* theEnergyRecoilMins,
             Double_t* theEnergyRecoilMaxs,
             const Char_t* anEventCategory);
             
    ClassDef(KMultiQProjection,1);
};

#endif

