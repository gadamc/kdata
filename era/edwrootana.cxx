
#include "EdwUtils.h"
#include "EdwRawDB.h"
#include "EdwAnaDB.h"
#include "SambaToRoot.h"
#include "EdwNtuple.h"

#include "EdwParams.cxx"

void BuildRawDB_Step() ;
void ReadSamba_Step();
void BasicProcess_Step();
void Preprocess_Step();
void BuildTemplate_Step();
void Process_Step();
//void CalibWiener_Step();
void Event_Display();
void Event_Dump();
//void FrequencyWeights_Dump();
void RawDB_Display();
void AnaDB_Display();
void WienerSpectra_Plot();
void NoiseSpectra_Plot();

int main(int argc, char* argv[]) {

  // EDelWeiss ROOT ANAlysis routine
  // *******************************

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
  //  else if (gProcessingStep == "CalibWiener")
  //    CalibWiener_Step();
  else if (gProcessingStep == "DisplayEvent")
    Event_Display();
  else if (gProcessingStep == "DumpEvent")
    Event_Dump();
  //  else if (gProcessingStep == "DumpFrequencyWeights")
  //    FrequencyWeights_Dump();
  else if (gProcessingStep == "DisplayRawDB")
    RawDB_Display();
  else if (gProcessingStep == "DisplayAnaDB")
    AnaDB_Display();
  else if (gProcessingStep == "PlotWienerSpectra")
    WienerSpectra_Plot();
  else if (gProcessingStep == "PlotNoiseSpectra")
    NoiseSpectra_Plot();
  else cout << "****** EdwRootAna: Unidentified processing step." << endl;
  
  cout << "***** EdwRootAna: Normal end."<<endl;

  return 0;
}

void BuildRawDB_Step() { // Fill the database
  EdwRawDB* FillDB = new EdwRawDB(gRawDBFile,"UPDATE");
  ByteOrderType lOrder=kBigEndian;
  if (gByteOrder == "little") lOrder=kLittleEndian;
  if (gRunListFile != "None") 
    FillDB->FillRunData(gSambaDir,GlobalRunList(),lOrder);
  else FillDB->FillRunData(gSambaDir,lOrder);
  // Here big endian is assumed
  // No setup data and automate data yet.
  
  FillDB->DumpRunData();
  
  delete FillDB;
  cout << "****** EdwRootAna: Raw DB filled" << endl;
}

