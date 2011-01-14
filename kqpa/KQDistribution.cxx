//_____________________________________________
//
// KQDistribution.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

#include "KQDistribution.h"

#ifndef __KQFUNCTIONS__
#define __KQFUNCTIONS__

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

bool operator<(const KQDistribution::DataRecord& aFirstDataRecord,const KQDistribution::DataRecord& aSecondDataRecord) {
	//implemented to order DataRecords according ascending fERecoil values
  return(aFirstDataRecord.fERecoil<aSecondDataRecord.fERecoil);
}

#endif

ClassImp(KQDistribution);

KQDistribution::KQDistribution() : fAlpha(0.165), fBeta(0.185)
{
	// default constructor
	InitializeMembers();
	
}

void KQDistribution::InitializeMembers() 
{
	// sets initial members
	cout << "Setting members ... " << endl;
	fSourceFile = "Kds_Run12_v3.0_skim";	//user-specific
	fSourceDir = "/kalinka/storage/edelweiss/Bolo/Run12/kdata/Final/Neutron/"; //user-specific
	fTargetDir = "QDistributionFit-Plots/"; //user-specific
	fNumBinsERecoil = 100;
	fNumBinsQ = 100;
	fERecoilMin = 0;
	fERecoilMax = 500;
	fQMin = 0;
	fQMax = 2;
	fHistogramCounter = 0;
	fConfidenceInSigmas = 2;
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

void KQDistribution::DrawHistogram(bool aDirection) {
	// draws a TH1D Q-ERecoil histogram, which is a projection on Q-axis of a previously created TH2D Q-ERecoil histogram 
	// this histogram is in the fHistogramCounter'th element of type HistogramRecord in fHistogramRecords
	// aDirection=true: fHistogramCounter is incremented, next histogram is drawn
	// aDirection=false: fHistogramCounter is decremented, previous histogram is drawn
	//
	// two gaussian fits are applied for gamma and neutron bands within fixed intervals and the corresponding chi2 values are drawn on a TPaveText
	cout << "Drawing Histogram ... " << endl;
	if(aDirection)
		fHistogramCounter = (fHistogramCounter + 1) % fNumProjections;
	else
		fHistogramCounter = (fHistogramCounter - 1 + fNumProjections) % fNumProjections;
	// TH2D* hist = (TH2D*)gDirectory->FindObject(TString::Format("hist_%s_%i",GetCategoryName(),fHistogramCounter).Data());
	if(fHistogramRecords[fHistogramCounter].fHistogram) {
		fRightSubPad->cd();
		fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Draw();
		SetBoundaries(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),0.6,fHistogramRecords[fHistogramCounter].fQNeutronTheo,fHistogramRecords[fHistogramCounter].fQGammaTheo,fHistogramRecords[fHistogramCounter].fQNeutronTheoError/fConfidenceInSigmas,fHistogramRecords[fHistogramCounter].fQGammaTheoError/fConfidenceInSigmas);
		fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LMI");

		fChi2PaveText->Clear();
		fChi2PaveText->SetX1NDC(0.6);
		fChi2PaveText->SetX2NDC(1);
		fChi2PaveText->SetY1NDC(0.2);
		fChi2PaveText->SetY2NDC(0.35);
		
		Double_t aChi2Gamma = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6));
		Int_t anNDFGamma = GetNDF(fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6),3);
		Double_t aChi2Neutron = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8));
		Int_t anNDFNeutron = GetNDF(fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8),3);

		fChi2PaveText->AddText(TString::Format("#chi_{g}^{2}/ndf_{g}     \t%f/%i",aChi2Gamma,anNDFGamma).Data());
		fChi2PaveText->AddText(TString::Format("#chi_{n}^{2}/ndf_{n}     %f/%i",aChi2Neutron,anNDFNeutron).Data());
		cout << "aChi2Gamma / anNDFGamma " << aChi2Gamma << "/" << anNDFGamma << " = " << aChi2Gamma/anNDFGamma << endl;
		cout << "aChi2Neutron / anNDFNeutron " << aChi2Neutron << "/" << anNDFNeutron << " = " << aChi2Neutron/anNDFNeutron << endl;

		fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LMI");
		fGammaGaus->SetParameter(0,fDoubleGaus->GetParameter(0));
		fGammaGaus->SetParameter(1,fDoubleGaus->GetParameter(1));
		fGammaGaus->SetParameter(2,fDoubleGaus->GetParameter(2));
		fNeutronGaus->SetParameter(0,fDoubleGaus->GetParameter(3));
		fNeutronGaus->SetParameter(1,fDoubleGaus->GetParameter(4));
		fNeutronGaus->SetParameter(2,fDoubleGaus->GetParameter(5));
		//TFitResultPtr r = hists[k]->ProjectionY()->Fit(fGammaGaus,"LRMI","",0.1,1.2);
		fGammaGaus->Draw("SAME");
		fNeutronGaus->Draw("SAME");

		fChi2PaveText->Draw();
		


	}
	else
	{
		cout << "KQDistribution::DrawHistogram(): Histogram " << fHistogramCounter << " is not available " << endl;
	}	
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
  
  fDoubleGaus->SetParameter(0,QGammaMaxY); // C_{#gamma}
  fDoubleGaus->SetParameter(1,QGammaMaxX); // Q_{#gamma}
  fDoubleGaus->SetParameter(2,aSecondError); // #sigma_{Q_{#gamma}}
  fDoubleGaus->SetParameter(3,QNeutronMaxY); // C_{neutron}
  fDoubleGaus->SetParameter(4,QNeutronMaxX); // Q_{neutron}
  fDoubleGaus->SetParameter(5,aFirstError); // #sigma_{Q_{neutron}}
  fDoubleGaus->SetParameter(6,QGammaMaxX + aNumSigmas*aSecondError); // Q_{#gamma,high}
  fDoubleGaus->SetParameter(7,QGammaMaxX - aNumSigmas*aSecondError); // Q_{#gamma,low}
  fDoubleGaus->SetParameter(8,QNeutronMaxX + aNumSigmas*aFirstError); // Q_{neutron,high}
  fDoubleGaus->SetParameter(9,QNeutronMaxX - aNumSigmas*aFirstError); // Q_{neutron,low}
}

