//_____________________________________________
//
// KQDistribution.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// The basic purpose of this class is to show Q gamma and neutron band widths for a number of KBolometerEvents,
// read from a specified KDATA tree file which stores KEvents
// For this purpose the Q and ERecoil values are read from all KBolometerEvents within the tree file and stored in a vector of struct "DataRecord"
// This vector is sorted according to the ERecoil values in ascending order
// and then equal numbers of adjacent data pairs are filled in a specified number "fNumProjections" of TH2D Q-ERecoil histograms
// These histograms are projected on the Q-axis resulting in ERecoilvalues resulting in TH1D histograms
// Mean and error values for the Q gamma and neutron band are estimated by applying Gaussian fits in certain ERecoil intervals
// determined by theoretically calculated values: mean value  +/- a specified number of error value
// 

#include "KQDistribution.h"

#ifndef __KQFUNCTIONS__
#define __KQFUNCTIONS__

/*
Double_t GammaFit(Double_t* x,Double_t* par) {
  // this function is used to fit gaussian shapes in two seperate intervals with variable boundaries
  if(x[0]>par[6]) {
    TF1::RejectPoint();
    return 0;
    }
  else
  if(x[0]>par[7]) 
    return par[0]*exp(-(x[0]-par[1])*(x[0]-par[1])/par[2]/par[2]/2);
  else
  if(x[0]>par[8]) {
    TF1::RejectPoint();
    return 0;
  }
  else
  if(x[0]>par[9])
    return par[3]*exp(-(x[0]-par[4])*(x[0]-par[4])/par[5]/par[5]/2);
  else {
    TF1::RejectPoint();
    return 0;
  }
}
*/

Double_t GausFit(Double_t* x,Double_t* par) {
  if(x[0]>par[3] && x[0]<par[4])
    return(par[0]*exp(-(x[0]-par[1])*(x[0]-par[1])/par[2]/par[2]/2));
  else
    TF1::RejectPoint();
  return 0;
}

bool operator<(const KQDistribution::DataRecord& aFirstDataRecord,const KQDistribution::DataRecord& aSecondDataRecord) {
  //implemented to order DataRecords according ascending fERecoil values
  return(aFirstDataRecord.fERecoil<aSecondDataRecord.fERecoil);
}

#endif

ClassImp(KQDistribution);

KQDistribution::KQDistribution(const Char_t* aSourceFile, const Char_t* aTargetDir = gSystem->pwd(),Int_t aNumProjections = 20) : fAlpha(0.165), fBeta(0.185)
{
  // default constructor
  cout << "Setting members ... " << endl;
  fBoloConfigFile = "BoloConfigFile.txt"; //user-specific
  fSourceFile = aSourceFile; //user-specific
  fTargetDir = aTargetDir; //user-specific
  fImageFormat = "pdf";
  fNumProjections = aNumProjections;
  fNumBinsERecoil = 100;
  fNumBinsQ = 100;
  fERecoilMin = 0;
  fERecoilMax = 500;
  fQMin = 0;
  fQMax = 2;
  fHistogramCounter = 0;
  fConfidenceInSigmas = sqrt(2)*TMath::ErfInverse(0.9);

  
}

TH1D* KQDistribution::GetHistogram(Int_t anIndex) {
  if(anIndex>=fNumProjections || anIndex<=0) {
    cout << "KQDistribution::GetHistogram(Int_t): invalid index " << anIndex << ", must be in [0," << fNumProjections-1 << endl;
    return 0;
  }
  return fHistogramRecords[anIndex].fHistogram->ProjectionY();
  
}


const Char_t* KQDistribution::GetCategoryName(Int_t anEventCategory = -1) {
  // gets a C string for the category of current fEventCategory if anEventCategory=-1,
  // otherwise for anEventCategory itself
  if(anEventCategory==-1)
    anEventCategory = fEventCategory;
  
  switch(anEventCategory) {
    case 0: return("noise"); break;
    case 1: return("heat_only"); break;
    case 2: return("fiducial"); break;
    case 3: return("surface"); break;
    case 4: return("pure_guard"); break;
    case 5: return("other"); break;
    default: return(""); break;
  }
}

Double_t* KQDistribution::GetData(DataType aType) {
  // returns a Double_t array containing all Q data (KQDistribution::Q) or all ERecoil data (KQDistribution::ERecoil)
  Double_t* aDataArray = new Double_t[fDataSize];
  switch(aType) {
    case ERECOIL: for(int k = 0; k<fDataSize; ++k)
        aDataArray[k] = fData[k].fERecoil; break;
    case Q: for(int k = 0; k<fDataSize; ++k)
      aDataArray[k] = fData[k].fQ; break;
  }
  return aDataArray;
}

void KQDistribution::SortData() {
  // sorts fData with respect to ERecoil in ascending order
  sort(fData.begin(),fData.end());
}

void KQDistribution::DrawHistogram(TPad* aPad,Int_t aHistogramCounter) {
  // draws a TH1D Q-ERecoil histogram, which is a projection on Q-axis of a previously created TH2D Q-ERecoil histogram 
  // this histogram is in the fHistogramCounter'th element of type HistogramRecord in fHistogramRecords
  // aDirection=true: fHistogramCounter is incremented, next histogram is drawn
  // aDirection=false: fHistogramCounter is decremented, previous histogram is drawn
  //
  // two gaussian fits are applied for gamma and neutron bands within fixed intervals and the corresponding chi2 values are drawn on a TPaveText
  cout << "Drawing Histogram ... " << endl;


  // TH2D* hist = (TH2D*)gDirectory->FindObject(TString::Format("hist_%s_%i",GetCategoryName(),fHistogramCounter).Data());
  if(fHistogramRecords[aHistogramCounter].fHistogram) {
    aPad->cd();
    fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY()->Draw();
    SetBoundaries(fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY(),
                  3,
                  fHistogramRecords[aHistogramCounter].fQNeutronTheo,
                  fHistogramRecords[aHistogramCounter].fQGammaTheo,
                  fHistogramRecords[aHistogramCounter].fQNeutronTheoError/fConfidenceInSigmas,
                  fHistogramRecords[aHistogramCounter].fQGammaTheoError/fConfidenceInSigmas
                  );
    fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY()->Fit(fGammaGaus,"LM");
    aPad->Update();

    TPaveStats* aTempTPaveStats = 0;
    aTempTPaveStats = (TPaveStats*)fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY()->FindObject("stats");
    if(!aTempTPaveStats) {
      cout << "KQDistribution::DrawHistogram(): no stats found for histogram " << fHistogramCounter << endl;
      return;
    }
    fChi2PaveText = (TPaveStats*)aTempTPaveStats->Clone();

    
    fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY()->Fit(fNeutronGaus,"LM");
    aPad->Update();
    
    aTempTPaveStats = (TPaveStats*)fHistogramRecords[aHistogramCounter].fHistogram->ProjectionY()->FindObject("stats");
    if(!aTempTPaveStats) {
      cout << "KQDistribution::DrawHistogram(): no stats found for histogram " << fHistogramCounter << endl;
      return;
    }
    
    
    for(int k = 4; k<aTempTPaveStats->GetSize(); ++k)
      fChi2PaveText->AddText(aTempTPaveStats->GetLine(k)->GetTitle()); //copying results of neutron fit in the TPaveStats of the gamma fit

    fChi2PaveText->Draw();
    fGammaGaus->Draw("SAME");
    fNeutronGaus->Draw("SAME");
    aPad->Update();
    


  }
  else
  {
    cout << "KQDistribution::DrawHistogram(): Histogram " << fHistogramCounter << " is not available " << endl;
  }	
}

