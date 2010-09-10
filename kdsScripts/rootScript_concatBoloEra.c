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
	string kInputPath1="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/";
	string kInputPath2="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/";
	string kOutputPath="/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/";
	string kQSubScriptFileDir = "/kalinka/home/edelweiss/Bolo/Run12/Eds/scripts/";
		
	Int_t kNumberBolos=13;
	string kDetectorNames[kNumberBolos]=
	{"FID401", "FID402", "ID2",
		"ID3", "ID4", "ID5", 
		"ID6", "ID401", "ID402",
		"ID403", "ID404", "ID405", "GGA4"};
	
	

	kInputPath1.append("/Bckgd/EricRun12FirstPart/");
	kInputPath2.append("/Bckgd/EricRun12SecondPart/");
	
	string kRoot=".root";
	string kdst="dst_";
	string keheat = "eheat_";
	string keion = "eion_";
	string kcuts = "cuts_";
	
	string in1, in2, out;

	TString theTime;
	TString command;
	TString myFileName;
	TString myFileContents;
	ofstream myFile;

	for(Int_t j = 1; j <= 4; j++){
		
		
		for(Int_t i = 0; i < kNumberBolos; i++){
			in1=kInputPath1;
			in1.append(kdst); in1.append(kDetectorNames[i]); in1.append(kRoot);
			in2=kInputPath2;
			in2.append(kdst); in2.append(kDetectorNames[i]); in2.append(kRoot);
			out=kOutputPath;
			out.append(kdst); out.append(kDetectorNames[i]); out.append(kRoot);
			
			TTimeStamp myTime;
			theTime = myTime.GetDate() + "." myTime.GetTime() + "." + myTime.GetNanoSec();
			myFileName  = kQSubScriptFileDir + "/concatDstBckgd." + theTime;
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst " + j + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0" << endl;
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();
			
			cout << command << endl;
			command = "qsub -wd /kalinka/storage/edelweiss/qsubOutputs -q all.q -V -b y " + myFileName.Data();
			gSystem->Exec(command.Data());
			
			//Do the same thing for the neutrons.
			
			in1.replace(in1.find("Bckgd"), 5, "Neutron");
			out.replace(out.find("Bckgd"),5, "Neutron");
			in2.replace(in2.find("Bckgd"),5, "Neutron");
			
			theTime = myTime.GetDate() + "." myTime.GetTime() + "." + myTime.GetNanoSec();
			myFileName  = kQSubScriptFileDir + "/concatDstNeutron." + theTime;
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst " + j + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0" << endl;
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();
			
			cout << command << endl;
			command = "qsub -wd /kalinka/storage/edelweiss/qsubOutputs -q all.q -V -b y " + myFileName.Data();
			gSystem->Exec(command.Data());
			
			
			in1.replace(in1.find("Neutron"),7, "Gamma");
			out.replace(out.find("Neutron"),7, "Gamma");
			in2.replace(in2.find("Neutron"),7, "Gamma");
			
			theTime = myTime.GetDate() + "." myTime.GetTime() + "." + myTime.GetNanoSec();
			myFileName  = kQSubScriptFileDir + "/concatDstGamma." + theTime;
			cout << " opening " << myFileName.Data() << endl;
			myFile.open(myFileName.Data());
			
			myFileContents = "#!/bin/bash";
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = myKdataPath + "/bin/concatDst " + j + " " + in1 + " " + in2 + " " + out; //don't forget the '1' to indicate which option you're running
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFileContents = "exit 0" << endl;
			cout << myFileContents.Data() << endl;
			myFile << myFileContents.Data() << endl;
			myFile.close();
			
			cout << command << endl;
			command = "qsub -wd /kalinka/storage/edelweiss/qsubOutputs -q all.q -V -b y " + myFileName.Data();
			gSystem->Exec(command.Data());
			
			/*
			 in1=kInputPath1;
			 in1.append(keion); in1.append(kDetectorNames[i]); in1.append(kRoot);
			 in2=kInputPath2;
			 in2.append(keion); in2.append(kDetectorNames[i]); in2.append(kRoot);
			 out=kOutputPath;
			 out.append(keion); out.append(kDetectorNames[i]); out.append(kRoot);
			 concatEion(in1, in2, out);
			 
			 
			 in1=kInputPath1;
			 in1.append(keheat); in1.append(kDetectorNames[i]); in1.append(kRoot);
			 in2=kInputPath2;
			 in2.append(keheat); in2.append(kDetectorNames[i]); in2.append(kRoot);
			 out=kOutputPath;
			 out.append(keheat); out.append(kDetectorNames[i]); out.append(kRoot);
			 concatEheat(in1, in2, out);
			 
			 
			 in1=kInputPath1;
			 in1.append(kcuts); in1.append(kDetectorNames[i]); in1.append(kRoot);
			 in2=kInputPath2;
			 in2.append(kcuts); in2.append(kDetectorNames[i]); in2.append(kRoot);
			 out=kOutputPath;
			 out.append(kcuts); out.append(kDetectorNames[i]); out.append(kRoot);
			 concatCuts(in1, in2, out);
			 */
		}
		
	}
		
		
	
	
	//You may compile your own classes in this same way
}