void KQDistribution::SaveImage(Int_t anIndex) {
	// saves the image drawn on fDrawCanvas in a pdf file in path "fTargetDir/fTargetSubDir/CategoryName_fDectorName_anIndex.pdf"
	cout << "Saving Image ... " << endl;
	if(fDrawCanvas)
		fDrawCanvas->Print(TString::Format("%s/%s/%s_%s_%i.pdf",fTargetDir.c_str(),fTargetSubDir.c_str(),GetCategoryName(),fDetectorName.c_str(),anIndex).Data());
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
	fDoubleGaus->SetParameter(parameterIndex,fDoubleGaus->GetParameter(parameterIndex)+aStep);
	fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LMI");
	GetFitParameters(fHistogramCounter);
	RedrawPeaks();
	
}

void KQDistribution::SetPaths(char* aSourceDir,char* aSourceFile,char* aTargetDir) {
	// sets the specified paths
	
	fSourceDir = aSourceDir;
	fSourceFile = aSourceFile;
	fTargetDir = aTargetDir;
}
12000
Double_t KQDistribution::GetGammaMax(TH1D* aHistogram) {
	// returns the Q value corresponding to the bin with maximal entries in aHistogram
	return(fQMin + aHistogram->GetMaximumBin()*(fQMax-fQMin)/fNumBinsQ);
	// return(Qmin+hist->GetMaximumBin(Int_t((1-Qmin)/(Qmax-Qmin)*numbinsQ))*(Qmax-Qmin)/numbinsQ);
}

