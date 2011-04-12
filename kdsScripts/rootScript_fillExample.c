/*
 *  rootScript_fillExample.c
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/9/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

//run this inside a rootcint by calling
//root[0] .x examples/rootScript_fillExample.c

{
	gSystem->Load("../lib/libKDS.so");  //load the KDataStructure library
	gSystem->AddIncludePath("-I$HOME/softwareDev/KDataStructure/src");
	gROOT->ProcessLine(".L fillExample.cc+"); //compile your code, which uses the KDataStructure class library
	fillEventExampleFunction("new.root","KT_log.txt"); //run the function that you just compiled.
	
	//note fillExample.cc or other files should not invoke a 'main' function
	
	//You may compile your own classes in this same way
}