
#include "EdwUtils.h"

#include "EdwRawDB.h"
#include "EdwAnaDB.h"
#include "EdwSambaToRoot.h"
#include "EdwNtuple.h"

// Syntaxe specifique fichiers params a documenter

string gProcessingStep, gBaseDir;
string gRawDBFile, gAnaDBFile;
string gSambaDir, gRootEventDir;
string gRootNtpFile, gRootNtpName;
string gRunListFile, gChannelListFile;
string gAnaDBListFile;
string gPlotDir, gEvtListFile;
UInt_t gIStart, gIStop;
Int_t gTimeStepMinutes;
void ReadParamFile(const char* aParamFile) ;
vector<string> GlobalRunList();
vector<string> GlobalAnaDBList();
vector<string> GlobalChannelList();
vector<Float_t> GlobalChannelAmpls();
vector<Int_t> GlobalChannelBins();
vector<Float_t> GlobalChannelDeltaBins();
void BuildRawDB_Step() ;
void ReadSamba_Step();
void BasicProcess_Step();
void Preprocess_Step();
void BuildTemplate_Step();
void Process_Step();
void Event_Display();

int main(int argc, char* argv[]) {

  // EDelWeiss ROOT ANAlysis routine
  // *******************************
  // A simple analysis chain for test on run 8 data.
  // *******************************

  // Recalcul des weights pour le run en question.
  gAnaDBListFile = "/afs/in2p3.fr/home/a/armengau/RootAna_NTD/TestGGA10/anadblist_test.txt";
  vector<string> AnaDBList = GlobalAnaDBList();
  gAnaDBFile = "/afs/in2p3.fr/home/a/armengau/SPSEdw/tmpdb.root";
  EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
  lDb->FillFromDBs(AnaDBList);
  lDb->CloseFile();
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  vector<string> RunList ;
  RunList.push_back("ia30a004");
  vector<string> Channels ; // Channels to process
  Channels.push_back("chaleur GGA10");

  AnaDb->DumpFrequencyWeights("/afs/in2p3.fr/home/a/armengau/freqw_debug.root",Channels);
  exit(-1);

  
  // analyse avec des filtres creneau en frequence pr voir l'effet sur calib
  ReadParamFile(argv[1]);
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> AnaDBList = GlobalAnaDBList();
  EdwAnaDB* llDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
  llDb->FillFromDBs(AnaDBList);
  llDb->CloseFile();
  EdwAnaDB* llAnaDb = new EdwAnaDB(gAnaDBFile,"UPDATE");
  vector<string> RunList ; // Runs to process
  RunList = GlobalRunList();
  vector<string> Channels ; // Channels to process
  Channels = GlobalChannelList();
  if (Channels.size() != 1 || AnaDBList.size() != 1) cerr <<"oups"<<endl;

  // Here : add a preprocessdata with noise creneau to anadb
  EdwPreprocessData lPreprocess(10,time(NULL),Channels[0]);
  UInt_t size = (llAnaDb->GetTemplate(Channels[0])).Trace().size();
  vector<Float_t> noise(size,1.e10);
  Int_t istart=50, istop=60;
  cout << istart<<" "<<istop<<endl;
  for (int j=istart; j<istop; j++) {
    noise[j]=1;
    noise[size-j-1]=1;
  }
  lPreprocess.SetNoiseSpectrum(noise);
  //lPreprocess.PlotSpectrum("/afs/in2p3.fr/home/a/armengau/spec_1.gif");
  llAnaDb->FillPreprocess(lPreprocess);
  llAnaDb->CloseFile();
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->SetAmplFlag(AMPL_WIENER); // AMPL_ALL
  Ntuple->FillDataByChannelOrder(Chain,Channels,AnaDb,1,1,gIStart,gIStop,RawDb);

  delete Ntuple; delete Chain;
  delete RawDb; delete AnaDb;
  exit(-1);
  


  if (argc < 2) {
    cerr << "EdwRootAna: No parameter file given. Exiting." << endl;
    exit(-1);
  }
  ReadParamFile(argv[1]);

  cout << "****** EdwRootAna: Start analysis step " <<gProcessingStep<<endl;
  if (gProcessingStep == "BuildRawDB")
    BuildRawDB_Step();
  else if (gProcessingStep == "ReadSamba")
    ReadSamba_Step();
  else if (gProcessingStep == "BasicProcess")
    BasicProcess_Step();
  else if (gProcessingStep == "BuildTemplate")
    BuildTemplate_Step();
  else if (gProcessingStep == "Preprocess")
    Preprocess_Step();
  else if (gProcessingStep == "Process")
    Process_Step();
  else if (gProcessingStep == "EventDisplay")
    Event_Display();
  else cout << "****** EdwRootAna: Unidentified processing step." << endl;
  
  cout << "***** EdwRootAna: Normal end."<<endl;

  return 0;
}