void ReadSamba_Step() { // Build Root files from Samba
  EdwRawDB* Db = new EdwRawDB(gRawDBFile,"READ");
  EdwRunData* lRunData = new EdwRunData();
  Db->fRunBranch->SetAddress(&lRunData);
  ifstream lRunListFile(gRunListFile.c_str(),ios::in);
  Bool_t lCheckRunList = 0;
  if (lRunListFile) {
    lCheckRunList=1;
    lRunListFile.close();
  }
  for (UInt_t i=0; i<Db->fRunBranch->GetEntries(); i++) {
    // Loop on all runs given in the EdwRawDB
    Db->fRunBranch->GetEntry(i);
    Bool_t do_read = 1;
    if (lCheckRunList) {
      if ( vectnum(GlobalRunList(),lRunData->fRunName) == -1 ) do_read=0;
    }
    if (do_read) {
      string lRunDir=gSambaDir+"/"+lRunData->fRunName;
      for (UInt_t j=0; j<(lRunData->fRunFiles).size();j++) {
	ByteOrderType lByteOrder = lRunData->fByteOrdering;
	string lEdwFile = lRunDir+"/"+(lRunData->fRunFiles).at(j);
	string lRootFile = gRootEventDir+"/"+(lRunData->fRunFiles).at(j)+".root";
	// Check if the root file already exists
	if ((! OVERWRITE_ROOTEVTFILE) && fileexists(lRootFile)) {
	  cout << "****** EdwRootAna: "<<lRootFile<<" already exists."<<endl;
	} else {
	  SambaToRoot lSambaData(lByteOrder,lEdwFile,lRootFile);
	  lSambaData.FillRootFile();
	  cout << "****** EdwRootAna: "<<lRootFile<<" filled."<<endl;
	}
      }
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
  Ntuple->SetShapeFlag(SHAPE_NO);
  Ntuple->FillDataFromChain(Chain,Channels,NULL,gIStart,gIStop,Db);

  delete Ntuple;
  delete Chain;
  delete Db;
  cout << "****** EdwRootAna: Basic event processing done" << endl;

}

void BuildTemplate_Step() {

  if (gChannelListFile == "None") {
    cerr << "A file for channels must be specified in the template building. Exit."<<endl; exit(-1);
  }
  vector<string> RunList ; // Runs to process
  vector<string> ChannelList = GlobalChannelList();

  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"UPDATE"); 
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();    
  vector<Float_t> Ampls = GlobalChannelAmpls();
  vector<Int_t> Bins = GlobalChannelBins();
  vector<Float_t> DeltaBins = GlobalChannelDeltaBins();
  
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  vector<time_t> tbounds = GetMinMaxTimes(Chain);
  // Build the templates
  for (UInt_t i=0; i<ChannelList.size(); i++) {
    EdwTemplate lTmplt(0,time(NULL),RunList,ChannelList[i]);
    if (gTemplateFromSinglePulse)
      lTmplt.BuildBasicTemplate(Chain,Ampls[i],Bins[i],DeltaBins[i]);
    else
      lTmplt.BuildFitTemplate(Chain,Ampls[i],Bins[i],DeltaBins[i]); // no chi2cut by default
    lTmplt.SetFilterParams();
    lTmplt.BuildOffsetFFTs();
    if (gPlotDir != "None") {
      lTmplt.PlotTrace(gPlotDir+"/tmplt_"+ChannelList[i]+".gif");
      lTmplt.PlotFilteredTrace(gPlotDir+"/tmplt_filt_"+ChannelList[i]+".gif");
      lTmplt.PlotExtraTrace(gPlotDir+"/tmplt_extra_"+ChannelList[i]+".gif");
    }

    if (gASCIITemplateDir == "None") {
      AnaDb->FillTemplate(lTmplt);
    } else {
      string lFile = gASCIITemplateDir+"/template_"+ChannelList[i]+".txt";
      lTmplt.DumpTrace(lFile);
    }
    cerr << "Template for channel "<<ChannelList[i]<<" created." << endl;
  }
  
  delete Chain;
  delete RawDb;
  
  //  delete AnaDb; // La ya un pbl on comprend pas pourquoi!
  cout << "****** EdwRootAna: templates created and saved." << endl;
  
}

void Preprocess_Step() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"UPDATE");
  vector<string> RunList ; // Runs to process
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  vector<string> lChannels ; // Channels to process
  vector<Int_t> lPulseBins, lPatterns;
  if (gChannelListFile != "None") {
    lChannels = GlobalChannelList();
    lPulseBins = GlobalChannelBins();
    lPatterns = GlobalChannelPatterns();
  }
  else lChannels = RawDb->GetRunChannels(RunList);
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  time_t timestep = 60*(time_t)gTimeStepMinutes;
  vector<time_t> tbounds = GetMinMaxTimes(Chain);
  // same algorithm for time period division as in post-processing analysis routines
  Double_t nbdivisions=((Double_t)(tbounds[1]-tbounds[0]))/timestep;
  if (nbdivisions < 1) nbdivisions=1;
  if (nbdivisions-floor(nbdivisions) < 0.5) nbdivisions=floor(nbdivisions);
  else nbdivisions=ceil(nbdivisions);
  Double_t localperiodlength=((Double_t)(tbounds[1]-tbounds[0]))/nbdivisions;
  cout << "** Number of time steps: " << nbdivisions <<" with length "<<localperiodlength<< endl;
  for (UInt_t i=0; i<nbdivisions; i++) {
    //  for (time_t tcut=tbounds[0]; tcut < tbounds[1]; tcut+=timestep) {
    time_t tcut=tbounds[0]+(time_t)(i*localperiodlength);
    time_t t_end=tbounds[0]+(time_t)((i+1)*localperiodlength);
    if ( t_end >= tbounds[1] ) t_end = tbounds[1]+1; // avoid overlap between different runs! (should not be useful now)
    if (gPlotDir == "None")
      AnaDb->BasicFillPreprocessData(Chain,lChannels,lPulseBins,lPatterns,tcut,t_end);
    else
      AnaDb->BasicFillPreprocessData(Chain,lChannels,lPulseBins,lPatterns,tcut,t_end,gPlotDir+"/spectrum_"+int2str(i));
    cout << "* Step "<<int2str(i) <<": Preprocess done." << endl;
  }

  delete Chain;
  delete RawDb;
  delete AnaDb;
  cout << "****** EdwRootAna: "<<gAnaDBFile<<" filled with preprocessing data."<<endl;
}

