/*
 *  rootScript_mergeKdsTree_Step1.C
 *  kData
 *
 *  Created by Adam Cox on 9/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


string submitCommand(const char* fileName, const char* binary, const char* arg1 = "", const char* arg2 = "", const char* arg3 = "",const char* arg4 = "", const char* arg5 = "", const char* arg6 = "", const char* arg7 = "", UInt_t numHolds = 0, string* listOfHolds=0)
{
  cout << endl << endl;
  
  TString qsubWorkingDir = "/kalinka/storage/edelweiss/qsubOutputs/";
  string kQSubScriptFileDir = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/scripts/";

  string sarg1 = arg1;
  string sarg2 = arg2;
  string sarg3 = arg3;
  string sarg4 = arg4;
  string sarg5 = arg5;
  string sarg6 = arg6;
  string sarg7 = arg7;

  TString myFileContents;
  TString myFileName;
  TString theTime;
  TTimeStamp myTime;
  ofstream myFile;

  UInt_t myTimeDate = myTime.GetDate();
  UInt_t myTimeTime = myTime.GetTime();
  Int_t myTimeNano = myTime.GetNanoSec();
  
  theTime = "_";
  theTime += myTimeDate;
  theTime += + "_";
  theTime +=  myTimeTime;
  theTime += + "_";
  theTime += myTimeNano;

  TString myFileName = kQSubScriptFileDir + fileName;
  myFileName += theTime + ".sh";
  
  TString myJobName = fileName + theTime;

  //cout << "opening " << myFileName.Data() << endl;
  
  myFile.open(myFileName.Data());
  myFileContents = "#!/bin/bash"; 
  //cout << myFileContents.Data() << endl;
  
  myFile << myFileContents.Data() << endl;
  
  myFileContents = binary;
 
  myFileContents += " " + sarg1 + " " + sarg2 + " " + sarg3 + " " + sarg4;
  myFileContents += " " + sarg5 + " " + sarg6 + " " + sarg7;// + " " + sarg8;
  //myFileContents += " " + sarg9 + " " + sarg10;

  cout << myFileContents.Data() << endl;
  myFile << myFileContents.Data() << endl;
  myFileContents = "exit 0";
  //cout << myFileContents.Data() << endl;
  myFile << myFileContents.Data() << endl;
  myFile.close();
	
  command = "chmod +x " + myFileName;
  //cout << command << endl;
  gSystem->Exec(command.Data());

  command = "qsub -wd " + qsubWorkingDir;
  if(numHolds > 0){
    command += " -hold_jid ";
    for(int i = 0; i < numHolds; i++){
      command += listOfHolds[i];
      if(i != numHolds-1) command += + ",";
    } 
  }
  cout << endl;
  command += " -q all.q -V -N " + myJobName + " " + myFileName;
  cout << command << endl;
  
  gSystem->Exec(command.Data());
  
  string jobName = myJobName.Data();
  return jobName;
}


void rootScript_mergeKdsTree(){

  TString myKdataPath = "$KDATA_ROOT";
  string kInputPath1="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
  //string kInputPath1="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
  string kOutputPath="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/";
  //string kOutputPath="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/";
  string kLogFile="/kalinka/home/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Merge.log";
  //string kLogFile="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/Merge.log";
  string muonVetoPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/uVeto/";
  string kFinalPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Final/Bckgd/";
  string kVersionNumber = "3.0dev";
  string kFillEraInputPath = "/kalinka/home/edelweiss/Bolo/Run12/Eds/Input/Bckgd/";
  string kFillEraOutputPath = kInputPath1;
  string tstNewPath = "/kalinka/home/edelweiss/Bolo/Run12/tstnew/";
  string fMuonVetoDataInPath = "kalinka/home/edelweiss/Runs/";
  string fMuonVetoDataOutPath = "kalinka/home/edelweiss/Bolo/Runs12/Eds/Input/uVeto/";
  Int_t startMuonRun = 54;
  Int_t stopMuonRun = 67;
  TString muonVetoCommand = myKdataPath + "bin/fillMuonVetoEvents";
  

  Int_t kNumberBolos=13;
  string kDetectorNames[13]=
    {"FID401", "FID402", "ID2",
     "ID3", "ID4", "ID5", 
     "ID6", "ID401", "ID402",
     "ID403", "ID404", "ID405", "GGA4"};
  
  string kRoot=".root";
  string kEds="Kds_";
  string kEntryOfBoloStart = "272394"; //From this event on the timelines match and merging begins, before this event we just fill in the uVeto data
  string bigNeutron = "0"; //when set to zero, exclude the strong AmBe source data when merging the Muon data to the Bckgd data
  string debug = "-24129205"; //a debug value for the muon veto merging
  //Int_t kEntryOfBoloStart=272395; 
  //Int_t badMuonEntry[2]={396278 ,400838};//first bad entry, last bad entry, period of time resets in uVeto
  string in1, in2, out, log;
  
  
  TString command;
  TString theTime;
  TString myFileName;
  TString myFileContents;
  TString logFile;
  
  vector<string> listOfFillEraToKdsJobsBckgd;
  vector<string> listOfFillEraToKdsJobsNeutron;
  vector<string> listOfFillEraToKdsJobsGamma;

  vector<string> listOfJobsMergeLoopOneBckgd;
  vector<string> listOfJobsMergeLoopOneNeutron;
  vector<string> listOfJobsMergeLoopOneGamma;
  
  vector<string> listOfJobsMergeLoopTwoBckgd;
  vector<string> listOfJobsMergeLoopTwoNeutron;
  vector<string> listOfJobsMergeLoopTwoGamma;

  vector<string> listOfJobsMergeLoopThreeBckgd;
  vector<string> listOfJobsMergeLoopThreeNeutron;
  vector<string> listOfJobsMergeLoopThreeGamma;

  vector<string> listOfJobsMergeLoopFourBckgd;
  vector<string> listOfJobsMergeLoopFourNeutron;
  vector<string> listOfJobsMergeLoopFourGamma;

  string* holdListBckgd = 0;
  string* holdListNeutron = 0;
  string* holdListGamma = 0;
  
  
  //fillEra to Kds loop
  command= myKdataPath + "/bin/fillEraToKEds"; //the binary file to be executed by the shell
  for(Int_t i = 0; i < kNumberBolos; i++){
    
    in1 = kFillEraInputPath;
    out = kFillEraOutputPath;
    out.append(kEds); out.append(kDetectorNames[i]); out.append(kRoot);
    log = kFillEraOutputPath;
    log.append("eraToKds_"); log.append(kDetectorNames[i]); log.append(".log");
    myFileName = "fillEraToKds_Bckgd";
 
    listOfFillEraToKdsJobsBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), kDetectorNames[i].c_str(), out.c_str(), log.c_str(),tstNewPath.c_str()));
  
    in1.replace(in1.find("Bckgd"), 5, "Neutron");
    out.replace(out.find("Bckgd"), 5, "Neutron");
    log.replace(log.find("Bckgd"), 5, "Neutron");
    myFileName = "fillEraToKds_Neutron";
    
    listOfFillEraToKdsJobsNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), kDetectorNames[i].c_str(), out.c_str(), log.c_str(),tstNewPath.c_str()));

    in1.replace(in1.find("Neutron"), 7, "Gamma");
    out.replace(out.find("Neutron"), 7, "Gamma");
    log.replace(log.find("Neutron"), 7, "Gamma");
    myFileName = "fillEraToKds_Gamma";
    
    listOfFillEraToKdsJobsGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), kDetectorNames[i].c_str(), out.c_str(), log.c_str(),tstNewPath.c_str()));
    
  }

  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[listOfFillEraToKdsJobsBckgd.size()];
  holdListNeutron = new string[listOfFillEraToKdsJobsNeutron.size()];
  holdListGamma = new string[listOfFillEraToKdsJobsGamma.size()];
  
  for(UInt_t k = 0; k < listOfFillEraToKdsJobsBckgd.size(); k++){
    holdListBckgd[k] = listOfFillEraToKdsJobsBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfFillEraToKdsJobsNeutron.size(); k++){
    holdListNeutron[k] = listOfFillEraToKdsJobsNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfFillEraToKdsJobsGamma.size(); k++){
    holdListGamma[k] = listOfFillEraToKdsJobsGamma.at(k);
  }


  //set the comamnd for merging
  command= myKdataPath + "/bin/mergeKEdsTree"; //the binary file to be executed by the shell
  //merging loop one
  for(Int_t i=0; i<kNumberBolos-1;i+=2) {
    
    logFile = kOutputPath;
    logFile += "Kds_";
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
    
    myFileName = "mergeKdsTree_Bckgd_step1";

    listOfJobsMergeLoopOneBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(), "", "", "", listOfFillEraToKdsJobsBckgd.size(), holdListBckgd));
     
    in1.replace(in1.find("Bckgd"), 5,"Neutron" );
    in2.replace(in2.find("Bckgd"), 5,"Neutron" );
    out.replace(out.find("Bckgd"), 5,"Neutron" );
    log.replace(log.find("Bckgd"), 5,"Neutron" );
    
    myFileName = "mergeKdsTree_Neutron_step1";

    listOfJobsMergeLoopOneNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(), "", "", "", listOfFillEraToKdsJobsNeutron.size(), holdListNeutron));
    
    in1.replace(in1.find("Neutron"), 7,"Gamma" );
    in2.replace(in2.find("Neutron"), 7,"Gamma" );
    out.replace(out.find("Neutron"), 7,"Gamma" );
    log.replace(log.find("Neutron"), 7,"Gamma" );
    
    myFileName = "mergeKdsTree_Gamma_step1";
    
    listOfJobsMergeLoopOneGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(), "", "", "", listOfFillEraToKdsJobsGamma.size(), holdListGamma));
    
  }
  
  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[listOfJobsMergeLoopOneBckgd.size()];
  holdListNeutron = new string[listOfJobsMergeLoopOneNeutron.size()];
  holdListGamma = new string[listOfJobsMergeLoopOneGamma.size()];
  
  for(UInt_t k = 0; k < listOfJobsMergeLoopOneBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeLoopOneBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopOneNeutron.size(); k++){
    holdListNeutron[k] = listOfJobsMergeLoopOneNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopOneGamma.size(); k++){
    holdListGamma[k] = listOfJobsMergeLoopOneGamma.at(k);
  }
  
  
  //merging loop two
  for(Int_t i=0; i<kNumberBolos-1;i+=4) {
    
    logFile = kOutputPath;
    logFile += "Kds_";
    logFile += kDetectorNames[i];
    logFile += kDetectorNames[i+1]; 
    logFile += kDetectorNames[i+2]; 
    logFile += kDetectorNames[i+3]; 
    logFile += ".log";		
    log= logFile.Data();
    
    in1=kOutputPath;
    in1.append(kEds); 
    in1.append(kDetectorNames[i]); 
    in1.append(kDetectorNames[i+1]); 
    in1.append(kRoot);
    
    in2=kOutputPath;
    in2.append(kEds); 
    in2.append(kDetectorNames[i+2]);
    in2.append(kDetectorNames[i+3]); 
    in2.append(kRoot);

    out=kOutputPath;
    out.append(kEds); out.append(kDetectorNames[i]);
    out.append(kDetectorNames[i+1]); 
    out.append(kDetectorNames[i+2]); 
    out.append(kDetectorNames[i+3]); 
    out.append(kRoot);
    
    myFileName = "mergeKdsTree_Bckgd_step2";

    listOfJobsMergeLoopTwoBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopOneBckgd.size(), holdListBckgd));
    
    in1.replace(in1.find("Bckgd"), 5,"Neutron" );
    in2.replace(in2.find("Bckgd"), 5,"Neutron" );
    out.replace(out.find("Bckgd"), 5,"Neutron" );
    log.replace(log.find("Bckgd"), 5,"Neutron" );
    
    myFileName = "mergeKdsTree_Neutron_step2";
    
    listOfJobsMergeLoopTwoNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopOneNeutron.size(), holdListNeutron));
    
    in1.replace(in1.find("Neutron"), 7,"Gamma" );
    in2.replace(in2.find("Neutron"), 7,"Gamma" );
    out.replace(out.find("Neutron"), 7,"Gamma" );
    log.replace(log.find("Neutron"), 7,"Gamma" );
    
    myFileName = "mergeKdsTree_Gamma_step2";
    
    listOfJobsMergeLoopTwoGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "",listOfJobsMergeLoopOneGamma.size(), holdListGamma));
    
  }
  
  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[listOfJobsMergeLoopTwoBckgd.size()];
  holdListNeutron = new string[listOfJobsMergeLoopTwoNeutron.size()];
  holdListGamma = new string[listOfJobsMergeLoopTwoGamma.size()];
  
  for(UInt_t k = 0; k < listOfJobsMergeLoopTwoBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeLoopTwoBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopTwoNeutron.size(); k++){
    holdListNeutron[k] = listOfJobsMergeLoopTwoNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopTwoGamma.size(); k++){
    holdListGamma[k] = listOfJobsMergeLoopTwoGamma.at(k);
  }

  //merging step3 
  in1 = kOutputPath;
  in1.append("Kds_FID401FID402ID2ID3.root");
  in2 = kOutputPath;
  in2.append("Kds_ID4ID5ID6ID401.root");
  out = kOutputPath;
  out.append("Kds_AlmostAll1.root");
  log = kOutputPath;
  log.append("Merge_AlmostAll1.log");
  
  myFileName = "mergeKdsTree_Bckgd_step3";
  listOfJobsMergeLoopThreeBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoBckgd.size(), holdListBckgd));
  
  in1.replace(in1.find("Bckgd"), 5,"Neutron" );
  in2.replace(in2.find("Bckgd"), 5,"Neutron" );
  out.replace(out.find("Bckgd"), 5,"Neutron" );
  log.replace(log.find("Bckgd"), 5,"Neutron" );
  
  myFileName = "mergeKdsTree_Neutron_step3";
  
  listOfJobsMergeLoopThreeNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoNeutron.size(), holdListNeutron));
  
  in1.replace(in1.find("Neutron"), 7,"Gamma" );
  in2.replace(in2.find("Neutron"), 7,"Gamma" );
  out.replace(out.find("Neutron"), 7,"Gamma" );
  log.replace(log.find("Neutron"), 7,"Gamma" );
  
  myFileName = "mergeKdsTree_Gamma_step3";
  
  listOfJobsMergeLoopThreeGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoGamma.size(), holdListGamma));
 
  //step 3b...
  in1 = kOutputPath;
  in1.append("Kds_ID402ID403ID404ID405.root");
  in2 = kOutputPath;
  in2.append("Kds_GGA4.root");
  out = kOutputPath;
  out.append("Kds_AlmostAll2.root");
  log = kOutputPath;
  log.append("Merge_AlmostAll2.log");
  
  myFileName = "mergeKdsTree_Bckgd_step3b";
  listOfJobsMergeLoopThreeBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoBckgd.size(), holdListBckgd));
  
  in1.replace(in1.find("Bckgd"), 5,"Neutron" );
  in2.replace(in2.find("Bckgd"), 5,"Neutron" );
  out.replace(out.find("Bckgd"), 5,"Neutron" );
  log.replace(log.find("Bckgd"), 5,"Neutron" );
  
  myFileName = "mergeKdsTree_Neutron_step3b";
  
  listOfJobsMergeLoopThreeNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoNeutron.size(), holdListNeutron));
  
  in1.replace(in1.find("Neutron"), 7,"Gamma" );
  in2.replace(in2.find("Neutron"), 7,"Gamma" );
  out.replace(out.find("Neutron"), 7,"Gamma" );
  log.replace(log.find("Neutron"), 7,"Gamma" );
  
  myFileName = "mergeKdsTree_Gamma_step3b";
  
  listOfJobsMergeLoopThreeGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopTwoGamma.size(), holdListGamma));
 

  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[listOfJobsMergeLoopThreeBckgd.size()];
  holdListNeutron = new string[listOfJobsMergeLoopThreeNeutron.size()];
  holdListGamma = new string[listOfJobsMergeLoopThreeGamma.size()];
  
  for(UInt_t k = 0; k < listOfJobsMergeLoopThreeBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeLoopThreeBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopThreeNeutron.size(); k++){
    holdListNeutron[k] = listOfJobsMergeLoopThreeNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopThreeGamma.size(); k++){
    holdListGamma[k] = listOfJobsMergeLoopThreeGamma.at(k);
  }

  //merging step 4
  in1 = kOutputPath;
  in1.append("Kds_AlmostAll1.root");
  in2 = kOutputPath;
  in2.append("Kds_AlmostAll2.root");
  out = kOutputPath;
  out.append("Kds_AllBolos.root");
  log = kOutputPath;
  log.append("Merge_AllBolos.log");
  
  myFileName = "mergeKdsTree_Bckgd_step4";
  listOfJobsMergeLoopFourBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopThreeBckgd.size(), holdListBckgd));
  
  in1.replace(in1.find("Bckgd"), 5,"Neutron" );
  in2.replace(in2.find("Bckgd"), 5,"Neutron" );
  out.replace(out.find("Bckgd"), 5,"Neutron" );
  log.replace(log.find("Bckgd"), 5,"Neutron" );
  
  myFileName = "mergeKdsTree_Neutron_step4";
  
  listOfJobsMergeLoopFourNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopThreeNeutron.size(), holdListNeutron));
  
  in1.replace(in1.find("Neutron"), 7,"Gamma" );
  in2.replace(in2.find("Neutron"), 7,"Gamma" );
  out.replace(out.find("Neutron"), 7,"Gamma" );
  log.replace(log.find("Neutron"), 7,"Gamma" );
  
  myFileName = "mergeKdsTree_Gamma_step4";
  
  listOfJobsMergeLoopFourGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"", "", "", listOfJobsMergeLoopThreeGamma.size(), holdListGamma));


  //merge Muon Veto Data 
  //the next step for the Bckgd data is to merge the Muon Veto Events
  //but, first we make sure the muon veto data is properly filled with the latest data format
  //and we add these jobs to the listOfJobsMergeLoopFourBckgd.
  command= myKdataPath + "/bin/fillMuonVetoEvents"; //the binary file to be executed by the shell
  for(Int_t i = startMuonRun; i <= stopMuonRun; i++) {

    TString muonIn = fMuonVetoDataInPath;
    muonIn += "Events_Run";
    muonIn += i;
    muonIn += "_Laser_time.root";
    
    TString muonOut = fMuonVetoDataOutPath;
    muonOut += "KdsRun12_MvRun";
    muonOut += i;
    muonOut += ".root";
    
    myFileName = "fillMuonVetoEvents"; 
  
    listOfJobsMergeLoopFourBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), muonIn.Data(), muonOut.Data()));
  
  }

  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[listOfJobsMergeLoopFourBckgd.size()];
  holdListNeutron = new string[listOfJobsMergeLoopFourNeutron.size()];
  holdListGamma = new string[listOfJobsMergeLoopFourGamma.size()];
  
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeLoopFourBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourNeutron.size(); k++){
    holdListNeutron[k] = listOfJobsMergeLoopFourNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourGamma.size(); k++){
    holdListGamma[k] = listOfJobsMergeLoopFourGamma.at(k);
  }
  
  

  vector<string> listOfJobsMergeMuonVetoBckgd;
  command= myKdataPath + "/bin/mergeKEdsTree"; //the binary file to be executed by the shell
  in1 = kOutputPath;
  in1.append("Kds_AllBolos.root");
  in2 = muonVetoPath;
  in2.append("KdsRun12_MuonVetoAll.root");
  out = kOutputPath;
  out.append("Kds_AllBolosVeto.root");
  log = kOutputPath;
  log.append("Merge_AllBolosVeto.log");
  
  myFileName = "mergeKdsTree_MuonVetoAndBckgd";

  listOfJobsMergeMuonVetoBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), in2.c_str(), out.c_str(), log.c_str(),"","", "", listOfJobsMergeLoopFourBckgd.size(), holdListBckgd));


  if(holdListBckgd != 0) delete[] holdListBckgd;
  holdListBckgd = new string[listOfJobsMergeMuonVetoBckgd.size()];
  for(UInt_t k = 0; k < listOfJobsMergeMuonVetoBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeMuonVetoBckgd.at(k);
  }

  //allPassOne_GSEventNumber -- add the event numbers;
  vector<string> allPassOneJobNameBckgd;
  vector<string> allPassOneJobNameNeutron;
  vector<string> allPassOneJobNameGamma;

  command = myKdataPath + "/bin/allPassOne_GSEventNumbers"; //the binary  
  in1 = kOutputPath;
  in1.append("Kds_AllBolosVeto.root");
  out = kFinalPath;
  out.append("Kds_Run12_");
  out.append(kVersionNumber.c_str());
  out.append(".root");

  myFileName = "allPassOne_GSEventNumberBckgd";
  allPassOneJobNameBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", listOfJobsMergeMuonVetoBckgd.size(), holdListBckgd));

  //now do the allPassOne for the Neutron and Gamma data after the merge step five
  in1 = kOutputPath;
  in1.append("Kds_AllBolos.root");
  
  in1.replace(in1.find("Bckgd"), 5, "Neutron");
  out.replace(out.find("Bckgd"), 5, "Neutron");

  myFileName = "allPassOne_GSEventNumberNeutron";
  allPassOneJobNameNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", listOfJobsMergeLoopFourNeutron.size(), holdListNeutron));

  
  in1.replace(in1.find("Neutron"), 7, "Gamma");
  out.replace(out.find("Neutron"), 7, "Gamma");

  myFileName = "allPassOne_GSEventNumberGamma";
  allPassOneJobNameGamma.push_back(submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", listOfJobsMergeLoopFourGamma.size(), holdListGamma));

  if(holdListBckgd != 0) delete[] holdListBckgd;
  if(holdListNeutron != 0) delete[] holdListNeutron;
  if(holdListGamma != 0) delete[] holdListGamma;

  holdListBckgd = new string[allPassOneJobNameBckgd.size()];
  holdListNeutron = new string[allPassOneJobNameNeutron.size()];
  holdListGamma = new string[allPassOneJobNameGamma.size()];
  
  for(UInt_t k = 0; k < allPassOneJobNameBckgd.size(); k++){
    holdListBckgd[k] = allPassOneJobNameBckgd.at(k);
  }
  for(UInt_t k = 0; k < allPassOneJobNameNeutron.size(); k++){
    holdListNeutron[k] = allPassOneJobNameNeutron.at(k);
  }
  for(UInt_t k = 0; k < allPassOneJobNameGamma.size(); k++){
    holdListGamma[k] = allPassOneJobNameGamma.at(k);
  }
  
  //now skim all files
  command = myKdataPath + "/bin/skimNoNoiseBolos"; //the binary  
  in1 = kFinalPath;
  in1.append("Kds_Run12_");
  in1.append(kVersionNumber.c_str());
  in1.append(".root");
  out = kFinalPath;
  out.append("Kds_Run12_");
  out.append(kVersionNumber.c_str());
  out.append("_skim.root");

  myFileName = "skimNoNoiseBolos_Bckgd";
  submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", allPassOneJobNameBckgd.size(), holdListBckgd);

  in1.replace(in1.find("Bckgd"), 5, "Neutron");
  out.replace(out.find("Bckgd"), 5, "Neutron");

  myFileName = "skimNoNoiseBolos_Neutron";
  submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", allPassOneJobNameNeutron.size(), holdListNeutron);

  in1.replace(in1.find("Neutron"), 7, "Gamma");
  out.replace(out.find("Neutron"), 7, "Gamma");

  myFileName = "skimNoNoiseBolos_Gamma";
  submitCommand(myFileName.Data(), command.Data(), in1.c_str(), out.c_str(), "", "", "", "", "", allPassOneJobNameGamma.size(), holdListGamma);

}
