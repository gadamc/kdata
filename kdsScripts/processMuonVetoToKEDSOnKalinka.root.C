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
	string myKdataPath = "$KDATA_ROOT";
	string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/";
	TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/";
	TString fMuonVetoDataInPath = "/kalinka/home/edelweiss/Runs/";
	//TString fDataOutPath = "/kalinka/home/gadamc/data/muonVeto/eds/";
	TString fDataOutPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/";
	
	Int_t startMuonRun=54; 
	Int_t stopMuonRun=67; // should be 54 to 67 for Run 12
	TString fIn;
	TString fOut;
	
	for(Int_t i = startMuonRun ; i <= stopMuonRun; i++){
		
		TTimeStamp myTime;
		UInt_t myTimeDate = myTime.GetDate();
		UInt_t myTimeTime = myTime.GetTime();
		Int_t myTimeNano = myTime.GetNanoSec();
		theTime = ".";
		theTime += myTimeDate;
		theTime += + ".";
		theTime +=  myTimeTime;
		theTime += + ".";
		theTime += myTimeNano;

		
	  fIn = fMuonVetoDataInPath;
	  fIn += "Events_Run";
	  fIn += i;
	  fIn += "_Laser_time.root";
	  fOut = fDataOutPath;
	  fOut += "KdsRun12_MvRun";
	  fOut += i;
	  fOut += ".root"; 
		
		myFileName = kQSubScriptFileDir + "fillMuonVetoEvents" + theTime.Data();
		cout << endl << "opending " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		
		//the executable
		myFileContents = myKdataPath + "/bin/fillMuonVetoEvents ";
		
		//the arguments
		myFileContents += fIn;
		myFileContents += fOut;
		
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = "exit 0";
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFile.close();
		
		command = "chmod +x " + myFileName;
		cout << command << endl;
		gSystem->Exec(command.Data());
		
		command = "qsub -wd " + qsubWorkingDir + " -q all.q -V -b y " + myFileName;
		cout << command << endl;
		
		gSystem->Exec(command.Data());
		
	  //MuonVetoToDS(fIn.Data(), fOut.Data());
	}

		
}

