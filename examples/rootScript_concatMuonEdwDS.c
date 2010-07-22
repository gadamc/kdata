/*
 *  rootScript_fillExample.c
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 4/9/10. Modified by Benjamin Schmidt
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

//run this inside a rootcint by calling
//root[0] .x examples/rootScript_fillExample.c

{
	gSystem->Load("../lib/libEdwDS.so");  //load the EdwDataStructure library
	gSystem->AddIncludePath("-I/kalinka/home/schmidt-b/develSvn/src");//changed to absolute path, thus I can execute it from kalinka account
	gROOT->ProcessLine(".L concatDst.cc+"); //compile your code, which uses the EdwDataStructure class library

	
	Int_t vetoRuns=14; //14 but File 66 is huge 40 000 000 events
	string path="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/";
	string myNames[vetoRuns]=
	{"EdwDSRun12_MvRun54.root",
	"EdwDSRun12_MvRun55.root",
 	"EdwDSRun12_MvRun56.root",
  	"EdwDSRun12_MvRun57.root",
   	"EdwDSRun12_MvRun58.root",
    	"EdwDSRun12_MvRun59.root",
     	"EdwDSRun12_MvRun60.root",
     	"EdwDSRun12_MvRun61.root",
	"EdwDSRun12_MvRun62.root",
      	"EdwDSRun12_MvRun63.root",
      	"EdwDSRun12_MvRun64.root", // ends 13.02.2010 00:00:21
	"EdwDSRun12_MvRun65.root",
	"EdwDSRun12_MvRun66.root",
	"EdwDSRun12_MvRun67.root"}; 
	string myArray[vetoRuns];
	for(Int_t i=0; i<  vetoRuns; i++){
		myArray[i]=path+myNames[i];
	}
	
	concatEds(myArray, vetoRuns, "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/EdwDSRun12_MuonVetoAll.root");
	
}
