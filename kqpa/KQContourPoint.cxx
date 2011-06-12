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



KQContourPoint::KQContourPoint(Double_t aQvalueOrEnergyIon,
                               Double_t anEnergyRecoilOrEnergyHeat,
                               const Char_t* aMode,
                               Double_t aSigmaEnergyIon,
                               Double_t aSigmaEnergyHeat,
                               Double_t aSigmaEnergyIonHeat,
                               Double_t aConfidenceLevel,
                               Double_t aVoltageBias,
                               Double_t anEpsilon,
                               Double_t aNumBinsX,
                               Double_t aNumBinsY,
                               Double_t aNumSigmas
                              )
  : fQvalue(aQvalueOrEnergyIon), fEnergyRecoil(anEnergyRecoilOrEnergyHeat),
  fSigmaEnergyIon(aSigmaEnergyIon),
  fSigmaEnergyHeat(aSigmaEnergyHeat), fSigmaEnergyIonHeat(aSigmaEnergyIonHeat),
  fVoltageBias(aVoltageBias), fEpsilon(anEpsilon),
  fConfidenceLevel(aConfidenceLevel), fNumBinsX(aNumBinsX),
  fNumBinsY(aNumBinsY)
{
  // The constructor generates the pdf g(E_recoil,Q)
  // (documentation in ~/doc/ERecoiLQDistribution.pdf)
  // and sets the  specified fixed parameters
  // The uncertainties in E_recoil and Q direction are estimated
  // by standard error propagation and then the ranges of the function
  // set to the modal values (aQvalue, anEnergyRecoil)
  // +/- a specified multiple of them
  // A marker is set for the modal values and a contour line for the specified
  // confidence level
  
  if(!strcmp(aMode,"QErecoil"))
  {
    fEnergyIon = fQvalue* fEnergyRecoil;
    fEnergyHeat = (1+ fQvalue*fVoltageBias/fEpsilon)/(1+fVoltageBias/fEpsilon) *
    fEnergyRecoil;
  }
  else
    if(!strcmp(aMode,"IonHeat"))
    {
      fEnergyIon = aQvalueOrEnergyIon;
      fEnergyHeat = anEnergyRecoilOrEnergyHeat;
      fEnergyRecoil = (1 + fVoltageBias/ fEpsilon) * fEnergyHeat
        - fVoltageBias/fEpsilon * fEnergyIon;
      fQvalue = fEnergyIon/fEnergyRecoil;
    }
    else
    {
      cout << "KQContourPoint(): invalid mode " << endl;
      cout << "  possible modes are \"QErecoil\" and \"IonHeat\"" << endl;
      return;
    }
  
  
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
  
  fFunction = new TF2(TString::Format("f%d_%d_%d_%d_%d",
                                      Int_t(aQvalueOrEnergyIon*100),
                                      Int_t(anEnergyRecoilOrEnergyHeat),
                                      Int_t(aSigmaEnergyIon*100),
                                      Int_t(aSigmaEnergyHeat*100),
                                      Int_t(aSigmaEnergyIonHeat*100)).Data(),
 "TMath::Exp(-0.5 *( [3]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0])^2"
 "+ [2]^2/([3]^2*[2]^2 - [4]^4) *( (1 + y * [5])/(1 + [5]) * x "
 "- [1])^2"
 "- 2 * [4]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0]) * "
 "( (1 + y * [5])/(1 + [5]) * x - [1])))"
 "* TMath::Abs(x)/ 2 / TMath::Pi() / TMath::Sqrt([2]^2 * [3]^2 - [4]^4) /"
 "(1+[5])",
                     fEnergyRecoil - aNumSigmas*aSigmaEnergyRecoil,
                     fEnergyRecoil + aNumSigmas*aSigmaEnergyRecoil,
                     fQvalue - aNumSigmas*aSigmaQvalue,
                     fQvalue + aNumSigmas*aSigmaQvalue);

  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
  fFunction->SetParameter(2,fSigmaEnergyIon);
  fFunction->SetParameter(3,fSigmaEnergyHeat);
  fFunction->SetParameter(4,fSigmaEnergyIonHeat);
  fFunction->SetParameter(5,fVoltageBias/fEpsilon);
  fFunction->SetNpx(2000);
  fFunction->SetNpy(2000);
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
  // This method changes the confidence level and adjusts the single contour
  // line of the function
  
  fConfidenceLevel = aConfidenceLevel;
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  fFunction->SetContour(1);
  fFunction->SetContourLevel(0,aContour.GetContour(fConfidenceLevel));
  fConfidenceLevelError = aContour.GetConfidenceLevelError();
}

