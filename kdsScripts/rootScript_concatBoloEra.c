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

	gSystem->Load("$KDATA_ROOT/lib/libkds.so");  //load the KDataStructure library
	gSystem->AddIncludePath("-I$KDATA_ROOT/include");//changed to absolute path, thus I can execute it from kalinka account
	gSystem->AddIncludePath("-I$KDATA_ROOT/era");//changed to absolute path, thus I can execute it from kalinka account
	gROOT->ProcessLine(".L $KDATA_ROOT/kdsScripts/concatDst.cc+"); //compile your code, which uses the KDataStructure class library
	//gROOT->ProcessLine(".L filldstEvents.cc+");

	
	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
		{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		 "ID403", "ID404", "ID405", "GGA4"};
	//in Principal it should not matter in what way you merge them, but I will start with merging files from the same Mac since I already tested that.
	//set your data paths here. 
	//string kInputPath1="/Users/adam/analysis/edelweiss/data/eraLinks/eraRun12Gamma/";
	//string kInputPath2="/Users/adam/analysis/edelweiss/data/eraLinks/eraRun12BisGamma/";
	//string kOutputPath="/Users/adam/analysis/edelweiss/data/eraConcat/eraFullGamma/";
	
	string kInputPath1="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/EricRun12FirstPart/";
	string kInputPath2="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/EricRun12SecondPart/";
	string kOutputPath="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/";
	
	string kRoot=".root";
	string kdst="dst_";
	string keheat = "eheat_";
	string keion = "eion_";
	string kcuts = "cuts_";

	string in1, in2, out;

	
	//for loop creating all Eds files from the dstTrees
	for(int i=0; i<kNumberBolos;i++){
		in1=kInputPath1;
		in1.append(kdst); in1.append(kDetectorNames[i]); in1.append(kRoot);
		in2=kInputPath2;
		in2.append(kdst); in2.append(kDetectorNames[i]); in2.append(kRoot);
		out=kOutputPath;
		out.append(kdst); out.append(kDetectorNames[i]); out.append(kRoot);
		concatDst(in1, in2, out);
		//in1.replace(in1.find("Bckgd"), 5, "Neutron");
		//in2.replace(in2.find("Bckgd"), 5, "Neutron");
		//out.replace(out.find("Bckgd"), 5, "Neutron");
		//concatDst(in1, in2, out);
		//in1.replace(in1.find("Neutron"), 7, "Gamma");
		//in2.replace(in2.find("Neutron"), 7, "Gamma");
		//out.replace(out.find("Neutron"), 7, "Gamma");
		//concatDst(in1, in2, out);
	} 
	


	//for loop creating all Eds files from the dstTrees
	for(int i=0; i<kNumberBolos;i++){
		in1=kInputPath1;
		in1.append(keion); in1.append(kDetectorNames[i]); in1.append(kRoot);
		in2=kInputPath2;
		in2.append(keion); in2.append(kDetectorNames[i]); in2.append(kRoot);
		out=kOutputPath;
		out.append(keion); out.append(kDetectorNames[i]); out.append(kRoot);
		concatEion(in1, in2, out);
		//in1.replace(in1.find("Bckgd"), 5, "Neutron");
		//in2.replace(in2.find("Bckgd"), 5, "Neutron");
		//out.replace(out.find("Bckgd"), 5, "Neutron");
		//concatEion(in1, in2, out);
		//in1.replace(in1.find("Neutron"), 7, "Gamma");
		//in2.replace(in2.find("Neutron"), 7, "Gamma");
		//out.replace(out.find("Neutron"), 7, "Gamma");
		//concatEion(in1, in2, out);
	} 
	


	//for loop creating all Eds files from the dstTrees
	for(int i=0; i<kNumberBolos;i++){
		in1=kInputPath1;
		in1.append(keheat); in1.append(kDetectorNames[i]); in1.append(kRoot);
		in2=kInputPath2;
		in2.append(keheat); in2.append(kDetectorNames[i]); in2.append(kRoot);
		out=kOutputPath;
		out.append(keheat); out.append(kDetectorNames[i]); out.append(kRoot);
		concatEheat(in1, in2, out);
		//in1.replace(in1.find("Bckgd"), 5, "Neutron");
		//in2.replace(in2.find("Bckgd"), 5, "Neutron");
		//out.replace(out.find("Bckgd"), 5, "Neutron");
		//concatEheat(in1, in2, out);
		//in1.replace(in1.find("Neutron"), 7, "Gamma");
		//in2.replace(in2.find("Neutron"), 7, "Gamma");
		//out.replace(out.find("Neutron"), 7, "Gamma");
		//concatEheat(in1, in2, out);
	} 
	


	//for loop creating all Eds files from the dstTrees
	for(int i=0; i<kNumberBolos;i++){
		in1=kInputPath1;
		in1.append(kcuts); in1.append(kDetectorNames[i]); in1.append(kRoot);
		in2=kInputPath2;
		in2.append(kcuts); in2.append(kDetectorNames[i]); in2.append(kRoot);
		out=kOutputPath;
		out.append(kcuts); out.append(kDetectorNames[i]); out.append(kRoot);
		concatCuts(in1, in2, out);
		//in1.replace(in1.find("Bckgd"), 5, "Neutron");
		//in2.replace(in2.find("Bckgd"), 5, "Neutron");
		//out.replace(out.find("Bckgd"), 5, "Neutron");
		//concatCuts(in1, in2, out);
		//in1.replace(in1.find("Neutron"), 7, "Gamma");
		//in2.replace(in2.find("Neutron"), 7, "Gamma");
		//out.replace(out.find("Neutron"), 7, "Gamma");
		//concatCuts(in1, in2, out);
	} 
	
	
	
	
	//You may compile your own classes in this same way
}
