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
  fNumBinsY(aNumBinsY), fNumSigmas(aNumSigmas)
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
  
  fFunction = 0;
  fMarker = 0;
  
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
    
   SetFunction();
   fPreviousVersion = new KQContourPoint(*this);
}

KQContourPoint::KQContourPoint(const KQContourPoint& anotherPoint)
{
    this->fPreviousVersion = 0;
    this->fQvalue = anotherPoint.fQvalue;
    this->fEnergyRecoil = anotherPoint.fEnergyRecoil;
    this->fEnergyIon = anotherPoint.fEnergyIon;
    this->fEnergyHeat = anotherPoint.fEnergyHeat;
    this->fSigmaEnergyIon = anotherPoint.fSigmaEnergyIon;
    this->fSigmaEnergyHeat = anotherPoint.fSigmaEnergyHeat;
    this->fSigmaEnergyIonHeat = anotherPoint.fSigmaEnergyIonHeat;
    this->fVoltageBias = anotherPoint.fVoltageBias;
    this->fEpsilon = anotherPoint.fEpsilon;
    this->fConfidenceLevel = anotherPoint.fConfidenceLevel;
    this->fConfidenceLevelError = anotherPoint.fConfidenceLevelError;
    if(anotherPoint.fFunction)
      this->fFunction = new TF2(*(anotherPoint.fFunction));
    else
      this->fFunction = 0;
    if(anotherPoint.fMarker)
      this->fMarker = new TMarker(*(anotherPoint.fMarker));
    else
      this->fMarker = 0;
    this->fNumBinsX = anotherPoint.fNumBinsX;
    this->fNumBinsY = anotherPoint.fNumBinsY;
    this->fNumSigmas = anotherPoint.fNumSigmas;
}

KQContourPoint::~KQContourPoint()
{
  if(fFunction) {
    delete this->fFunction;
    this->fFunction = 0;
  }
  if(fMarker) {
      delete this->fMarker;
      this->fMarker = 0;
  }
}

void KQContourPoint::SetFunction()
{
     // This method resets the function due to parameter  changes 
    Double_t aSigmaEnergyRecoil = TMath::Sqrt(
  (1+ fVoltageBias/fEpsilon)*(1+fVoltageBias/fEpsilon)*fSigmaEnergyHeat*
  fSigmaEnergyHeat
  +
fVoltageBias*fVoltageBias/fEpsilon/fEpsilon*fSigmaEnergyIon*fSigmaEnergyIon);
  Double_t aSigmaQvalue = TMath::Sqrt(
  fQvalue*fQvalue/fEnergyRecoil/fEnergyRecoil*(1+fVoltageBias/fEpsilon)*
  (1+fVoltageBias/fEpsilon)*fSigmaEnergyHeat*fSigmaEnergyHeat
  + TMath::Power((1/fEnergyRecoil +
fVoltageBias/fEpsilon*fQvalue/fEnergyRecoil)*
  fSigmaEnergyIon,2));
  // [0] : fEnergyIon
  // [1] : fEnergyHeat                      0,
  // [2] : fSigmaIon
  // [3] : fSigmaHeat
  // [4] : fSigmaIonHeat
  // [5] : fVoltageBias
  // [6] : fEpsilon


  fFunction = new TF2(TString::Format("f%d_%d_%d_%d_%d",
                                      Int_t(fQvalue*100),
                                      Int_t(fEnergyRecoil*100),
                                      Int_t(fSigmaEnergyIon*100),
                                      Int_t(fSigmaEnergyHeat*100),
                                      Int_t(fSigmaEnergyIonHeat*100)).Data(),
                     // 0,
                      &KErecoilQDensity::SingleEventProbDensity,
                     fEnergyRecoil - fNumSigmas*aSigmaEnergyRecoil,
                     fEnergyRecoil + fNumSigmas*aSigmaEnergyRecoil,
                     fQvalue - fNumSigmas*aSigmaQvalue,
                     fQvalue + fNumSigmas*aSigmaQvalue,
                     6
                      //"KErecoilQDensity",
                      //"SingleEventProbDensity"
                     );
  fFunction->SetParameter(0,fEnergyIon);
  fFunction->SetParameter(1,fEnergyHeat);
  fFunction->SetParameter(2,fSigmaEnergyIon);
  fFunction->SetParameter(3,fSigmaEnergyHeat);
  fFunction->SetParameter(4,fSigmaEnergyIonHeat);
  fFunction->SetParameter(5,fVoltageBias/fEpsilon);
  fFunction->SetNpx(1000);
  fFunction->SetNpy(1000);
  fFunction->SetLineStyle(1);
  fFunction->SetLineWidth(0.5);
}
  
  
void KQContourPoint::CalculateContour()
{
  /*
  fFunction->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fFunction->GetYaxis()->SetTitle("Q");
  */
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  fFunction->SetContour(1);
  fFunction->SetContourLevel(0,aContour.GetContour(fConfidenceLevel));
  fConfidenceLevelError = aContour.GetConfidenceLevelError();                  
 
}
  