void Process_Step() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> FullRunList ; // Runs to process
  if (gRunListFile != "None") FullRunList = GlobalRunList();
  else FullRunList = RawDb->GetRunList();
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = RawDb->GetRunChannels(FullRunList);

  //  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  ULong_t i_offset=0; 
  vector<string> RunList; // list of runs that are really used given gIStart/Stop.
  TChain* Chain=RawDb->GetEventSubChain(FullRunList,gRootEventDir,gIStart,gIStop,i_offset,RunList,gScratchDir);

  if (gAnaDBListFile != "None") { // Create the db from a list
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBsAndRuns(AnaDBList,RunList);
    if (gASCIITemplateDir != "None")
      lDb->FillFromASCIITemplates(Channels,gASCIITemplateDir,gPlotDir);
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");


  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  if (gWienerOnly) Ntuple->SetAmplFlag(AMPL_WIENER);
  else Ntuple->SetAmplFlag(AMPL_ALL);
  Ntuple->SetShapeFlag(SHAPE_ALL);
  if (gHeatIonSynchroFile == "None") {
    Ntuple->FillDataByChannelOrder(Chain,Channels,AnaDb,gIStart-i_offset,gIStop-i_offset,RawDb);
  } else {
    Ntuple->SetHeatIonSynchro(gHeatIonSynchroFile);
    Ntuple->FillDataWithChannelSynchro(Chain,Channels,AnaDb,gIStart-i_offset,gIStop-i_offset,RawDb);
  }

  delete Ntuple;
  delete Chain;
  delete RawDb; delete AnaDb;
  cout << "****** EdwRootAna: Basic event processing done" << endl;

}

/*
  void CalibWiener_Step() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> AnaDBList = GlobalAnaDBList();
  EdwAnaDB* tmpDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
  tmpDb->FillFromDBs(AnaDBList);
  tmpDb->CloseFile();
  vector<string> RunList ; // Runs to process
  RunList = GlobalRunList();
  vector<string> Channels ; // Channels to process
  Channels = GlobalChannelList();

  // Add a preprocessdata with noise creneau to anadb
  EdwAnaDB* tmpDb2 = new EdwAnaDB(gAnaDBFile,"UPDATE");
  for (UInt_t i=0; i<Channels.size(); i++) {
    EdwPreprocessData lPreprocess(10,time(NULL),Channels[i]);
    UInt_t size = (tmpDb2->GetTemplate(Channels[i])).Trace().size();
    vector<Float_t> noise(size,1.e10);
    for (UInt_t k=gWienerWindowStart; k<gWienerWindowStop; k++) {
      noise[k]=1;
      noise[size-k-1]=1;
    }
    lPreprocess.SetNoiseSpectrum(noise);
    tmpDb2->FillPreprocess(lPreprocess);
  }
  tmpDb2->CloseFile();

  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->SetAmplFlag(AMPL_WIENER);
  Ntuple->FillDataByChannelOrder(Chain,Channels,AnaDb,gIStart,gIStop,RawDb);

  delete Ntuple; delete Chain;
  delete RawDb; delete AnaDb;
  cout << "****** EdwRootAna: CalibWiener step done" << endl;
  } */