void KQDistribution::Previous() {
  cout << "Previous()" << endl;
  fHistogramCounter = (fHistogramCounter - 1 + fNumProjections) % fNumProjections;
  DrawHistogram(fRightSubPad,fHistogramCounter);
}

void KQDistribution::Next() {
  cout << "Next()" << endl;
  fHistogramCounter = (fHistogramCounter + 1) % fNumProjections;
  DrawHistogram(fRightSubPad,fHistogramCounter);
}


void KQDistribution::SetBoundaries(TH1D* aHistogram,Double_t aNumSigmas,Double_t aFirstValue, Double_t aSecondValue, Double_t aFirstError, Double_t aSecondError) {
  // searches and sets the boundaries for the two gaussian fits in the fitting function fDoubleGaus
  // the central Q values of the fit intervals, QNeutronMaxX and QGammaMaxX, are the Q values corresponding to the bins with maximal entries in aHistogram in the Q intervals [0.2,0.6] (neutron) and [0.8,1.2] (gamma)
  // the intervals are then [QNeutronMaxX - aNumSigmas*aFirstError, QNeutronMaxX + aNumSigmas*aFirstError] (neutron)
  // [QGammaMaxX - aNumSigmas*aSecondError, QGammaMaxX + aNumSigmas*aSecondError] (gamma)
  cout << "Setting boundaries ... " << endl;
  Double_t QNeutronMaxY = 0;
  Double_t QGammaMaxY = 0;
  Double_t QNeutronMaxX = 0.2;
  Double_t QGammaMaxX = 1.2;
  Double_t QNeutronIntegral = 0;
  Double_t QGammaIntegral = 0;
  for(int k = QtoBin(0.6); k>QtoBin(0.2); --k) {
    QNeutronIntegral += aHistogram->GetBinContent(k);
    if(QNeutronMaxY<aHistogram->GetBinContent(k)) {
      QNeutronMaxY = aHistogram->GetBinContent(k);
      QNeutronMaxX = BintoQ(k);
    }
  }
  
  for(int k = QtoBin(1.2); k>QtoBin(0.8); --k) {
    QGammaIntegral += aHistogram->GetBinContent(k);
    if(QGammaMaxY<aHistogram->GetBinContent(k)) {
      QGammaMaxY = aHistogram->GetBinContent(k);
      QGammaMaxX = BintoQ(k);
    }
  }
  
  
  
  cout << "QNeutronLow: " << QNeutronMaxX - aNumSigmas*aFirstError << endl;
  cout << "QNeutronHigh: " << QNeutronMaxX + aNumSigmas*aFirstError << endl;
  cout << "QNeutronIntegral: " << QNeutronIntegral << endl;
  cout << "QGammaLow: " << QGammaMaxX - aNumSigmas*aSecondError << endl;
  cout << "QGammaHigh: " << QGammaMaxX + aNumSigmas*aSecondError << endl;
  cout << "QGammaIntegral: " << QGammaIntegral << endl;
  
  fGammaGaus->SetParameter(0,QGammaMaxY); // C_{#gamma}
  fGammaGaus->SetParameter(1,QGammaMaxX); // Q_{#gamma}
  fGammaGaus->SetParameter(2,aSecondError); // #sigma_{Q_{#gamma}}
  fGammaGaus->FixParameter(3,QGammaMaxX - aNumSigmas*aSecondError); // Q_{#gamma,high}
  fGammaGaus->FixParameter(4,QGammaMaxX + aNumSigmas*aSecondError); // Q_{#gamma,low}
  
  fNeutronGaus->SetParameter(0,QNeutronMaxY); // C_{neutron}
  fNeutronGaus->SetParameter(1,QNeutronMaxX); // Q_{neutron}
  fNeutronGaus->SetParameter(2,aFirstError); // #sigma_{Q_{neutron}}
  fNeutronGaus->FixParameter(3,QNeutronMaxX - aNumSigmas*aFirstError); // Q_{neutron,high}
  fNeutronGaus->FixParameter(4,QNeutronMaxX + aNumSigmas*aFirstError); // Q_{neutron,low}
}

void KQDistribution::SaveImage(Int_t anIndex) {
  // saves the image drawn on fDrawCanvas in an image file with format fImageFormat in path "fTargetDir/fTargetSubDir/CategoryName_fDectorName_anIndex.fImageFormat"
  cout << TString::Format("Saving %s/%s_%s_%i.%s ...",
                                       fTargetDir.c_str(),
                                       GetCategoryName(),
                                       fDetectorName.c_str(),
                                       anIndex,fImageFormat.c_str()).Data() << endl;
  if(fDrawCanvas)
    fDrawCanvas->Print(TString::Format("%s/%s_%s_%i.%s",
                                       fTargetDir.c_str(),
                                       GetCategoryName(),
                                       fDetectorName.c_str(),
                                       anIndex,fImageFormat.c_str()).Data());
}

void KQDistribution::SaveImage(const Char_t* aFileName) {
  cout << "Saving " << aFileName << endl;
  if(fDrawCanvas) {
    if(fTargetDir!="")
      fDrawCanvas->Print(TString::Format("%s/%s",fTargetDir.c_str(),aFileName).Data());
    else
      fDrawCanvas->Print(TString::Format("%s/%s",gSystem->pwd(),aFileName).Data());
  }
}

Int_t KQDistribution::QtoBin(Double_t aQ) {
  // converts a Q value in the corresponding bin number
  return Int_t((aQ-fQMin)/(fQMax-fQMin)*fNumBinsQ);
}