void KQContourPoint::ResetMarker()
{
  // This method resets the marker due to parameter changes
  if(fMarker) {
    delete fMarker;
    fMarker = 0;
  }
  fMarker = new TMarker(fEnergyRecoil,fQvalue,2);
  fMarker->SetMarkerSize(0.1);
}





void KQContourPoint::SetConfidenceLevel(Double_t aConfidenceLevel)
{
  // This method changes the confidence level and adjusts the single contour
  // line of the function
  
  fConfidenceLevel = aConfidenceLevel;
}

void KQContourPoint::Draw(Option_t* anOption)
{
  // This method draws the event
  // (marker for the modal values and contour line for the confidence region)
  if(fPreviousVersion)
    if(*this!=*fPreviousVersion||!fMarker) {
      CalculateContour();
      ResetMarker();
      //delete fPreviousVersion;
      fPreviousVersion = new KQContourPoint(*this);
    }
  fFunction->Draw(anOption);
  fMarker->Draw("same");
}

Bool_t KQContourPoint::CutsALine(TF1* aFunction,Int_t aNumPoints)
{
  // This method checks if a given function cuts the confidence region
  // of the event
  // This is done by comparing the function value corresponding to the contour
  // line with a specified number of function values
  //
  //BEGIN_LATEX
  // g_{i} = g(E_{recoil,i},aFunction(E_{recoil,i})
  //END_LATEX
  //
  // with
  //
  //BEGIN_LATEX
  // E_{recoil,i} = E_{recoil,min} + i * (E_{recoil,max} - E_{recoil,min})
  //END_LATEX
  //
  // with the minimal and maximal values for E_recoil
  //BEGIN_LATEX
  // E_{recoil,min}
  // E_{recoil,max}
  //END_LATEX
  // of the internal TF1 representing the contour
  // If there is one value g_i exceeding the function value on the contour line
  // the method returns true otherwise false
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  Double_t aThreshold = aContour.GetContour(fConfidenceLevel);
  Double_t anErecoilMin = aFunction->GetXmin();
  Double_t anErecoilMax = aFunction->GetXmax();
  
  for(Int_t k = 0; k<aNumPoints; ++k) {
    if(fFunction->Eval(anErecoilMin + (Double_t)k/aNumPoints*
                       (anErecoilMax - anErecoilMin),
                       aFunction->Eval(anErecoilMin + (Double_t)k/
                       aNumPoints*(anErecoilMax - anErecoilMin)))>aThreshold)
      return true;
  }
  return false;
}

Bool_t KQContourPoint::CutsLindhardLine(Int_t aNumPoints)
{
  // This method checks if the Lindhard function
  //BEGIN_LATEX
  // f(E_{recoil}) = 0.165 * E_{recoil}^{0.185}
  //END_LATEX
  //cuts the confidence region of the event
  KLindhard lind;
  return(this->CutsALine(lind.GetFormula(),aNumPoints));
}