void KQDistribution::GetFitParameters(Int_t anIndex) {
	// changes the current values, fQGamma, fQGammaError, fQNeutron, fQNeutronError, in the anIndex'th element of fHistogramRecords to the fit parameter values and errors in fDoubleGaus
	// these values are also changed in the graphs, fQGammaErrorGraph and fQNeutronErrorGraph,
	// as well as in the ASCII files, "fTargetDir/fTargetSubDir/QNeutronValues.dat" and "fTargetDir/fTargetSubDir/QGammaValues.dat" containing the data of the graphs
	// fTotalMultiGraph containing fQGammaErrorGraph and fQNeutronErrorGraph is redrawn
	cout << "Saving new fit parameters ... " << endl;
	fHistogramRecords[anIndex].fQGamma = fDoubleGaus->GetParameter(1);
	fHistogramRecords[anIndex].fQGammaError = fConfidenceInSigmas*abs(fDoubleGaus->GetParameter(2));
	fHistogramRecords[anIndex].fQNeutron = fDoubleGaus->GetParameter(4);
	fHistogramRecords[anIndex].fQNeutronError = fConfidenceInSigmas*abs(fDoubleGaus->GetParameter(5));
	fQNeutronErrorGraph->SetPoint(anIndex,fHistogramRecords[anIndex].fERecoilMean,fHistogramRecords[anIndex].fQNeutron);
	fQNeutronErrorGraph->SetPointError(anIndex,fHistogramRecords[anIndex].fERecoilError,fHistogramRecords[anIndex].fQNeutronError);
	fQGammaErrorGraph->SetPoint(anIndex,fHistogramRecords[anIndex].fERecoilMean,fHistogramRecords[anIndex].fQGamma);
	fQGammaErrorGraph->SetPointError(anIndex,fHistogramRecords[anIndex].fERecoilError,fHistogramRecords[anIndex].fQGammaError);


	ofstream QNeutronRawFile(TString::Format("%s/%s/QNeutronValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
	QNeutronRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QNeutron" << setw(20) << "ERecoilError" << setw(20) << "QNeutronError" << endl;
	ofstream QGammaRawFile(TString::Format("%s/%s/QGammaValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
	QGammaRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGamma" << setw(20) << "ERecoilError" << setw(20) << "QGammaError" << endl;

	for(int k = 0; k<fNumProjections; ++k) {
		if(fHistogramRecords[k].fQNeutron<0.5&&fHistogramRecords[k].fQNeutron>0.1)
		QNeutronRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQNeutron << setw(20) << fHistogramRecords[k].fERecoilError <<  setw(20) << fHistogramRecords[k].fQNeutronError << endl;
		if(fHistogramRecords[k].fQGamma<1.2&&fHistogramRecords[k].fQGamma>0.8)
		QGammaRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQGamma << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQGammaError << endl;
		cout << TString::Format("Qg = %f +/- %f \t Q_n = %f +/- %f",fHistogramRecords[k].fQGamma,fHistogramRecords[k].fQGammaError,fHistogramRecords[k].fQNeutron,fHistogramRecords[k].fQNeutronError).Data() << endl;
	}

	fLeftSubPad->cd();
	fTotalMultiGraph->Draw();
	fDrawCanvas->cd();
}

void KQDistribution::RedrawPeaks() {
		// redraws the single gaussian shapes of the current TH1D Q-ERecoil histogram and the TPaveText showing the chi2 of the fits
		cout << "Redrawing Gaussian shapes ... " << endl;
		fDrawCanvas->cd();
		
		fChi2PaveText->Clear();
		fChi2PaveText->SetX1NDC(0.6);
		fChi2PaveText->SetX2NDC(1);
		fChi2PaveText->SetY1NDC(0.2);
		fChi2PaveText->SetY2NDC(0.35);
		Double_t aChi2Gamma = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6));
		Int_t anNDFGamma = GetNDF(fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6),3);
		Double_t aChi2Neutron = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8));
		Int_t anNDFNeutron = GetNDF(fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8),3);

		fChi2PaveText->AddText(TString::Format("#chi_{g}^{2}/ndf_{g}     \t%f/%i",aChi2Gamma,anNDFGamma).Data());
		fChi2PaveText->AddText(TString::Format("#chi_{n}^{2}/ndf_{n}     %f/%i",aChi2Neutron,anNDFNeutron).Data());
		cout << "aChi2Gamma / anNDFGamma " << aChi2Gamma << "/" << anNDFGamma << " = " << aChi2Gamma/anNDFGamma << endl;
		cout << "aChi2Neutron / anNDFNeutron " << aChi2Neutron << "/" << anNDFNeutron << " = " << aChi2Neutron/anNDFNeutron << endl;

		fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LMI");

		fGammaGaus->SetParameter(0,fDoubleGaus->GetParameter(0));
		fGammaGaus->SetParameter(1,fDoubleGaus->GetParameter(1));
		fGammaGaus->SetParameter(2,fDoubleGaus->GetParameter(2));
		fNeutronGaus->SetParameter(0,fDoubleGaus->GetParameter(3));
		fNeutronGaus->SetParameter(1,fDoubleGaus->GetParameter(4));
		fNeutronGaus->SetParameter(2,fDoubleGaus->GetParameter(5));
		fGammaGaus->Draw("SAME");
		fNeutronGaus->Draw("SAME");
		fChi2PaveText->Draw();
		fDrawCanvas->Update();
}

