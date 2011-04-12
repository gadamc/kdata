/**
   @file    EdwRawDB.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwRawDB class. See the .h file
*/

#include "EdwRawDB.h"

ClassImp(EdwRawDB); /**< Root dictionnary */

EdwRawDB::EdwRawDB() : EdwDB() {

  fAutomateTree = new TTree("AutomateTree","AutomateTree");
  fAutomateTree->SetDirectory(fFile);
  fRunTree = new TTree("RunTree","RunTree");
  fRunTree->SetDirectory(fFile);
  fSetupTree = new TTree("SetupName","SetupName");
  fSetupTree->SetDirectory(fFile);

  fAutomateData = new EdwAutomateData();
  fRunData = new EdwRunData();
  fSetupData = new EdwSetupData();

  fAutomateTree->Branch("AutomateBranch","EdwAutomateData",&fAutomateData);
  fAutomateBranch = fAutomateTree->GetBranch("AutomateBranch");
  fRunTree->Branch("RunBranch","EdwRunData",&fRunData);
  fRunBranch = fRunTree->GetBranch("RunBranch");
  fSetupTree->Branch("SetupBranch","EdwSetupData",&fSetupData);
  fSetupBranch = fSetupTree->GetBranch("SetupBranch");

}

EdwRawDB::EdwRawDB(string aFile, string aRead) : EdwDB(aFile,aRead) {

  fAutomateData = new EdwAutomateData();
  fRunData = new EdwRunData();
  fSetupData = new EdwSetupData();

  if (fFile->Get("AutomateTree")) {
    fAutomateTree = (TTree*)fFile->Get("AutomateTree");
    fAutomateBranch = fAutomateTree->GetBranch("AutomateBranch");
    fAutomateBranch->SetAddress(&fAutomateData);
  } else {
    fAutomateTree = new TTree("AutomateTree","AutomateTree");
    fAutomateTree->SetDirectory(fFile);
    fAutomateTree->Branch("AutomateBranch","EdwAutomateData",&fAutomateData);
    fAutomateBranch = fAutomateTree->GetBranch("AutomateBranch");
  }
  if (fFile->Get("RunTree")) {
    fRunTree = (TTree*)fFile->Get("RunTree");
    fRunBranch = fRunTree->GetBranch("RunBranch");
    fRunBranch->SetAddress(&fRunData);
  } else {
    fRunTree = new TTree("RunTree","RunTree");
    fRunTree->SetDirectory(fFile);
    fRunTree->Branch("RunBranch","EdwRunData",&fRunData);
    fRunBranch = fRunTree->GetBranch("RunBranch");
  }
  if (fFile->Get("SetupTree")) {
    fSetupTree = (TTree*)fFile->Get("SetupTree");
    fSetupBranch = fSetupTree->GetBranch("SetupBranch");
    fSetupBranch->SetAddress(&fSetupData);
  } else {
    fSetupTree = new TTree("SetupTree","SetupTree");
    fSetupTree->SetDirectory(fFile);
    fSetupTree->Branch("SetupBranch","EdwSetupData",&fSetupData);
    fSetupBranch = fSetupTree->GetBranch("SetupBranch");
  }

  if (!fAutomateBranch || !fRunBranch || !fSetupBranch) {
    cerr << "EdwRawDB: at least one missing branch in file. Exiting."<< endl; exit(-1);
  }

}

EdwRawDB::~EdwRawDB() {

}

