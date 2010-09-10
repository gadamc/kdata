/*
 *  rootScript_mergeKdsTree_Step1.C
 *  kData
 *
 *  Created by Adam Cox on 9/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


{
	TString kDataPath = "$KDATA_ROOT";
	string kInputPath1="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
	//string kInputPath1="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
	string kOutputPath="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
	//string kOutputPath="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/";
	string kLogFile="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Merge.log";
	//string kLogFile="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/Merge.log";
	string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/";
	TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/";

	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	
	string kRoot=".root";
	string kEds="Kds_";
	//Int_t kEntryOfBoloStart=272395; //From this event on the timelines match and merging begins, before this event we just fill in the uVeto data
	//Int_t badMuonEntry[2]={396278 ,400838};//first bad entry, last bad entry, period of time resets in uVeto
	string in1, in2, out, log;
	
	
	TString command;
	TString theTime;
	TString myFileName;
	TString myFileContents;
	TString logFile;
	
	ofstream myFile;
	
	for(Int_t i=0; i<kNumberBolos-1;i+=2) {
		
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
		
		logFile = kOutputPath;
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
		
		myFileName = kQSubScriptFileDir + "mergeKEdsTree_Bckgd" + theTime.Data() + ".sh";
		cout << endl;
		cout << "opening " << myFileName.Data() << endl;
		
		myFile.open(myFileName.Data());
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/mergeKEdsTree ";
		myFileContents += in1 + " " + in2 + " " + out + " " + log;
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
	
		
		
		in1.replace(in1.find("Bckgd"), 5,"Neutron" );
		in2.replace(in2.find("Bckgd"), 5,"Neutron" );
		out.replace(out.find("Bckgd"), 5,"Neutron" );
		log.replace(log.find("Bckgd"), 5,"Neutron" );
		
		
		myFileName = kQSubScriptFileDir + "mergeKEdsTree_Bckgd" + theTime.Data() + ".sh";
		cout << endl << "opening " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/mergeKEdsTree ";
		myFileContents += in1 + " " + in2 + " " + out + " " + log;
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
		
		in1.replace(in1.find("Neutron"), 7,"Gamma" );
		in2.replace(in2.find("Neutron"), 7,"Gamma" );
		out.replace(out.find("Neutron"), 7,"Gamma" );
		log.replace(log.find("Neutron"), 7,"Gamma" );
		
		myFileName = kQSubScriptFileDir + "mergeKEdsTree_Bckgd" + theTime.Data() + ".sh";
		cout << endl << "opening " << myFileName.Data() << endl;
		myFile.open(myFileName.Data());
		
		myFileContents = "#!/bin/bash"; 
		cout << myFileContents.Data() << endl;
		myFile << myFileContents.Data() << endl;
		myFileContents = myKdataPath + "/bin/mergeKEdsTree ";
		myFileContents += in1 + " " + in2 + " " + out + " " + log;
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

}
		
		