Double_t KQDistribution::BintoQ(Int_t aBin) {
  // converts a bin number in the corresponding Q value
  return fQMin + aBin * (fQMax-fQMin)/fNumBinsQ;
}

Double_t KQDistribution::GetChi2(TH1D* aHistogram,Double_t aQlow,Double_t aQhigh) {
  // calculates the chi2 value for aHistogram in [Qlow,Qhigh]
  Double_t chi2sum = 0;
  Double_t x,y;
  for(int k = QtoBin(aQlow); k<QtoBin(aQhigh); ++k)
    if(aHistogram->GetBinError(k)>=1) {
      x = aHistogram->GetBinContent(k) - fDoubleGaus->Eval(BintoQ(k));
      y = aHistogram->GetBinError(k);
    chi2sum += x*x/y/y;
  }
  return chi2sum;
}

Int_t KQDistribution::GetNDF(Double_t aQlow,Double_t aQhigh,Int_t aNumParameters) {
  // gets the number of degrees of freedom with aNumParameters and the number of bins in [Qlow,Qhigh]
  Int_t k;
  for(k = QtoBin(aQlow); k<QtoBin(aQhigh); ++k);
  return k-1-aNumParameters;
}

void KQDistribution::SetConfidenceLevel(Double_t aQuantil)
{
  //This method sets the confidence level according to the specified quantil
  fConfidenceInSigmas = sqrt(2)*TMath::ErfInverse(aQuantil);
}

/*
void KQDistribution::CorrectBoundaries(BoundaryType aBType,Double_t aStep) {
  // changes one of the boundaries aBType = KQDistribution::GAMMAHIGH , KQDistribution::GAMMALOW, KQDistribution::NEUTRONHIGH , KQDistribution::NEUTRONLOW by aStep in fDoubleGaus
  // the current TH1D histogram in fHistogramRecords is then refitted and redrawn on fDrawCanvas
  cout << "Correcting Boundaries ... " << endl;
  Int_t parameterIndex;
  switch(aBType) {
    case GAMMAHIGH: parameterIndex = 9; break;
    case GAMMALOW: parameterIndex = 8; break;
    case NEUTRONHIGH: parameterIndex = 7; break;
    case NEUTRONLOW: parameterIndex = 6; break;
    default: parameterIndex = 9; break;
  }
  fDoubleGaus->SetParameter(parameterIndex,fDoubleGaus->GetParameter()+aStep);
  fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LM");
  GetFitParameters(fHistogramCounter);
  
}
*/

void KQDistribution::SetPaths(const Char_t* aSourceFile,const Char_t* aTargetDir) {
  // This method sets the specified paths for the source file and the target directory where the plot images are saved
  
  fSourceFile = aSourceFile;
  fTargetDir = aTargetDir;
}

Double_t KQDistribution::GetGammaMax(TH1D* aHistogram) {
  //This method returns the Q value corresponding to the bin with maximal entries in aHistogram
  return(fQMin + aHistogram->GetMaximumBin()*(fQMax-fQMin)/fNumBinsQ);
  // return(Qmin+hist->GetMaximumBin(Int_t((1-Qmin)/(Qmax-Qmin)*numbinsQ))*(Qmax-Qmin)/numbinsQ);
}

void KQDistribution::GetFitParameters(Int_t anIndex) {
  //This method changes the current values, fQGamma, fQGammaError, fQNeutron, fQNeutronError, 
  //in the anIndex'th element of fHistogramRecords to the fit parameter values and errors in fDoubleGaus
  //These values are also changed in the graphs, fQGammaErrorGraph and fQNeutronErrorGraph,
  // as well as in the ASCII files, "fTargetDir/fTargetSubDir/QNeutronValues.dat" and "fTargetDir/fTargetSubDir/QGammaValues.dat" containing the data of the graphs
  // fTotalMultiGraph containing fQGammaErrorGraph and fQNeutronErrorGraph is redrawn
  cout << "Saving new fit parameters ... " << endl;
  fHistogramRecords[anIndex].fQGamma = fGammaGaus->GetParameter(1);
  fHistogramRecords[anIndex].fQGammaError = fConfidenceInSigmas*abs(fGammaGaus->GetParameter(2));
  fHistogramRecords[anIndex].fQNeutron = fNeutronGaus->GetParameter(1);
  fHistogramRecords[anIndex].fQNeutronError = fConfidenceInSigmas*abs(fGammaGaus->GetParameter(2));
  fQNeutronErrorGraph->SetPoint(anIndex,fHistogramRecords[anIndex].fERecoilMean,fHistogramRecords[anIndex].fQNeutron);
  fQNeutronErrorGraph->SetPointError(anIndex,fHistogramRecords[anIndex].fERecoilError,fHistogramRecords[anIndex].fQNeutronError);
  fQGammaErrorGraph->SetPoint(anIndex,fHistogramRecords[anIndex].fERecoilMean,fHistogramRecords[anIndex].fQGamma);
  fQGammaErrorGraph->SetPointError(anIndex,fHistogramRecords[anIndex].fERecoilError,fHistogramRecords[anIndex].fQGammaError);

  cout << "Writing data values for graphs ... " << endl;
  ofstream QNeutronRawFile(TString::Format("%s/QNeutronValues.dat",
                                           fTargetDir.c_str()).Data());
  QNeutronRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QNeutron" << setw(20) << "ERecoilError" << setw(20) << "QNeutronError" << endl;
  ofstream QGammaRawFile(TString::Format("%s/QGammaValues.dat",
                                         fTargetDir.c_str()).Data());
  QGammaRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGamma" << setw(20) << "ERecoilError" << setw(20) << "QGammaError" << endl;

  for(int k = 0; k<fNumProjections; ++k) {
    if(fHistogramRecords[k].fQNeutron<0.5&&fHistogramRecords[k].fQNeutron>0.1)
      QNeutronRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQNeutron << setw(20) << 
      fHistogramRecords[k].fERecoilError <<  setw(20) << fHistogramRecords[k].fQNeutronError << endl;
    else
      cout << "... neglecting Q_Neutron = " << fHistogramRecords[k].fQNeutron << " in histogram " << k << ": out of boundaries [0.1,0.5]" << endl;
    if(fHistogramRecords[k].fQGamma<1.2&&fHistogramRecords[k].fQGamma>0.8)
      QGammaRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQGamma << setw(20) <<
      fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQGammaError << endl;
    else
      cout << "... neglecting Q_Gamma = " << fHistogramRecords[k].fQGamma << " in histogram " << k << ": out of boundaries [0.8,1.2]" << endl;
    cout << TString::Format("Qg = %f +/- %f \t Q_n = %f +/- %f",fHistogramRecords[k].fQGamma,
                            fHistogramRecords[k].fQGammaError,
                            fHistogramRecords[k].fQNeutron,
                            fHistogramRecords[k].fQNeutronError).Data() << endl;
  }

  fLeftSubPad->cd();
  fTotalMultiGraph->Draw();
  fDrawCanvas->cd();
}


