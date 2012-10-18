//_____________________________________________
//
// KQContourPointList.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/26/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is used to draw multiple KQContourPoints in the same frame
// The user can manually add events by calling
//   void KQContourPointList::AddPoint(Double_t aQvalue,
//             Double_t anEnergyRecoil,
//             Double_t aSigmaIon,
//             Double_t aSigmaHeat)
//
//  or he can read ASCII files by calling
//    void KQContourPointList::ReadASCIIFile(const Char_t* aFileName,
//                                       const Char_t aMode)
//
// Then he can draw the events by calling
//        void Draw(Option_t* anOption = "");
//  or print the events by calling
//   void KQContourPointList::ShowPoints()

#include "KQContourPointList.h" 

ClassImp(KQContourPointList);

KQContourPointList::KQContourPointList(const Char_t* aMode,
                                       const Char_t* aFileName,
                                       Double_t aConfidenceLevel,
                                       Double_t anEnergyRecoilMin,
                                       Double_t anEnergyRecoilMax,
                                       Double_t aQvalueMin,
                                       Double_t aQvalueMax
                                      )
: fMode(aMode), fFileName(aFileName), fEnergyRecoilMin(anEnergyRecoilMin),
fEnergyRecoilMax(anEnergyRecoilMax), fQvalueMin(aQvalueMin),
fQvalueMax(aQvalueMax), fConfidenceLevel(aConfidenceLevel)
{
  // The constructor generates an empty frame with the specified range
  
  fEmptyFrame = new TF2("emptyframe",
                        "0*x*y",
                        fEnergyRecoilMin,
                        fEnergyRecoilMax,
                        fQvalueMin,
                        fQvalueMax);
  fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fEmptyFrame->GetYaxis()->SetTitle("Q");
}

KQContourPointList::~KQContourPointList()
{
  if(fEmptyFrame)
    delete fEmptyFrame;
}

void KQContourPointList::UpdateFunctions()
{
  // This method updates the empty frame if the ranges change
  if(fEmptyFrame) {
    fEmptyFrame->SetRange(fEnergyRecoilMin,
                          fQvalueMin,
                          fEnergyRecoilMax,
                          fQvalueMax);
    fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
    fEmptyFrame->GetYaxis()->SetTitle("Q");
  }
  for(UInt_t k = 0; k<fPoints.size(); ++k)
    fPoints[k]->SetRange(fEnergyRecoilMin,
                         fQvalueMin,
                         fEnergyRecoilMax,
                         fQvalueMax);
}

