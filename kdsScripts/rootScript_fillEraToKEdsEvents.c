/*
 *  rootScript_fillEraToKEdsEvents.c
 *  KDataStructure
 *
 *  Created by Adam Cox on 6/11/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


{
	string myKdataPath = "$KDATA_ROOT";
	string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/";
	TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/";
	string inPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/";
	//TString inPath = "/Users/adam/analysis/edelweiss/data/eraConcat/eraFullNeutron/";
	TString dataOutputPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";	
	//TString dataOutputPath = "/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
	//TString tstNewPath = "/Users/adam/analysis/edelweiss/data/tstnew/";
	string tstNewPath = "/kalinka/home/edelweiss/Bolo/Run12/tstnew/";
	
	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	
	
	/////////////////////////////////////
	
	TString kRoot=".root";
	TString kEds="Kds_";
	
	string outFile;
	string inPath;
	string logFile;
	
	TString command;
	TString theTime;
	TString myFileName;
	TString myFileContents;
	
	
	ofstream myFile;
	
	
	//for loop creating all Eds files from the dstTrees
	
	for(int i=0; i<kNumberBolos;i++){
	  
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
		
		logFile = dataOutputPath.Data();
	  logFile.append("eraToKEds_"); logFile.append(kDetectorNames[i]); logFile.append(".log");
		
	  outFile = dataOutputPath.Data();
	  outFile.append(kEds); outFile.append(kDetectorNames[i]); outFile.append(kRoot);	
		
		
		myFileName = kQSubScriptFileDir + "fillEraToKEds_Bckgd" + theTime.Data() + ".sh";
		cout << endl << "opending " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/fillEraToKEds ";
		myFileContents += inPath;
		myFileContents += kDetectorNames[i] + " " + outFile + " " + logFile + " " + tstNewPath; 
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
		
	
	  inPath.replace(inPath.find("Bckgd"), 5, "Neutron");
	  outFile.replace(outFile.find("Bckgd"),5, "Neutron");
	  logFile.replace(logFile.find("Bckgd"),5, "Neutron");
		
		myFileName = kQSubScriptFileDir + "fillEraToKEds_Neutron" + theTime.Data() + ".sh";
		cout << endl << "opending " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/fillEraToKEds ";
		myFileContents += inPath;
		myFileContents += kDetectorNames[i] + " " + outFile + " " + logFile + " " + tstNewPath; 
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
		
		
	  inPath.replace(inPath.find("Neutron"), 7, "Gamma");
	  outFile.replace(outFile.find("Neutron"),7, "Gamma");
	  logFile.replace(logFile.find("Neutron"),7, "Gamma");
	  myFileName = kQSubScriptFileDir + "fillEraToKEds_Gamma" + theTime.Data() + ".sh";
		cout << endl << "opending " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/fillEraToKEds ";
		myFileContents += inPath;
		myFileContents += kDetectorNames[i] + " " + outFile + " " + logFile + " " + tstNewPath; 
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
		
		
	}
 	
	//You may compile your own classes in this same way
}