void KQDistribution::SetStyle() {
  //This method sets styles for histogram and fit result drawing
  cout << "Setting style ... " << endl;
  gStyle->SetOptFit(12); 
  gStyle->SetHistFillColor(2);
  gStyle->SetHistTopMargin(1);
  gStyle->SetFrameFillColor(42);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetStatColor(0);
}

void KQDistribution::ResetVars() {
  //This method disposes all dynamically allocated class members
  cout << "Resetting variables ... " << endl;
  if(fAdjustPanel) {
    delete fAdjustPanel;
    fAdjustPanel = 0;
  }
  
  if(fKHLAEvent) {
    delete fKHLAEvent;
    fKHLAEvent = 0;
  }

  
  if(fTotalGraph) {
    delete fTotalGraph;
    fTotalGraph = 0;
  }
  
  if(fTotalMultiGraph) {
    delete fTotalMultiGraph;
    fTotalMultiGraph = 0;
  }
  
  if(fChi2PaveText) {
    delete fChi2PaveText;
    fChi2PaveText = 0;
  }
  
  if(fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
  
  if(fDrawCanvas) {
    delete fDrawCanvas;
    fDrawCanvas = 0;
  }

   if(fOutputFile) {
    delete fOutputFile;
    fOutputFile = 0;
  }
            cout << "Test" << endl;

  fData.clear();
  fDataSize = 0;


  
  if(fERecoilMean) {
    delete[] fERecoilMean;
    fERecoilMean = 0;
  }
  
  if(fERecoilError) {
    delete[] fERecoilError;
    fERecoilError = 0;
  }
  
  if(fQNeutron) {
    delete[] fQNeutron;
    fQNeutron = 0;
  }
  
  if(fQNeutronError) {
    delete[] fQNeutronError;
    fQNeutronError = 0;
  }
  
  if(fQGamma) {
    delete[] fQGamma;
    fQGamma = 0;
  }
  
  if(fQGammaError) {
    delete[] fQGammaError;
    fQGammaError = 0;
  }
  
  if(fQGammaTheo) {
    delete[] fQGammaTheo;
    fQGammaTheo = 0;
  }
  
  if(fQGammaTheoError) {
    delete[] fQGammaTheoError;
    fQGammaTheoError = 0;
  }
  
  if(fQGammaErrorGraph) {
    delete fQGammaErrorGraph;
    fQGammaErrorGraph = 0;
  }
  
  if(fQGammaTheoGraph) {
    delete fQGammaTheoGraph;
    fQGammaTheoGraph = 0;
  }
  
  if(fQNeutronTheoGraph) {
    delete fQNeutronTheoGraph;
    fQNeutronTheoGraph = 0;
  }
  
  if(fDoubleGaus) {
    delete fDoubleGaus;
    fDoubleGaus = 0;
  }
  
  if(fGammaGaus) {
    delete fGammaGaus;
    fGammaGaus = 0;
  }
  
  if(fNeutronGaus) {
    delete fNeutronGaus;
    fNeutronGaus = 0;
  }
  
  if(fLinhardFunction) {
    delete fLinhardFunction;
    fLinhardFunction = 0;
  }
  
  if(fConstantFunction) {
    delete fConstantFunction;
    fConstantFunction = 0;
  }
}

void KQDistribution::MakeTargetDir() { 
  //This method creates the target directory
  cout << "Making target directory ..." << endl;
  gSystem->Exec(TString::Format("mkdir -p %s",fTargetDir.c_str()).Data()); //if directory exists, nothing happens
}

/*
bool KQDistribution::ReadEvents() {
  //reads the KBolometerEvents in "fSourceDir/fSourceFile.root"
  cout << "Reading events ... " << endl;
  fOutputFile = new TFile(TString::Format("%s/%s_%s_%s.root",fTargetDir.c_str(),fSourceFile.c_str(),GetCategoryName(),fDetectorName.c_str()).Data(),"RECREATE");
  fInputFile = new TFile(TString::Format("%s/%s.root",fSourceDir.c_str(),fSourceFile.c_str()).Data());
  TTree* tr = (TTree*)fInputFile->Get("t");
  TBranch* aBranch = tr->GetBranch("fBolo");
  TClonesArray* aClonesArray = 0;
  tr->SetBranchAddress("fBolo",&aClonesArray);
  KHLABolometerRecord* aBoloRecord;
  
  Int_t aNumEntries = aBranch->GetEntries();
  
  if(aNumEntries==0) {
    cout << "KQDistribution::ReadEvents(): " << fSourceFile << ".root has no entries! " << endl;
    return false;
  }
  
  cout << "Open " << fSourceFile << ".root" << endl;
  cout << "with " << aNumEntries << " entries " << endl;
  cout << "fNumBinsERecoil: " << fNumBinsERecoil << endl;
  cout << "fNumBinsQ: " << fNumBinsQ << endl;
  cout << "fERecoilMin: " << fERecoilMin << endl;
  cout << "fERecoilMax: " << fERecoilMax << endl;
  cout << "fQMin: " << fQMin << endl;
  cout << "fQMax: " << fQMax << endl;
  
  Int_t aNumBoloEvents;
  if(fDetectorName!="ALL")
    for(int k = 0; k<aNumEntries; ++k) {
      aBranch->GetEntry(k);
      if(!(k%100000))
        cout << "entry " << k << endl;
      aNumBoloEvents += aClonesArray->GetEntries();
      for(int l = 0; l<aClonesArray->GetEntries(); ++l) {
        aBoloRecord = (KHLABolometerRecord*)aClonesArray->At(l);
        if(aBoloRecord->GetEventFlag()==fEventCategory && aBoloRecord->GetDetectorName()==fDetectorName 
          && aBoloRecord->GetEnergyRecoil()<fERecoilMax)
          fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),
                                     aBoloRecord->GetQvalue(),
                                     aBoloRecord->GetEnergyIon(),
                                     aBoloRecord->GetEnergyHeat(1),
                                     aBoloRecord->GetBaselineNoiseHeat(1),
                                     aBoloRecord->GetBaselineNoiseIonFiducial()
                          ));

      }
    }
    else
    for(int k = 0; k<aNumEntries; ++k) {
      aBranch->GetEntry(k);
      if(!(k%100000))
        cout << "entry " << k << endl;
      aNumBoloEvents += aClonesArray->GetEntries();
      for(int l = 0; l<aClonesArray->GetEntries(); ++l) {
        aBoloRecord = (KHLABolometerRecord*)aClonesArray->At(l);
        if(aBoloRecord->GetEventFlag()==fEventCategory)
          fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),
                                     aBoloRecord->GetQvalue(),
                                     aBoloRecord->GetEnergyIon(),
                                     aBoloRecord->GetEnergyHeat(1),
                                     aBoloRecord->GetBaselineNoiseHeat(1),
                                     aBoloRecord->GetBaselineNoiseIonFiducial()
                          ));
      }
        
    }
    
  cout << "All bolometer events: " << aNumBoloEvents << endl;
  cout << GetCategoryName() << " events in detector " << fDetectorName << ": " << fData.size() << endl;
  
  if(!fData.size()) {
    cout << "KQDistribution::ReadEvents(): " << "no bolometer events found, nothing to be done! " << endl;
    return false;
  }
  sort(fData.begin(),fData.end());
  fDataSize = fData.size();
  return true;
}
*/


bool KQDistribution::ReadEvents() { //deprecated
  //This method reads the KBolometerEvents in "fSourceDir/fSourcefile.root"
  //For each KBolometerEvents the Q value and the recoil energy is saved in a data structure "DataRecord",
  //which is appended to the vector "fData".
  //Double_t arrays for the single quantities can directly be retrieved with
  //KQDistribution::GetData(KQDistribution::ERECOIL) or KQDistribution::GetData(KQDistribution::Q).
  
  cout << "Reading events ... " << endl;
  fOutputFile = new TFile(TString::Format("%s/%s_%s",
                                          fTargetDir.c_str(),
                                          GetCategoryName(),
                                          fDetectorName.c_str()).Data(),"RECREATE");
                                          
  fKDataReader = new KDataReader(TString::Format("%s",fSourceFile.c_str()).Data());
  fKHLAEvent = (KHLAEvent*)fKDataReader->GetEvent();
  if(!fKHLAEvent) {
    cout << "KQDistribution::ReadEvents(): no KHLAEvent found! " << endl;
    return false;
  }
  
  Int_t aNumEntries = fKDataReader->GetEntries();
  Int_t aNumBoloEvents = 0;
  
  if(aNumEntries==0) {
    cout << "KQDistribution::ReadEvents(): " <<  fSourceFile << ".root has no entries! " << endl;
    return false;
  }
  
  cout << "Open " << fSourceFile << endl;
  cout << "with " << aNumEntries << " entries " << endl;
  
  cout << "fNumBinsERecoil: " << fNumBinsERecoil << endl;
  cout << "fNumBinsQ: " << fNumBinsQ << endl;
  cout << "fERecoilMin: " << fERecoilMin << endl;
  cout << "fERecoilMax: " << fERecoilMax << endl;
  cout << "fQMin: " << fQMin << endl;
  cout << "fQMax: " << fQMax << endl;
  
  
  KHLABolometerRecord* aBoloRecord;
  if(fDetectorName!="ALL")
    for(int k = 0; k<aNumEntries; ++k) {
      fKDataReader->GetEntry(k);
      if(!(k%100000))
  cout << "Entry " << k << endl;
      aNumBoloEvents += fKHLAEvent->GetNumBolos(); 
      for(int l = 0; l<fKHLAEvent->GetNumBolos(); ++l)  {
  aBoloRecord = fKHLAEvent->GetBolo(l);
  if(aBoloRecord->GetEventFlag()==fEventCategory && aBoloRecord->GetDetectorName()==fDetectorName && aBoloRecord->GetEnergyRecoil()<fERecoilMax)
    fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),
                               aBoloRecord->GetQvalue(),
                               aBoloRecord->GetEnergyIon(),
                               aBoloRecord->GetEnergyHeat(0)
                              ));
      }
    }
    else
    for(int k = 0; k<aNumEntries; ++k) {
      fKDataReader->GetEntry(k);
      if(!(k%100000))
  cout << "Entry " << k << endl;
      aNumBoloEvents += fKHLAEvent->GetNumBolos();
      for(int l = 0; l<fKHLAEvent->GetNumBolos(); ++l) {
  aBoloRecord = fKHLAEvent->GetBolo(l);
  if(aBoloRecord->GetEventFlag()==fEventCategory)
    fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),
                               aBoloRecord->GetQvalue(),
                              aBoloRecord->GetEnergyIon(),
                              aBoloRecord->GetEnergyHeat(0)
                              ));
      }
    }

  cout << "All bolometer events: " << aNumBoloEvents << endl;
  cout << GetCategoryName() << " events in detector " << fDetectorName << ": " << fData.size() << endl;
  
  if(!fData.size()) {
    cout << "KQDistribution::ReadEvents(): " << "no bolometer events found, nothing to be done! " << endl;
    return false;
  }
  
  sort(fData.begin(),fData.end());
  fDataSize = fData.size();
  return true;
} 

