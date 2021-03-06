//_____________________________________________
//
// KQDistribution.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//



#include <list>
#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>
#include <utility>
#include <algorithm>
#include <string>
#include <fstream>
#include <cmath>
#include "Rtypes.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TBranch.h"
#include "TDirectory.h"
#include "TRandom.h"
#include "TH2D.h"
#include "TSystem.h"
#include "TPad.h"
#include "TButton.h"
#include "TString.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"
#include "TMath.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TPaveText.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TGClient.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButtonGroup.h"
#include "TGNumberEntry.h"
#include "KHLABolometerRecord.h"
#include "KDataReader.h"
#include "KHLAEvent.h"
#include "KQAdjustPanel.h"
#include "KLindhard.h"
#include "KQUncertainty.h"
#include "KBoloConfig.h"
using namespace std;

#ifndef __KQDISTRIBUTION__
#define __KQDISTRIBUTION__

Double_t GammaFit(Double_t* x,Double_t* par);

class KQDistribution {
	private:
    
    string fBoloConfigFile; //source ASCII file for heat and ion channel uncertainties for all bolometers
		string fSourceFile; // source KDATA tree file
		string fTargetDir; // target directory for created files
		string fDetectorName; // temporary detector
		Int_t fEventCategory; // event category in KHLABolometerRecord
		Double_t fConfidenceInSigmas; // confidence level in units of sigma
		
		Int_t fNumBinsERecoil; // number of bins on E_Recoil-axis in TH2D histograms
		Int_t fNumBinsQ; // number of bins on Q-axis in TH2D histograms
		Double_t fERecoilMax; // maximal value of E_Recoil
		Double_t fERecoilMin; // minimal value of E_Recoil
		Double_t fQMax; // maximal value of Q
		Double_t fQMin; // minimal value of Q
		Int_t fNumProjections; // number of TH2D histograms
		bool fIsBatch; // batch-mode
		
		
		KDataReader* fKDataReader; // kdata reader
		KHLAEvent* fKHLAEvent;  //used to read KHLABolometerRecord events
		KBoloConfig* fBoloConfig; //bolometer configuration
		
		TGraph* fTotalGraph; // showing Q-ERecoil plot for all KHLABolometerRecord events
		TCanvas* fDrawCanvas; // seperate canvas to draw and save to files
		TPad* pad3;
		TButton* fPreviousButton; // switch to previous Q-projection histogram
		TButton* fNextButton; // switch to next Q-projection histogram
		TFile* fOutputFile; // stores all fHistograms and fTotalGraph
		TFile* fInputFile; // source file with KHLABolometerRecord events
		string fImageFormat; //format for the images to be created
		//TH2D** fHistograms; // Q-ERecoil pairs each for adjacent ERecoil intervals
		Int_t fHistogramCounter; // index for current histogram
		Double_t* fERecoilMean; // mean estimations for the single ERecoil intervals
		Double_t* fERecoilError; // RMS estimations for the single ERecoil intervals


		TF1* fDoubleGaus; // sum of two gaussians functions
		TF1* fGammaGaus; // function fitting gamma peak for a single element of fHistograms
		TF1* fNeutronGaus; // function fitting neutron peak for a single element of fHistograms

		struct DataRecord {
			//contains all required information for one KBolometerEvent
			Double_t fERecoil;
      Double_t fEIon;
      Double_t fEHeat;
			Double_t fQ;
      Double_t fSigmaIonZero;
      Double_t fSigmaHeatZero;
			DataRecord(Double_t anERecoil,Double_t aQ,Double_t anEIon = 0, Double_t anEHeat = 0,
                 Double_t aSigmaIonZero = 0,Double_t aSigmaHeatZero = 0)
                 : fERecoil(anERecoil),fEIon(anEIon),fEHeat(anEHeat), fQ(aQ),fSigmaIonZero(aSigmaIonZero),fSigmaHeatZero(aSigmaHeatZero) { }
			//bool operator<(DataRecord& anotherDataRecord) { return(this->fERecoil<anotherDataRecord.fERecoil); }
		};
		
    struct HistogramRecord {
      //contains all information concerning one histogram
      TH2D* fHistogram;
      Double_t fERecoilMean;
      Double_t fERecoilError;
      Double_t fQNeutron;
      Double_t fQNeutronError;
      Double_t fQGamma;
      Double_t fQGammaError;
      Double_t fQNeutronTheo;
      Double_t fQNeutronTheoError;
      Double_t fQGammaTheo;
      Double_t fQGammaTheoError;
      HistogramRecord() { }
    };
    
    vector<DataRecord> fData; //stores ERecoil-Q pairs
    Int_t fDataSize; // size of fData
    
    vector<HistogramRecord> fHistogramRecords;
     
    Double_t* fQNeutron;
    Double_t* fQNeutronError;
    Double_t* fQGamma;
    Double_t* fQGammaError;
    Double_t* fQNeutronTheo;
    Double_t* fQNeutronTheoError;
    Double_t* fQGammaTheo;
    Double_t* fQGammaTheoError;
    TGraphErrors* fQGammaErrorGraph;
    TGraphErrors* fQNeutronErrorGraph;
    TGraphErrors* fQGammaTheoGraph;
    TGraphErrors* fQNeutronTheoGraph;
    TMultiGraph* fTotalMultiGraph;
    TPaveStats* fChi2PaveText;
    