void EdwRawDB::FillAutomateData(string aFile) {

  // Read the automate file and fill the branch.
  fAutomateData->Clear();

  // Constants needed to convert the Excel time of automate data to Unix time
  Double_t kShiftTime_Day = 24107 ; // between 1904 and 1970, for automate file reading
  Double_t kShiftTime_Sec = kShiftTime_Day*3600*24;

  // Automate file header
  ifstream lFile(aFile.c_str(),ios::in) ;
  if (lFile.bad()) cerr << "Warning, Automate data file not correctly open" << endl;
  string field;
  getline(lFile,field);
  int pt_max = lFile.tellg();
  unsigned int n_field=0;
  int i_tbolo=-1, i_date=-1, i_basource1=-1, i_basource2=-1,
    i_cosource1=-1, i_cosource2 = -1;
  lFile.seekg(0);
  while(lFile.tellg() <= pt_max-3) { // need this "3" offset, don't know why!
    lFile >> field ;
    if (n_field == 0 && field == "Time") i_date = 0;
    if (field == "T_Bolo") i_tbolo = n_field;
    if (field == "S_grotte") i_basource1 = n_field; // a bien mieux gerer bien sur!
    if (field == "S_nemo") i_basource2 = n_field;
    n_field++;
  }
  lFile.seekg(0);
  getline(lFile,field);
  
  // Binary data reading.
  // Byte ordering is big endian in these files too.. Put by hand for the moment..
  // I didn't find a routine to byteswap 64-bit types, so I do it by myself...
  while (! lFile.eof()) {
    char* array = new char[n_field*8];
    lFile.read(array,n_field*8);
    if (! lFile.eof()) {
      char* permarray = new char[n_field*8];
      for (UShort_t i=0;i<n_field*8;i++) permarray[i]=array[n_field*8-i-1];
      Double_t* tmparray = (Double_t*)permarray;
      Double_t* lArray = new Double_t[n_field];
      for (UShort_t i=0;i<n_field;i++) lArray[i]=tmparray[n_field-i-1];
      // Fill the AutomateData structure
      fAutomateData->fDate = (i_date == -1) ? 0 : (time_t)(lArray[i_date]-kShiftTime_Sec) ;
      fAutomateData->fTBolo = (i_tbolo == -1) ? 0 : lArray[i_tbolo] ;
      fAutomateData->fBaSource1 = (i_basource1 == -1) ? 0 : lArray[i_basource1] ;
      fAutomateData->fBaSource2 = (i_basource2 == -1) ? 0 : lArray[i_basource2] ;
      fAutomateData->fCoSource1 = (i_cosource1 == -1) ? 0 : lArray[i_cosource1] ;
      fAutomateData->fCoSource2 = (i_cosource2 == -1) ? 0 : lArray[i_cosource2] ;
      fFile->cd(); fAutomateBranch->Fill();
      fAutomateData->Clear();
    }
  } ;
  fFile->cd(); fAutomateTree->Write("",kOverwrite);
  if (fFile->IsZombie()) {
    cerr << "FillAutomateData: fFile is zombie. Exiting." << endl; exit(-1);
  }
  lFile.close();

}

