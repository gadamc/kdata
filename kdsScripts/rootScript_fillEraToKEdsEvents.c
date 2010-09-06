/*
 *  rootScript_fillEraToKEdsEvents.c
 *  KDataStructure
 *
 *  Created by Adam Cox on 6/11/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


{
	
	//TString devPath = "/kalinka/storage/edelweiss/EdwSoftware/Kdata";
	TString devPath = "$KDATA_ROOT";
	TString libPath = devPath + "/lib/libkds.so";
	TString includePath = "-I" + devPath + "/include";
	TString includePath2 = "-I" + devPath + "/era";
	TString processLine = ".L " + devPath + "/kdsScripts/fillEraToKEds.cc++"; 
	//TString dataInputPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/";
	TString dataInputPath = "/Users/adam/analysis/edelweiss/data/eraConcat/eraFullNeutron/";
	
	//TString dataOutputPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
	
	TString dataOutputPath = "/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
	TString tstNewPath = "/Users/adam/analysis/edelweiss/data/tstnew/";
	
	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	
	cout << includePath.Data() << endl;
	cout << libPath.Data() << endl;
	cout << processLine.Data() << endl;
	//cout << tstNewPath.Data() << endl;
	
	/////////////////////////////////////
	
	gSystem->AddIncludePath(includePath.Data());
	gSystem->AddIncludePath(includePath2.Data());

	gSystem->Load(libPath.Data());  //load the KDataStructure library
	//gSystem->AddIncludePath("-I/kalinka/home/gadamc/dev/KDataStructure");
	//gSystem->Load("/kalinka/home/gadamc/dev/KDataStructure/lib/libKDS.so");  //load the KDataStructure library
	//gSystem->AddIncludePath("-I/kalinka/home/gadamc/dev/KDataStructure/src");//changed to absolute path, thus I can execute it from kalinka account
	//gROOT->ProcessLine(".L mergeEdsTree.cc+"); //compile your code, which uses the KDataStructure class library
	gROOT->ProcessLine(processLine.Data());
	
	string kInputPath = dataInputPath.Data();
	string kOutputPath= dataOutputPath.Data();
	string kRoot=".root";
	string kEds="Kds_";
	
	//string ststnewpath = tstNewPath.Data();
	string inPath, outFile;
	
	//for loop creating all Eds files from the dstTrees

	for(int i=0; i<kNumberBolos;i++){
	  TString logFile = dataOutputPath;
	  logFile += "eraToKEds_";
	  logFile += kDetectorNames[i];
	  logFile += ".log";
	  string kLogFile= logFile.Data();
		
	  cout << logFile.Data() << endl;
	  inPath=kInputPath;
		
	  outFile=kOutputPath;
	  outFile.append(kEds); outFile.append(kDetectorNames[i]); outFile.append(kRoot);	
		
		
	  eraToKEds(inPath, kDetectorNames[i], outFile, kLogFile, tstNewPath.Data());
	  //inPath.replace(inPath.find("Bckgd"), 5, "Neutron");
	  //outFile.replace(outFile.find("Bckgd"),5, "Neutron");
	  //kLogFile.replace(kLogFile.find("Bckgd"),5, "Neutron");
	  //eraToKEds(inPath, kDetectorNames[i], outFile, kLogFile);
	  //inPath.replace(inPath.find("Neutron"), 7, "Gamma");
	  //outFile.replace(outFile.find("Neutron"),7, "Gamma");
	  //kLogFile.replace(kLogFile.find("Neutron"),7, "Gamma");
	  //eraToKEds(inPath, kDetectorNames[i], outFile, kLogFile);
	}
 	
	//You may compile your own classes in this same way
}
