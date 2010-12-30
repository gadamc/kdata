

#include "EdwUtils.h"
#include <climits>

string gProcessingStep, gBaseDir;
string gRawDBFile, gAnaDBFile;
string gSambaDir, gRootEventDir, gScratchDir;
string gRootNtpFile, gRootNtpName;
string gASCIIOutputDir;
string gRunListFile, gChannelListFile;
string gHeatIonSynchroFile;
string gAnaDBListFile;
string gASCIITemplateDir;
string gPlotDir, gEvtListFile;
ULong_t gIStart, gIStop;
//UInt_t gWienerWindowStart, gWienerWindowStop;
Int_t gTimeStepMinutes;
Bool_t gTemplateFromSinglePulse;
Bool_t gWienerOnly;
string gByteOrder;

void ReadParamFile(const char* aParamFile) ;
vector<string> GlobalRunList();
vector<string> GlobalAnaDBList();
vector<string> GlobalChannelList();
vector<Float_t> GlobalChannelAmpls();
vector<Int_t> GlobalChannelBins();
vector<Float_t> GlobalChannelDeltaBins();
vector<Int_t> GlobalChannelPatterns();
vector<string> GlobalEvtList_Runs();
vector<UInt_t> GlobalEvtList_EventNums();
vector<string> GlobalEvtList_Channels();

// Special routines used in the main for parameter file reading to get global variables.

/* ***********************************************
*********** Format of parameter files ************

*** gRunListFile:
runname_a
runname_b
...

*** gChannelListFile:
"channel a" [ ref_ampl ref_bin delta_bin pattern_length ]
"channel_b" [ ref_ampl ref_bin delta_bin pattern_length ]
...

*** gHeatIonSynchroFile:
"heat a" : coef_a coef_b "ion a1" "ion a2" ...
"heat b" : coef_a coef_b "ion b1" "ion b2" ...
...

*** gAnaDBListFile:
dbfile_a
dbfile_b
...

*** gEvtListFile:
run_1 evt_1
run_2 evt_2
...

*** gASCIITemplateDir:
directory with files "template_my favorite channel.txt"
each one has the trace as one line per point

*********************************************** */

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
    // cout << "Missing gBaseDir in parameter file." << endl; exit(-1);
    gBaseDir = "~/";
  } else {
    lFile >> s; checkequal(s); lFile >> gBaseDir;
  }

  // Default parameters from gBaseDir
  gRawDBFile=gBaseDir+"/dbfiles/edwrawdb.root";
  gAnaDBFile=gBaseDir+"/dbfiles/edwanadb.root";
  gSambaDir=gBaseDir+"/events";
  gRootEventDir=gBaseDir+"/rootevts";
  gScratchDir="";
  gRootNtpFile=gBaseDir+"/ntps/evtntuple.root";
  gRootNtpName="EdwRootNtp";
  gASCIIOutputDir="None";
  gRunListFile="None";
  gChannelListFile="None";
  gHeatIonSynchroFile="None";
  gAnaDBListFile="None";
  gASCIITemplateDir="None";
  gPlotDir="None";
  gEvtListFile="None";
  gIStart = 0; gIStop = 0;
  //  gWienerWindowStart = 0; gWienerWindowStop = 0;
  gTimeStepMinutes = 60;
  gTemplateFromSinglePulse = 0;
  gWienerOnly = 0;
  gByteOrder="big";

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
    } else if (s == "gScratchDir") {
      lFile >> s; checkequal(s); lFile >> gScratchDir;
    } else if (s == "gRootNtpFile") {
      lFile >> s; checkequal(s); lFile >> gRootNtpFile;
    } else if (s == "gRootNtpName") {
      lFile >> s; checkequal(s); lFile >> gRootNtpName;
    } else if (s == "gASCIIOutputDir") {
      lFile >> s; checkequal(s); lFile >> gASCIIOutputDir;
    } else if (s == "gRunListFile") {
      lFile >> s; checkequal(s); lFile >> gRunListFile;
    } else if (s == "gChannelListFile") {
      lFile >> s; checkequal(s); lFile >> gChannelListFile;
    } else if (s == "gHeatIonSynchroFile") {
      lFile >> s; checkequal(s); lFile >> gHeatIonSynchroFile;
    } else if (s == "gAnaDBListFile") {
      lFile >> s; checkequal(s); lFile >> gAnaDBListFile;
    } else if (s == "gASCIITemplateDir") {
      lFile >> s; checkequal(s); lFile >> gASCIITemplateDir;
    } else if (s == "gPlotDir") {
      lFile >> s; checkequal(s); lFile >> gPlotDir;
    } else if (s == "gEvtListFile") {
      lFile >> s; checkequal(s); lFile >> gEvtListFile;
    } else if (s == "gIStart") {
      lFile >> s; checkequal(s); lFile >> gIStart;
    } else if (s == "gIStop") {
      lFile >> s; checkequal(s); lFile >> gIStop;
      //    } else if (s == "gWienerWindowStart") {
      //      lFile >> s; checkequal(s); lFile >> gWienerWindowStart;
      //    } else if (s == "gWienerWindowStop") {
      //      lFile >> s; checkequal(s); lFile >> gWienerWindowStop;
    } else if (s == "gTimeStepMinutes") {
      lFile >> s; checkequal(s); lFile >> gTimeStepMinutes;
    } else if (s == "gTemplateFromSinglePulse") {
      lFile >> s; checkequal(s); lFile >> gTemplateFromSinglePulse;
    } else if (s == "gWienerOnly") {
      lFile >> s; checkequal(s); lFile >> gWienerOnly;
    } else if (s == "gByteOrder") {
      lFile >> s; checkequal(s); lFile >> gByteOrder;
      if (gByteOrder != "little" && gByteOrder != "big") cout << "Error in parameter gByteOrder" << endl;
    }
  }
  lFile.close();

  string names[15] = {gBaseDir,gRawDBFile,gAnaDBFile,gSambaDir,gRootEventDir,gRootNtpFile,gASCIIOutputDir,gRunListFile,gChannelListFile,gHeatIonSynchroFile,gAnaDBListFile,gASCIITemplateDir,gPlotDir,gEvtListFile,gScratchDir};
  for (int i=0; i<15; i++) {
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
      if (i == 6) gASCIIOutputDir = newstr;
      if (i == 7) gRunListFile = newstr;
      if (i == 8) gChannelListFile = newstr;
      if (i == 9) gHeatIonSynchroFile = newstr;
      if (i == 10) gAnaDBListFile = newstr;
      if (i == 11) gASCIITemplateDir = newstr;
      if (i == 12) gPlotDir = newstr;
      if (i == 13) gEvtListFile = newstr;
      if (i == 14) gScratchDir = newstr;
    }
  }

  if (KVERBOSE) {
    cout << "****** EdwRootAna Parameters:"<<endl ;
    cout << "gProcessingStep = "<<gProcessingStep<<endl ;
    cout << "gBaseDir = "<<gBaseDir<<endl ;
    cout << "gRawDBFile = "<<gRawDBFile<<" - gAnaDBFile = "<<gAnaDBFile<<endl ;
    cout << "gSambaDir = "<<gSambaDir<<" - gRootEventDir = "<<gRootEventDir<<endl ;
    cout << "gRootNtpFile = "<<gRootNtpFile<<" - gRootNtpName = "<<gRootNtpName<<endl;
    cout << "gASCIIOutputDir = "<<gASCIIOutputDir << " - gScratchDir = "<<gScratchDir<<endl;
    cout << "gRunListFile = "<<gRunListFile<<endl ;
    cout << "gChannelListFile = "<<gChannelListFile<<endl;
    cout << "gHeatIonSynchroFile = "<<gHeatIonSynchroFile<<endl;
    cout << "gAnaDBListFile = "<<gAnaDBListFile<<endl;
    cout << "gASCIITemplateDir = "<<gASCIITemplateDir<<endl;
    cout << "gPlotDir = "<<gPlotDir<<endl;
    if (gEvtListFile != "None") cout << "gEvtListFile = "<<gEvtListFile<<endl;
    cout << "gIStart = "<<gIStart<<" - gIStop = "<<gIStop<<endl;
    //    cout << "gWienerWindowStart = "<<gWienerWindowStart<<" - gWienerWindowStop = "<<gWienerWindowStop<<endl;
    cout << "gTimeStepMinutes = "<<gTimeStepMinutes<<" - gTemplateFromSinglePulse = "<<gTemplateFromSinglePulse<<endl;
    cout << "gWienerOnly = "<<gWienerOnly<<" - gByteOrder = "<<gByteOrder<<endl;
  }

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

