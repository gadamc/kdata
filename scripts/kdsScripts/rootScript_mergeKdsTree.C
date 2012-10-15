/*
 *  rootScript_mergeKdsTree_Step1.C
 *  kData
 *
 *  Created by Adam Cox on 9/10/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */


string submitCommand(const char* fileName, const char* executeCmd, const int argc = 0, const string* argv = 0, UInt_t numHolds = 0, string* listOfHolds=0)
{
  cout << endl << endl;
  
  TString qsubWorkingDir = "/kalinka/home/gadamc/qsubOutputs/";
  string kQSubScriptFileDir = "/kalinka/home/gadamc/Eds/scripts/";


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
  
  myFileContents = executeCmd;
  for(int i = 0; i < argc; i++){
    myFileContents += " " + argv[i];
  }

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

  //what this script does - fillERAtoKds (need to have rootScript_concatBoloEra first), merges bolometer data together
  //fills Muon Veto Data into Kds format, concatenates muon veto Kds format into single file, merges that file with the 
  //background bolo merged file, calls allPassOne_GSEventNumber on all the Merged files, calls skimNoNoiseBolos on all of the
  //merged files. Results are found in ~edelweiss/Bolo/Run12/Eds/Final

  bool bFillBolos = true;
  bool bMergeBolos = true;
  bool bMergeBolosAndMuon = true;
  bool bFillMuons = true;
  bool bConcatenateMuons = true;
  bool bAddGlobalSoftwareEventNumber = true;
  bool bSkimBoloNoiseEvents = true;

  TString myKdataPath = "$KDATA_ROOT";
  string kWimpDirectoryName = "Bckgd";
  string kInputPath1="/kalinka/home/gadamc/Eds/Merge/" + kWimpDirectoryName + "/";
  //string kInputPath1="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/";
  string kOutputPath="/kalinka/home/gadamc/Eds/Merge/" + kWimpDirectoryName + "/";
  string kGammaDirectoryName = "Gamma";
  string kNeutronDirectoryName = "Neutron";
  //string kOutputPath="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/";
  string kLogFile="/kalinka/home/gadamc/Eds/Merge/Bckgd/Merge.log";
  //string kLogFile="/Users/adam/analysis/edelweiss/data/boloEds/boloEdsNeutron/Merge/Merge.log";
  string muonVetoPath = "/kalinka/home/gadamc/Eds/Input/uVeto/";
  string kFinalPath = "/kalinka/home/gadamc/Eds/Final/" + kWimpDirectoryName + "/";
  string kVersionNumber = "v3.0";
  string kFillEraInputPath = "/kalinka/home/gadamc/Eds/Input/" + kWimpDirectoryName + "/";
  string kFillEraOutputPath = kInputPath1;
  string tstNewPath = "/kalinka/home/edelweiss/Bolo/Run12/tstnew/";
  string fMuonVetoDataInPath = "/kalinka/home/edelweiss/Runs/";
  string fMuonVetoDataOutPath = "/kalinka/home/gadamc/Eds/Input/uVeto/";
  string outMuonVetoCombinedFile = fMuonVetoDataOutPath + "KdsRun12_v3.0_MuonVetoAll.root";


  Int_t startMuonRun = 54;
  Int_t stopMuonRun = 67;
  TString muonVetoCommand = myKdataPath + "bin/fillMuonVetoEvents";
  string bBigNeutron = "0"; //set to 1 if you want the strong AmBe source data added to the output file when merging the muon veto data
  string kEntryOfBoloStart = "272394"; //need to tell the muon veto merging to start here
  string kDebugMuonVeto = "-24129205"; //muon veto merge debug flag??

  Int_t kNumberBolos=13;
  string kDetectorNames[13]=
    {"FID401", "FID402", "ID2",
     "ID3", "ID4", "ID5", 
     "ID6", "ID401", "ID402",
     "ID403", "ID404", "ID405", "GGA4"};
  
  string kRoot=".root";
  string kEds="Kds_";
  //string kEntryOfBoloStart = "272394"; //From this event on the timelines match and merging begins, before this event we just fill in the uVeto data
  //string bigNeutron = "0"; //when set to zero, exclude the strong AmBe source data when merging the Muon data to the Bckgd data
  //string debug = "-24129205"; //a debug value for the muon veto merging
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

  vector<string> listOfFillMuonJobs;
  
  string* holdListBckgd = 0;
  string* holdListNeutron = 0;
  string* holdListGamma = 0;
  string* holdListMuon = 0;
  
  string* myArgs;
  int myNumArgs;
  
  //fillEra to Kds loop
  command= myKdataPath + "/bin/fillEraToKEds"; //the executeCmd file to be executed by the shell
  for(Int_t i = 0; i < kNumberBolos && bFillBolos; i++){
    
    myNumArgs = 5;
    myArgs = new string[myNumArgs];
    
    in1 = kFillEraInputPath;
    out = kFillEraOutputPath;
    out.append(kEds); out.append(kDetectorNames[i]); out.append(kRoot);
    log = kFillEraOutputPath;
    log.append("eraToKds_"); log.append(kDetectorNames[i]); log.append(".log");
    myFileName = "fillEraToKds_Bckgd";

    myArgs[0] = in1;
    myArgs[1] = kDetectorNames[i];
    myArgs[2] = out;
    myArgs[3] = log;
    myArgs[4] = tstNewPath;
    
    listOfFillEraToKdsJobsBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs));
  
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    myFileName = "fillEraToKds_Neutron";
    
    myArgs[0] = in1;
    myArgs[1] = kDetectorNames[i];
    myArgs[2] = out;
    myArgs[3] = log;
    myArgs[4] = tstNewPath;
    
    listOfFillEraToKdsJobsNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs));
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    myFileName = "fillEraToKds_Gamma";
    
    myArgs[0] = in1;
    myArgs[1] = kDetectorNames[i];
    myArgs[2] = out;
    myArgs[3] = log;
    myArgs[4] = tstNewPath;
    
    listOfFillEraToKdsJobsGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs));    
    
    delete [] myArgs;
  }

  if(holdListBckgd != 0){
    delete[] holdListBckgd;
    holdListBckgd = 0;
  }
  if(holdListNeutron != 0) {
    delete[] holdListNeutron;
    holdListNeutron = 0;
  }
  if(holdListGamma != 0) {
    delete[] holdListGamma;
    holdListGamma = 0;
  }

  if(listOfFillEraToKdsJobsBckgd.size())
    holdListBckgd = new string[listOfFillEraToKdsJobsBckgd.size()];
  if(listOfFillEraToKdsJobsNeutron.size())
    holdListNeutron = new string[listOfFillEraToKdsJobsNeutron.size()];
  if(listOfFillEraToKdsJobsGamma.size())
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


  if(bMergeBolos){
    //set the comamnd for merging
    command= myKdataPath + "/bin/mergeKEdsTree"; //the executeCmd file to be executed by the shell
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
      myNumArgs = 4;
      myArgs = new string[myNumArgs];
      
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopOneBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfFillEraToKdsJobsBckgd.size(), holdListBckgd));
      
      in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      in2.replace(in2.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      
      myFileName = "mergeKdsTree_Neutron_step1";
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopOneNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfFillEraToKdsJobsNeutron.size(), holdListNeutron));
      
      in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      in2.replace(in2.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      
      myFileName = "mergeKdsTree_Gamma_step1";
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopOneGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfFillEraToKdsJobsGamma.size(), holdListGamma));
      
      delete [] myArgs;
      
    }
   
    if(holdListBckgd != 0){
      delete[] holdListBckgd;
      holdListBckgd = 0;
    }
    if(holdListNeutron != 0) {
      delete[] holdListNeutron;
      holdListNeutron = 0;
    }
    if(holdListGamma != 0) {
      delete[] holdListGamma;
      holdListGamma = 0;
    }
       
    if(listOfJobsMergeLoopOneBckgd.size())
      holdListBckgd = new string[listOfJobsMergeLoopOneBckgd.size()];
    if(listOfJobsMergeLoopOneNeutron.size())
      holdListNeutron = new string[listOfJobsMergeLoopOneNeutron.size()];
    if(listOfJobsMergeLoopOneGamma.size())
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
      myNumArgs = 4;
      myArgs = new string[myNumArgs];
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopTwoBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopOneBckgd.size(), holdListBckgd));
      
      in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      in2.replace(in2.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
      
      myFileName = "mergeKdsTree_Neutron_step2";
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopTwoNeutron.push_back(submitCommand(myFileName.Data(), command.Data(),  myNumArgs, myArgs, listOfJobsMergeLoopOneNeutron.size(), holdListNeutron));
      
      in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      in2.replace(in2.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
      
      myFileName = "mergeKdsTree_Gamma_step2";
      myArgs[0] = in1;
      myArgs[1] = in2;
      myArgs[2] = out;
      myArgs[3] = log;
      
      listOfJobsMergeLoopTwoGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopOneGamma.size(), holdListGamma));
      delete [] myArgs;
    }
    
    if(holdListBckgd != 0){
      delete[] holdListBckgd;
      holdListBckgd = 0;
    }
    if(holdListNeutron != 0) {
      delete[] holdListNeutron;
      holdListNeutron = 0;
    }
    if(holdListGamma != 0) {
      delete[] holdListGamma;
      holdListGamma = 0;
    }

    
    if(listOfJobsMergeLoopTwoBckgd.size())
      holdListBckgd = new string[listOfJobsMergeLoopTwoBckgd.size()];
    if(listOfJobsMergeLoopTwoBckgd.size())
      holdListNeutron = new string[listOfJobsMergeLoopTwoNeutron.size()];
    if(listOfJobsMergeLoopTwoBckgd.size())
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
    myNumArgs = 4;
    myArgs = new string[myNumArgs];
    
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopThreeBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfJobsMergeLoopTwoBckgd.size(), holdListBckgd));
    
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    in2.replace(in2.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Neutron_step3";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopThreeNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfJobsMergeLoopTwoNeutron.size(), holdListNeutron));
    
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    in2.replace(in2.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Gamma_step3";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopThreeGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopTwoGamma.size(), holdListGamma));
    
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
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopThreeBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopTwoBckgd.size(), holdListBckgd));
    
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    in2.replace(in2.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Neutron_step3b";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopThreeNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopTwoNeutron.size(), holdListNeutron));
    
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    in2.replace(in2.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Gamma_step3b";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    
    listOfJobsMergeLoopThreeGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,listOfJobsMergeLoopTwoGamma.size(), holdListGamma));
    
    if(holdListBckgd != 0){
      delete[] holdListBckgd;
      holdListBckgd = 0;
    }
    if(holdListNeutron != 0) {
      delete[] holdListNeutron;
      holdListNeutron = 0;
    }
    if(holdListGamma != 0) {
      delete[] holdListGamma;
      holdListGamma = 0;
    }
       
    if(listOfJobsMergeLoopThreeBckgd.size()) 
      holdListBckgd = new string[listOfJobsMergeLoopThreeBckgd.size()];
    if(listOfJobsMergeLoopThreeNeutron.size())
      holdListNeutron = new string[listOfJobsMergeLoopThreeNeutron.size()];
    if(listOfJobsMergeLoopThreeGamma.size())
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
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopFourBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopThreeBckgd.size(), holdListBckgd));
    
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    in2.replace(in2.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    log.replace(log.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(),kNeutronDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Neutron_step4";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopFourNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfJobsMergeLoopThreeNeutron.size(), holdListNeutron));
    
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    in2.replace(in2.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    log.replace(log.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(),kGammaDirectoryName.c_str() );
    
    myFileName = "mergeKdsTree_Gamma_step4";
    myArgs[0] = in1;
    myArgs[1] = in2;
    myArgs[2] = out;
    myArgs[3] = log;
    
    listOfJobsMergeLoopFourGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfJobsMergeLoopThreeGamma.size(), holdListGamma));
    
    delete [] myArgs;
  }
   
  if(bFillMuons){
    //merge Muon Veto Data 
    //the next step for the Bckgd data is to merge the Muon Veto Events
    //but, first we make sure the muon veto data is properly filled with the latest data format
    //and we add these jobs to the listOfJobsMergeLoopFourBckgd.
    command= myKdataPath + "/bin/fillMuonVetoEvents"; //the executeCmd file to be executed by the shell
    
    Int_t vetoRuns = stopMuonRun - startMuonRun + 1;
    string *myArrayOfMuonRuns = new string[vetoRuns];  
    cout << "Creating " << vetoRuns << " muon veto files. " << endl;
    
    listOfFillMuonJobs.clear(); //should already be empty at this point.
    for(Int_t i = startMuonRun; i <= stopMuonRun; i++) {
      
      TString muonIn = fMuonVetoDataInPath;
      muonIn += "Events_Run";
      muonIn += i;
      muonIn += "_Laser_time.root";
      
      TString muonOut = fMuonVetoDataOutPath;
      muonOut += "KdsRun12_";
      muonOut += kVersionNumber;
      muonOut += "_MuonVetoRun";
      muonOut += i;
      muonOut += ".root";
      
      myArrayOfMuonRuns[i-startMuonRun] = muonOut.Data();
      
      myFileName = "fillMuonVetoEvents"; 
      myNumArgs = 2;
      myArgs = new string[myNumArgs];
      myArgs[0] = muonIn.Data();
      myArgs[1] = muonOut.Data();
      
      if(bFillMuons) listOfFillMuonJobs.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs));
      else 
	cout << "Skipping Muon Veto Fill " << i << endl;
      
      delete [] myArgs;
    }
    
  }

  if(bConcatenateMuons){
    //need to concatenate the Muon Vetos into a single file
    if(holdListMuon != 0) {
      delete [] holdListMuon;
      holdListMuon = 0;
    }
    if(listOfFillMuonJobs.size()) 
      holdListMuon = new string[listOfFillMuonJobs.size()];
    for(UInt_t k = 0; k < listOfFillMuonJobs.size(); k++){
      holdListMuon[k] = listOfFillMuonJobs.at(k);
    }
    myNumArgs = vetoRuns + 1;
    myArgs = new string[myNumArgs];
    myArgs[0] = outMuonVetoCombinedFile;
    
    for(UInt_t k = 0; k < vetoRuns; k++){
      myArgs[k+1] = myArrayOfMuonRuns[k];
    }
    myFileName = "concatenateMuonVeto";
    
    command= myKdataPath + "/bin/concatDst 5"; //the executeCmd file to be executed by the shell. the '5' is really an argument.. but i put it here.
    listOfJobsMergeLoopFourBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfFillMuonJobs.size(), holdListMuon ));
    
    delete [] myArgs;
  }

  if(holdListBckgd != 0){
    delete[] holdListBckgd;
    holdListBckgd = 0;
  }
  if(holdListNeutron != 0) {
    delete[] holdListNeutron;
    holdListNeutron = 0;
  }
  if(holdListGamma != 0) {
    delete[] holdListGamma;
    holdListGamma = 0;
  }
  
  if(listOfJobsMergeLoopFourBckgd.size()) holdListBckgd = new string[listOfJobsMergeLoopFourBckgd.size()];
  if(listOfJobsMergeLoopFourNeutron.size()) holdListNeutron = new string[listOfJobsMergeLoopFourNeutron.size()];
  if(listOfJobsMergeLoopFourGamma.size()) holdListGamma = new string[listOfJobsMergeLoopFourGamma.size()];
  
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeLoopFourBckgd.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourNeutron.size(); k++){
    holdListNeutron[k] = listOfJobsMergeLoopFourNeutron.at(k);
  }
  for(UInt_t k = 0; k < listOfJobsMergeLoopFourGamma.size(); k++){
    holdListGamma[k] = listOfJobsMergeLoopFourGamma.at(k);
  }
  
  //string bBigNeutron = "0"; //set to 1 if you want the strong AmBe source data added to the output file when merging the muon veto data
  //string kEntryOfBoloStart = "272394"; //need to tell the muon veto merging to start here
  //string kDebugMuonVeto = "-24129205"; //muon veto merge debug flag??
  

  vector<string> listOfJobsMergeMuonVetoBckgd;
  command= myKdataPath + "/bin/mergeKEdsTree"; //the executeCmd file to be executed by the shell
  in1 = kOutputPath;
  in1.append("Kds_AllBolos.root");
  in2 = outMuonVetoCombinedFile;
  //  in2.append("KdsRun12_MuonVetoAll.root");
  out = kOutputPath;
  out.append("Kds_AllBolosVeto.root");
  log = kOutputPath;
  log.append("Merge_AllBolosVeto.log");
  
  myFileName = "mergeKdsTree_MuonVetoAndBckgd";
  myNumArgs = 7;
  myArgs = new string[myNumArgs];
  
  myArgs[0] = in1;
  myArgs[1] = in2;
  myArgs[2] = out;
  myArgs[3] = log;
  myArgs[4] = kEntryOfBoloStart;
  myArgs[5] = bBigNeutron;
  myArgs[6] = kDebugMuonVeto;
  
  if(bMergeBolosAndMuon) listOfJobsMergeMuonVetoBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopFourBckgd.size(), holdListBckgd));

  delete [] myArgs;

  if(holdListBckgd != 0) {
    delete[] holdListBckgd;
    holdListBckgd = 0;
  }

  if(listOfJobsMergeMuonVetoBckgd.size()) holdListBckgd = new string[listOfJobsMergeMuonVetoBckgd.size()];
  for(UInt_t k = 0; k < listOfJobsMergeMuonVetoBckgd.size(); k++){
    holdListBckgd[k] = listOfJobsMergeMuonVetoBckgd.at(k);
  }

  //allPassOne_GSEventNumber -- add the event numbers;
  vector<string> allPassOneJobNameBckgd;
  vector<string> allPassOneJobNameNeutron;
  vector<string> allPassOneJobNameGamma;

  if(bAddGlobalSoftwareEventNumber){
    command = myKdataPath + "/bin/allPassOne_GSEventNumbers"; //the executeCmd  
    in1 = kOutputPath;
    in1.append("Kds_AllBolosVeto.root");
    out = kFinalPath;
    out.append("Kds_Run12_");
    out.append(kVersionNumber.c_str());
    out.append(".root");
    
    myFileName = "allPassOne_GSEventNumberBckgd";
    myNumArgs = 2;
    myArgs = new string[myNumArgs];
    
    myArgs[0] = in1;
    myArgs[1] = out;
    
    allPassOneJobNameBckgd.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeMuonVetoBckgd.size(), holdListBckgd));
    
    //now do the allPassOne for the Neutron and Gamma data after the merge step five
    in1 = kOutputPath;
    in1.append("Kds_AllBolos.root");
    
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    
    myFileName = "allPassOne_GSEventNumberNeutron";
    myArgs[0] = in1;
    myArgs[1] = out;
    allPassOneJobNameNeutron.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs,  listOfJobsMergeLoopFourNeutron.size(), holdListNeutron));
    
    
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    
    myFileName = "allPassOne_GSEventNumberGamma";
    myArgs[0] = in1;
    myArgs[1] = out;
    allPassOneJobNameGamma.push_back(submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, listOfJobsMergeLoopFourGamma.size(), holdListGamma));

    delete [] myArgs;
  }

  if(holdListBckgd != 0){
    delete[] holdListBckgd;
    holdListBckgd = 0;
  }
  if(holdListNeutron != 0) {
    delete[] holdListNeutron;
    holdListNeutron = 0;
  }
  if(holdListGamma != 0) {
    delete[] holdListGamma;
    holdListGamma = 0;
  }

  if(allPassOneJobNameBckgd.size()) holdListBckgd = new string[allPassOneJobNameBckgd.size()];
  if(allPassOneJobNameNeutron.size()) holdListNeutron = new string[allPassOneJobNameNeutron.size()];
  if(allPassOneJobNameGamma.size()) holdListGamma = new string[allPassOneJobNameGamma.size()];
  
  for(UInt_t k = 0; k < allPassOneJobNameBckgd.size(); k++){
    holdListBckgd[k] = allPassOneJobNameBckgd.at(k);
  }
  for(UInt_t k = 0; k < allPassOneJobNameNeutron.size(); k++){
    holdListNeutron[k] = allPassOneJobNameNeutron.at(k);
  }
  for(UInt_t k = 0; k < allPassOneJobNameGamma.size(); k++){
    holdListGamma[k] = allPassOneJobNameGamma.at(k);
  }
  
  if(bSkimBoloNoiseEvents){
    //now skim all files
    command = myKdataPath + "/bin/skimNoNoiseBolos"; //the executeCmd  
    in1 = kFinalPath;
    in1.append("Kds_Run12_");
    in1.append(kVersionNumber.c_str());
    in1.append(".root");
    out = kFinalPath;
    out.append("Kds_Run12_");
    out.append(kVersionNumber.c_str());
    out.append("_skim.root");
    
    myNumArgs = 2;
    myArgs = new string[myNumArgs];
    
    myFileName = "skimNoNoiseBolos_Bckgd";
    myArgs[0] = in1;
    myArgs[1] = out;
    
    submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, allPassOneJobNameBckgd.size(), holdListBckgd);
    
    in1.replace(in1.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    out.replace(out.find(kWimpDirectoryName.c_str()), kWimpDirectoryName.size(), kNeutronDirectoryName.c_str());
    
    myFileName = "skimNoNoiseBolos_Neutron";
    myArgs[0] = in1;
    myArgs[1] = out;
    
    submitCommand(myFileName.Data(), command.Data(),  myNumArgs, myArgs, allPassOneJobNameNeutron.size(), holdListNeutron);
    
    in1.replace(in1.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    out.replace(out.find(kNeutronDirectoryName.c_str()), kNeutronDirectoryName.size(), kGammaDirectoryName.c_str());
    
    myFileName = "skimNoNoiseBolos_Gamma";
    myArgs[0] = in1;
    myArgs[1] = out;
    submitCommand(myFileName.Data(), command.Data(), myNumArgs, myArgs, allPassOneJobNameGamma.size(), holdListGamma);
  }

  //end of automatic processing
}