void ReadParamFile(const char* aParamFile) {
  ifstream lFile(aParamFile,ios::in);
  if (!lFile) {
    cout << "Error opening parameter file " << aParamFile << endl; exit(-1);
  }
  Bool_t lFound = 0;
  string s;

  // First find gProcessingStep
  while ( lFile >> s ) {
    if (s == "gProcessingStep") {
      lFound = 1; break;
    }
  }
  if (!lFound) {
    cout << "Missing gProcessingStep in parameter file." << endl; exit(-1);
  }
  lFile >> s; checkequal(s); lFile >> gProcessingStep;

  // Find gBaseDir
  lFound = 0;
  while ( lFile >> s ) {
    if (s == "gBaseDir") {
      lFound = 1; break;
    }
  }
  if (!lFound) {
    cout << "Missing gBaseDir in parameter file." << endl; exit(-1);
  }
  lFile >> s; checkequal(s); lFile >> gBaseDir;

  // Default parameters from gBaseDir
  gRawDBFile=gBaseDir+"/edwrawdb.root";
  gAnaDBFile=gBaseDir+"/edwanadb.root";
  gSambaDir=gBaseDir+"/events";
  gRootEventDir=gBaseDir+"/rootevts";
  gRootNtpFile=gBaseDir+"/ntp/evtntuple.root";
  gRootNtpName="EdwRootNtp";
  gRunListFile="None";
  gChannelListFile="None";
  gAnaDBListFile="None";
  gPlotDir=gBaseDir+"/plots";
  gEvtListFile="None";
  gIStart = 0; gIStop = 0;
  gTimeStepMinutes = 60;

  // Parse the file and identify parameters
  lFile.seekg(0);
  while ( lFile >> s) {
    if (s == "gRawDBFile") {
      lFile >> s; checkequal(s); lFile >> gRawDBFile;
    } else if (s == "gAnaDBFile") {
     lFile >> s; checkequal(s); lFile >> gAnaDBFile;
    } else if (s == "gSambaDir") {
      lFile >> s; checkequal(s); lFile >> gSambaDir;
    } else if (s == "gRootEventDir") {
      lFile >> s; checkequal(s); lFile >> gRootEventDir;
    } else if (s == "gRootNtpFile") {
      lFile >> s; checkequal(s); lFile >> gRootNtpFile;
    } else if (s == "gRootNtpName") {
      lFile >> s; checkequal(s); lFile >> gRootNtpName;
    } else if (s == "gRunListFile") {
      lFile >> s; checkequal(s); lFile >> gRunListFile;
    } else if (s == "gChannelListFile") {
      lFile >> s; checkequal(s); lFile >> gChannelListFile;
    } else if (s == "gAnaDBListFile") {
      lFile >> s; checkequal(s); lFile >> gAnaDBListFile;
    } else if (s == "gPlotDir") {
      lFile >> s; checkequal(s); lFile >> gPlotDir;
    } else if (s == "gEvtListFile") {
      lFile >> s; checkequal(s); lFile >> gEvtListFile;
    } else if (s == "gIStart") {
      lFile >> s; checkequal(s); lFile >> gIStart;
    } else if (s == "gIStop") {
      lFile >> s; checkequal(s); lFile >> gIStop;
    } else if (s == "gTimeStepMinutes") {
      lFile >> s; checkequal(s); lFile >> gTimeStepMinutes;
    }

  }
  lFile.close();

  string names[11] = {gBaseDir,gRawDBFile,gAnaDBFile,gSambaDir,gRootEventDir,gRootNtpFile,gRunListFile,gChannelListFile,gAnaDBListFile,gPlotDir,gEvtListFile};
  for (int i=0; i<11; i++) {
    if ((names[i]).find('$',0) != string::npos) {
      if ((names[i]).find('$',0) != 0) cerr <<"ah ben zut.."<<endl;
      string::size_type bra = (names[i]).find('{',0);
      string::size_type ket = (names[i]).find('}',0);
      if (bra == string::npos || ket == string::npos) {
	cerr << "uncorrect bracketing for "<<names[i]<<endl; exit(-1);
      }
      char* fullpath = getenv(((names[i]).substr(bra+1,ket-bra-1)).c_str());
      string newstr(fullpath);
      newstr+=(names[i]).substr(ket+1);
      if (i == 0) gBaseDir = newstr;
      if (i == 1) gRawDBFile = newstr;
      if (i == 2) gAnaDBFile = newstr;
      if (i == 3) gSambaDir = newstr;
      if (i == 4) gRootEventDir = newstr;
      if (i == 5) gRootNtpFile = newstr;
      if (i == 6) gRunListFile = newstr;
      if (i == 7) gChannelListFile = newstr;
      if (i == 8) gAnaDBListFile = newstr;
      if (i == 9) gPlotDir = newstr;
      if (i == 10) gEvtListFile = newstr;
    }
  }

  cout << "****** EdwRootAna Parameters:"<<endl ;
  cout << "gProcessingStep = "<<gProcessingStep<<endl ;
  cout << "gBaseDir = "<<gBaseDir<<endl ;
  cout << "gRawDBFile = "<<gRawDBFile<<" - gAnaDBFile = "<<gAnaDBFile<<endl ;
  cout << "gSambaDir = "<<gSambaDir<<" - gRootEventDir = "<<gRootEventDir<<endl ;
  cout << "gRootNtpFile = "<<gRootNtpFile<<" - gRootNtpName = "<<gRootNtpName<<endl;
  cout << "gRunListFile = "<<gRunListFile<<endl ;
  cout << "gChannelListFile = "<<gChannelListFile<<endl;
  cout << "gAnaDBListFile = "<<gAnaDBListFile<<endl;
  cout << "gPlotDir = "<<gPlotDir<<endl;
  if (gEvtListFile != "None") cout << "gEvtListFile = "<<gEvtListFile<<endl;
  cout << "gIStart = "<<gIStart<<" - gIStop = "<<gIStop<<endl;
  cout << "gTimeStepMinutes = "<<gTimeStepMinutes<<endl;
}

