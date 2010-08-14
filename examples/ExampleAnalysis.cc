/*
 *  ExampleAnalysis.cc
 *  KDataStructure
 *
 *  Created by Adam Cox on 6/22/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


#include "KDataReader"
#include "TFile"
#include "KEvent"
#include "KHLAEvent"
#include "TH1D"

// myAnalysisFile.C

void myAnalysisScript(int arg1, const char* arg2)
{
	KDataReader fIn("/path/to/my/Eds_Run12_v1.0_0.root");  
	
	KHLAEvent *mEvent = (KHLAEvent *)fIn.GetEvent();	
	TFile fSaveOut("/path/to/my/results.root");
	
	TH1D myEnergyHist("ERecoil","Recoil Energy", 100, 0, 100);  //100 bins between 0 and 100 keV 

	for(Int_t entry = 0; entry < fIn.GetEntries(); entry++){
		fIn.GetEntry(entry);
		
		//analyze your event!
		for(Int_t bolo = 0; bolo < mEvent->GetNumBolos(); bolo++){
			
			KHLASingleBoloSubRecord *mBolo = mEvent->GetBolo(bolo);
			
			Double_t qVal = mBolo->GetQvalue();
			Double_t Erec = mBolo->GetEnergyRecoil();
			Double_t ECollectrode0 = mBolo->GetEnergyCollectrode(0);
			Double_t ECollectrode1 = mBolo->GetEnergyCollectrode(1);
			
			if(qVal > 0 && qVal < 0.5 && ECollectrod0 > 3.4)
				myEnergyHist->Fill(Erec);
		
		}
	}
	
	fSaveOut.cd();
	myEnergyHist.Write();
	fIn.Close();
	fSaveOut.Close();
}