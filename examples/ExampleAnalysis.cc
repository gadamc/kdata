/*
 *  ExampleAnalysis.cc
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 6/22/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


#include "EdwDSReader"
#include "TFile"
#include "EdwEventBase"
#include "EdwHLAEvent"
#include "TH1D"

int main(int argc, char* argv[])
{
	EdwDSReader fIn("/path/to/my/EdwDS_File.root");  
	//EdwDSReader will figure out if its a HLA file or Raw level file for you.
	
	EdwEventBase *mEvBase = fIn.GetEvent();	
	TFile fSaveOut("/path/to/my/results.root");
	
	TH1D myEnergyHist("ERecoil","Recoil Energy", 100, 0, 100);  //100 bins between 0 and 100 keV 
	if(EdwHLAEvent *mHlaEvent = dynamic_cast<EdwHLAEvent *> (mEvBase)){
		//dynamically cast the object to see if its an HLA Event
		for(Int_t entry = 0; entry < fIn.GetEntries(); entry++){
			fIn.GetEntry(entry);
			
			//analyze your event!
			for(Int_t bolo = 0; bolo < mHlaEvent->GetNumBolos(); bolo++){
				
				EdwHLASingleBoloSubRecord *mBolo = mHlaEvent->GetBolo(bolo);
				
				Double_t qVal = mBolo->GetQvalue();
				Double_t Erec = mBolo->GetEnergyRecoil();
				Double_t ECollectrode0 = mBolo->GetEnergyCollectrode(0);
				Double_t ECollectrode1 = mBolo->GetEnergyCollectrode(1);
				
				if(qVal > 0 && qVal < 0.5)
					myEnergyHist->Fill(Erec)
			}
		}
		else if(EdwRawEvent *mRawEvent = dynamic_cast<EdwRawEvent *> (mEvBase)){
			//do something else!
		}
	}
	
	fSaveOut.cd();
	myEnergyHist.Write();
	fIn.Close();
	fSaveOut.Close();
}