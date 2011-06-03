//_____________________________________________
//
// KQContour.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


#include "Rtypes.h"
#include "TF2.h"
#include "TH2D.h"
#include "KQBinRecord.h"
#include <vector>
#include <algorithm>
using namespace std;

#ifndef __KQCONTOUR_H__
#define __KQCONTOUR_H__

class KQContour {
  private:
    Int_t fNumBinsX;
    Int_t fNumBinsY;
    Int_t fNumEntries;
    TF2* fFunction;
    TH2D* fHistogram;
    vector<KQBinRecord> fBins;
    Double_t fConfidenceLevel;
    Double_t fConfidenceLevelError;
    
    void RefillHistogram();
    
  public:
    KQContour(TF2* aFunction);
    Double_t GetContour(Double_t aConfidenceLevel);
    
    TH2D* GetContourHistogram(Double_t aConfidenceLevel);
                        
    Double_t GetConfidence(Double_t aValue);
    Double_t EvaluateConfidence(Double_t* x,Double_t* p);
    //setters
    void SetNumBinsX(Double_t aNumBinsX) { fNumBinsX = aNumBinsX; }
    void SetNumBinsY(Double_t aNumBinsY) { fNumBinsY = aNumBinsY; }
    void SetNumEntries(Double_t aNumEntries)
    { 
      fNumEntries = aNumEntries;
      RefillHistogram();
    }

    
    //getters
    Int_t GetNumBinsX() const { return fNumBinsX; }
    Int_t GetNumBinsY() const { return fNumBinsY; }
    Int_t GetNumEntries() const { return fNumEntries; }
    Double_t GetConfidenceLevel() const { return fConfidenceLevel; }
    Double_t GetConfidenceLevelError() const { return fConfidenceLevelError; }
    
    
    TF2* GetFunction() { return fFunction; }
    TH2D* GetHistogram() { return (TH2D*)(fHistogram->Clone()); }

                        
  ClassDef(KQContour,0);
};

#endif