void KQDistribution::FillHistograms() {
  //This method fills fNumProjections TH2D Q-ERecoil histograms with equal numbers of entries from fData
  //Each histogram is filled with the amount of entries corresponding to
  //the integer quotient of the total entries "fDataSize"  and the number of  histograms "fNumProjections".
  //If this quotient is zero, the number of histograms "fNumProjections" is set to one.
  //The last histogram contains the remaining events "fDataSize" modulo the number of entries per histogram.
  
  cout << "Filling histograms ... " << endl;
  gDirectory= fOutputFile;
  
  Int_t entriesPerHistogram = fDataSize/fNumProjections;
  Int_t remainEvents;
  cout << "Entries per histogram: " << entriesPerHistogram << endl;
  if(entriesPerHistogram==0) {
    fNumProjections = 1;
    remainEvents = fDataSize;
  }
  else
  {
    fNumProjections = fDataSize/entriesPerHistogram + 1;
    remainEvents = fDataSize % entriesPerHistogram;
  }
  cout << "Remaining events: " << remainEvents << endl;
  
  fHistogramRecords.clear();
  
  //all but not the last histogram
  for(int k_hist = 0; k_hist<fNumProjections-1; ++k_hist) {
    cout << "Histogram: " << k_hist << endl;
    fHistogramRecords.push_back(HistogramRecord());
    fHistogramRecords[k_hist].fHistogram = new TH2D(TString::Format("hist_%s_%i",GetCategoryName(),k_hist).Data(),
                                                    "",
                                                    fNumBinsERecoil,
                                                    fData[k_hist*entriesPerHistogram].fERecoil,
                                                    fData[k_hist*entriesPerHistogram + entriesPerHistogram - 1].fERecoil,
                                                    fNumBinsQ,
                                                    fQMin,
                                                    fQMax
                                               );
    
    cout << TString::Format("Histogram %i created",k_hist).Data() << endl;
    for(int k_entry = 0; k_entry<entriesPerHistogram; ++k_entry)
      fHistogramRecords[k_hist].fHistogram->Fill(fData[k_hist*entriesPerHistogram+k_entry].fERecoil,
                                                 fData[k_hist*entriesPerHistogram+k_entry].fQ
                                            );
    fHistogramRecords[k_hist].fERecoilMean = fHistogramRecords[k_hist].fHistogram->GetMean(1);
    fHistogramRecords[k_hist].fERecoilError = fConfidenceInSigmas*sqrt(fHistogramRecords[k_hist].fHistogram->GetCovariance(1,1));
    cout << TString::Format("Histogram %i filled: range of E [%lf,%lf]",
                            k_hist,
                            fHistogramRecords[k_hist].fHistogram->GetXaxis()->GetXmin(),
                            fHistogramRecords[k_hist].fHistogram->GetXaxis()->GetXmax()).Data() << endl;
  }
  
  //last histogram
  fHistogramRecords.push_back(HistogramRecord());
  fHistogramRecords[fNumProjections-1].fHistogram = new TH2D(TString::Format("hist_%s_%i",GetCategoryName(),
                                                                             fNumProjections-1).Data(),
                                                             "",fNumBinsERecoil,
                                                             fData[(fNumProjections-1)*entriesPerHistogram].fERecoil,
                                                             fData[(fNumProjections-1)*entriesPerHistogram + remainEvents -1].fERecoil,
                                                             fNumBinsQ,
                                                             fQMin,
                                                             fQMax
                                                        );
                                                        
  for(int k_entry = 0; k_entry<remainEvents; ++k_entry)
    fHistogramRecords[fNumProjections-1].fHistogram->Fill(fData[(fNumProjections-1)*entriesPerHistogram+k_entry].fERecoil,
                                                          fData[(fNumProjections-1)*entriesPerHistogram+k_entry].fQ
                                                         );
    cout << TString::Format("Histogram %i filled: range of E [%lf,%lf]",
                            fNumProjections-1,
                            fHistogramRecords[fNumProjections-1].fHistogram->GetXaxis()->GetXmin(),
                            fHistogramRecords[fNumProjections-1].fHistogram->GetXaxis()->GetXmax()).Data() << endl;
  cout << "Histograms filled" << endl;
  fHistogramRecords[fNumProjections-1].fERecoilMean = fHistogramRecords[fNumProjections-1].fHistogram->GetMean(1);
  fHistogramRecords[fNumProjections-1].fERecoilError = fConfidenceInSigmas*sqrt(fHistogramRecords[fNumProjections-1].fHistogram->GetCovariance(1,1));
  
  //labels for all histograms
  for(int k = 0; k<fNumProjections; ++k) {
    fHistogramRecords[k].fHistogram->SetTitle(TString::Format("category: %s #splitline{E_{R} = %f..%f}{#bar{E_{R}}=%f #pm %f}",
                                                              GetCategoryName(),
                                                              fHistogramRecords[k].fHistogram->GetXaxis()->GetXmin(),
                                                              fHistogramRecords[k].fHistogram->GetXaxis()->GetXmax(),
                                                              fHistogramRecords[k].fERecoilMean,
                                                              fHistogramRecords[k].fERecoilError).Data());
    fHistogramRecords[k].fHistogram->GetXaxis()->SetTitle("E_{Recoil}[keV]");
    fHistogramRecords[k].fHistogram->GetYaxis()->SetTitle("Q");
  }
  cout << "Histogram attributes set ... " << endl;  
}

