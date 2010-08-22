/**
 @file    EdwPreprocessData.cc
 @author  Eric Armengaud, armengau@in2p3.fr
 @brief   Implementation of the EdwPreprocessData class. See the .h file
 */

#include "EdwPreprocessData.h"

ClassImp(EdwPreprocessData); /**< Root dictionnary */

EdwPreprocessData::EdwPreprocessData() : TObject() {
  fStartTime = 0;
  fEndTime = 0;
  fChannel = "";
  fNbTracesForNoiseSpectrum = 0;
  fCorrelSlope = 0;
  fCorrelFactor = 0;
  fPulseBin = 0;
}

EdwPreprocessData::~EdwPreprocessData() {
  //  delete fNoiseSpectrum;
}

EdwPreprocessData::EdwPreprocessData(time_t aStart, time_t aEnd, 
																		 string aChannel) : TObject() {
  fStartTime = aStart;
  fEndTime = aEnd;
  fChannel = aChannel;
  fNoiseSpectrum.clear();
  fNbPtsPatterns.clear(); fCorrelChannels.clear();
  fNbTracesForNoiseSpectrum = 0;
  fCorrelSlope = 0; fCorrelFactor = 0;
  fPulseBin = 0 ;
  
}

void EdwPreprocessData::Clear() {
  fNbPtsPatterns.clear();
  fNoiseSpectrum.clear();
  fCorrelChannels.clear();
  fStartTime = 0;
  fEndTime = 0;
  fChannel = "";
  fNbTracesForNoiseSpectrum = 0;
  fCorrelSlope = 0;
  fCorrelFactor = 0;
  fPulseBin = 0;
}

void EdwPreprocessData::DumpInfo() const {
  cout << "--------------------------"<<endl;
  cout << "Preprocess data for "<<fChannel<<endl;
  cout << "Validity from "<<fStartTime<<" to "<<fEndTime<<endl;
  cout << "Noise spectrum : "<<fNoiseSpectrum.size()<<" points ("<<fNbTracesForNoiseSpectrum<<" traces used)"<<endl;
  cout << "Pulse bin: "<<fPulseBin<<endl;
}

Bool_t EdwPreprocessData::IsTimeOk(time_t aTime) {
  Bool_t lCheck = 0;
  if ( aTime >= fStartTime && aTime <= fEndTime ) lCheck = 1;
  return lCheck;
}

void EdwPreprocessData::PlotSpectrum(string aPlotName, Bool_t aNoLog) {
  //  fNoiseSpectrum.Plot(aPlotName, !(aNoLog)) ;
  plotvector(fNoiseSpectrum, aPlotName, !(aNoLog), "Noise Spectrum");
}

void EdwPreprocessData::ComputeNoiseSpectrum(TChain* aChain) {
  // For the moment: using "basic" preprocessing
  // (no exotic depatterning or decorrelation!)
	
  fNoiseSpectrum.clear();
  fNbTracesForNoiseSpectrum = 0;
	
  Long64_t lFirst = GetFirstEventAfter(aChain,fStartTime);
  Long64_t lLast = GetLastEventBefore(aChain,fEndTime);
  if (lFirst == aChain->GetEntries() || lLast == aChain->GetEntries()) {
    cout << "ComputeNoiseSpectrum: Event times completely out of the expected interval - Exiting"<<endl;
    exit(-1);
  }
  
  FloatVect lNoiseSpectrum(0);
  EdwEvent* lEvt=new EdwEvent();
  EdwPulse* lEdwPulse = new EdwPulse();
  aChain->SetBranchAddress("Event",&lEvt);
  for (Long64_t ievt=lFirst; ievt<lLast; ievt++) {
    aChain->GetEntry((ULong_t)ievt);
    time_t lTime = lEvt->Header()->DateSec();
    lEdwPulse = lEvt->Pulse(fChannel);
    if (lTime >= fStartTime && lTime <= fEndTime && lEdwPulse != NULL) {
      FitPulse lPulse(lEdwPulse);
      lPulse.SetBaseFromPeakBin(fPulseBin) ; // hehe
      lPulse.FindPeaks(FP_STRICT,1); // standard parameters here - want to be sure there's no peak
      // Now with flag anysign = 1. It's more strict, so better, and no pbl with unknown pulse sign
      if (lPulse.GetPeakBins().size() == 0) {
				lPulse.ComputeTraceFFT();
				FloatVect lSpec = lPulse.ProcessedTraceFFT();
				lSpec.Multiply(lPulse.ProcessedTraceFFT());  //gets the Power spectrum
				fNbTracesForNoiseSpectrum+=1;
				if (lNoiseSpectrum.size() == 0) lNoiseSpectrum = lSpec;
				else lNoiseSpectrum.Add(lSpec);
      }
    }
    lEvt->Clear();
  }
	
  // Case of not enough noise spectrum events: Redo the loop with less strict criteria!
  if (fNbTracesForNoiseSpectrum < 10) {
    cerr << "ComputeNoiseSpectrum : New loop with loose FindPeaks cut" << endl;
    for (Long64_t ievt=lFirst; ievt<lLast; ievt++) {
      aChain->GetEntry((ULong_t)ievt);
      time_t lTime = lEvt->Header()->DateSec();
      lEdwPulse = lEvt->Pulse(fChannel);
      if (lTime >= fStartTime && lTime <= fEndTime && lEdwPulse != NULL) {
				FitPulse lPulse(lEdwPulse);
				lPulse.SetBaseFromPeakBin(fPulseBin) ;
				lPulse.FindPeaks(FP_WEAK,0); // Less strict cut!
				if (lPulse.GetPeakBins().size() == 0) {
					lPulse.ComputeTraceFFT();
					FloatVect lSpec = lPulse.ProcessedTraceFFT();
					lSpec.Multiply(lPulse.ProcessedTraceFFT());
					fNbTracesForNoiseSpectrum+=1;
					if (lNoiseSpectrum.size() == 0) lNoiseSpectrum = lSpec;
					else lNoiseSpectrum.Add(lSpec);
				}
      }
      lEvt->Clear();
    }
  }
	
  lNoiseSpectrum.Scale(((Float_t)1)/(Float_t)fNbTracesForNoiseSpectrum);
  fNoiseSpectrum = lNoiseSpectrum.Vect();
  if (KVERBOSE) cout << "ComputeNoiseSpectrum: "<<fNbTracesForNoiseSpectrum<<" traces used"<<endl;
  
}