void EdwRawDB::FillRunData(string aRunDir, string aRunName, ByteOrderType aByteOrder) {
  // Read various run log files and fills the branch
  string field, thechannel, thebolo;
  size_t pos1, pos2;
  struct stat lFileInfo;
  int lVersion = 0; // a single int to have an unambiguous version flag

  // may do some tests with this before...
  fRunData->Clear(); // Major point! (case of a rawdb completion)

  // First filling the "basic" fields
  fRunData->fByteOrdering = aByteOrder;
  fRunData->fRunName = aRunName ;
  FILE* lSystemPipe;
  char thechar[300];
  lSystemPipe = popen(("ls "+aRunDir+"/"+aRunName+"/"+aRunName+"_0*").c_str(),"r");
  while (fgets(thechar,300,lSystemPipe)) {
    field = thechar;
    pos1 = field.find_last_of("/");
    pos2 = field.find("\n");
    if (pos1 != string::npos)(fRunData->fRunFiles).push_back(field.substr(pos1+1,pos2-pos1-1));
  }
  pclose(lSystemPipe);

  // Parsing the log file
  ifstream lLogFile((aRunDir+"/"+aRunName+"/"+aRunName+"_log").c_str(),ios::in) ;
  if (lLogFile.bad()) cerr << "Warning, Run Log file not correctly open" << endl;
  while (!lLogFile.eof()) {
    getline(lLogFile,field);
    if (field.find("avec la version") != string::npos) {
      pos1 = field.find("version") ;
      pos2 = field.find(")");
      fRunData->fSambaVersion = field.substr(pos1+8,pos2-pos1-8);
      pos1 = (fRunData->fSambaVersion).find(".");
      pos2 = (fRunData->fSambaVersion).find(".",pos1+1);
      lVersion = atoi(((fRunData->fSambaVersion).substr(0,pos1)).c_str())*100 
	+ atoi(((fRunData->fSambaVersion).substr(pos1+1,pos2-pos1)).c_str());
    }
    if (field.find("blocs lus") != string::npos) {
      pos1 = field.find("(");
      pos2 = field.find(")");
      fRunData->fSambaSecondsRead = (ULong_t)atof(field.substr(pos1+1,pos2-pos1-1).c_str()) ;
    }
    if (field.find("Exposition") != string::npos) {
      pos1 = field.find(":");
      pos2 = field.find("Kg");
      fRunData->fSambaExposure = atof((field.substr(pos1+1,pos2-pos1-1)).c_str()) ;
    }
    if (field.find("valeurs non traitees") != string::npos) {
      pos1 = field.find("(");
      pos2 = field.find("s");
      fRunData->fSambaSecondsNotTreated = (ULong_t)atof((field.substr(pos1+1,pos2-pos1-1)).c_str()) ;
    }
  }
  lLogFile.close();
  lLogFile.clear();

  // Parsing the list of identified channels
  lLogFile.open((aRunDir+"/"+aRunName+"/"+aRunName+"_log").c_str(),ios::in);
  if (lVersion < 900) {
    if (lVersion >= 723) {
      while (field.find("Identification des voies") == string::npos)
	getline(lLogFile,field);
    } else {
      while (field.find("Identification des numeriseurs") == string::npos) 
	getline(lLogFile,field);
    }
    for (unsigned int i=0; i<4; i++) getline(lLogFile,field);
    while (field.find("____") == string::npos) {
      if (lVersion >= 808 && field.find("SuperCluzel") != string::npos) {
	getline(lLogFile,field); getline(lLogFile,field);
      }
      if (lVersion >= 808 && field.find("OPERA") != string::npos) {
	for (unsigned int i=0; i<3; i++) getline(lLogFile,field);
      }
      if (lVersion >= 808 && field.find("SuperCluzel") != string::npos) {
	getline(lLogFile,field); getline(lLogFile,field);
      }
      if (lVersion >= 802) thechannel = field.substr(50,25);
      else thechannel = field.substr(46,25);
      if (thechannel.find("invalidee") == string::npos &&
	  thechannel.find("inutilisee") == string::npos ) {
	if (thechannel.find("chaleur") == string::npos && 
	    thechannel.find("centre") == string::npos && 
	    thechannel.find("garde") == string::npos) {
	  cerr << "Warning: channel "<<thechannel<<" is not conventionnal." << endl;
	  cerr << "Run "<<aRunName<<endl; exit(-1);
	}
	pos1 = thechannel.find_first_not_of(" ");
	pos2 = thechannel.find_last_not_of(" ");
	(fRunData->fChannelNames).push_back(thechannel.substr(pos1,pos2-pos1+1));
      }
      getline(lLogFile,field);
    }
  } else { // We look for the list of channels in another place
    while (field.find("Recherche d'evenements") == string::npos) getline(lLogFile,field);
    while (field.find("  voie  ") == string::npos) getline(lLogFile,field);
    for (unsigned int i=0; i<2; i++) getline(lLogFile,field);
    while (field.find("____") == string::npos) {
      thechannel = field.substr(1,18);
      if (thechannel.find("chaleur") == string::npos && 
	  thechannel.find("centre") == string::npos && 
	  thechannel.find("garde") == string::npos) {
	cerr << "Warning: channel "<<thechannel<<" is not conventionnal." << endl;
	cerr << "Run "<<aRunName<<endl; exit(-1);
      }
      pos1 = thechannel.find_first_not_of(" ");
      pos2 = thechannel.find_last_not_of(" ");
      (fRunData->fChannelNames).push_back(thechannel.substr(pos1,pos2-pos1+1));
      getline(lLogFile,field);
    }
  }
  lLogFile.close();
  lLogFile.clear();

  // Get the corresponding voltages
  // (simplified for now. with an access to a setup db, can make cases for id, ge-ntd, nbsi...)
  lLogFile.open((aRunDir+"/"+aRunName+"/"+aRunName+"_log").c_str(),ios::in);
  for (unsigned int i=0;i<(fRunData->fChannelNames).size();i++) {
    thechannel = (fRunData->fChannelNames).at(i);
    pos1 = thechannel.find(" ");
    thebolo = thechannel.substr(pos1+1);
    thechannel = thechannel.substr(0,pos1);
    lLogFile.seekg(0);
    while (field.find("(detecteur: "+thebolo) == string::npos) 
      getline(lLogFile,field);
    // "Deblocage des dacs"..
    pos2 = lLogFile.tellg();
    getline(lLogFile,field);
    if (field.find("Deblocage") != string::npos) {
      while (field.find("Fin de la sequence") == string::npos) {
	pos2 = lLogFile.tellg();
	getline(lLogFile,field);
      }
    }
    // here there should be some "case" for Nbsi, ID, GeNtd
    if (thechannel == "chaleur") {
      while (field.find("ampl-modul") == string::npos) getline(lLogFile,field);
      pos1 = field.find("=");
      (fRunData->fHeatModulations).push_back(atof((field.substr(pos1+1)).c_str()));
      lLogFile.seekg(pos2);
      while (field.find(" d2 =") == string::npos) getline(lLogFile,field);
      pos1 = field.find("=");
      (fRunData->fHeatModulationPoints).push_back(atoi((field.substr(pos1+1)).c_str()));
      (fRunData->fIonizationVoltages).push_back(0);
      (fRunData->fRelativeVoltages).push_back(0);
    } else { // ionisation
      // CASE OF InterDigit IS TAKEN INTO ACCOUNT.
      if (thebolo.substr(0,2) == "ID" || thebolo.substr(0,3) == "FID") {
	if (thechannel == "centre") {
	  if (lVersion < 900) {
	    // Change in polar consigne starting run 11.
	    // Before run 11: centre = polar-voie3, garde = polar-ionis
	    // After run 11: centre = polar-ionis, garde = polar-voie3
	    char lYear = (fRunData->fRunName)[0];
	    if (lYear <= 'i') {
	      while (field.find("polar-voie3") == string::npos) getline(lLogFile,field);
	    } else {
	      while (field.find("polar-ionis") == string::npos) getline(lLogFile,field);
	    }
	  } else {
	    while (field.find("polar-centre") == string::npos) getline(lLogFile,field);
	  }
	} else {
	  if (lVersion < 900) {
	    char lYear = (fRunData->fRunName)[0];
	    if (lYear <= 'i') {
	      while (field.find("polar-ionis") == string::npos) getline(lLogFile,field);
	    } else {
	      while (field.find("polar-voie3") == string::npos) getline(lLogFile,field);
	    }
	  } else {
	    while (field.find("polar-garde") == string::npos) getline(lLogFile,field);
	  }
	}
      } else { // GeNTD old generation : same voltage for centre and garde
	if (lVersion < 900) {
	  while (field.find("polar-ionis") == string::npos) getline(lLogFile,field);
	} else {
	  while (field.find("polar-centre") == string::npos) getline(lLogFile,field);
	}
      }

      pos1 = field.find("=");
      (fRunData->fIonizationVoltages).push_back(atof((field.substr(pos1+1)).c_str()));
      (fRunData->fRelativeVoltages).push_back(atof((field.substr(pos1+1)).c_str()));
      (fRunData->fHeatModulations).push_back(0);
      (fRunData->fHeatModulationPoints).push_back(0);
    }
  }
  // Compute the relative voltages for ID detectors !!
  vector<string> listidbolos; // first have the list of IDs
  for (unsigned int i=0;i<(fRunData->fChannelNames).size();i++) {
    thechannel = (fRunData->fChannelNames).at(i);
    pos1 = thechannel.find(" ");
    thebolo = thechannel.substr(pos1+1);
    thebolo = thebolo.substr(0,thebolo.length()-2); // remove "AB/CD/GH"
    if (thebolo.substr(0,2) == "ID" || thebolo.substr(0,3) == "FID") {
      bool flag=1;
      for (UInt_t p=0; p<listidbolos.size(); p++) {
	if (thebolo == listidbolos[p]) flag = 0;
      }
      if (flag) listidbolos.push_back(thebolo);
    }
  }
  vector<Float_t> listidvmean(listidbolos.size(),0); // Get mean voltage for each ID bolo
  for (UInt_t p=0; p<listidbolos.size(); p++) {
    Int_t nbelectrodes=0;
    for (unsigned int i=0;i<(fRunData->fChannelNames).size();i++) {
      thechannel = (fRunData->fChannelNames).at(i);
      if (thechannel.find("chaleur") == string::npos) {
	pos1 = thechannel.find(" ");
	thechannel = thechannel.substr(pos1+1);
	thechannel = thechannel.substr(0,thechannel.length()-2);
	if (thechannel == listidbolos[p]) {
	  (listidvmean[p])+=(fRunData->fIonizationVoltages[i]);
	  nbelectrodes++;
	}
      }
    }
    (listidvmean[p]) /= nbelectrodes;
  }

  for (unsigned int i=0;i<(fRunData->fChannelNames).size();i++) {
    thechannel = (fRunData->fChannelNames).at(i);
    for (UInt_t p=0; p<listidbolos.size(); p++) {
      if (thechannel.find("chaleur") == string::npos) {
	pos1 = thechannel.find(" ");
	string lbolo = thechannel.substr(pos1+1);
	lbolo = lbolo.substr(0,lbolo.length()-2);
	if (lbolo == listidbolos[p])
	  (fRunData->fRelativeVoltages[i]) -= listidvmean[p];
      }
    }
  }
  lLogFile.close();
  lLogFile.clear();
  unsigned int nbchannels=(fRunData->fChannelNames).size();

  // Thresholds. First check if there are variable triggers.
  int lStat = stat((aRunDir+"/"+aRunName+"/"+aRunName+"_seuils").c_str(),&lFileInfo);

  if (!lStat) { // adaptative trigger
    ifstream lThreshFile((aRunDir+"/"+aRunName+"/"+aRunName+"_seuils").c_str(),ios::in);
    getline(lThreshFile,field);
    istringstream ss(field);
    ss >> field;
    vector<string> lThreshList;
    while (ss >> field) lThreshList.push_back(field);
    vector<int> lChannelList;
    for (unsigned int i=0;i<lThreshList.size();i++) {
      for (unsigned int j=0;j<nbchannels;j++) {
	string lchan=(fRunData->fChannelNames)[j];
	pos1=lchan.find(" ");
	string s1=lchan.substr(0,pos1-1);
	string s2=lchan.substr(pos1+1);
	if ( lThreshList[i].find(s1) != string::npos && lThreshList[i].find(s2) != string::npos ) lChannelList.push_back(j);
      }
    }
    if (lChannelList.size() != lThreshList.size()) {
      cerr << "Pbl threshold file, run "<<aRunName << endl;
      for (UInt_t i=0;i<lThreshList.size();i++) cerr<<lThreshList[i]<<" ";
      cerr << endl;
      for (UInt_t j=0;j<nbchannels;j++) cerr<<(fRunData->fChannelNames)[j]<<" ";
      cerr << endl;
      for (UInt_t i=0;i<lChannelList.size();i++) cerr<<lChannelList[i]<<" ";
      cerr << endl;
    }
    getline(lThreshFile,field);
    int jj=0;
    while (! lThreshFile.eof()) {
      istringstream lss(field);
      time_t toto;
      lss >> toto;
      (fRunData->fThresholdsTimes).push_back(toto);
      (fRunData->fThresholdsPos).push_back(vector<Float_t>(nbchannels,0));
      (fRunData->fThresholdsNeg).push_back(vector<Float_t>(nbchannels,0));
      float tata;
      int i=0;
      while (lss >> tata) {
	if ((lThreshList[i]).find("neg") != string::npos)
	  (fRunData->fThresholdsNeg)[jj][lChannelList[i]]=tata;
	else (fRunData->fThresholdsPos)[jj][lChannelList[i]]=tata;
	i+=1;	
      }
      getline(lThreshFile,field);
      jj+=1;
    }
    lThreshFile.close();

  } else { // fixed trigger, from the log file
    (fRunData->fThresholdsTimes).push_back(0); // a ameliorer apres
    // avec par exemple le temps du 1er evt d'apres le fichier _000 ou _ntp..
    (fRunData->fThresholdsPos).push_back(vector<Float_t>(nbchannels,0));
    (fRunData->fThresholdsNeg).push_back(vector<Float_t>(nbchannels,0));
    lLogFile.open((aRunDir+"/"+aRunName+"/"+aRunName+"_log").c_str(),ios::in);
    while (field.find("condition") == string::npos) getline(lLogFile,field);
    getline(lLogFile,field);
    do {
      getline(lLogFile,field);
      // first get the corresponding channel
      pos1=field.find("|"); 
      string thename=field.substr(pos1+1);
      pos2=thename.find("|"); 
      thename = thename.substr(0,pos2-1);
      pos1 = thename.find_first_not_of(" ");
      pos2 = thename.find_last_not_of(" ");
      thename = thename.substr(pos1,pos2-pos1+1);
      int jj = 0;
      for (unsigned int i=0; i<nbchannels;i++) {
	if (thename == (fRunData->fChannelNames)[i]) jj=i;
      }
      // then get the threshold
      istringstream ss(field);
      while (ss >> thename) {
	if (thename.find("<") != string::npos)
	  ss >> ((fRunData->fThresholdsNeg)[0]).at(jj);
	if (thename.find(">") != string::npos)
	  ss >> ((fRunData->fThresholdsPos)[0]).at(jj);
      }
    } while(field.find("______") == string::npos);
    lLogFile.close();
  }

  fFile->cd(); fRunBranch->Fill();
  fRunData->Clear();
  fFile->cd(); fRunTree->Write("",kOverwrite);
  if (fFile->IsZombie()) {
    cerr << "FillRunData: fFile is zombie. Exiting." << endl; exit(-1);
  }
}