void KQDistribution::CalculateTheoErrors() {
  //This method calculates the theoretical central Q values and band widths for the gamma and neutron recoil bands for all histograms.
  //At the moment, this is done the following way:
  //The central Q value for gamma recoil band is always 1.
  //The central Q value for the neutron recoil is calculated by the Lindhard formula Q(E_Recoil) = 0.165* E_Recoil^0.185.
  //For the band uncertainties, first the heat and ion channel  uncertainties on baseline and calibration level E=365.0 keV
  //for the specific bolometer "fDetectorName" are read from an ASCII file with tabled values via KBoloConfig.
  //There might be a more sophisticated way in the future to handle this information
  //Then via KQUncertainty the heat and ion channel uncertainties for all 
  
  ofstream QNeutronTheoRawFile(TString::Format("%s/QNeutronTheoValues.dat",fTargetDir.c_str()).Data());
  ofstream QGammaTheoRawFile(TString::Format("%s/QGammaTheoValues.dat",fTargetDir.c_str()).Data());
  QNeutronTheoRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" <<  setw(20) << "QNeutronTheo" << setw(20) << "ERecoilError" << setw(20) << "QNeutronTheoError" << endl;
  QGammaTheoRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGammaTheo" << setw(20) << "ERecoilError" << setw(20) << "QGammaTheoError" << endl;
  
  KBoloConfig aBoloConfig(fDetectorName,fBoloConfigFile);
  
  
  for(int k = 0; k<fNumProjections; ++k) {
    fHistogramRecords[k].fQGammaTheo = 1;
    fHistogramRecords[k].fQNeutronTheo = KQUncertainty::GetQValue(fHistogramRecords[k].fERecoilMean);
    
    Double_t aHeatUncertainty = KQUncertainty::GetChannelUncertainty(fHistogramRecords[k].fERecoilMean,
                                                                     aBoloConfig.GetSigmaHeatZero(),
                                                                     aBoloConfig.GetSigmaHeatCalibration(),
                                                                     aBoloConfig.GetEnergyCalibration());
    
    Double_t anIonUncertainty =  KQUncertainty::GetChannelUncertainty(fHistogramRecords[k].fERecoilMean,
                                                                      aBoloConfig.GetSigmaIonZero(),
                                                                      aBoloConfig.GetSigmaIonCalibration(),
                                                                      aBoloConfig.GetEnergyCalibration());
    
    Double_t aVoltageBias = aBoloConfig.GetChannelVoltage();
    
    fHistogramRecords[k].fQGammaTheoError =fConfidenceInSigmas* KQUncertainty::GetElecRecoilWidth(fHistogramRecords[k].fERecoilMean,
                                                                              aHeatUncertainty,
                                                                              anIonUncertainty,
                                                                              aVoltageBias);
                                                                              
    fHistogramRecords[k].fQNeutronTheoError = fConfidenceInSigmas*KQUncertainty::GetNuclRecWidth(fHistogramRecords[k].fERecoilMean,
                                                                              aHeatUncertainty,
                                                                              anIonUncertainty,
                                                                              aVoltageBias);
    
    
    QNeutronTheoRawFile << setw(5) << " " << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << 
    fHistogramRecords[k].fQNeutronTheo << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << 
    fHistogramRecords[k].fQNeutronTheoError << endl;
    QGammaTheoRawFile << setw(5) << " " << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << 
    fHistogramRecords[k].fQGammaTheo << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << 
    fHistogramRecords[k].fQGammaTheoError << endl;
  }
}

