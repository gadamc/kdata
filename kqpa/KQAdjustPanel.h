//_____________________________________________
//
// KQAdjustPanel.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This is a simple frame class which offers some buttons and edit fields to adjust the boundaries for the gaussian fits
// which give the estimations for mean and error values of the Q gamma and neutron band widths


#include <list>
#include <iostream>
#include <limits>
#include <iomanip>
#include <utility>
#include <string>
#include <fstream>
#include <cmath>
#include "Rtypes.h"
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
using namespace std;

#ifndef __KQADJUSTPANEL__
#define __KQADJUSTPANEL__

class IDList {
	private:
		Int_t nID; 
	
	public:
		IDList() : nID(0) {}
		~IDList() {}
		Int_t GetUnID(void) { return ++nID; }
};

class KQAdjustPanel : public TGMainFrame {
	private:
		TGVButtonGroup *fButtonGroup;
		TGNumberEntry* fStep;
		TGRadioButton *fBoundaryButtons[4];
		TGTextButton* fAdjust;
		TGTextButton* fSave;
		IDList IDs;
		void (*fCorrBound)(Int_t,Double_t);
		void (*fStoreImage)(Int_t);

	public:
		KQAdjustPanel(const TGWindow * p, UInt_t w, UInt_t h,void (*func)(Int_t,Double_t),void (*func2)(Int_t));
		virtual ~KQAdjustPanel();
		
		void DoExit(void);
		void SetGroupEnabled(Bool_t);
		void Adjust(void);
		void Save(Int_t anIndex);
		
		ClassDef(KQAdjustPanel,0);
};

#endif