Bool_t KQContourPoint::CutsOne(Int_t aNumPoints)
{
  //This method checks if the function
  //BEGIN_LATEX
  // f(E_{recoil}) = 1
  //END_LATEX
  //cuts the confidence region of the event
  TF1* aFunction = new TF1("f",
                           "1",
                           fFunction->GetXmin(),
                           fFunction->GetXmax());
  return(this->CutsALine(aFunction,aNumPoints));
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
  if(fPreviousVersion)
    if(*this!=*fPreviousVersion||!fMarker) {
      CalculateContour();
      ResetMarker();
      //delete fPreviousVersion;
      fPreviousVersion = new KQContourPoint(*this);
    }
  KQContour aContour(fFunction,
                     fNumBinsX,
                     fNumBinsY);
  return aContour.GetHistogram();
}

TH2D* KQContourPoint::GetContourHistogram()
{
  //This method returns a hard copy of a histogram which has bin contents of 1
  //for bins in the confidence region and 0 else
  if(fPreviousVersion) {
    CalculateContour();
  }
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
  ResetMarker();
}

void KQContourPoint::SetEnergyRecoil(Double_t anEnergyRecoil)
{
  // This method sets the modal value of the recoil energy and adjusts the
  // corresponding modal/mean values (Gaussians) of the ion and heat energy
  
  fEnergyRecoil = anEnergyRecoil;
  fEnergyIon = fQvalue* fEnergyRecoil;
  fEnergyHeat = (1+ fQvalue*fVoltageBias/fEpsilon)/(1+fVoltageBias/fEpsilon) *
    fEnergyRecoil;
  ResetMarker();
}

void KQContourPoint::SetEnergyIon(Double_t anEnergyIon)
{
  // This method sets the modal/mean value of the ion energy and adjusts
  // the corresponding modal values of Q and the recoil energy
  
  fEnergyIon = anEnergyIon;
  fEnergyRecoil = (1 + fVoltageBias/ fEpsilon) * fEnergyHeat
    - fVoltageBias/fEpsilon * fEnergyIon;
  fQvalue = fEnergyIon/fEnergyRecoil;
  ResetMarker();
}

void KQContourPoint::SetEnergyHeat(Double_t anEnergyHeat)
{
  // This method sets the modal values of the heat energy and adjusts
  // the corresponding modal values of Q and the recoil energy
  
  fEnergyHeat = anEnergyHeat;
  fEnergyRecoil = (1 + fVoltageBias/ fEpsilon) * fEnergyHeat
    - fVoltageBias/fEpsilon * fEnergyIon;
  fQvalue = fEnergyIon/fEnergyRecoil;
  ResetMarker();
}

void KQContourPoint::SetResolutionX(Int_t aNumBinsX)
{
  // This method sets the number of bins in X direction of the  distribution
 // distribution histogram and recalculates the contour for the specified
 // confidence level with the new histogram
  fNumBinsX = aNumBinsX;
}

void KQContourPoint::SetResolutionY(Int_t aNumBinsY)
{
  // This method sets the number of bins in Y direction of the  distribution
  // distribution histogram and recalculates the contour for the specified
  // confidence level with the new histogram
  fNumBinsY = aNumBinsY;
}

void KQContourPoint::SetSigmaEnergyHeat(Double_t aSigmaEnergyHeat)
{
  // This method sets the uncertainty of the heat energy and adjusts the contour
  // line
  fSigmaEnergyHeat = aSigmaEnergyHeat;
}

void KQContourPoint::SetSigmaEnergyIon(Double_t aSigmaEnergyIon)
{
  // This methods sets the uncertainty of the ion energy and adjusts the contour
  // line
  fSigmaEnergyIon = aSigmaEnergyIon;
}