void KQDistribution::MakeCanvases() {
  //This method creates fMainCanvas showing two pads, the left pad shows fTotalMultiGraph 
  //and the right pad shows the current TH1D Q-ERecoil histrom with gaussian fits and two buttons to navigate to previous or next histrogram.
  //It creates also fDrawCanvas which is used to store the images of histrograms in image files with format "fFileFormat"
  cout << "Making Canvases ... " << endl;
  fMainCanvas = new TCanvas("fMainCanvas","Q plots",10,10,800,400);
  fMainCanvas->Divide(2,1);
  fMainCanvas->cd(1);
  fLeftSubPad = new TPad("fLeftSubPad","fLeftSubPad",0.01,0.01,0.99,0.99);
  fLeftSubPad->SetFillColor(33);
  fLeftSubPad->Draw();
  fMainCanvas->Update();
  fLeftSubPad->cd();
  fMainCanvas->Update(); //necessary ???
  
  fMainCanvas->cd(2);
  fPreviousButton = new TButton("previous","(KQDistribution*)(gROOT->FindObject(\"default\"))->Previous()",0.01,0.90,0.49,0.99);
  fPreviousButton->Connect("TButton","Clicked()","KQDistribution",this,"Previous()");
  fPreviousButton->Draw();
  fNextButton = new TButton("next","(KQDistribution*)(gROOT->FindObject(\"default\"))->Next()",0.5,0.90,0.99,0.99);
  fNextButton->Connect("TButton","Clicked()","KQDistribution",this,"Next()");
  fNextButton->Draw();
  
  fDrawCanvas = new TCanvas("fDrawCanvas","fDrawCanvas",10,10,800,400);
  fDrawCanvas->Draw();
  fDrawCanvas->cd(); 
}

void KQDistribution::SaveGraphData() {
  //This method creates ASCII files containing the Q values from the gaussian fits and ERecoil mean values of the TH1D Q-ERecoil histograms
  //these are the sourcefiles for subsequent graph creation
  CalculateTheoErrors();
  cout << "Saving source data for graphs ... " << endl;
  fDoubleGaus = new TF1("fDoubleGaus",GausFit,0.1,2,10);
  fDoubleGaus->SetParNames("C_{#gamma}",
                           "Q_{#gamma}",
                           "#sigma_{Q_{#gamma}}",
                           "C_{n}","Q_{n}",
                           "#sigma_{Q_{n}}",
                           "Q_{#gamma,high}",
                           "Q_{#gamma,low}",
                           "Q_{n,high}",
                           "Q_{n,low}"
               );
  fGammaGaus = new TF1("fGammaGaus",GausFit,fQMin,fQMax,5);
  fGammaGaus->SetParNames("C_{#gamma}","Q_{#gamma}","#sigma_{Q_{#gamma}}","Q_{#gamma,low}","Q_{#gamma,high}");
  fGammaGaus->SetLineColor(3);
  fGammaGaus->SetNpx(1000);
  fNeutronGaus = new TF1("fNeutronGaus",GausFit,fQMin,fQMax,5);
  fNeutronGaus->SetParNames("C_{n}","Q_{n}","#sigma_{Q_{n}}","Q_{n,low}","Q_{n,high}");
  fNeutronGaus->SetLineColor(4);
  fNeutronGaus->SetNpx(1000);

  ofstream QNeutronRawFile(TString::Format("%s/QNeutronValues.dat",fTargetDir.c_str()).Data());
  ofstream QGammaRawFile(TString::Format("%s/QGammaValues.dat",fTargetDir.c_str()).Data());
  QNeutronRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QNeutron" << setw(20) << "ERecoilError" << setw(20) << "QNeutronError" << endl;
  QGammaRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGamma" << setw(20) << "ERecoilError" << setw(20) << "QGammaError" << endl;
  
  for(int k = 0; k<fNumProjections; ++k) {

     
    DrawHistogram(fDrawCanvas,k);
    
    fHistogramRecords[k].fQGamma = fGammaGaus->GetParameter(1);
    fHistogramRecords[k].fQGammaError = fConfidenceInSigmas* abs(fGammaGaus->GetParameter(2));
    fHistogramRecords[k].fQNeutron = fNeutronGaus->GetParameter(1);
    fHistogramRecords[k].fQNeutronError = fConfidenceInSigmas * abs(fNeutronGaus->GetParameter(2));
    if(fHistogramRecords[k].fQNeutron<0.5&&fHistogramRecords[k].fQNeutron>0.1)
      QNeutronRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << 
      fHistogramRecords[k].fQNeutron << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << 
      fHistogramRecords[k].fQNeutronError << endl;
    else
      cout << "KQDistribution::SaveGraphData(): rejecting Q_Neutron = " << fHistogramRecords[k].fQNeutron << ", out of boundaries [0.1,0.5] " << endl;
    if(fHistogramRecords[k].fQGamma<1.2 && fHistogramRecords[k].fQGamma > 0.8)
      QGammaRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << 
      fHistogramRecords[k].fQGamma << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << 
      fHistogramRecords[k].fQGammaError << endl;
    else
      cout << "KQDistribution::SaveGraphData(): rejecting Q_Gamma = " << fHistogramRecords[k].fQGamma << ", out of boundaries [0.8,1.2] " << endl;

    cout << TString::Format("QGamma = %f +/- %f \t QNeutron = %f +/- %f",
                            fHistogramRecords[k].fQGamma,
                            fHistogramRecords[k].fQGammaError,
                            fHistogramRecords[k].fQNeutron,
                            fHistogramRecords[k].fQNeutronError).Data() << endl;
    fDrawCanvas->Print(TString::Format("%s/%s_%s_%i.%s",
                                       fTargetDir.c_str(),
                                       GetCategoryName(),
                                       fDetectorName.c_str(),
                                       k,fImageFormat.c_str()).Data());
  }
}

