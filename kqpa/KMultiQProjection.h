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
#include "KQProjection.h"

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
    Bool_t fVerbose;
    string fSourceFile;
    string fBoloName;
    Int_t fEventCategory;
    Int_t fNumProjections;
    Int_t fNumBinsEnergyRecoil;
    Int_t fNumBinsQ;
    Double_t fQMin;
    Double_t fQMax;
    vector<KQProjection*> fHistograms;
  public:
    KMultiQProjection(const Char_t* aSourceFile,
                      const Char_t* aBoloName,
                      const Char_t* anEventCategory = "fiducial",
                      Double_t aNumBinsEnergyRecoil = 1000,
                      Double_t aNumBinsQ = 1000,
                      Double_t aQMin = 0,
                      Double_t aQMax = 2);
    ~KMultiQProjection();
                      
    //getters
    Bool_t GetVerbose() const { return fVerbose; }
    Double_t GetNumProjections() const { return fNumProjections; }
    Double_t GetNumBinsEnergyRecoil() const { return fNumBinsEnergyRecoil; }
    Double_t GetNumBinsQ() const { return fNumBinsQ; }
    Double_t GetQMin() const { return fQMin; }
    Double_t GetQMax() const { return fQMax; }
    const Char_t* GetEventCategoryName() const;
    
    TH2D* GetHistogram(UInt_t anIndex) { 
      if(anIndex>=fHistograms.size()) {
        cout << " KMultiQProjection::Get(UInt_t anIndex): invalid index, must be in [0,"
        << fHistograms.size()-1 << "]" << endl;
        return 0;
      }
      else
        return(fHistograms[anIndex]->GetHistogram());
    }
    
    TH1D* GetProjection(UInt_t anIndex) {
      if(anIndex>=fHistograms.size()) {
        cout << "KMultiQProjection::Get(UInt_t anIndex): invalid index, must be in [0,"
        << fHistograms.size()-1 << "]" << endl;
        return 0;
      }
      else
        return fHistograms[anIndex]->GetProjection();
    }
    
    //setters
    void SetVerbose(Bool_t aVerbose) { fVerbose = aVerbose; }
    void SetNumBinsEnergyRecoil(Int_t aNumBinsEnergyRecoil)
    { fNumBinsEnergyRecoil = aNumBinsEnergyRecoil; }
    void SetNumBinsQ(Int_t aNumBinsQ)
    { fNumBinsQ = aNumBinsQ; }
    void SetQMin(Double_t aQMin) { fQMin = aQMin; }
    void SetQMax(Double_t aQMax) { fQMax = aQMax; } 
    void SetEventCategory(const Char_t* anEventCategory);
    
    Bool_t ReadData(Int_t aNumProjections,
             Double_t* theEnergyRecoilMins,
             Double_t* theEnergyRecoilMaxs);
             
    ClassDef(KMultiQProjection,0);
};

#endif