/*

void EdwPreprocessData::BasicChainPreprocess(TChain* aChain) {
	
	EdwEvent* lEvt=new EdwEvent();
	EdwPulse* lEdwPulse = new EdwPulse();
	aChain->SetBranchAddress("Event",&lEvt);
	UInt_t lNbChannels = fChannels.size();
	
	// Parameters for peak finding
	Int_t lLengthPeak = 20;
	Float_t lSigPeakIon = 7;
	Float_t lSigPeakHeat = 4;
	
	// current pattern substraction reduced to a minimum
	for (UInt_t j=0; j<lNbChannels; j++) {
		string lChannel = fChannels[j];
		vector<Int_t> lNbPtsPattern(0);
		if ( ! isheat(lChannel) ) lNbPtsPattern.push_back(100);
		fNbPtsPatterns.push_back(lNbPtsPattern);
		fFFTs.push_back(NULL);
	}
	
	fNbTracesFFT.clear();
	fNbTracesFFT.resize(lNbChannels,0);
	for (UInt_t ievt=0; ievt<aChain->GetEntries(); ievt++) { // nevts
		aChain->GetEntry(ievt);
		time_t lTime = lEvt->Header()->DateSec();
		if (lTime >= fStartTime && lTime <= fEndTime) {
			for (UInt_t j=0; j<lNbChannels; j++) {
				string lChannel = fChannels[j];
				bool lIsHeat = isheat(lChannel);
				lEdwPulse = lEvt->Pulse(lChannel);
				if (lEdwPulse != NULL ) {
					TFitPulse fitpulse(lEdwPulse);
					// baseline removal
					fitpulse.RemoveBaseline();
					// pattern substraction
					for (UInt_t k=0; k<(fNbPtsPatterns[j]).size(); k++)
						fitpulse.RemovePattern(fNbPtsPatterns[j][k]);
					// check if there are some pulses in the trace
					Float_t lSigPeak = lSigPeakIon;
					if (lIsHeat) lSigPeak = lSigPeakHeat;
					vector<Int_t> pts = fitpulse.FindPeaks(lSigPeak,lLengthPeak);
					if (pts.size() == 0) {
						// FFT estimation
						TH1F* lafft = (TH1F*)(fitpulse.ProcessedTrace())->FFT(NULL,"MAG R2C ES"); // a optimiser
						(fNbTracesFFT[j])+=1;
						if (fFFTs[j] == NULL) {
							delete fFFTs[j];
							fFFTs[j] = new TH1F(*lafft);
						} else {
							(fFFTs[j])->Add(lafft);
						}
					}
				}
			}
		}
		lEvt->Clear();
	}
	
	TF1* lFunc = new TF1("dummy","1",-10,1000000);
	for (UInt_t j=0; j<lNbChannels; j++) (fFFTs[j])->Divide(lFunc,(Double_t)(fNbTracesFFT[j]));
	
	delete lFunc;
	delete lEdwPulse;
	delete lEvt;
	
}

void EdwPreprocessData::ComputeBasicCorrelation(TChain* aChain) {
	
	EdwEvent* lEvt=new EdwEvent();
	EdwPulse* lEdwPulse = new EdwPulse();
	aChain->SetBranchAddress("Event",&lEvt);
	UInt_t lNbChannels = fChannels.size();
	
	// Parameters for peak finding
	Int_t lLengthPeak = 20;
	Float_t lSigPeakIon = 7;
	Float_t lSigPeakHeat = 4;
	// Decorrelating above 3 channels...
	UInt_t lNbChannelsMin = 3;
	
	vector< vector<Float_t> > lCorrelSlopes, lCorrelFactors;
	vector<Float_t> lDum(0);
	for (UInt_t j=0; j<lNbChannels; j++) {
		lCorrelSlopes.push_back(lDum);
		lCorrelFactors.push_back(lDum);
	}
	
	// Main loop on events (correlations are made event by event and then we take the average)
	for (UInt_t ievt=0; ievt<aChain->GetEntries(); ievt++) {
		aChain->GetEntry(ievt);
		time_t lTime = lEvt->Header()->DateSec();
		if (lTime >= fStartTime && lTime <= fEndTime) {
			vector<Int_t> lListIonChannelsOK;
			vector<TH1F*> lListIonTracesOK;
			vector<Int_t> lListHeatChannelsOK;
			vector<TH1F*> lListHeatTracesOK;
			// Loop on channels to see if they are ok and get their processed traces
			for (UInt_t j=0; j<lNbChannels; j++) {
				string lChannel = fChannels[j];
				bool lIsHeat = isheat(lChannel);
				lEdwPulse = lEvt->Pulse(lChannel);
				if (lEdwPulse != NULL ) {
					TFitPulse fitpulse(lEdwPulse);
					fitpulse.RemoveBaseline();
					for (UInt_t k=0; k<(fNbPtsPatterns[j]).size(); k++)
						fitpulse.RemovePattern(fNbPtsPatterns[j][k]);
					Float_t lSigPeak = lSigPeakIon;
					if (lIsHeat) lSigPeak = lSigPeakHeat;
					vector<Int_t> pts = fitpulse.FindPeaks(lSigPeak,lLengthPeak);
					if (pts.size() == 0) { // This trace is ok for decorrelation
						if (lIsHeat) {
							lListHeatChannelsOK.push_back(j);
							lListHeatTracesOK.push_back(fitpulse.ProcessedTrace());
						} else {
							lListIonChannelsOK.push_back(j);
							lListIonTracesOK.push_back(fitpulse.ProcessedTrace());
						}
					}
				}
			}
			// Correlation analysis on good channels
			UInt_t lNbIons=lListIonChannelsOK.size();
			UInt_t lNbHeat=lListHeatChannelsOK.size();
			TF1* lFunc = new TF1("dummy","1",-10,1000000);
			if ( lNbIons >= lNbChannelsMin) {
				TH1F* lMeanTrace = new TH1F(*(lListIonTracesOK[0]));
				for (UInt_t j=1; j<lNbIons; j++) lMeanTrace->Add(lListIonTracesOK[j]);
				lMeanTrace->Divide(lFunc,(Float_t)lNbIons);
				for (UInt_t j=1; j<lNbIons; j++) {
					TGraph* lGraph = new TGraph(lMeanTrace->GetEntries(),lMeanTrace->GetArray(),(lListIonTracesOK[j])->GetArray());
					TF1* lFitFunc = new TF1("fitfunc","++x",-100000,100000);
					lGraph->Fit(lFitFunc,"WQNC");
					(lCorrelSlopes[lListIonChannelsOK[j]]).push_back(lFitFunc->GetParameter(0));
					(lCorrelFactors[lListIonChannelsOK[j]]).push_back(lGraph->GetCorrelationFactor());
					delete lGraph;
					delete lFitFunc;
				}
				delete lMeanTrace;
			}
			if ( lNbHeat >= lNbChannelsMin) {
				TH1F* lMeanTrace = new TH1F(*(lListHeatTracesOK[0]));
				for (UInt_t j=1; j<lNbHeat; j++) lMeanTrace->Add(lListHeatTracesOK[j]);
				lMeanTrace->Divide(lFunc,(Float_t)lNbHeat);
				for (UInt_t j=1; j<lNbHeat; j++) {
					TGraph* lGraph = new TGraph(lMeanTrace->GetEntries(),lMeanTrace->GetArray(),(lListHeatTracesOK[j])->GetArray());
					TF1* lFitFunc = new TF1("fitfunc","++x",-100000,100000);
					lGraph->Fit(lFitFunc,"WQNC");
					(lCorrelSlopes[lListHeatChannelsOK[j]]).push_back(lFitFunc->GetParameter(0));
					(lCorrelFactors[lListHeatChannelsOK[j]]).push_back(lGraph->GetCorrelationFactor());
					delete lGraph;
					delete lFitFunc;
				}
				delete lMeanTrace;
			}
			delete lFunc;
			for (UInt_t l=0; l<lListIonTracesOK.size(); l++) delete lListIonTracesOK[l];
			for (UInt_t l=0; l<lListHeatTracesOK.size(); l++) delete lListHeatTracesOK[l];
		}
	}
	
	fCorrelSlope.clear();
	fCorrelFactor.clear();
	for (UInt_t j=0; j<lNbChannels; j++) {
		fCorrelSlope.push_back(vectmedian(lCorrelSlopes[j]));
		fCorrelFactor.push_back(vectmedian(lCorrelFactors[j]));
	}
	
	delete lEvt;
	delete lEdwPulse;
}
*/