void KQContourPoint::SetSigmaIonHeat(Double_t aSigmaEnergyIonHeat) 
{
  // This method sets the root square of the covariance between the heat and ion
 // energy and adjusts the contour line
  fSigmaEnergyIonHeat = aSigmaEnergyIonHeat;
}

void KQContourPoint::SetVoltageBias(Double_t aVoltageBias)
{
  // This method sets the voltage bias and adjusts the contour line
  fVoltageBias = aVoltageBias;
}

void KQContourPoint::SetEpsilon(Double_t anEpsilon)
{
  // This method sets the epsilon and adjusts the contour line
  fEpsilon = anEpsilon;
}

void KQContourPoint::SetNpx(Int_t anNpx)
{
  // This method sets the number of points of the contour function in E_recoil
  // direction
  fFunction->SetNpx(anNpx);
}

void KQContourPoint::SetNpy(Int_t anNpy)
{
  // This method sets the number of points of the contour function in Q 
  // direction
  fFunction->SetNpy(anNpy);
}

void KQContourPoint::SetNumSigmas(Double_t aNumSigmas)
{
  // This method sets the number of sigmas determining the ranges of the 
  // histogram used to calculate the contour
  fNumSigmas = aNumSigmas;
}

Bool_t operator!=(KQContourPoint& aPoint,
                  KQContourPoint& anotherPoint)
{
  return(aPoint.fQvalue != anotherPoint.fQvalue ||
               aPoint.fEnergyRecoil != anotherPoint.fEnergyRecoil ||
               aPoint.fEnergyIon != anotherPoint.fEnergyIon ||
               aPoint.fEnergyHeat != anotherPoint.fEnergyHeat ||
               aPoint.fSigmaEnergyIon != anotherPoint.fSigmaEnergyIon ||
               aPoint.fSigmaEnergyHeat != anotherPoint.fSigmaEnergyHeat ||
               aPoint.fSigmaEnergyIonHeat != anotherPoint.fSigmaEnergyIonHeat ||
               aPoint.fVoltageBias != anotherPoint.fVoltageBias ||
               aPoint.fEpsilon != anotherPoint.fEpsilon ||
               aPoint.fConfidenceLevel != anotherPoint.fConfidenceLevel ||
               aPoint.fConfidenceLevelError !=
               anotherPoint.fConfidenceLevelError ||
               aPoint.fNumBinsX != anotherPoint.fNumBinsX ||
               aPoint.fNumBinsY != anotherPoint.fNumBinsY ||
               aPoint.fNumSigmas != anotherPoint.fNumSigmas);
}

Bool_t operator==(KQContourPoint& aPoint,
                  KQContourPoint& anotherPoint)
{
  return(aPoint.fQvalue == anotherPoint.fQvalue &&
               aPoint.fEnergyRecoil == anotherPoint.fEnergyRecoil &&
               aPoint.fEnergyIon == anotherPoint.fEnergyIon &&
               aPoint.fEnergyHeat == anotherPoint.fEnergyHeat &&
               aPoint.fSigmaEnergyIon == anotherPoint.fSigmaEnergyIon &&
               aPoint.fSigmaEnergyHeat == anotherPoint.fSigmaEnergyHeat &&
               aPoint.fSigmaEnergyIonHeat == anotherPoint.fSigmaEnergyIonHeat &&
               aPoint.fVoltageBias == anotherPoint.fVoltageBias &&
               aPoint.fEpsilon == anotherPoint.fEpsilon &&
               aPoint.fConfidenceLevel == anotherPoint.fConfidenceLevel &&
               aPoint.fConfidenceLevelError ==
               anotherPoint.fConfidenceLevelError &&
               aPoint.fNumBinsX == anotherPoint.fNumBinsX &&
               aPoint.fNumBinsY == anotherPoint.fNumBinsY &&
               aPoint.fNumSigmas == anotherPoint.fNumSigmas);
}