void EdwRawDB::FillRunData(string aRunDir, ByteOrderType aByteOrder) {
  
  FILE* lSystemPipe;
  char thechar[300];
  string lRunName;
  lSystemPipe = popen(("ls "+aRunDir).c_str(),"r");
  while (fgets(thechar,300,lSystemPipe)) {
    lRunName = thechar;
    lRunName=lRunName.substr(0,lRunName.find("\n"));
    this->FillRunData(aRunDir,lRunName, aByteOrder);
  }
  pclose(lSystemPipe);
  
}

void EdwRawDB::FillRunData(string aRunDir, vector<string> aRunList, ByteOrderType aByteOrder) {

  vector<string> lExistingRuns = this->GetRunList();
  for (UInt_t i=0; i<aRunList.size(); i++) {
    Bool_t run_exists=0;
    for (UInt_t j=0; j<lExistingRuns.size(); j++) {
      if (aRunList[i] == lExistingRuns[j]) run_exists=1;
    }
    if (!run_exists) this->FillRunData(aRunDir,aRunList[i],aByteOrder);
  }
}

void EdwRawDB::FillAllData(string aAutomateDir, string aRunDir) {

  // Fill the automate tree
  FILE* lSystemPipe;
  char thechar[300];
  string lAutomateFile, lRunName;
  lSystemPipe = popen(("ls "+aAutomateDir+"/*.map").c_str(),"r");
  while (fgets(thechar,300,lSystemPipe)) {
    lAutomateFile = thechar;
    lAutomateFile=lAutomateFile.substr(0,lAutomateFile.find("\n"));
    this->FillAutomateData(lAutomateFile);
  }
  pclose(lSystemPipe);

  // Fill the run data tree
  lSystemPipe = popen(("ls "+aRunDir).c_str(),"r");
  while (fgets(thechar,300,lSystemPipe)) {
    lRunName = thechar;
    lRunName=lRunName.substr(0,lRunName.find("\n"));
    this->FillRunData(aRunDir,lRunName);
  }
  pclose(lSystemPipe);

}