void KQContourPointList::ReadASCIIFile(const Char_t* aFileName,
                                       const Char_t* aMode,
                                       Int_t aMaxNumEntries,
                                       TF1* aFunction)
{
  // This method reads an ASCII file and builds a list of KQContourPoints
  // Depending form the specified mode a different format of the lines is
  // expected:
 //
 // aMode = "QErecoil":    < Q > <E_recoil> <sigma_ion> <sigma_heat>
 // aMode = "IonHeat": <E_ion> <E_heat> <sigma_ion> <sigma_heat>
 
  if(strcmp(aFileName,""))
    fFileName = aFileName;
  if(strcmp(aMode,""))
    fMode = aMode;
  
  if(strcmp(fFileName.c_str(),""))
  {
    ifstream is(aFileName);
    KQContourPoint* aPoint;
    Double_t anEnergyIon, anEnergyHeat;
    Double_t aQvalue = 0;
    Double_t anEnergyRecoil = 0;
    Double_t aSigmaIon = 0;
    Double_t aSigmaHeat = 0;
    Double_t aVoltageBias;
    char aCString[100];
    TString aString;
    TObjArray* theTokens;
    TObjString* anElement;
    Int_t aLineCounter = 0;
    
    if(fMode=="QErecoil")
      while(!is.eof() && aLineCounter < aMaxNumEntries)
      {
        //is >> aQvalue >> anEnergyRecoil >> aSigmaIon >> aSigmaHeat;
        ++aLineCounter;
        is.getline(aCString,100);
        aString = aCString;
        theTokens = aString.Tokenize(" ");
        
        //empty lines
        if(!theTokens->GetEntries())
          continue;
        
        //comment lines 
        anElement = (TObjString*)theTokens->At(0);
        if(anElement->GetString()=="#") 
          continue;
        
        if(theTokens->GetEntries()!=5) {
          cout << "KQContourPointList::ReadASCIIFile(): invalid format in line "
          << aLineCounter <<", 5 Double_t values <Q> <E_recoil> <sigma_ion> "
          << "<sigma_heat> <voltage bias> expected " << endl;
          continue;
        }
        
        anElement = (TObjString*)theTokens->At(0);
        aQvalue = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(1);
        anEnergyRecoil = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(2);
        aSigmaIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(3);
        aSigmaHeat = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(4);
        aVoltageBias = anElement->GetString().Atof();
          
        aPoint = new KQContourPoint(aQvalue, anEnergyRecoil,
                                             "QErecoil",
                                            aSigmaIon, aSigmaHeat,
                                            0,fConfidenceLevel,aVoltageBias);
        if(aFunction) {
          if(aPoint->CutsALine(aFunction))                                    
            fPoints.push_back(aPoint);
	  else
	    delete aPoint;
        }
        else
          fPoints.push_back(aPoint);
          
        if (fPoints.size() % 1000 == 0)
          cout << "... event " << fPoints.size() << " processed" << endl;
      }
      else
    if(fMode=="IonHeat")
      while(!is.eof() && aLineCounter < aMaxNumEntries)
      {
        //is >> aQvalue >> anEnergyRecoil >> aSigmaIon >> aSigmaHeat;
        ++aLineCounter;
        is.getline(aCString,100);
        aString = aCString;
        theTokens = aString.Tokenize(" ");
        
        //empty lines
        if(!theTokens->GetEntries())
          continue;
        
        //comment lines 
        anElement = (TObjString*)theTokens->At(0);
        if(anElement->GetString()=="#") 
          continue;
        
        if(theTokens->GetEntries()!=5) {
          cout << "KQContourPointList::ReadASCIIFile(): invalid format in line "
          << aLineCounter <<", 5 Double_t values <E_ion> <E_heat> <sigma_ion>"
          << " <sigma_heat> <voltage bias> expected " << endl;
          continue;
        }
        
        anElement = (TObjString*)theTokens->At(0);
        anEnergyIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(1);
        anEnergyHeat = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(2);
        aSigmaIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(3);
        aSigmaHeat = anElement->GetString().Atof();
         anElement = (TObjString*)theTokens->At(4);
        aVoltageBias = anElement->GetString().Atof();
          
        aPoint = new KQContourPoint(aQvalue, anEnergyRecoil,
                                             "QErecoil",
                                            aSigmaIon, aSigmaHeat,
                                            0,fConfidenceLevel,aVoltageBias);
        if(aFunction) {
          if(aPoint->CutsALine(aFunction))                                    
            fPoints.push_back(aPoint);
        }
        else
          fPoints.push_back(aPoint);
        
        cout << "... event " << fPoints.size() << " processed" << endl;
      }
  }
  else
  {
    cout << "void KQContourPointList::ReadASCIIFile(): no ASCII file specified"
    << endl;
    return;
  }
}

void KQContourPointList::Draw(Option_t* anOption)
{
  //This method draws all events in the list (marker + contour line)
  //in the empty frame
  fEmptyFrame->SetRange(fEnergyRecoilMin,
                        fQvalueMin,
                        fEnergyRecoilMax,
                        fQvalueMax);
  for(UInt_t k = 0; k< fPoints.size(); ++k) {
    fPoints[k]->SetRange(fEnergyRecoilMin,
                         fQvalueMin,
                         fEnergyRecoilMax,
                         fQvalueMax);
  }
  fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fEmptyFrame->GetYaxis()->SetTitle("Q");
  if(!fPoints.size()) {
    cout << "void KQContourPointList::Draw(Option_t* anOption): no points to"
    " draw" << endl;
    return;
  }
  fEmptyFrame->Draw(anOption);
  for(UInt_t k = 0; k < fPoints.size(); ++k) {
    fPoints[k]->Draw("same");
    cout << "event " << k << " drawn" << endl;
  }
}

