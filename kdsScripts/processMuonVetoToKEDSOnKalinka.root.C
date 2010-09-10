/*
 *  processMuonVetoToEDSOnKalinka.root.C
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/9/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

{
	//gSystem->Load("$HOME/dev/KDataStructure/lib/libKDS.so");  //load the KDataStructure library
	gSystem->Load("$KDATA_ROOT/lib/libkds.so");
	gSystem->AddIncludePath("-I$KDATA_ROOT/include");
	gROOT->ProcessLine(".L $KDATA_ROOT/kdsScripts/fillMuonVetoEvents.cc+"); //compile your code, which uses the KDataStructure class library
	RedirectHandle_t *h = new RedirectHandle_t;
	//TString outputLog = "/kalinka/home/gadamc/data/muonVeto/eds/MuonVetoToEDS_log.txt";
	TString outputLog = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/MuonVetoToEDS_log.txt";
	cout << " The Script is running... see the output file:" << endl;
	cout << outputLog.Data() << endl;
	gSystem->RedirectOutput(outputLog.Data(),"w",h);
	gSystem->ShowOutput(h)
	TString fMuonVetoDataInPath = "/kalinka/home/edelweiss/Runs/";
	//TString fDataOutPath = "/kalinka/home/gadamc/data/muonVeto/eds/";
	TString fDataOutPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/";
	Int_t startMuonRun=54; 
	Int_t stopMuonRun=67; // should be 67
	TString fIn;
	TString fOut;
		
	
	for(Int_t i = startMuonRun ; i <= stopMuonRun; i++){
	  fIn = fMuonVetoDataInPath;
	  fIn += "Events_Run";
	  fIn += i;
	  fIn += "_Laser_time.root";
	  fOut = fDataOutPath;
	  fOut += "KdsRun12_MvRun";
	  fOut += i;
	  fOut += ".root"; 
	  MuonVetoToDS(fIn.Data(), fOut.Data());
	  cout << "Finished MuonVetoToDS()" << endl;
	}
	cout << "closing programm";
	gROOT->Reset();
}

