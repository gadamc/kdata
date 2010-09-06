/*
 *  rootScript_fillExample.c
 *  KDataStructure
 *
 *  Created by Benjamin Schmidt 28/4/10
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 *  Modified by Adam Cox 1/9/10
 */

//run this inside a rootcint by calling
//root[0] .x examples/rootScript_fillExample.c

{
	TString devPath = "$KDATA_ROOT";
	//TString devPath = "/kalinka/home/edelweiss/EdwSoftware/Kdata";
	
	//string kInputPath1="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
	string kInputPath1="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
	//string kOutputPath="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
	string kOutputPath="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/";
	//string kLogFile="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Merge.log";
	string kLogFile="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/Merge.log";

	TString includePath = "-I" + devPath + "/include/";
	gSystem->AddIncludePath(includePath.Data());
	includePath = "-I" + devPath + "/era/";
	gSystem->AddIncludePath(includePath.Data());
	
	TString kdsLibrary = devPath + "/lib/libkds.so";
	gSystem->Load(kdsLibrary.Data()); 
	TString mergeScript = ".L " + devPath + "/kdsScripts/mergeKEdsTree.cc++"; 
	gROOT->ProcessLine(mergeScript.Data());

	//defining input and outputpath, bolonames:
	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
	"ID3", "ID4", "ID5", 
	"ID6", "ID401", "ID402",
	"ID403", "ID404", "ID405", "GGA4"};
	

	string kRoot=".root";
	string kdst="dst_";
	string kEds="Kds_";
	Int_t kEntryOfBoloStart=272395; //From this event on the timelines match and merging begins, before this event we just fill in the uVeto data
	Int_t badMuonEntry[2]={396278 ,400838};//first bad entry, last bad entry, period of time resets in uVeto
	string in1, in2, out, log;

	//merging of the bolometers
	//merging step1
	
	for(Int_t i=0; i<kNumberBolos-1;i+=2){ // kNumberBolos
		TString logFile = kOutputPath;
		logFile += kDetectorNames[i];
	  	logFile += kDetectorNames[i+1]; 
	  	logFile += ".log";		
	  	log= logFile.Data();
		in1=kInputPath1;
		in1.append(kEds); in1.append(kDetectorNames[i]); in1.append(kRoot);
		in2=kInputPath1;
		in2.append(kEds); in2.append(kDetectorNames[i+1]); in2.append(kRoot);
		out=kOutputPath;
		out.append(kEds); out.append(kDetectorNames[i]);out.append(kDetectorNames[i+1]); out.append(kRoot);
		cout << "loop 1" << endl;
		cout << "merging " << in1 << " and " << in2 << endl;
		mergeKEdsTree(in1, in2, out, log);
		//in1.replace(in1.find("Bckgd"), 5,"Neutron" );
		//in2.replace(in2.find("Bckgd"), 5,"Neutron" );
		//out.replace(out.find("Bckgd"), 5,"Neutron" );
		//log.replace(log.find("Bckgd"), 5,"Neutron" );
		//mergeKEdsTree(in1, in2, out, log);
		//in1.replace(in1.find("Neutron"), 7,"Gamma" );
		//in2.replace(in2.find("Neutron"), 7,"Gamma" );
		//out.replace(out.find("Neutron"), 7,"Gamma" );
		//log.replace(log.find("Neutron"), 7,"Gamma" );
		//mergeKEdsTree(in1, in2, out, log);
 	}	
	

	 
	
	//merging step2
	for(Int_t i=0; i<kNumberBolos-1;i+=4){//kNumberBolos
		TString logFile = kOutputPath;
		logFile += kDetectorNames[i]; logFile += kDetectorNames[i+1]; logFile += kDetectorNames[i+2]; logFile += kDetectorNames[i+3];
	  	logFile += ".log";
	  	log= logFile.Data();
		in1=kOutputPath;
		in1.append(kEds); in1.append(kDetectorNames[i]);in1.append(kDetectorNames[i+1]); in1.append(kRoot);
		in2=kOutputPath;
		in2.append(kEds); in2.append(kDetectorNames[i+2]);in2.append(kDetectorNames[i+3]); in2.append(kRoot);
		out=kOutputPath;
		out.append(kEds); out.append(kDetectorNames[i]); out.append(kDetectorNames[i+1]); 
		out.append(kDetectorNames[i+2]); out.append(kDetectorNames[i+3]); out.append(kRoot);
		cout << "loop 2" << endl;
		cout << "merging " << in1 << " and " << in2 << endl;
		mergeKEdsTree(in1, in2, out, log);
		//in1.replace(in1.find("Bckgd"), 5,"Neutron" );
		//in2.replace(in2.find("Bckgd"), 5,"Neutron" );
		//out.replace(out.find("Bckgd"), 5,"Neutron" );
		//log.replace(log.find("Bckgd"), 5,"Neutron" );
		//mergeKEdsTree(in1, in2, out, log);
		//in1.replace(in1.find("Neutron"), 7,"Gamma" );
		//in2.replace(in2.find("Neutron"), 7,"Gamma" );
		//out.replace(out.find("Neutron"), 7,"Gamma" );
		//log.replace(log.find("Neutron"), 7,"Gamma" );
		//mergeKEdsTree(in1, in2, out, log); 
 	}
	
	
	//merging step3 
		//3 single merging operations left (merge 4+4) merge(4+GGA4) -> merge (8+5)
	in1 = kOutputPath; 
	in1.append("Kds_FID401FID402ID2ID3.root");
	in2 = kOutputPath;
	in2.append("Kds_ID4ID5ID6ID401.root");
	out = kOutputPath;
	out.append("Kds_AlmostAll1.root");
	log = kOutputPath;
	log.append("Merge_AlmostAll1.log");
	mergeKEdsTree(in1, in2, out, log);

	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_FID401FID402ID2ID3.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_ID4ID5ID6ID401.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_AlmostAll1.root","/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Merge_AlmostAll1.log");
	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_FID401FID402ID2ID3.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_ID4ID5ID6ID401.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_AlmostAll1.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Merge_AlmostAll1.log");
	cout <<"Merging AlmosAll1 finished" << endl;

	in1 = kOutputPath; 
	in1.append("Kds_ID402ID403ID404ID405.root");
	in2 = kInputPath1;
	in2.append("Kds_GGA4.root");
	out = kOutputPath;
	out.append("Kds_AlmostAll2.root");
	log = kOutputPath;
	log.append("Merge_AlmostAll2.log");
	mergeKEdsTree(in1, in2, out, log);
	
	
	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_ID402ID403ID404ID405.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_GGA4.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_AlmostAll2.root","/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Merge_AlmostAll2.log");
	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_ID402ID403ID404ID405.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_GGA4.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Merge_AlmostAll2.log", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_AlmostAll2.root");
	cout <<"Merging AlmosAll2 finished" << endl;
	
	in1 = kOutputPath; 
	in1.append("Kds_AlmostAll1.root");
	in2 = kOutputPath;
	in2.append("Kds_AlmostAll2.root");
	out = kOutputPath;
	out.append("Kds_AllBolos.root");
	log = kOutputPath;
	log.append("Merge_AllBolos.log");
	mergeKEdsTree(in1, in2, out, log);	

	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_AlmostAll1.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_AlmostAll2.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Kds_AllBolos.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Neutron/Merge_AllBolos.log");
	//mergeKEdsTree("/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_AlmostAll1.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_AlmostAll2.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Kds_AllBolos.root", "/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Gamma/Merge_AllBolos.log");
	cout <<"Merging AllBolos finished" << endl;
	

	//merging of the muonVetoSystem
	/*
	//First produce the concatenated muonVetoFile and the merged bolo files!
	//Long64_t startTime=1258735463;
	Int_t kEntryOfBoloStart=272394; //From this event on the timelines match and merging begins, before this event we just fill in the uVeto data
	//Int_t kBadArrayLength=4;
	//Int_t badMuonEntry[kBadArrayLength]={396278 ,400838, 453204, 455202};// directly set in the source file;
	// first bad entry, second bad entry, period of time resets in uVeto
	//third bad Entry to fourth bad Entry: no bolo data available + time resets in uVeto
	
	string logfile="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/MergeAllBolosVeto.log";
	 in1 ="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Kds_AllBolos.root";
	 in2 ="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/KdsRun12_MuonVetoAll.root";
	 out="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Kds_AllBolosVeto.root";
	Bool_t bigNeutron=false; //defines if the data of the big Neutron calibration should be included, default is off
	Int_t debug=-24129205;
	mergeKEdsTree( in1, in2, out, logfile, kEntryOfBoloStart, bigNeutron, debug);
	 */
	
}


