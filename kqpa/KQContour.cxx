//_____________________________________________
//
// KQContour.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is used to evaluate the function value of the two-dimensional
// propability density function g(E_recoil,Q) describing the distribution of
// (E_recoil,Q), (documentation in ~/doc/ERecoilQDistribution.pdf)
// so that the region where values are higher than this value is the
// confidence region for a specific confidence level.

#include "KQContour.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

ClassImp(KQContour);


KQContour::KQContour(TF2* aFunction,
                     Double_t aNumBinsX,
                     Double_t aNumBinsY,
                     Double_t aNumEntries)
: fFunction(aFunction), fNumBinsX(aNumBinsX),
fNumBinsY(aNumBinsY), fNumEntries(aNumEntries)
{
  // This constructor creates a histogram  and fills it with the expected
  // numbers for each bin according the the specified 
  // function f(X,Y), expediently  the propability density function
  // g(E_recoil,Q)
  //
  // The bin contents 
  // 
  // BEGIN_LATEX
  //
  // n_{i,j} = l_{X}  l_{Y} n_{entries} f(X_{i},Y_{j})
  //
  // END_LATEX
  //
  // with corresponding indices i and j
  // are then sorted in a list of KQBinRecords

  fHistogram = new TH2D("hist","hist",
            fNumBinsX,fFunction->GetXmin(),fFunction->GetXmax(),
            fNumBinsY,fFunction->GetYmin(),fFunction->GetYmax());
  fHistogram->SetDirectory(0);
  fHistogram->GetXaxis()->SetTitle("E_{recoil} [keV]");
  fHistogram->GetYaxis()->SetTitle("Q");
  
  // list of  KQBinRecords is built
  Double_t aBinContent = 0;
  for(Int_t k = 0; k<fHistogram->GetNbinsX(); ++k)
    for(Int_t l = 0; l<fHistogram->GetNbinsY(); ++l) {
      aBinContent= fNumEntries * fHistogram->GetXaxis()->GetBinWidth(0)
      * fHistogram->GetYaxis()->GetBinWidth(0) *
      fFunction->Eval(fHistogram->GetXaxis()->GetXmin()+(k-0.5) *               
           fHistogram->GetXaxis()->GetBinWidth(0),
                               fHistogram->GetYaxis()->GetXmin() + (l -0.5) *
                               fHistogram->GetYaxis()->GetBinWidth(0));
      fHistogram->SetBinContent(k,l,aBinContent);
      fBins.push_back(KQBinRecord(k,l,aBinContent));
    }
    
   //list of KQBinRecords is sorted
  sort(fBins.begin(),fBins.end());
}

KQContour::~KQContour()
{
  if(fHistogram) {
    delete fHistogram;
    fHistogram = 0;
  }
}

void KQContour::RefillHistogram()
{
  //This method refills the histogram the same way the constructor does
  // (if the properties of the histogram or the function has changed)
  
  Double_t aBinContent = 0;
  for(Int_t k = 0; k<fHistogram->GetNbinsX(); ++k)
    for(Int_t l = 0; l<fHistogram->GetNbinsY(); ++l) {
      aBinContent= fNumEntries *
      fFunction->Eval(fHistogram->GetXaxis()->GetXmin()+(k-0.5) *               
           fHistogram->GetXaxis()->GetBinWidth(0),
                               fHistogram->GetYaxis()->GetXmin() + (l -0.5) *
                               fHistogram->GetYaxis()->GetBinWidth(0));
      fHistogram->SetBinContent(k,l,aBinContent);
      fBins.push_back(KQBinRecord(k,l,aBinContent));
    }
    sort(fBins.begin(),fBins.end());
}