void Event_Display() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> RunList ;
  if (gRunListFile != "None") RunList = GlobalRunList();
  else RunList = RawDb->GetRunList();
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = RawDb->GetRunChannels(RunList);

  if (gAnaDBListFile != "None") {
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBs(AnaDBList);
    if (gASCIITemplateDir != "None")
      lDb->FillFromASCIITemplates(Channels,gASCIITemplateDir,gPlotDir);
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");

  TChain* Chain = RawDb->GetEventChain(RunList,gRootEventDir);
  EdwNtuple* Ntuple = new EdwNtuple(gRootNtpFile,gRootNtpName);
  Ntuple->PlotSelectEvents(Chain,Channels,gEvtListFile,gPlotDir,AnaDb,RawDb);

  delete Ntuple;
  delete Chain;
  delete RawDb; delete AnaDb;
  cout << "****** EdwRootAna: Event Display done" << endl;

}

void Event_Dump() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> lRuns = GlobalEvtList_Runs();
  vector<UInt_t> lNums = GlobalEvtList_EventNums();
  vector<string> lChannels = GlobalEvtList_Channels();
  for (UInt_t i=0; i<lRuns.size(); i++) {
    string lFileName = gASCIIOutputDir+"/trace_"+lChannels[i]+"_"+lRuns[i]+"_evt"+int2str(lNums[i],5)+".txt";
    RawDb->DumpEvent(lRuns[i],lNums[i],gRootEventDir,lFileName,lChannels[i]);
    cout << "****** Event_Dump: evt " << lNums[i] << " done" << endl; 
  }
}

/*void FrequencyWeights_Dump() {
  // Noms pas optimises..
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  if (gChannelListFile != "None") {
    vector<string> Channels = GlobalChannelList();
    AnaDb->DumpFrequencyWeights(gASCIIOutputDir+"/freqweights.txt",Channels,gWienerWindowStart,gWienerWindowStop);
  } else {
    AnaDb->DumpFrequencyWeights(gASCIIOutputDir+"/freqweights.txt",gWienerWindowStart,gWienerWindowStop);
  }
  }*/


void WienerSpectra_Plot() {

  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = RawDb->GetRunChannels(RawDb->GetRunList());

  if (gAnaDBListFile != "None") {
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBs(AnaDBList);
    if (gASCIITemplateDir != "None")
      lDb->FillFromASCIITemplates(Channels,gASCIITemplateDir);
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");

  for (UInt_t i=0; i<Channels.size(); i++) {
    string plot=gPlotDir+"/wienerspec_"+Channels[i];
    AnaDb->PlotWienerSpectra(Channels[i],plot);
    cout << "**** Spectra plotted for channel "<<Channels[i]<<endl;
  }

}

void NoiseSpectra_Plot() {

  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  vector<string> Channels ; // Channels to process
  if (gChannelListFile != "None") Channels = GlobalChannelList();
  else Channels = RawDb->GetRunChannels(RawDb->GetRunList());
  if (gAnaDBListFile != "None") {
    vector<string> AnaDBList = GlobalAnaDBList();
    EdwAnaDB* lDb = new EdwAnaDB(gAnaDBFile,"RECREATE");
    lDb->FillFromDBs(AnaDBList);
    lDb->CloseFile();
  }
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");

  for (UInt_t i=0; i<Channels.size(); i++) {
    string plot=gPlotDir+"/noisespec_"+Channels[i]+".gif";
    AnaDb->PlotNoiseSpectra(Channels[i],plot);
    cout << "**** Spectra plotted for channel "<<Channels[i]<<endl;
  }

}

void RawDB_Display() {
  EdwRawDB* RawDb = new EdwRawDB(gRawDBFile,"READ");
  RawDb->DumpRunData();
  delete RawDb;
  cout << "***** EdwRootAna: RawDB Display done" << endl;
}

void AnaDB_Display() {
  EdwAnaDB* AnaDb = new EdwAnaDB(gAnaDBFile,"READ");
  AnaDb->DumpTemplatesInfo(gASCIITemplateDir) ;
  AnaDb->DumpPreprocessInfo();
  delete AnaDb;
  cout << "***** EdwRootAna: AnaDB Display done" << endl;
}