vector<string> GlobalRunList() {
  ifstream lFile(gRunListFile.c_str(),ios::in);
  string s;
  vector<string> lList;
  if (!lFile) {
    cout << "Error opening run list file " << gRunListFile << endl; exit(-1);
  }
  while ( lFile >> s ) lList.push_back(s);
  lFile.close();
  return lList;
}

vector<string> GlobalAnaDBList() {
  ifstream lFile(gAnaDBListFile.c_str(),ios::in);
  string s;
  vector<string> lList;
  if (!lFile) {
    cout << "Error opening AnaDB list file " << gAnaDBListFile << endl; exit(-1);
  }
  while ( lFile >> s ) lList.push_back(s);
  lFile.close();
  return lList;
}

vector<string> GlobalChannelList() {
  ifstream lFile(gChannelListFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening channel list file " << gChannelListFile << endl; exit(-1);
  }
  string line;
  vector<string> lList;
  while (getline(lFile,line)) {
    string::size_type quotea_pos = line.find('"',0);
    string::size_type quoteb_pos = line.find('"',quotea_pos+1);
    if (quotea_pos == string::npos || quoteb_pos == string::npos) {
      cerr << "Channels not correctly quoted in "<<gChannelListFile<<" - Exiting."<<endl;
      exit(-1);
    }
    lList.push_back(line.substr(quotea_pos+1,quoteb_pos-quotea_pos-1));
  }
  lFile.close();
  return lList;
}