Float_t EdwRawDB::GetTBolo(time_t aTime) {
  // interpolate the TBolo array at a given time.
  // (check if there is some data at that time)

  Float_t lTBolo_interp = 0;

  time_t kDeltat_max = 60*30 ; // 30 minutes
  time_t t0=0; time_t t1=0;
  Float_t tb0=0; Float_t tb1=0;
  fAutomateBranch->GetEntry(0);
  for (Long_t i=1; i< fAutomateBranch->GetEntries(); i++) {
    t0 = fAutomateData->fDate;
    tb0 = fAutomateData->fTBolo;
    fAutomateBranch->GetEntry(i);
    t1 = fAutomateData->fDate;
    tb1 = fAutomateData->fTBolo;
    if (aTime >= t0 && aTime <= t1) {
      if (t1-t0 > kDeltat_max) cerr << "deltat depasse" << endl;
      lTBolo_interp = tb0+(tb1-tb0)*(Float_t)(aTime-t0)/(Float_t)(t1-t0);
    }
  }

  return lTBolo_interp;
}

vector<string> EdwRawDB::GetRunChannels(vector<string> aRunNames) {

  vector<string> lChannelList;
  bool ok1, ok2;
  // Loop on all runs listed in the DB
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    // Check if the run fits to the list
    ok1=0;
    for (UInt_t i=0;i<aRunNames.size();i++) {
      if (fRunData->fRunName == aRunNames.at(i)) ok1=1;
    }
    if (ok1) {
      for (UInt_t i=0;i<(fRunData->fChannelNames).size();i++) {
	string lName = (fRunData->fChannelNames).at(i);
	// Check that the channel is new
	ok2=1;
	for (UInt_t j=0; j<lChannelList.size(); j++) {
	  if (lName == lChannelList.at(j)) ok2=0;
	}
	if (ok2) lChannelList.push_back(lName);
      }
    }
  }

  return lChannelList;
}

