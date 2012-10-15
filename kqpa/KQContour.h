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

class KQContour  {
  private:
     TF2* fFunction; // function describing the distribution the histogram is
    // filled with
    Int_t fNumBinsX; // number of bins on the X axis
    Int_t fNumBinsY; // number of bins on the Y axis
    Int_t fNumEntries; // number of total entries

    TH2D* fHistogram; // histogram
    vector<KQBinRecord> fBins; // list of KQBinRecords sorted in descending
    // order with respect to the bin content
    Double_t fConfidenceLevel; // confidence level
    Double_t fConfidenceLevelError; // uncertainty on the confidence level
    
    void RefillHistogram();
    
  public:
    KQContour(TF2* aFunction = 0,
              Double_t aNumBinsX = 1000,
              Double_t aNumBinsY = 1000,
              Double_t aNumEntries = 100000000 );
    
    virtual ~KQContour();
    
     void SetNumBinsX(Double_t aNumBinsX)
    { 
      fNumBinsX = aNumBinsX;
      RefillHistogram();
    }
    void SetNumBinsY(Double_t aNumBinsY)
    {
      fNumBinsY = aNumBinsY;
      RefillHistogram();
    }
    void SetNumEntries(Double_t aNumEntries)
    { 
      fNumEntries = aNumEntries;
      RefillHistogram();
    }
   void SetFunction(TF2* aFunction)
   {
     fFunction = aFunction;
     RefillHistogram();
   }
   
    Int_t GetNumBinsX() const { return fNumBinsX; }
    Int_t GetNumBinsY() const { return fNumBinsY; }
    Int_t GetNumEntries() const { return fNumEntries; }
    Double_t GetConfidenceLevel() const { return fConfidenceLevel; }
    Double_t GetConfidenceLevelError() const { return fConfidenceLevelError; }
    TF2* GetFunction() { return fFunction; }
    TH2D* GetHistogram() { return (TH2D*)(fHistogram->Clone()); }
     Double_t GetContour(Double_t aConfidenceLevel);
    TH2D* GetContourHistogram(Double_t aConfidenceLevel);                    
    Double_t GetConfidence(Double_t aValue);
    Double_t EvaluateConfidence(Double_t* x,Double_t* p);

                        
  ClassDef(KQContour,1);
};

#endif