vector<Float_t> GlobalChannelAmpls() {
  ifstream lFile(gChannelListFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening channel list file " << gChannelListFile << endl; exit(-1);
  }
  string line;
  vector<Float_t> lList;
  while (getline(lFile,line)) {
    string::size_type quotea_pos = line.find('"',0);
    string::size_type quoteb_pos = line.find('"',quotea_pos+1);
    line = line.substr(quoteb_pos+1);
    istringstream ss(line);
    Float_t f; ss >> f; lList.push_back(f);
  }
  lFile.close();
  return lList;
}

vector<Int_t> GlobalChannelBins() {
  ifstream lFile(gChannelListFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening channel list file " << gChannelListFile << endl; exit(-1);
  }
  string line;
  vector<Int_t> lList;
  while (getline(lFile,line)) {
    UInt_t quotea_pos = line.find('"',0);
    UInt_t quoteb_pos = line.find('"',quotea_pos+1);
    line = line.substr(quoteb_pos+1);
    istringstream ss(line);
    Float_t f; ss >> f; 
    Int_t i; ss >> i; lList.push_back(i);
  }
  lFile.close();
  return lList;
}

vector<Float_t> GlobalChannelDeltaBins() {
  ifstream lFile(gChannelListFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening channel list file " << gChannelListFile << endl; exit(-1);
  }
  string line;
  vector<Float_t> lList;
  while (getline(lFile,line)) {
    int quotea_pos = line.find('"',0);
    int quoteb_pos = line.find('"',quotea_pos+1);
    line = line.substr(quoteb_pos+1);
    istringstream ss(line);
    Float_t f; ss >> f; 
    Int_t i; ss >> i; 
    ss >> f; lList.push_back(f);
  }
  lFile.close();
  return lList;
}

void BuildRawDB_Step() { // Fill the database
  EdwRawDB* FillDB = new EdwRawDB(gRawDBFile,"RECREATE");
  if (gRunListFile != "None") 
    FillDB->FillRunData(gSambaDir,GlobalRunList());
  else FillDB->FillRunData(gSambaDir);
  // No setup data and automate data yet.
  //    FillDB->DumpRunData();
  
  delete FillDB;
  cout << "****** EdwRootAna: Raw DB filled" << endl;
}

void ReadSamba_Step() { // Build Root files from Samba
  EdwRawDB* Db = new EdwRawDB(gRawDBFile,"READ");
  EdwRunData* lRunData = new EdwRunData();
  Db->fRunBranch->SetAddress(&lRunData);
  for (UInt_t i=0; i<Db->fRunBranch->GetEntries(); i++) {
    // Loop on all runs given in the EdwRawDB
    Db->fRunBranch->GetEntry(i);
    string lRunDir=gSambaDir+"/"+lRunData->fRunName;
    for (UInt_t j=0; j<(lRunData->fRunFiles).size();j++) {
      ByteOrderType lByteOrder = lRunData->fByteOrdering;
      string lEdwFile = lRunDir+"/"+(lRunData->fRunFiles).at(j);
      string lRootFile = gRootEventDir+"/"+(lRunData->fRunFiles).at(j)+".root";
      SambaToRoot lSambaData(lByteOrder,lEdwFile,lRootFile);
      lSambaData.FillRootFile();
      cout << "****** EdwRootAna: "<<lRootFile<<" filled."<<endl;
    }
  }
  delete Db;
  cout << "****** EdwRootAna: Samba files read" <<endl;
}

void BasicProcess_Step() {
  EdwRawDB* Db = new EdwRawDB(gRawDBFile,"READ");
  vector<string> RunList ; // Runs to process
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = Db->GetRunList();
  TChain* Chain = Db->GetEventChain(RunList,gRootEventDir);
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = Db->GetRunChannels(RunList);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->SetAmplFlag(AMPL_RAW);
  Ntuple->FillDataFromChain(Chain,Channels,NULL,1,1,gIStart,gIStop,Db);

  delete Ntuple;
  delete Chain;
  delete Db;
  cout << "****** EdwRootAna: Basic event processing done" << endl;

}