vector<string> EdwRawDB::GetRunList() {
  vector<string> lRunList;
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    lRunList.push_back(fRunData->fRunName);
  }
  return lRunList;
}

Float_t EdwRawDB::GetVoltage(string aRun, string aChannel) {
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    if (fRunData->fRunName == aRun) return fRunData->GetVoltage(aChannel);
  }
  
  return 0;
}

Short_t EdwRawDB::GetVoltageSign(string aRun, string aChannel) {
  Float_t lVolt = this->GetVoltage(aRun,aChannel);
  if (lVolt != 0) lVolt /= TMath::Abs(lVolt);
  return (Short_t)lVolt;
}

Int_t EdwRawDB::GetPatternLength(string aRun, string aChannel) {
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    if (fRunData->fRunName == aRun) return fRunData->GetPatternLength(aChannel);
  }
  return 0;
}

TChain* EdwRawDB::GetEventChain(vector<string> aRunNames, string aDir) {
  TChain* lChain = new TChain("EdwTree");

  // Loop on all runs listed in the DB
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    string lRunName = fRunData->fRunName;
    // Check if the run fits to the list
    bool ok=0;
    for (UInt_t i=0;i<aRunNames.size();i++) {
      if (lRunName == aRunNames.at(i)) ok=1;
    }
    if (ok) { // Add the different partition ROOT files to the chain
      vector<string> lPartitions = fRunData->fRunFiles;
      for (UInt_t ipart=0; ipart<lPartitions.size(); ipart++) {
	string lFile = aDir+"/"+lPartitions[ipart]+".root";
	lChain->Add(lFile.c_str());
      }
    }
  }

  return lChain;
}