void KQContourPointList::SetNpx(Int_t anNpx)
{
  for(UInt_t k = 0; k<fPoints.size(); ++k)
    fPoints[k]->SetNpx(anNpx);
}

void KQContourPointList::AddPoint(Double_t aQvalueOrEnergyIon,
              Double_t anEnergyRecoilOrEnergyHeat,
              Double_t aSigmaIon,
              Double_t aSigmaHeat)
{
  //This method adds a point to the list of events
  fPoints.push_back(new KQContourPoint(aQvalueOrEnergyIon,
                                       anEnergyRecoilOrEnergyHeat,
                                       fMode.c_str(),
                                       aSigmaIon,
                                       aSigmaHeat,
                                       0,
                                       fConfidenceLevel));
}

void KQContourPointList::ClearPoints()
{
  // This method clears the events
  fPoints.clear();
}

void KQContourPointList::RemovePoint(UInt_t anIndex)
{
  // This method removes a single event from the list
  if(anIndex>=fPoints.size()) {
    cout << "void KQContourPointList::RemovePoint(): index out of range, must"
    << " be smaller than " << fPoints.size() << endl;
    return;
  }
  fPoints.erase(fPoints.begin()+anIndex);
}

void KQContourPointList::ShowPoints()
{
  //This methods prints the parameters of all events in the list
  for(UInt_t k = 0; k< fPoints.size(); ++k) {
    cout << setw(4) << "Q: " << setw(15) <<  fPoints[k]->GetQvalue() 
    << setw(10) << "E_recoil: " << setw(15) << 
    fPoints[k]->GetEnergyRecoil() << setw(10) << "sigma_ion:" << setw(15) << 
    fPoints[k]->GetSigmaEnergyIon() << setw(10) << "sigma_heat:" << setw(15) <<
    fPoints[k]->GetSigmaEnergyHeat() << endl;
  }
}

TF2* KQContourPointList::GetCummulativeProbDensity(
        const Char_t* aFunctionName)
{
  TF2* aFunction = new TF2(TString::Format("h%s",aFunctionName).Data(),
                           &KErecoilQDensity::MultiEventCummulativeProbDensity,
                           fEmptyFrame->GetXmin(),
                           fEmptyFrame->GetXmax(),
                           fEmptyFrame->GetYmin(),
                           fEmptyFrame->GetYmax(),
                           6*fPoints.size()+1);
  aFunction->SetParameter(0,fPoints.size());
  for(UInt_t k = 0; k<fPoints.size(); ++k) {
    aFunction->SetParameter(6*k+1,fPoints[k]->GetEnergyIon());
    aFunction->SetParameter(6*k+2,fPoints[k]->GetEnergyHeat());
    aFunction->SetParameter(6*k+3,fPoints[k]->GetSigmaEnergyIon());
    aFunction->SetParameter(6*k+4,fPoints[k]->GetSigmaEnergyHeat());
    aFunction->SetParameter(6*k+5,fPoints[k]->GetSigmaEnergyIonHeat());
    aFunction->SetParameter(6*k+6,fPoints[k]->GetVoltageBias()/
                                  fPoints[k]->GetEpsilon());  
  }
  aFunction->SetNpx(1000);
  aFunction->SetNpy(1000);
  aFunction->SetLineStyle(1);
  //aFunction->SetLineWidth(0.5);
  return aFunction;
}