Double_t KQContour::GetConfidence(Double_t aValue)
{
  // This method gets the confidence level for a specific function value
  //
  // BEGIN_LATEX
  //
  // f_{0} = f(z)
  // z  #in { (X,Y) | f(X,Y) = f_{0} }
  //
  // END_LATEX
  //
  // The bin contents of the sorted list of KQBinRecords are successively 
  // summed from highest bin content to decreasing bin contents until the 
  // bin contents fall below 
  //
  // BEGIN_LATEX
  // 
  // n_{min} = f_{0} \cdot l_{X} \cdot l_{Y} \cdot n_{entries} 
 //
 // END_LATEX
  
  Int_t aSum = 0;
  
  Int_t anIndex = 0;
  while(fBins[anIndex].GetBinContent()>aValue *
             fHistogram->GetXaxis()->GetBinWidth(0) *
             fHistogram->GetYaxis()->GetBinWidth(0) * 
             fNumEntries)
  {
       aSum +=fBins[anIndex].GetBinContent();
  }
     
  Double_t aResult = (Double_t)aSum/fNumEntries;
  return aResult;     
}


Double_t KQContour::EvaluateConfidence(Double_t* x, Double_t* p)
{
  Double_t result = GetConfidence(x[0])-p[0];
  return result;
}




Double_t KQContour::GetContour(Double_t aConfidenceLevel)
{
  // This method gets the function value for  a specified confidence level
  // This is done by summing the bin contents of the sorted list of KQBinRecords
  // starting from the bin with highest bin content and proceeding with bins
  // with decreasing bin content until the specified confidence level is reached
  //
  // BEGIN_LATEX
  //
  // CL = #frac{\Sigma}{n_{entries}}
  //
  // END_LATEX
  //
  // Then the function value is given by quotient of the most recently added
  // bin content and the bin widths and number of total entries of the histogram
  //
  // BEGIN_LATEX
  //
  // g(CL) = #frac{n_{last}}{l_{x} l_{y} n_{entries}}
  //
  // END_LATEX
  //
  // The uncertainty on the confidence level is then set to
  //
  // BEGIN_LATEX
  //
  // #sigma_{CL} = #frac{n_{last}}{n_{entries}}
  //
  // END_LATEX
  
 
  fConfidenceLevel = aConfidenceLevel;

  Int_t aSum = 0;
  Int_t anIndex = fBins.size()-1;
  for(;anIndex>=0 && aSum < aConfidenceLevel*fNumEntries; --anIndex)
    aSum += fBins[anIndex].GetBinContent();
  
  if(anIndex<0)
    ++anIndex;
  
  fConfidenceLevelError = fBins[anIndex].GetBinContent()/fNumEntries;
  
  return fBins[anIndex].GetBinContent()/fHistogram->GetXaxis()->GetBinWidth(0)
         /fHistogram->GetYaxis()->GetBinWidth(0)/fNumEntries;
}



TH2D* KQContour::GetContourHistogram(Double_t aConfidenceLevel)
{
  // This method gets a histogram representing the confidence region for the 
  // specified confidence level
  // It has bin content 1 for bins in the confidence region and 0 else
  
  TH2D* aContourHistogram = new TH2D("hist_contour",
                              TString::Format("contour for CL=%f",aConfidenceLevel).Data(),
                              fNumBinsX,
                              fHistogram->GetXaxis()->GetXmin(),
                              fHistogram->GetXaxis()->GetXmax(),
                              fNumBinsY,
                              fHistogram->GetYaxis()->GetXmin(),
                              fHistogram->GetYaxis()->GetXmax());
  aContourHistogram->AddDirectory(0);
  aContourHistogram->GetXaxis()->SetTitle("E_{recoil} [keV]");
  aContourHistogram->GetYaxis()->SetTitle("Q");
  Int_t aSum = 0;
  Int_t anIndex = fBins.size()-1;
  for(;anIndex>=0 && aSum < aConfidenceLevel*fNumEntries; --anIndex) {
    aSum += fBins[anIndex].GetBinContent();
    aContourHistogram->SetBinContent(fBins[anIndex].GetX(),
                                     fBins[anIndex].GetY(),
                                     1);
  }
  cout << "confidence: " << (Double_t)aSum/fNumEntries << endl;
  return aContourHistogram;
}