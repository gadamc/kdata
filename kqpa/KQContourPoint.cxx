//_____________________________________________
//
// KQContourPoint.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class represents a single EDW event (Q, E_{Recoil}, sigma_ion,
// sigma_heat), whose confidence region for a specified confidence level can be
// drawn ( marker for central value, and contour line surrounding the
// confidence region

#include "KQContourPoint.h"

#include "TMath.h"

ClassImp(KQContourPoint);



KQContourPoint::KQContourPoint(Double_t aQvalue,
                               Double_t anEnergyRecoil,
                               Double_t aSigmaEnergyIon,
                               Double_t aSigmaEnergyHeat,
                               Double_t aSigmaEnergyIonHeat,
                               Double_t aConfidenceLevel,
                               Double_t aVoltageBias,
                               Double_t anEpsilon,
                               Double_t anNpx,
                               Double_t anNpy,
                               Double_t aNumSigmas
                              )
  : fQvalue(aQvalue), fEnergyRecoil(anEnergyRecoil),
  fSigmaEnergyIon(aSigmaEnergyIon),
  fSigmaEnergyHeat(aSigmaEnergyHeat), fSigmaEnergyIonHeat(aSigmaEnergyIonHeat),
  fVoltageBias(aVoltageBias), fEpsilon(anEpsilon),
  fConfidenceLevel(aConfidenceLevel)
{
  
  Double_t aSigmaEnergyRecoil = TMath::Sqrt(
  (1+ fVoltageBias/fEpsilon)*(1+fVoltageBias/fEpsilon)*fSigmaEnergyHeat*
  fSigmaEnergyHeat
  + fVoltageBias*fVoltageBias/fEpsilon/fEpsilon*fSigmaEnergyIon*fSigmaEnergyIon);
  Double_t aSigmaQvalue = TMath::Sqrt(
  fQvalue*fQvalue/fEnergyRecoil/fEnergyRecoil*(1+fVoltageBias/fEpsilon)*
  (1+fVoltageBias/fEpsilon)*fSigmaEnergyHeat*fSigmaEnergyHeat
  + TMath::Power((1/fEnergyRecoil + fVoltageBias/fEpsilon*fQvalue/fEnergyRecoil)*
  fSigmaEnergyIon,2));
  // [0] : fQvalue
  // [1] : fEnergyRecoil
  // [2] : fSigmaIon
  // [3] : fSigmaHeat
  // [4] : fSigmaIonHeat
  // [5] : fVoltageBias
  // [6] : fEpsilon
  
  fFunction = new TF2(TString::Format("f%d_%d_%d_%d_%d",Int_t(aQvalue*100),
                                      Int_t(anEnergyRecoil),
                                      Int_t(aSigmaEnergyIon*100),
                                      Int_t(aSigmaEnergyHeat*100),
                                      Int_t(aSigmaEnergyIonHeat*100)).Data(),
 "TMath::Exp(-0.5 *( [3]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0]*[1])^2"
 "+ [2]^2/([3]^2*[2]^2 - [4]^4) *( (1 + y * [5])/(1 + [5]) * x "
 "- (1 + [0] * [5])/(1 + [5]) * [1])^2"
 "- 2 * [4]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0]*[1]) * "
 "( (1 + y * [5])/(1 + [5]) * x - (1 + [0] * [5])/(1 + [5]) *[1])))"
 "* TMath::Abs([1])/ 2 / TMath::Pi() / TMath::Sqrt([2]^2 * [3]^2 - [4]^4) /"
 "(1+[5])",
                      anEnergyRecoil-aNumSigmas*aSigmaEnergyRecoil,
                      anEnergyRecoil+aNumSigmas*aSigmaEnergyRecoil,
                      aQvalue-aNumSigmas*aSigmaQvalue,
                      aQvalue+aNumSigmas*aSigmaQvalue);

  fFunction->SetParameter(0,fQvalue);
  fFunction->SetParameter(1,fEnergyRecoil);
  fFunction->SetParameter(2,fSigmaEnergyIon);
  fFunction->SetParameter(3,fSigmaEnergyHeat);
  fFunction->SetParameter(4,fSigmaEnergyIonHeat);
  fFunction->SetParameter(5,fVoltageBias/fEpsilon);
  fFunction->SetNpx(anNpx);
  fFunction->SetNpy(anNpy);
  fFunction->SetLineStyle(1);
  fFunction->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fFunction->GetYaxis()->SetTitle("Q");
 
  fMarker = new TMarker(fEnergyRecoil,fQvalue,2);
  
 SetConfidenceLevel(aConfidenceLevel);
}




KQContourPoint::~KQContourPoint()
{
  if(fFunction)
    delete fFunction;
  if(fMarker)
      delete fMarker;
}

void KQContourPoint::SetConfidenceLevel(Double_t aConfidenceLevel)
{
  fConfidenceLevel = aConfidenceLevel;
  KQContour aContour(fFunction);
  fFunction->SetContour(1);
  fFunction->SetContourLevel(0,aContour.GetContour(fConfidenceLevel));
  fConfidenceLevelError = aContour.GetConfidenceLevelError();
}

void KQContourPoint::Draw(Option_t* anOption)
{
  //draws the event
  fFunction->Draw(anOption);
  fMarker->Draw("same");
}

void KQContourPoint::SetRange(Double_t xmin, Double_t ymin,
                              Double_t xmax, Double_t ymax)
{
  fFunction->SetRange(xmin,ymin,xmax,ymax);
}

TH2D* KQContourPoint::GetHistogram()
{
  //This method returns the 
  KQContour aContour(fFunction);
  return aContour.GetHistogram();
}

TH2D* KQContourPoint::GetContourHistogram()
{
  //This method returns a hard copy of a histogram which has bin contents of 1
  //for bins in the confidence region and 0 else
  KQContour aContour(fFunction);
  return aContour.GetContourHistogram(fConfidenceLevel);
}