TF2* KQContourPointList::GetProbabilityOfAtLeastOneEvent(
  const Char_t* aFunctionName,
  Double_t aTolerance)
{
  TF2*  aFunction = new TF2(TString::Format("g%s",aFunctionName).Data(),
                &KErecoilQDensity::MultiEventProbabilityOfAtLeastOneEvent,
                0,
                1,
                0,
                1,
                6*fPoints.size()+5);
  aFunction->SetParameter(0,fPoints.size());
  aFunction->SetParameter(1,aTolerance);
  aFunction->SetParameter(2,0);
  aFunction->SetParameter(3,1);
  aFunction->SetParameter(4,0);
  aFunction->SetParameter(5,1);
  for(UInt_t k = 0; k<fPoints.size(); ++k) {
    aFunction->SetParameter(6*k+6,fPoints[k]->GetEnergyIon());
    aFunction->SetParameter(6*k+7,fPoints[k]->GetEnergyHeat());
    aFunction->SetParameter(6*k+8,fPoints[k]->GetSigmaEnergyIon());
    aFunction->SetParameter(6*k+9,fPoints[k]->GetSigmaEnergyHeat());
    aFunction->SetParameter(6*k+10,fPoints[k]->GetSigmaEnergyIonHeat());
    aFunction->SetParameter(6*k+11,fPoints[k]->GetVoltageBias()/
                                                                fPoints[k]->GetEpsilon());
  }
    return aFunction;
}

TH1D* KQContourPointList::GetDistributionOfTrueValuesMonteCarlo(
                              const Char_t* aHistogramName,
                              TF1* aLowerBoundary,
                              TF1* anUpperBoundary,
                              UInt_t aMonteCarloSize,
                              Int_t aNumElements)
{
  //This method returns a histogram showing the distribution of the number of 
  // true events within an area between boundary values given by
  // aLowerBoundary and anUpperBoundary 
  // for the measured events of this KQContourPointList
  // This histogram is created by generating random events, one for each KQContourPoint
  // in the list, from their pdfs, incrementing a counter for each event which
  // lies in the specified area and then filling the histogram with this counter
  // This procedure is repeated <aMonteCarloSize> times.
  // Only one or none boundary functions can be specified, then the area
  // is regarded unrestricted upwards, downwards or on both sides.
  
  TH1D* result = new TH1D(aHistogramName,
                          "Distribution of number of true events in a specified area"
                          "for this KQContouPointList",
                          fPoints.size()+1,
                          -0.5,
                          fPoints.size()+0.5);
  
  Int_t aTrueEventCounter = 0;
  Double_t anEnergyRecoil =0;
  Double_t aQvalue = 0;
  Int_t aFunctionFlag = -1;
  if(aNumElements == -1)
    aNumElements = fPoints.size();
  
  if(aLowerBoundary&&anUpperBoundary)
    aFunctionFlag = 0;
  else
    if(aLowerBoundary&&!anUpperBoundary)
      aFunctionFlag = 1;
    else
      if(!aLowerBoundary&&anUpperBoundary)
        aFunctionFlag = 2;
  for(UInt_t k = 0; k< aMonteCarloSize; ++k)
  {
      cout << "iteration " << k << endl;
    aTrueEventCounter = 0;
    for(UInt_t l = 0; l<(UInt_t)aNumElements; ++l) {
      cout << "point " << l << endl;
      this->GetElement(l)->GetFunction()->GetRandom2(anEnergyRecoil,aQvalue);
      switch(aFunctionFlag) {
        case 0: 
          if(aQvalue>aLowerBoundary->Eval(anEnergyRecoil)&&
             aQvalue<anUpperBoundary->Eval(anEnergyRecoil))
            ++aTrueEventCounter;
          break;
        case 1:
          if(aQvalue>aLowerBoundary->Eval(anEnergyRecoil))
            ++aTrueEventCounter;
          break;
        case 2:
          if(aQvalue<anUpperBoundary->Eval(anEnergyRecoil))
            ++aTrueEventCounter;
          break;
        default:
          ++aTrueEventCounter;
          break;
      }
    }
    result->Fill(aTrueEventCounter); 
  }    
  return result;
}