void KQDistribution::MakeGraphs() {
  //This method  creates the graphs shown on the left pad of fMainCanvas, which contain single points for each TH1D histogram,
  // fQGammaErrorGraph with fQGamma +/- fQGammaError
  // fQGammaTheoGraph with fQGammaTheo +/- fQGammaTheoError
  // fQNeutronErrorGraph with fQNeutron +/ fQNeutronError
  // fQNeutronTheoGraph with fQNeutronTheo +/- fQNeutronTheoError
  cout << "Creating and drawing graphs ... " << endl;

  fTotalGraph = new TGraph(fDataSize,GetData(ERECOIL),GetData(Q));
  cout << "Graph with all data created ... " << endl;
  fTotalGraph->SetName("fTotalGraph");
  fTotalGraph->SetTitle(TString::Format("category: %s \n detector: %s events: %i",
                                        GetCategoryName(),
                                        fDetectorName.c_str(),
                                        fDataSize).Data());
  fTotalGraph->GetXaxis()->SetTitle("E_{recoil}[keV]");
  fTotalGraph->GetYaxis()->SetTitle("Q");
  fTotalGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fTotalGraph->GetYaxis()->SetRangeUser(fQMin,fQMax);
  cout << "attributes of graph with all data set" << endl;
  gDirectory = fOutputFile;
  fTotalGraph->Write();
  fQGammaErrorGraph = new TGraphErrors(TString::Format("%s/QGammaValues.dat",
                                                       fTargetDir.c_str()
                                                      ).Data(),"%lg %lg %lg %lg");
  cout << "fQGammaErrorGraph created" << endl;
  fQNeutronErrorGraph = new TGraphErrors(TString::Format("%s/QNeutronValues.dat",
                                                         fTargetDir.c_str()
                                                        ).Data(),"%lg %lg %lg %lg");
  cout << "fQNeutronErrorGraph created" << endl;
  fQGammaTheoGraph = new TGraphErrors(TString::Format("%s/QGammaTheoValues.dat",
                                                      fTargetDir.c_str()
                                                     ).Data(),"%lg %lg %lg %lg");
  cout << "fQGammaTheoGraph created" << endl;
  fQNeutronTheoGraph = new TGraphErrors(TString::Format("%s/QNeutronTheoValues.dat",
                                                        fTargetDir.c_str()
                                                       ).Data(),"%lg %lg %lg %lg");
  cout << "fQNeutronTheoGraph created" << endl;
  
  fTotalMultiGraph = new TMultiGraph();
  cout << "MultiGraph created" << endl;
  fTotalMultiGraph->Add(fTotalGraph,"APX");
  cout << "graph with all data created" << endl;
  
  fQGammaErrorGraph->SetName("QGammaData");
  fQGammaErrorGraph->SetTitle("");
  fQGammaErrorGraph->SetLineColor(0);
  fQGammaErrorGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fQGammaErrorGraph->SetFillStyle(3001);
  fQGammaErrorGraph->SetFillColor(0);
  fTotalMultiGraph->Add(fQGammaErrorGraph,"P");
  cout << "fQGammaErrorGraph added" << endl;
  
  fQNeutronErrorGraph->SetName("QNeutronData");
  fQNeutronErrorGraph->SetTitle("");
  fQNeutronErrorGraph->SetLineColor(0);
  fQNeutronErrorGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fTotalMultiGraph->Add(fQNeutronErrorGraph,"P");
  cout << "fQNeutronErrorGraph added" << endl;
  
  fQNeutronTheoGraph->SetName("fQNeutronTheoGraph");
  fQNeutronTheoGraph->SetTitle("");
  fQNeutronTheoGraph->SetLineColor(4);
  fQNeutronTheoGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fQNeutronTheoGraph->SetFillStyle(3002);
  fQNeutronTheoGraph->SetFillColor(fQNeutronTheoGraph->GetLineColor());
  fTotalMultiGraph->Add(fQNeutronTheoGraph,"P3");
  cout << "fQNeutronTheoGraph added" << endl;
  
  fQGammaTheoGraph->SetName("fQGammaTheoGraph");
  fQGammaTheoGraph->SetTitle("");
  fQGammaTheoGraph->SetLineColor(5);
  fQGammaTheoGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fQGammaTheoGraph->SetFillStyle(3002);
  fQGammaTheoGraph->SetFillColor(fQGammaTheoGraph->GetLineColor());
  fTotalMultiGraph->Add(fQGammaTheoGraph,"P3");
  cout << "fQGammaTheoGraph added" << endl;
  
  fLeftSubPad->cd();
  fTotalMultiGraph->Draw("");
  fLeftSubPad->Update();
  fDrawCanvas->cd();
  fDrawCanvas->Clear("");
  fTotalMultiGraph->Draw("");
  fDrawCanvas->BuildLegend(0.1,0.67,0.48,0.88);
  TLegend* aLegend = (TLegend*)fDrawCanvas->FindObject("TPave");
  if(aLegend) aLegend->SetFillColor(0);
  TPaveText* aTitle = (TPaveText*)fDrawCanvas->FindObject("title");
  if(aTitle) aTitle->SetFillColor(0);
  fDrawCanvas->Update();
  fDrawCanvas->Print(TString::Format("%s/%s_%s_total.%s",
                                     fTargetDir.c_str(),
                                     GetCategoryName(),
                                     fDetectorName.c_str(),
                                     fImageFormat.c_str()
                                    ).Data());
}

void KQDistribution::MakeAll(Int_t anEventCategory,const Char_t* aDetectorName,Int_t aNumProjections) {
  //This is the main routine, which reads and evaluates fSourcefile.ROOT by calling the other routines
  cout << "Making all ... " << endl;
  fEventCategory = anEventCategory;
  fDetectorName = aDetectorName;
  fNumProjections = aNumProjections;
  
  gROOT->SetBatch(fIsBatch);
  SetStyle();
  //ResetVars();
  MakeTargetDir();
  if(!ReadEvents())
    return;
  MakeCanvases();
  FillHistograms();
  SaveGraphData();
  MakeGraphs();
  //if(fIsBatch)
    //fAdjustPanel = new KQAdjustPanel(gClient->GetRoot(),200,200,CorrectBoundaries,StoreImage);
}