    //private methods 
    void SetStyle();
    void ResetVars();
    void MakeTargetDir();
    bool ReadEvents();
    void MakeCanvases();
    void FillHistograms();
    void SaveGraphData();
    void MakeGraphs();
    void CalculateTheoErrors(); //calculates theoretical gamma and nuclear recoil bands for the histograms
    void Next(); //signal for "Next" button
    void Previous(); //signal for "Previous" button


  public:

    KQDistribution(const Char_t* aSourceFile,const Char_t* aTargetDir,Int_t aNumProjections);
    virtual ~KQDistribution(void){}

    //getters
    const Char_t* GetBoloConfigFile() const  { return fBoloConfigFile.c_str(); }
    KBoloConfig* GetBoloConfig() const { return fBoloConfig; }
    const Char_t* GetSourceFile() const { return fSourceFile.c_str(); }
    const Char_t* GetTargetDir() const { return fTargetDir.c_str(); }
    const Char_t* GetDetectorName() const { return fDetectorName.c_str();  }
    Double_t GetConfidenceInSigmas() const  { return fConfidenceInSigmas; }
    Int_t GetNumBinsERecoil() const { return fNumBinsERecoil; }
    Int_t GetNumBinsQ() const  { return fNumBinsQ; }
    Double_t GetERecoilMax() const { return fERecoilMax; }
    Double_t GetERecoilMin() const { return fERecoilMin; }
    Double_t GetQMax() const { return fQMax; }
    Double_t GetQMin() const { return fQMin; }
    Int_t GetNumProjections() const { return fNumProjections; }
    Bool_t GetIsBatch() const { return fIsBatch; }
    const Char_t* GetImageFormat() const { return fImageFormat.c_str(); }
    TCanvas* GetDrawCanvas() const { return fDrawCanvas; }
    
    Int_t GetDataSize() const { return fDataSize; }
    const Char_t* GetCategoryName(Int_t anEventCategory);
    TH1D* GetHistogram(Int_t anIndex);
    
    
    
    enum DataType { ERECOIL , Q };
    Double_t* GetData(DataType aType);
    void SortData();
    
    //setters
    void SetBoloConfigFile(const Char_t* aBoloConfigFile) { fBoloConfigFile = aBoloConfigFile; }
    void SetSourceFile(const Char_t* aSourceFile) { fSourceFile = aSourceFile; }
    void SetTargetDir(const Char_t* aTargetDir) { fTargetDir = aTargetDir; }
    void SetDetectorName(const Char_t* aDetectorName) { fDetectorName = aDetectorName; }
    void SetConfidenceInSigmas(Double_t aConfidenceInSigmas) { fConfidenceInSigmas = aConfidenceInSigmas; }
    void SetNumBinsERecoil(Int_t aNumBinsERecoil) { fNumBinsERecoil = aNumBinsERecoil; }
    void SetNumBinsQ(Int_t aNumBinsQ) { fNumBinsQ = aNumBinsQ; }
    void SetERecoilMax(Double_t anERecoilMax) { fERecoilMax = anERecoilMax; }
    void SetERecoilMin(Double_t anERecoilMin) { fERecoilMin = anERecoilMin; }
    void SetQMax(Double_t aQMax) { fQMax = aQMax; }
    void SetQMin(Double_t aQMin) { fQMin = aQMin; }
    void SetNumProjections(Int_t aNumProjections) { fNumProjections = aNumProjections; }
    void SetIsBatch(Bool_t anIsBatch) { fIsBatch = anIsBatch; }
    void SetImageFormat(const Char_t* anImageFormat) { fImageFormat = anImageFormat; }
    
    void SetPaths(const Char_t* aSourceFile,const Char_t* aTargetDir);
    void SetConfidenceLevel(Double_t aQuantil = 0.9);
    Double_t GetConfidenceLevel();
    
    void SaveImage(Int_t anIndex);
    void SaveImage(const Char_t* aFileName);
    void DrawHistogram(TPad* aPad,Int_t aHistogramCounter);
    void SetBoundaries(TH1D* aHistogram,Double_t aNumSigmas = 2.5,
                       Double_t aFirstValue = 0.01,
                       Double_t aSecondValue = 0.01,
                       Double_t aFirstError = 0.01,
                       Double_t aSecondError = 0.01);
    Int_t QtoBin(Double_t aQ);
    Double_t BintoQ(Int_t aBin);
    //Double_t GetChi2(TH1D* aHistogram,Double_t aQlow,Double_t aQhigh);
    //Int_t GetNDF(Double_t aQlow,Double_t aQhigh,Int_t aNumParameters);
    
    enum BoundaryType { GAMMALOW, GAMMAHIGH, NEUTRONLOW, NEUTRONHIGH  };
    //void CorrectBoundaries(BoundaryType aBType,Double_t aStep);
    //void RedrawPeaks();
    void GetFitParameters(Int_t anIndex);
    Double_t GetGammaMax(TH1D* aHistogram);
    

    void MakeAll(Int_t anEventCategory,const Char_t* aDetectorName,Int_t aNumProjections);
    friend bool operator<(const KQDistribution::DataRecord& aFirstDataRecord,const KQDistribution::DataRecord& aSecondDataRecord);
    
    
    
  ClassDef(KQDistribution,0);
};

bool operator<(const KQDistribution::DataRecord& aFirstDataRecord,const KQDistribution::DataRecord& aSecondDataRecord);



#endif