TH1D* KQContourPointList::GetDistributionOfTrueValuesMergeProb(
                            const Char_t* aHistogramName,
                            TF2* anIndicatorFunction,
                            Int_t aNumElements,
			    ROOT::Math::IntegrationMultiDim::Type anIntegrationMethod,
                            Double_t anIntegrationTolerance)
{
  // This method returns a histogram showing the distribution of the number of 
  // true events within an selected area determined by an indicator function
  // First by integrating the contour functions of single events in the list
  // over the selected area, the probabilties for the single events to having
  // occured in this area are calculated. Then this probabilites are merged 
  // and convoluted pairwise, successivly in blocks of 2, 4, 8, 16 elements
  // until the full list size is reached
  // (Double_t* KErecoilQDensity::GetDistributionOfMultipleEvents(
  //                                 UInt_t aSize, Double_t* aVector))
  //
  // Available integration routines are
  //   ROOT::Math::IntegrationMultiDim::kVEGAS (default)
  //   ROOT::Math::IntegrationMultiDim::kADAPTIVE
  //   ROOT::Math::IntegrationMultiDim::kPLAIN
  //   ROOT::Math::IntegrationMultiDim::kMISER
  //
  // Example:
  //
  // KQContourPointList list;
  // list.ReadASCIIFile("<anASCIIFile>","QErecoil");
  // TF2 anIndicator("ind","<condition of area> ? 1 : 0");
  // TH1D* hist = list.GetDistributionOfTrueValuesMergeProb(
  //                                 "<aHistogramName>",
  //                                 &anIndicator,
  //                                 <aNumberOfElements>,
  //                                 <anIntegrationTolerance>);
  
  if(aNumElements == -1)
    aNumElements = fPoints.size();
  TF2 z("z",
        &KErecoilQDensity::SingleEventProbDensityWithIndicator,
        fEnergyRecoilMin,
        fEnergyRecoilMax,
        fQvalueMin,
        fQvalueMax,
        7);
  z.SetParameter(0,(Double_t)(Long_t)anIndicatorFunction);
  ROOT::Math::WrappedMultiTF1 wf1(z,2);
  
  // create the integrator
  //ROOT::Math::AdaptiveIntegratorMultiDim ig;
  ROOT::Math::IntegratorMultiDim ig(anIntegrationMethod);
  
  //Set parameters of the integration
  ig.SetFunction(wf1);
  ig.SetRelTolerance(anIntegrationTolerance);
  
  Double_t xmin[] = { fEnergyRecoilMin, fQvalueMin };
  Double_t xmax[] = { fEnergyRecoilMax, fQvalueMax };
  
  Double_t probabilities[aNumElements];
  Double_t probabilityErrors[aNumElements];
  
  Double_t p[7];
  p[0] = z.GetParameter(0);
  for(Int_t k = 0; k<aNumElements; ++k) {
    p[1] = this->GetElement(k)->GetFunction()->GetParameter(0);
    p[2] = this->GetElement(k)->GetFunction()->GetParameter(1);
    p[3] = this->GetElement(k)->GetFunction()->GetParameter(2);
    p[4] = this->GetElement(k)->GetFunction()->GetParameter(3);
    p[5] = this->GetElement(k)->GetFunction()->GetParameter(4);
    p[6] = this->GetElement(k)->GetFunction()->GetParameter(5);
    
    wf1.SetParameters(p);
    probabilities[k] = ig.Integral(xmin,xmax);
    probabilityErrors[k] = ig.Error();
    if(probabilities[k]!=probabilities[k]) // probabilities[k] == nan ?
      probabilities[k] = 0;
    cout << "event " << k+1 << " integrated: prob = " << probabilities[k]
    << " +/- " << ig.Error()<< endl;
  }
  Double_t* distribution = KErecoilQDensity::GetDistributionOfMultipleEvents(
                                                  aNumElements,
                                                  probabilities);
  TH1D* result = new TH1D(aHistogramName,
                          aHistogramName,
                          aNumElements+1,
                          -0.5,
                          aNumElements+0.5);
  for(Int_t k = 0; k<aNumElements+1; ++k)
    result->SetBinContent(k+1,distribution[k]);
  
  if(distribution != 0) delete [] distribution;
  
  return result;                     
}