TChain* EdwRawDB::GetEventSubChain(vector<string>& aRunNames, string aDir, ULong_t istart, ULong_t istop, ULong_t& i_offset, vector<string>& runnames_restrict, string aLocalDir) {

  // First loop to get the "restricted list" of runnames.
  TChain* lFullChain = new TChain("EdwTree");
  runnames_restrict.clear();
  ULong_t nb_local=0;
  i_offset=0;
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    string lRunName = fRunData->fRunName;
    // Check if the run fits to the list
    bool ok=0;
    for (UInt_t i=0;i<aRunNames.size();i++) {
      if (lRunName == aRunNames.at(i)) ok=1;
    }
    if (ok) { // Add the different partition ROOT files to the chain
      vector<string> lPartitions = fRunData->fRunFiles;
      for (UInt_t ipart=0; ipart<lPartitions.size(); ipart++) {
	string lFile = aDir+"/"+lPartitions[ipart]+".root";
	lFullChain->Add(lFile.c_str());
      }
      nb_local=lFullChain->GetEntries();
      if (nb_local >= istart)  runnames_restrict.push_back(lRunName);
      else i_offset=nb_local;
    }
    if (nb_local > istop) break;
  }
  lFullChain->Reset();// on sait jamais...
  //  lFullChain->Delete(); // on sait jamais tjs..
  delete lFullChain;
  if (runnames_restrict.size() == 0) {
    cout <<"* No run for this istart "<<istart<<endl;
    exit(0);
  }

  // Fill the subchain from runnames_restrict
  TChain* lSubChain = new TChain("EdwTree");
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    string lRunName = fRunData->fRunName;
    // Check if the run fits to the list
    bool ok=0;
    for (UInt_t i=0;i<runnames_restrict.size();i++) {
      if (lRunName == runnames_restrict.at(i)) ok=1;
    }
    if (ok) { // Add the different partition ROOT files to the chain
      vector<string> lPartitions = fRunData->fRunFiles;
      for (UInt_t ipart=0; ipart<lPartitions.size(); ipart++) {
	string lFile = aDir+"/"+lPartitions[ipart]+".root";
	if (aLocalDir != "") { // copy the partition on another local space
	  // (useful in batch to limit the access to sps)
	  system(("cp "+lFile+" "+aLocalDir+"/"+lPartitions[ipart]+".root").c_str());
	  lFile=aLocalDir+"/"+lPartitions[ipart]+".root";
	}
	lSubChain->Add(lFile.c_str());
      }
    }
  }
  
  cout << "* List of runs involved:"<<endl;
  for (UInt_t p=0;p<runnames_restrict.size();p++) cout <<runnames_restrict[p]<<endl;
  cout <<"* Subchain size: "<<lSubChain->GetEntries()<<endl;
  cout <<"* Subchain offset: "<<i_offset<<endl;
  //  aRunNames=runnames_restrict; // astuce..

  return lSubChain;  
}

