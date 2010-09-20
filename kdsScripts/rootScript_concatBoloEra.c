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
	string myKdataPath = "$KDATA_ROOT"; //locating of the directory where bin/concatDst lives.
	string kInputPath1="/kalinka/storage/edelweiss/Bolo/Run12/Eds/Input/";
	string kInputPath2="/kalinka/storage/edelweiss/Bolo/Run12/Eds/Input/";
	string kOutputPath="/kalinka/storage/edelweiss/Bolo/Run12/Eds/Input/";
	string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/concatDst/";
	TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/concatDst";

	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	
	

	kInputPath1.append("Bckgd/EricRun12FirstPart/");
	kInputPath2.append("Bckgd/EricRun12SecondPart/");
	kOutputPath.append("Bckgd/");
	
	string kRoot=".root";
	string keraName[5];
	//the order here is IMPORTANT!
	keraName[1] ="dst_";
	keraName[2] = "cuts_";
	keraName[3] = "eheat_";
	keraName[4] = "eion_";
	
	
	string in1, in2, out;

	TString theTime;
	TString command;
	TString myFileName;
	TString myFileContents;
	ofstream myFile;

	for(Int_t j = 1; j <= 4; j++){
		
		
		for(Int_t i = 0; i < kNumberBolos; i++){
			in1=kInputPath1;
			in1.append(keraName[j]); in1.append(kDetectorNames[i]); in1.append(kRoot);
			in2=kInputPath2;
			in2.append(keraName[j]); in2.append(kDetectorNames[i]); in2.append(kRoot);
			out=kOutputPath;
			out.append(keraName[j]); out.append(kDetectorNames[i]); out.append(kRoot);
			
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

			myFileName  = kQSubScriptFileDir + "concatDstBckgd" + theTime.Data() + ".sh";
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst ";
			myFileContents += j;  //the option here tells concatDst exactly which type of ERA file its concatenating.
			myFileContents += + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();
			command = "chmod +x " + myFileName;
			cout << command << endl;
			gSystem->Exec(command.Data());

			command = "qsub -wd " + qsubWorkingDir + " -q all.q -V " + myFileName;
			cout << command << endl;
	
			gSystem->Exec(command.Data());
			
			//Do the same thing for the neutrons.
			
			in1.replace(in1.find("Bckgd"),5, "Neutron");
			in2.replace(in2.find("Bckgd"),5, "Neutron");
			out.replace(out.find("Bckgd"),5, "Neutron");

			
			//theTime = myTime.GetDate() + "." myTime.GetTime() + "." + myTime.GetNanoSec();
			myFileName  = kQSubScriptFileDir + "concatDstNeutron" + theTime.Data() + ".sh";
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst ";
			myFileContents += j; //the option here tells concatDst exactly which type of ERA file its concatenating.
			myFileContents += + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();

			command = "chmod +x " + myFileName;
			cout << command << endl;
			gSystem->Exec(command.Data());
			
			command = "qsub -wd " + qsubWorkingDir + " -q all.q -V  " + myFileName;
			cout << command << endl;
			
			gSystem->Exec(command.Data());
			
			
			in1.replace(in1.find("Neutron"),7, "Gamma");
			out.replace(out.find("Neutron"),7, "Gamma");
			in2.replace(in2.find("Neutron"),7, "Gamma");
			
			//theTime = myTime.GetDate() + "." myTime.GetTime() + "." + myTime.GetNanoSec();
			myFileName  = kQSubScriptFileDir + "concatDstGamma" + theTime.Data() + ".sh";
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst ";
			myFileContents += j; //the option here tells concatDst exactly which type of ERA file its concatenating.
			myFileContents += + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();
			
			command = "chmod +x " + myFileName;
			cout << command << endl;
			gSystem->Exec(command.Data());

			command = "qsub -wd " + qsubWorkingDir + " -q all.q -V " + myFileName;
			cout << command << endl;
			
			gSystem->Exec(command.Data());
			
		}
		
	}
		
		
	
	
	//You may compile your own classes in this same way
}