void KQDistribution::SetStyle() {
	// sets styles for histogram and fit result drawing
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
	// disposes all dynamically allocated class members
	cout << "Resetting variables ... " << endl;
  if(fAdjustPanel) {
    delete fAdjustPanel;
    fAdjustPanel = 0;
  }

  
  if(fKDataReader) {
    delete fKDataReader;
    fKDataReader = 0;
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

void KQDistribution::MakeDirectories() { 
	// directory "fTargetDir/fTargetSubDir/" is created with fTargetSubDir="fSourceFile/fDetectorName/GetCategoryName()"
	cout << "Making Directories" << endl;
  gSystem->Exec(TString::Format("mkdir -p %s/%s/%s/%s",fTargetDir.c_str(),fSourceFile.c_str(),fDetectorName.c_str(),GetCategoryName()).Data());
	fTargetSubDir = "";
  fTargetSubDir += fSourceFile;
  fTargetSubDir += "/";
  fTargetSubDir += fDetectorName;
  fTargetSubDir += "/";
  fTargetSubDir += GetCategoryName();
  fTargetSubDir += "/";
}

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
				if(aBoloRecord->GetEventFlag()==fEventCategory && aBoloRecord->GetDetectorName()==fDetectorName && aBoloRecord->GetEnergyRecoil()<fERecoilMax)
					fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),aBoloRecord->GetQvalue()));
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
					fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),aBoloRecord->GetQvalue()));
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

/*
bool KQDistribution::ReadEvents() { //deprecated
	//reads the KBolometerEvents in "fSourceDir/fSourcefile.root"
	cout << "Reading events ... " << endl;
  fOutputFile = new TFile(TString::Format("%s/%s_%s_%s.root",fTargetDir.c_str(),fSourceFile.c_str(),GetCategoryName(),fDetectorName.c_str()).Data(),"RECREATE");
  fKDataReader = new KDataReader(TString::Format("%s/%s.root",fSourceDir.c_str(),fSourceFile.c_str()).Data());
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
  
  cout << "Open " << fSourceFile << ".root" << endl;
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
	  fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),aBoloRecord->GetQvalue()));
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
	  fData.push_back(DataRecord(aBoloRecord->GetEnergyRecoil(),aBoloRecord->GetQvalue()));
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
} */

