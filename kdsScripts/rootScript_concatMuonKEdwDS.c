/*
 *  rootScript_fillExample.c
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/9/10. Modified by Benjamin Schmidt
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

//run this inside a rootcint by calling
//root[0] .x examples/rootScript_fillExample.c

{
	string myKdataPath = "$KDATA_ROOT";
	string myKdataPath = "$KDATA_ROOT";
	string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/";
	TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/";
	string path="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/";
	string outputFile = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/KdsRun12_MuonVetoAll.root";
	
	Int_t vetoRuns=14; //14 (File 66 is huge 40 000 000 events)
	
	string myNames[vetoRuns]=
	{"KdsRun12_MvRun54.root",
	"KdsRun12_MvRun55.root",
 	"KdsRun12_MvRun56.root",
  	"KdsRun12_MvRun57.root",
   	"KdsRun12_MvRun58.root",
    	"KdsRun12_MvRun59.root",
     	"KdsRun12_MvRun60.root",
     	"KdsRun12_MvRun61.root",
	"KdsRun12_MvRun62.root",
      	"KdsRun12_MvRun63.root",
      	"KdsRun12_MvRun64.root", // ends 13.02.2010 00:00:21
	"KdsRun12_MvRun65.root",
	"KdsRun12_MvRun66.root",
	"KdsRun12_MvRun67.root"}; 
	
	string myArray[vetoRuns];
	
	for(Int_t i=0; i<  vetoRuns; i++){
		myArray[i]=path+myNames[i];
	}
	
	TTimeStamp myTime;
	UInt_t myTimeDate = myTime.GetDate();
	UInt_t myTimeTime = myTime.GetTime();
	Int_t myTimeNano = myTime.GetNanoSec();
	
	TString theTime = ".";
	theTime += myTimeDate;
	theTime += + ".";
	theTime +=  myTimeTime;
	theTime += + ".";
	theTime += myTimeNano;
	
	TString myFileName  = kQSubScriptFileDir + "concatDst_Muon" + theTime.Data() + ".sh";
	cout << " opening " << myFileName.Data() << endl;
	ofstream myFile.open(myFileName.Data());
	
	TString myFileContents = "#!/bin/bash";
	cout << myFileContents.Data() << endl;
	myFile << myFileContents.Data() << endl;
	
	myFileContents = myKdataPath + "/bin/concatDst 5 " + outputFile;
	for(Int_t j = 0; j < vetoRuns; j++)
		myFileContents += " " + myArray[j];
			
	cout << myFileContents.Data() << endl;
	myFile << myFileContents.Data() << endl;
	myFileContents = "exit 0";
	cout << myFileContents.Data() << endl;
	myFile << myFileContents.Data() << endl;
	myFile.close();
	
	TString command = "chmod +x " + myFileName;
	cout << command << endl;
	gSystem->Exec(command.Data());
	
	command = "qsub -wd " + qsubWorkingDir + " -q all.q -V " + myFileName;
	cout << command << endl;
	
	gSystem->Exec(command.Data());
	
	concatKDS(myArray, vetoRuns, );
	
}