void BuildTemplate_Step() {

  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"UPDATE"); 
  // instead of RECREATE : there MUST not be already such a file if this is step 1 of ana..
  vector<string> RunList ; // Runs to process
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  if (gChannelListFile == "None") {
    cerr << "A file for channels must be specified in the template building. Exit."<<endl;
    exit(-1);
  }
  vector<string> ChannelList = GlobalChannelList();
  vector<Float_t> Ampls = GlobalChannelAmpls();
  vector<Int_t> Bins = GlobalChannelBins();
  vector<Float_t> DeltaBins = GlobalChannelDeltaBins();

  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  vector<time_t> tbounds = GetMinMaxTimes(Chain);
  // Ca je suis sur que ca sert a rien!!
  EdwEvent* lEvt=new EdwEvent();
  Chain->SetBranchAddress("Event",&lEvt);
    // Build the templates
  for (UInt_t i=0; i<ChannelList.size(); i++) {
    EdwTemplate lTmplt(0,time(NULL),RunList,ChannelList[i]);
    lTmplt.BuildFitTemplate(Chain,Ampls[i],Bins[i],DeltaBins[i]); // no chi2cut by default
    lTmplt.SetFilterParams();
    lTmplt.BuildOffsetFFTs();
    lTmplt.PlotTrace(gPlotDir+"/tmplt_test_"+ChannelList[i]+".gif");
    lTmplt.PlotFilteredTrace(gPlotDir+"/tmplt_filt_"+ChannelList[i]+".gif");
    lTmplt.PlotExtraTrace(gPlotDir+"/tmplt_extra_"+ChannelList[i]+".gif");
    AnaDb->FillTemplate(lTmplt);
    cerr << "Template for channel "<<ChannelList[i]<<" created." << endl;
  }
  // Fill the analysis database
  cout << "****** EdwRootAna: templates created and saved to DB." << endl;

}

void Preprocess_Step() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"UPDATE");
  // instead of RECREATE
  vector<string> RunList ; // Runs to process
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  vector<string> lChannels ; // Channels to process
  vector<Int_t> lPulseBins;
  if (gChannelListFile != "None") {
    lChannels = GlobalChannelList();
    lPulseBins = GlobalChannelBins();
  }
  else lChannels = RawDb->GetRunChannels(RunList);
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  time_t timestep = 60*(time_t)gTimeStepMinutes;
  vector<time_t> tbounds = GetMinMaxTimes(Chain); Int_t i=0;
  cout << "** Number of time steps: " << (tbounds[1]-tbounds[0])/timestep<< endl;
  for (time_t tcut=tbounds[0]; tcut < tbounds[1]; tcut+=timestep) {
    AnaDb->BasicFillPreprocessData(Chain,lChannels,lPulseBins,tcut,tcut+timestep);
    //AnaDb->BasicFillPreprocessData(Chain,lChannels,tcut,tcut+timestep,gPlotDir+"/spectrum_"+int2str(i));
    cout << "* Step "<<int2str(i) <<": Preprocess done." << endl; i++;
  }

  delete Chain;
  delete RawDb;
  delete AnaDb;
  cout << "****** EdwRootAna: "<<gAnaDBFile<<" filled with preprocessing data."<<endl;
}

void Process_Step() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  if (gAnaDBListFile != "None") {
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBs(AnaDBList);
    //    delete lDb; // ! crucial pour pas faire double free sur les objets
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  vector<string> RunList ; // Runs to process
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = RawDb->GetRunChannels(RunList);

  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->SetAmplFlag(AMPL_ALL); // AMPL_ALL
  //Ntuple->FillDataFromChain(Chain,Channels,AnaDb,1,1); // with rise and fall times
  Ntuple->FillDataByChannelOrder(Chain,Channels,AnaDb,1,1,gIStart,gIStop,RawDb); // with rise and fall times

  delete Ntuple;
  delete Chain;
  delete RawDb; delete AnaDb;
  cout << "****** EdwRootAna: Basic event processing done" << endl;

}

void Event_Display() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  if (gAnaDBListFile != "None") {
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBs(AnaDBList);
    //    delete lDb; // ! crucial pour pas faire double free sur les objets
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  vector<string> RunList ;
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->PlotSelectEvents(Chain,gEvtListFile,gPlotDir,AnaDb,RawDb);

  delete Ntuple;
  delete Chain;
  delete RawDb; delete AnaDb;
  cout << "****** EdwRootAna: Event Display done" << endl;

}