void KQDistribution::FillHistograms() {
	// fills fNumProjections TH2D Q-ERecoil histograms with equal numbers of entries from fData
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
  for(int k_hist = 0; k_hist<fNumProjections-1; ++k_hist) {
    cout << "Histogram: " << k_hist << endl;
    fHistogramRecords.push_back(HistogramRecord());
    fHistogramRecords[k_hist].fHistogram = new TH2D(TString::Format("hist_%s_%i",GetCategoryName(),k_hist).Data(),"",fNumBinsERecoil,fData[k_hist*entriesPerHistogram].fERecoil,fData[k_hist*entriesPerHistogram + entriesPerHistogram - 1].fERecoil,fNumBinsQ,fQMin,fQMax);
    cout << TString::Format("Histogram %i created",k_hist).Data() << endl;
    for(int k_entry = 0; k_entry<entriesPerHistogram; ++k_entry)
      fHistogramRecords[k_hist].fHistogram->Fill(fData[k_hist*entriesPerHistogram+k_entry].fERecoil,fData[k_hist*entriesPerHistogram+k_entry].fQ);
    fHistogramRecords[k_hist].fERecoilMean = fHistogramRecords[k_hist].fHistogram->GetMean(1);
    fHistogramRecords[k_hist].fERecoilError = fConfidenceInSigmas*sqrt(fHistogramRecords[k_hist].fHistogram->GetCovariance(1,1));
    cout << TString::Format("Histogram %i filled",k_hist).Data() << endl;
  }
  fHistogramRecords.push_back(HistogramRecord());
  fHistogramRecords[fNumProjections-1].fHistogram = new TH2D(TString::Format("hist_%s_%i",GetCategoryName(),fNumProjections-1).Data(),"",fNumBinsERecoil,fData[(fNumProjections-1)*entriesPerHistogram].fERecoil,fData[(fNumProjections-1)*entriesPerHistogram + remainEvents -1].fERecoil,fNumBinsQ,fQMin,fQMax);
  for(int k_entry = 0; k_entry<remainEvents; ++k_entry)
    fHistogramRecords[fNumProjections-1].fHistogram->Fill(fData[(fNumProjections-1)*entriesPerHistogram+k_entry].fERecoil,fData[(fNumProjections-1)*entriesPerHistogram+k_entry].fQ);
  cout << TString::Format("Histogram %i filled",fNumProjections-1).Data() << endl;
  cout << "Histograms filled" << endl;
  fHistogramRecords[fNumProjections-1].fERecoilMean = fHistogramRecords[fNumProjections-1].fHistogram->GetMean(1);
  fHistogramRecords[fNumProjections-1].fERecoilError = fConfidenceInSigmas*sqrt(fHistogramRecords[fNumProjections-1].fHistogram->GetCovariance(1,1));
  for(int k = 0; k<fNumProjections; ++k) {
    fHistogramRecords[k].fHistogram->SetTitle(TString::Format("category: %s #splitline{E_{R} = %f..%f}{#bar{E_{R}}=%f #pm %f}",GetCategoryName(),fHistogramRecords[k].fHistogram->GetXaxis()->GetXmin(),fHistogramRecords[k].fHistogram->GetXaxis()->GetXmax(),fHistogramRecords[k].fERecoilMean,fHistogramRecords[k].fERecoilError).Data());
    fHistogramRecords[k].fHistogram->GetXaxis()->SetTitle("E_{Recoil}[keV]");
    fHistogramRecords[k].fHistogram->GetYaxis()->SetTitle("Q");
    gDirectory=fOutputFile;
    fHistogramRecords[k].fHistogram->Write();
  }
  cout << "Histogram attributes set ... " << endl;  
}

void KQDistribution::MakeCanvases() {
	// creates fMainCanvas showing two pads, the left pad shows fTotalMultiGraph and the right pad shows the current TH1D Q-ERecoil histrom with gaussian fits and two buttons to navigate to previous or next histrogram
	// creates also fDrawCanvas which is used to store the images of histrograms in pdf files
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
  fPreviousButton = new TButton("previous","",0.01,0.90,0.49,0.99);
  fPreviousButton->Connect("Clicked()","KQDistribution",this,"DrawHistogram(false)");
  fPreviousButton->Draw();
  fNextButton = new TButton("next","",0.5,0.90,0.99,0.99);
  fNextButton->Connect("Clicked()","KQDistribution",this,"DrawHistogram(true)");
  fNextButton->Draw();
  
  fDrawCanvas = new TCanvas("fDrawCanvas","fDrawCanvas",10,10,800,400);
  fChi2PaveText = new TPaveText(0.6,0.2,1,0.35);
  fChi2PaveText->SetFillColor(0);
  fDrawCanvas->Draw();
  fDrawCanvas->cd(); 
}