void EdwRawDB::DumpRunData() {

  cout << "------ Dump of Run Data from the raw DB "<<this->FileName()<<" ------"<<endl;
  for (UInt_t irun=0; irun<fRunBranch->GetEntries(); irun++) {
    fRunBranch->GetEntry(irun);
    cout << "----------------------------------------" << endl;
    fRunData->Dump();
  }
  cout << "----------------------------------------"<<endl;

}

void EdwRawDB::DumpEvent(string aRun, UInt_t aSambaNum, string aRootDir, string aFile, string aChannel) {

  vector<string> lRun;
  lRun.push_back(aRun);
  TChain* lChain = this->GetEventChain(lRun, aRootDir);
  EdwEvent* lEvent = new EdwEvent();
  lChain->SetBranchAddress("Event",&lEvent);
  Bool_t evt_found=0;
  lChain->GetEntry(aSambaNum-1);
  if (lEvent->Header()->Num() == aSambaNum) {
    lEvent->Dump(aFile,aChannel);
  } else {
    for (Long64_t i=0; i<lChain->GetEntries(); i++) {
      lChain->GetEntry(i);
      if (lEvent->Header()->Num() == aSambaNum) {
	evt_found = 1;
	cerr << aSambaNum << " found." << endl;
	lEvent->Dump(aFile,aChannel);
	break;
      }
    }
    if (!evt_found) cerr << "*** DumpEvent: not found (" << aRun <<"," << aSambaNum <<")"<<endl;
  }


  delete lChain;
  // delete lEvent; (?)
}

void EdwRawDB::PlotEvent(string aRun, UInt_t aSambaNum, string aRootDir, string aPlotName) {

  vector<string> lRun;
  lRun.push_back(aRun);
  TChain* lChain = this->GetEventChain(lRun, aRootDir);
  EdwEvent* lEvent = new EdwEvent();
  lChain->SetBranchAddress("Event",&lEvent);
  for (Long64_t i=0; i<lChain->GetEntries(); i++) {
    lChain->GetEntry(i);
    if (lEvent->Header()->Num() == aSambaNum) {
      lEvent->PlotTraces(aPlotName);
      break;
    }
  }
  delete lChain;
  // delete lEvent; (?)
}