void KQContourPoint::Draw(Option_t* anOption)
{
  // This method draws the event
  // (marker for the modal values and contour line for the confidence region)
  fFunction->Draw(anOption);
  fMarker->Draw("same");
}

void KQContourPoint::SetRange(Double_t xmin, Double_t ymin,
                              Double_t xmax, Double_t ymax)
{
  //This method sets the range of the function
  
  fFunction->SetRange(xmin,ymin,xmax,ymax);
}

TH2D* KQContourPoint::GetHistogram()
{
  // This method returns a hard copy of a histogram representing the
  // (E_recoil,Q)-distribution,which was  used to determine the contour line for
  // the specified confidence level
  
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  return aContour.GetHistogram();
}

TH2D* KQContourPoint::GetContourHistogram()
{
  //This method returns a hard copy of a histogram which has bin contents of 1
  //for bins in the confidence region and 0 else
  
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  return aContour.GetContourHistogram(fConfidenceLevel);
}

void KQContourPoint::SetQvalue(Double_t aQvalue)
{
  // This method sets the modal value of Q and adjusts the corresponding
  // modal/mean values (Gaussians)  of the ion and heat energy 
  
  fQvalue = aQvalue;
  fEnergyIon = fQvalue*fEnergyRecoil;
  fEnergyHeat = (1+ fQvalue* fVoltageBias/fEpsilon)/(1+fVoltageBias/fEpsilon) *
    fEnergyRecoil;
  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
}

void KQContourPoint::SetEnergyRecoil(Double_t anEnergyRecoil)
{
  // This method sets the modal value of the recoil energy and adjusts the
  // corresponding modal/mean values (Gaussians) of the ion and heat energy
  
  fEnergyRecoil = anEnergyRecoil;
  fEnergyIon = fQvalue* fEnergyRecoil;
  fEnergyHeat = (1+ fQvalue*fVoltageBias/fEpsilon)/(1+fVoltageBias/fEpsilon) *
    fEnergyRecoil;
  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
}

void KQContourPoint::SetEnergyIon(Double_t anEnergyIon)
{
  // This method sets the modal/mean value of the ion energy and adjusts
  // the corresponding modal values of Q and the recoil energy
  
  fEnergyIon = anEnergyIon;
  fEnergyRecoil = (1 + fVoltageBias/ fEpsilon) * fEnergyHeat
    - fVoltageBias/fEpsilon * fEnergyIon;
  fQvalue = fEnergyIon/fEnergyRecoil;
  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
}

void KQContourPoint::SetEnergyHeat(Double_t anEnergyHeat)
{
  // This method sets the modal values of the heat energy and adjusts
  // the corresponding modal values of Q and the recoil energy
  
  fEnergyHeat = anEnergyHeat;
  fEnergyRecoil = (1 + fVoltageBias/ fEpsilon) * fEnergyHeat
    - fVoltageBias/fEpsilon * fEnergyIon;
  fQvalue = fEnergyIon/fEnergyRecoil;
  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
}

void KQContourPoint::SetNumBinsX(Int_t aNumBinsX)
{
  // This method sets the number of bins in X direction of the  distribution
 // distribution histogram and recalculates the contour for the specified
 // confidence level with the new histogram
  fNumBinsX = aNumBinsX;
  SetConfidenceLevel(fConfidenceLevel);
}

void KQContourPoint::SetNumBinsY(Int_t aNumBinsY)
{
  // This method sets the number of bins in Y direction of the  distribution
  // distribution histogram and recalculates the contour for the specified
  // confidence level with the new histogram
  fNumBinsY = aNumBinsY;
  SetConfidenceLevel(fConfidenceLevel);
}