void KQDistribution::SaveGraphData() {
	//creates ASCII files containing the Q values from the gaussian fits and ERecoil mean values of the TH1D Q-ERecoil histograms
	//these are the sourcefiles for subsequent graph creation
	cout << "Saving source data for graphs ... " << endl;
  fDoubleGaus = new TF1("fDoubleGaus",GammaFit,0.1,2,10);
  fDoubleGaus->SetParNames("C_{#gamma}","Q_{#gamma}","#sigma_{Q_{#gamma}}","C_{n}","Q_{n}","#sigma_{Q_{n}}","Q_{#gamma,high}","Q_{#gamma,low}","Q_{n,high}","Q_{n,low}");
  fGammaGaus = new TF1("fGammaGaus","gaus",fQMin,fQMax);
  fGammaGaus->SetParNames("C_{#gamma}","Q_{#gamma}","#sigma_{Q_{#gamma}}");
  fGammaGaus->SetLineColor(3);
  fNeutronGaus = new TF1("fNeutronGaus","gaus",fQMin,fQMax);
  fNeutronGaus->SetParNames("C_{n}","Q_{n}","#sigma_{Q_{n}}");
  fNeutronGaus->SetLineColor(4);
  fLinhardFunction = new TF1("fLinhardFunction","[0]*x^[1]+[2]",0,300);
  fLinhardFunction->SetParNames("#alpha","#beta","constant");
  fLinhardFunction->SetParameters(0.165,0.185,0);
  fConstantFunction = new TF1("constant","[0]",0,300);
  fConstantFunction->SetParNames("constant");
  
  ofstream QNeutronTheoRawFile(TString::Format("%s/%sQNeutronTheoValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
  ofstream QGammaTheoRawFile(TString::Format("%s/%sQGammaTheoValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
  ofstream QNeutronRawFile(TString::Format("%s/%sQNeutronValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
  ofstream QGammaRawFile(TString::Format("%s/%sQGammaValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data());
  QNeutronTheoRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" <<  setw(20) << "QNeutronTheo" << setw(20) << "ERecoilError" << setw(20) << "QNeutronTheoError" << endl;
  QGammaTheoRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGammaTheo" << setw(20) << "ERecoilError" << setw(20) << "QGammaTheoError" << endl;
  QNeutronRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QNeutron" << setw(20) << "ERecoilError" << setw(20) << "QNeutronError" << endl;
  QGammaRawFile << setw(5) << "#" << setw(20) << "ERecoilMean" << setw(20) << "QGamma" << setw(20) << "ERecoilError" << setw(20) << "QGammaError" << endl;
  
  //calculation of theoretical Q-bands TODO
  Double_t aChi2Gamma, aChi2Neutron;
  for(int k = 0; k<fNumProjections; ++k) {
		fHistogramRecords[k].fQGammaTheo = 1;
		fHistogramRecords[k].fQNeutronTheo = fLinhardFunction->Eval(fHistogramRecords[k].fERecoilMean);
		fHistogramRecords[k].fQGammaTheoError = 0.1;
		fHistogramRecords[k].fQNeutronTheoError = 0.1;
    fDrawCanvas->cd();
    fHistogramRecords[k].fHistogram->ProjectionY()->Draw();
    SetBoundaries(fHistogramRecords[k].fHistogram->ProjectionY(),2.5,fHistogramRecords[k].fQNeutronTheo,fHistogramRecords[k].fQGammaTheo,fHistogramRecords[k].fQNeutronTheoError/fConfidenceInSigmas,fHistogramRecords[k].fQGammaTheoError/fConfidenceInSigmas);
    TFitResultPtr aResultPtr = fHistogramRecords[k].fHistogram->ProjectionY()->Fit(fDoubleGaus,"LRSI","",fQMin,fQMax);
    fDrawCanvas->Update();
    fGammaGaus->SetParameter(0,fDoubleGaus->GetParameter(0));
    fGammaGaus->SetParameter(1,fDoubleGaus->GetParameter(1));
    fGammaGaus->SetParameter(2,fDoubleGaus->GetParameter(2));
    fNeutronGaus->SetParameter(0,fDoubleGaus->GetParameter(3));
    fNeutronGaus->SetParameter(1,fDoubleGaus->GetParameter(4));
    fNeutronGaus->SetParameter(2,fDoubleGaus->GetParameter(5));
    fGammaGaus->Draw("SAME");
    fNeutronGaus->Draw("SAME");
    fDrawCanvas->Update();
    fChi2PaveText->Clear();
    fChi2PaveText->SetX1NDC(0.6);
    fChi2PaveText->SetX2NDC(1);
    fChi2PaveText->SetY1NDC(0.2);
    fChi2PaveText->SetY2NDC(0.35); 
    aChi2Gamma = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6));
    Int_t anNDFGamma = GetNDF(fDoubleGaus->GetParameter(7),fDoubleGaus->GetParameter(6),3);
    aChi2Neutron = GetChi2(fHistogramRecords[fHistogramCounter].fHistogram->ProjectionY(),fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8));
    Int_t anNDFNeutron = GetNDF(fDoubleGaus->GetParameter(9),fDoubleGaus->GetParameter(8),3);
    fChi2PaveText->AddText(TString::Format("#chi_{g}^{2}/ndf_{g}     \t%f/%i",aChi2Gamma,anNDFGamma).Data());
    fChi2PaveText->AddText(TString::Format("#chi_{n}^{2}/ndf_{n}     %f/%i",aChi2Neutron,anNDFNeutron).Data());
    cout << "aChi2Gamma / anNDFGamma " << aChi2Gamma << "/" << anNDFGamma << " = " << aChi2Gamma/anNDFGamma << endl;
    cout << "aChi2Neutron / anNDFNeutron " << aChi2Neutron << "/" << anNDFNeutron << " = " << aChi2Neutron/anNDFNeutron << endl;
    fChi2PaveText->Draw("SAME");
    fDrawCanvas->Update();
    
    fHistogramRecords[k].fQGamma = aResultPtr->Value(1);
    fHistogramRecords[k].fQGammaError = fConfidenceInSigmas* abs(aResultPtr->Value(2));
    fHistogramRecords[k].fQNeutron = aResultPtr->Value(4);
    fHistogramRecords[k].fQNeutronError = fConfidenceInSigmas * abs(aResultPtr->Value(5));
    if(fHistogramRecords[k].fQNeutron<0.5&&fHistogramRecords[k].fQNeutron>0.1)
      QNeutronRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQNeutron << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQNeutronError << endl;
    if(fHistogramRecords[k].fQGamma<1.2 && fHistogramRecords[k].fQGamma > 0.8)
      QGammaRawFile << setw(5) << "" << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQGamma << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQGammaError << endl;
		QNeutronTheoRawFile << setw(5) << " " << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQNeutronTheo << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQNeutronTheoError << endl;
		QGammaTheoRawFile << setw(5) << " " << setw(20) << fHistogramRecords[k].fERecoilMean << setw(20) << fHistogramRecords[k].fQGammaTheo << setw(20) << fHistogramRecords[k].fERecoilError << setw(20) << fHistogramRecords[k].fQGammaTheoError << endl;
		cout << TString::Format("QGamma = %f +/- %f \t QNeutron = %f +/- %f",fHistogramRecords[k].fQGamma,fHistogramRecords[k].fQGammaError,fHistogramRecords[k].fQNeutron,fHistogramRecords[k].fQNeutronError).Data() << endl;
    fDrawCanvas->Print(TString::Format("%s%s%s_%s_%i.pdf",fTargetDir.c_str(),fTargetSubDir.c_str(),GetCategoryName(),fDetectorName.c_str(),k).Data());
  }
 
}

void KQDistribution::MakeGraphs() {
	// creates the graphs shown on the left pad of fMainCanvas, which contain single points for each TH1D histogram,
	// fQGammaErrorGraph with fQGamma +/- fQGammaError
	// fQGammaTheoGraph with fQGammaTheo +/- fQGammaTheoError
	// fQNeutronErrorGraph with fQNeutron +/ fQNeutronError
	// fQNeutronTheoGraph with fQNeutronTheo +/- fQNeutronTheoError
	cout << "Creating and drawing graphs ... " << endl;

  fTotalGraph = new TGraph(fDataSize,GetData(ERECOIL),GetData(Q));
  cout << "Graph with all data created ... " << endl;
  fTotalGraph->SetName("fTotalGraph");
  fTotalGraph->SetTitle(TString::Format("category: %s \n detector: %s events: %i",GetCategoryName(),fDetectorName.c_str(),fDataSize).Data());
  fTotalGraph->GetXaxis()->SetTitle("E_{recoil}[keV]");
  fTotalGraph->GetYaxis()->SetTitle("Q");
  fTotalGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fTotalGraph->GetYaxis()->SetRangeUser(fQMin,fQMax);
  cout << "attributes of graph with all data set" << endl;
  gDirectory = fOutputFile;
  fTotalGraph->Write();
  fQGammaErrorGraph = new TGraphErrors(TString::Format("%s/%sQGammaValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data(),"%lg %lg %lg %lg");
  cout << "fQGammaErrorGraph created" << endl;
  fQNeutronErrorGraph = new TGraphErrors(TString::Format("%s/%sQNeutronValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data(),"%lg %lg %lg %lg");
  cout << "fQNeutronErrorGraph created" << endl;
  fQGammaTheoGraph = new TGraphErrors(TString::Format("%s/%sQGammaTheoValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data(),"%lg %lg %lg %lg");
  cout << "fQGammaTheoGraph created" << endl;
  fQNeutronTheoGraph = new TGraphErrors(TString::Format("%s/%sQNeutronTheoValues.dat",fTargetDir.c_str(),fTargetSubDir.c_str()).Data(),"%lg %lg %lg %lg");
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
  fConstantFunction->SetLineColor(fQGammaErrorGraph->GetLineColor());
  fTotalMultiGraph->Add(fQGammaErrorGraph,"P");
  cout << "fQGammaErrorGraph added" << endl;
  
  fQNeutronErrorGraph->SetName("QNeutronData");
  fQNeutronErrorGraph->SetTitle("");
  fQNeutronErrorGraph->SetLineColor(0);
  fQNeutronErrorGraph->GetXaxis()->SetRangeUser(fERecoilMin,fERecoilMax);
  fLinhardFunction->SetLineColor(fQNeutronErrorGraph->GetLineColor());
  fLinhardFunction->SetParameters(0.1,0.2,0);
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
  fTotalMultiGraph->Draw("");
  fDrawCanvas->BuildLegend(0.1,0.67,0.48,0.88);
  TLegend* aLegend = (TLegend*)fDrawCanvas->FindObject("TPave");
  if(aLegend) aLegend->SetFillColor(0);
  TPaveText* aTitle = (TPaveText*)fDrawCanvas->FindObject("title");
  if(aTitle) aTitle->SetFillColor(0);
  fDrawCanvas->Update();
  fDrawCanvas->Print(TString::Format("%s/%s/%s_%s_total.pdf",fTargetDir.c_str(),fTargetSubDir.c_str(),GetCategoryName(),fDetectorName.c_str()).Data());
  
  
  
  
}



void KQDistribution::MakeAll(Int_t anEventCategory,const Char_t* aDetectorName,Int_t aNumProjections) {
	// main routine, which reads and evaluates fSourcefile.ROOT by calling the other routines
	cout << "Making all ... " << endl;
  fEventCategory = anEventCategory;
  fDetectorName = aDetectorName;
  fNumProjections = aNumProjections;
  
  gROOT->SetBatch(fIsBatch);
  SetStyle();
  //ResetVars();
  MakeDirectories();
  if(!ReadEvents())
    return;
  MakeCanvases();
  FillHistograms();
  SaveGraphData();
  MakeGraphs();
  //if(fIsBatch)
    //fAdjustPanel = new KQAdjustPanel(gClient->GetRoot(),200,200,CorrectBoundaries,StoreImage);
}