vector<string> GlobalAnaDBList(string aFile)
{
	gAnaDBListFile = aFile;
	return GlobalAnaDBList();
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

vector<Int_t> GlobalChannelPatterns() {
  ifstream lFile(gChannelListFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening channel list file " << gChannelListFile << endl; exit(-1);
  }
  string line;
  vector<Int_t> lList;
  while (getline(lFile,line)) {
    int quotea_pos = line.find('"',0);
    int quoteb_pos = line.find('"',quotea_pos+1);
    line = line.substr(quoteb_pos+1);
    istringstream ss(line);
    Float_t f; ss >> f; 
    Int_t i; ss >> i; ss >> f;
    i=0;
    // The pattern may not be specified in the line: in this case take zero
    if (!ss.eof()) {
      ss >> i;
      if (i >= SHRT_MAX || i < 0) i=0;
    }
    lList.push_back(i);
  }
  lFile.close();
  return lList;
}

vector<string> GlobalEvtList_Runs() {
  vector<string> lRunList;
  vector<UInt_t> lEvtList;
  string line;
  ifstream lFile(gEvtListFile.c_str(),ios::in);
  if (!lFile) cerr << "Error no evtlistfile!" <<endl;
  while (getline(lFile,line)) {
    istringstream ss(line);
    string run; ss >> run; lRunList.push_back(run);
    UInt_t i; ss >> i; lEvtList.push_back(i);
  }
  lFile.close();
  return lRunList;
}

vector<UInt_t> GlobalEvtList_EventNums() {
  vector<string> lRunList;
  vector<UInt_t> lEvtList;
  string line;
  ifstream lFile(gEvtListFile.c_str(),ios::in);
  if (!lFile) cerr << "Error no evtlistfile!" <<endl;
  while (getline(lFile,line)) {
    istringstream ss(line);
    string run; ss >> run; lRunList.push_back(run);
    UInt_t i; ss >> i; lEvtList.push_back(i);
  }
  lFile.close();
  return lEvtList;
}

vector<string> GlobalEvtList_Channels() {
  vector<string> lChannels;
  string line;
  ifstream lFile(gEvtListFile.c_str(),ios::in);
  if (!lFile) cerr << "Error no evtlistfile!" <<endl;
  while (getline(lFile,line)) {
    string::size_type quotea_pos = line.find('"',0);
    string::size_type quoteb_pos = line.find('"',quotea_pos+1);
    if (quotea_pos == string::npos || quoteb_pos == string::npos) {
      cerr << "Channels not correctly quoted in "<<gEvtListFile<<" - Exiting."<<endl;
      exit(-1);
    }
    lChannels.push_back(line.substr(quotea_pos+1,quoteb_pos-quotea_pos-1));
  }
  lFile.close();
  return lChannels;
}
