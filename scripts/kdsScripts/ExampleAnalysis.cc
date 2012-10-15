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
			
			KHLABolometerRecord *mBolo = mEvent->GetBolo(bolo);
			
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

//example script using the KPTA library classes.
{
  KPulseAnalysisChain myChain;
  myChain.SetIsOwner(false);
  KBaselineRemoval *baseRemove = new KBaselineRemoval;
  myChain.AddProcessor(baseRemove);
  KPatternRemoval *patternRemove = new KPatternRemoval;
  myChain.AddProcessor(patternRemove);
  KRealToHalfComplexDFT *real2hcomp = new KRealToHalfComplexDFT;
  real2hcomp->SetFlags("EX");
  myChain.AddProcessor(real2hcomp);
  KHalfComplexPower *power = new KHalfComplexPower;
  myChain.AddProcessor(power);
  
  vector< vector<double> > myHeatPowerSpectrum;
  
  KEraRawEventReader myEraEventFile("je25b001_000.root");
  
  for(Int_t i = 0; i < myEraEventFile.GetEntries(); i++){
    myEraEventFile.GetEntry(i);
    
    EdwEvent *e = myEraEventFile.GetEvent();
    
    for(UInt_t p = 0; p < e->NbPulses(); p++){
      EdwPulse* pulse = e->Pulse(p);
      if(pulse->IsHeat()){
        vector<double> aPowerSpectrum;
        myChain.SetInputPulse(pulse->Trace());
        if(myChain.RunProcess()){ //RunProcess returns true if successful
          aPowerSpectrum = myChain.GetOutputPulse();
          myHeatPowerSpectrum.push_back(aPowerSpectrum);
        }
      }
    }
  }
  
  //now do something with your myHeatPowerSpectrum!
  
  //clean up
  delete baseRemove;
  delete patternRemove;
  delete real2hcomp;
  delete power;
}

{
  KDataReader f("Kds_Run12_v3.0_skim.root");  
  
  Bool_t desiredEvent = false;
  Int_t desiredBolo = 0;
  string bolo = "ID3";
  Double_t fMaxQ = 0.4;
  Double_t fMinQ = 0.1;
  Double_t fMaxE = 50.0;
  Double_t fMinE = 10.0;
  
  //KDataReader::GetEntry(0) is already called in the constructor.
  do {
    KHLAEvent *e = (KHLAEvent *)f.GetEvent();

    for(Int_t i = 0; i < e->GetNumBolos(); i++){
      KHLABolometerRecord *b = e->GetBolo(i);
      if(b->GetDetectorName() == bolo && b->GetQvalue() < fMaxQ && 
         b->GetQvalue() > fMinQ && b->GetEnergyRecoil() < fMaxE && 
         b->GetEnergyRecoil() > fMinE) {
        desiredEvent = true;  //we found an interesting event!
        desiredBolo = i;
        break;  //quit the loop
      }
    }
        
    if(!desiredEvent) f.GetNextBoloEntry();
  }while (!desiredEvent);

  KEraEventFinder myFinder("gadamc"); 
  
  EdwEvent *edwEvent = myFinder.TransferEvent(e->GetBolo(desiredBolo));
  myFinder.DisplayEvent();
  
}

