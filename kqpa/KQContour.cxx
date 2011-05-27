//_____________________________________________
//
// KQContour.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is used to evaluate the function value of a two-dimensional
// propability density function, so that the region where values are higher than
// this value is the confidence region for a specific confidence level.

#include "KQContour.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

ClassImp(KQContour);


/*
Double_t GetValueForConfidence(const Double_t* xx)
{
  TH2D* hist = (TH2D*)((Long_t)xx[0]);
  Double_t aConfidenceLevel = xx[1];
  Double_t aValue = xx[2];
  Double_t aNumEntries = hist->GetEntries();
  
  Int_t aSum = 0;
  for(Int_t k = 0; k< hist->GetNbinsX(); ++k)
     for(Int_t l = 0; l<hist->GetNbinsY(); ++l)
       if(hist->GetBinContent(k,l)>aValue*hist->GetXaxis()->GetBinWidth(k)*
         hist->GetYaxis()->GetBinWidth(l)*aNumEntries)
       aSum += hist->GetBinContent(k,l);
     
  Double_t aResult = (Double_t)aSum/aNumEntries-aConfidenceLevel;
  return aResult*aResult;      
}
*/

KQContour::KQContour(TF2* aFunction) : fFunction(aFunction)
{
  fNumBinsX = 1000;
  fNumBinsY = 1000;
  fNumEntries = 1000000;
  
  fHistogram = new TH2D("hist","hist",
            fNumBinsX,fFunction->GetXmin(),fFunction->GetXmax(),
            fNumBinsY,fFunction->GetYmin(),fFunction->GetYmax());
  fHistogram->SetDirectory(0);
            
  
  fHistogram->FillRandom(fFunction->GetName(),fNumEntries);
  
  for(Int_t k = 0; k<fHistogram->GetNbinsX(); ++k)
    for(Int_t l = 0; l<fHistogram->GetNbinsY(); ++l)
      fBins.push_back(KQBinRecord(k,l,fHistogram->GetBinContent(k,l)));
    
  sort(fBins.begin(),fBins.end());
}

void KQContour::RefillHistogram()
{
  fHistogram->FillRandom(fFunction->GetName(),fNumEntries);
}


Double_t KQContour::GetConfidence(Double_t aValue)
{
  Int_t aSum = 0;
  for(Int_t k = 0; k< fHistogram->GetNbinsX(); ++k)
     for(Int_t l = 0; l<fHistogram->GetNbinsY(); ++l)
       if(fHistogram->GetBinContent(k,l)>aValue*fHistogram->GetXaxis()->GetBinWidth(k)*
         fHistogram->GetYaxis()->GetBinWidth(l)*fNumEntries)
       aSum += fHistogram->GetBinContent(k,l);
     
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

/*  
 ROOT::Math::Minimizer *theMinimizer = ROOT::Math::Factory::CreateMinimizer(aMinimizerName,aMethod);
 
 theMinimizer->SetMaxFunctionCalls(aMaxNumFunctionCalls); //this sets the maximal number of function calls
  theMinimizer->SetMaxIterations(aMaxNumIterations); //this sets the maximal number of iteration
  theMinimizer->SetTolerance(aTolerance); //this sets the tolerance
    
  ROOT::Math::Functor f(&GetValueForConfidence,3); 
   
  theMinimizer->SetFunction(f);
 
  // Set the fixed variables
  Long_t aNum = (Long_t)(fHistogram);
  theMinimizer->SetFixedVariable(0,"hist",aNum);
  theMinimizer->SetFixedVariable(1,"aConfidenceLevel",aConfidenceLevel);
   
  //set the variable to be minimized
  theMinimizer->SetVariable(2,"aValue",100,0.1);
 
  theMinimizer->Minimize(); 
  
  const Double_t* xs = theMinimizer->X();
  return xs[2];
*/

  Int_t aSum = 0;
  Int_t anIndex = fBins.size()-1;
  for(;anIndex>=0 && aSum < aConfidenceLevel*fHistogram->GetEntries(); --anIndex)
    aSum += fBins[anIndex].GetBinContent();
  
  if(anIndex<0)
    ++anIndex;
  
  return fBins[anIndex].GetBinContent()/fHistogram->GetXaxis()->GetBinWidth(0)
         /fHistogram->GetYaxis()->GetBinWidth(0)/fNumEntries;
}

TH2D* KQContour::GetContourHistogram(Double_t aConfidenceLevel)
{
  TH2D* aContourHistogram = new TH2D("hist_contour",
                              TString::Format("contour for CL=%f",aConfidenceLevel).Data(),
                              fNumBinsX,
                              fHistogram->GetXaxis()->GetXmin(),
                              fHistogram->GetXaxis()->GetXmax(),
                              fNumBinsY,
                              fHistogram->GetYaxis()->GetXmin(),
                              fHistogram->GetYaxis()->GetXmax());
  aContourHistogram->SetDirectory(0);
  Int_t aSum = 0;
  Int_t anIndex = fBins.size()-1;
  for(;anIndex>=0 && aSum < aConfidenceLevel*fHistogram->GetEntries(); --anIndex) {
    aSum += fBins[anIndex].GetBinContent();
    aContourHistogram->SetBinContent(fBins[anIndex].GetX(),
                                     fBins[anIndex].GetY(),
                                     1);
  }
  cout << "confidence: " << (Double_t)aSum/fNumEntries << endl;
  return aContourHistogram;
}