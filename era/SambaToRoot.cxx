/**
   @file    SambaToRoot.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the SambaToRoot class. See the .h file
*/

#include "SambaToRoot.h"
#include "byteswap.h"

ClassImp(SambaToRoot); /**< Root dictionnary */

SambaToRoot::SambaToRoot() : TObject() {
  fByteOrdering = kBigEndian ;
  fEdwFileName = "edw-file" ;
  fRootFileName = "root-file.root" ;
  fEdwRunName = "aa00a000" ;
  fRootFile = new TFile(fRootFileName.c_str());
}

SambaToRoot::SambaToRoot(ByteOrderType aByteOrder, string aEdwFile, string aRootFile) : TObject() {
  fByteOrdering = aByteOrder ;
  fEdwFileName = aEdwFile ;
  Int_t lLength = fEdwFileName.length();
  if (lLength < 12 || fEdwFileName.compare(lLength-4,1,"_") )
    cerr << "SambaToRoot: wrong file name" << endl;
  fRootFileName = aRootFile ;
  if (aRootFile.compare(aRootFile.length()-5,5,".root") )
    cerr << "SambaToRoot: wrong root file name" << endl;
  fEdwRunName = fEdwFileName.substr(fEdwFileName.length()-12,8) ;

  fEdwFile.open(fEdwFileName.c_str(),ios::in) ;
  if (!fEdwFile) cerr << "SambaToroot: error opening stream from " << fEdwFileName << endl;

  fRootFile = new TFile(fRootFileName.c_str(),"RECREATE") ;
  if (fRootFile->IsZombie()) cerr << "sambatoroot: error opening Root file " << fRootFileName << endl;

}


SambaToRoot::~SambaToRoot() {
  fRootFile->Close() ;
  fEdwFile.close() ;
}

Int_t SambaToRoot::FillRootFile(Int_t aBufSize) {

  string kEnteteEvt = "* Evenement" ; // empirical flags in the raw Edelweiss files
  string kEnteteVoie = "* Voie" ;
  string kSeparator = "* ----------" ;
  short kEnteteEvtSize = kEnteteEvt.size() ;
  short kEnteteVoieSize = kEnteteVoie.size() ;
  short kSeparatorSize = kSeparator.size() ;
  string lLine;
  
  if (!fEdwFile.is_open()) fEdwFile.open(fEdwFileName.c_str(),ios::in) ;
  if (!fRootFile->IsOpen()) fRootFile->Open(fRootFileName.c_str(),"RECREATE") ;
  if (!fEdwFile) {
    cerr << "FillRootFile: error opening stream from " << fEdwFileName << endl;
    return 1;
  }
  if (fRootFile->IsZombie()) {
    cerr << "FillRootFile: error opening Root file " << fRootFileName << endl;
    return 1;
  }
  // Run header
  while ( lLine.compare(0,kEnteteEvtSize,kEnteteEvt) ) {
    getline(fEdwFile,lLine);
    if (!fEdwFile.good()) {
      cerr << "FillRootFile: "<< kEnteteEvt<< " not found" << endl;
      return 1;
    }
  }

  fRootFile->cd();
  EdwEvent* Event = new EdwEvent();
  TTree EdwTree("EdwTree","Edelweiss Raw Data Tree");
  EdwTree.SetDirectory(fRootFile);
  EdwTree.Branch("Event","EdwEvent",&Event,aBufSize);
  // Loop on events
  while ( fEdwFile.good() ) {
    Event->Clear();
    Event->Header()->SetRun(fEdwRunName);
    while ( lLine.compare(0,kEnteteVoieSize,kEnteteVoie) ) {
      getline(fEdwFile,lLine);
      string::size_type diese_pos = lLine.find("#",0);
      string::size_type eq_pos = lLine.find("=",0);
      if (eq_pos != string::npos) {
	string lField = lLine.substr(0,eq_pos-1);
	string lValue = lLine.substr(eq_pos+1);
	if (diese_pos != string::npos) lValue = lLine.substr(eq_pos+1,diese_pos-1-eq_pos) ;
	if (lField == "Numero") Event->Header()->SetNum( atol(lValue.c_str()) );
	if (lField == "Date.secondes") Event->Header()->SetDateSec( atol(lValue.c_str()) );
	if (lField == "Date.microsecs") Event->Header()->SetDateMuSec( atoi(lValue.c_str()) );
	if (lField == "Delai") Event->Header()->SetSambaDelay( atof(lValue.c_str()) );
	if (lField == "Liste:31-0") Event->Header()->SetTriggerBit(1,(UInt_t)atoi(lValue.c_str()));
	if (lField == "Liste:63-32") 
	  Event->Header()->SetTriggerBit(2,(UInt_t)atoi(lValue.c_str()));
      } // else cerr << "Strange line for event header: " << lLine << endl;
    }

    // Loop on pulses within a given event
    int lNopulse=0;
    while (lNopulse == 0) {
      EdwPulse* lPulse = new EdwPulse();
      lPulse->SetChannel(lLine.substr(8,lLine.size()-9));
      Short_t lPulseSize=0;
      int lPtsFiltre=0;
      while ( lLine.compare(0,kSeparatorSize,kSeparator) ) { // Pulse header
	getline(fEdwFile,lLine);
	string::size_type diese_pos = lLine.find("#",0);
	string::size_type eq_pos = lLine.find("=",0);
	if (eq_pos != string::npos) {
	  string lField = lLine.substr(0,eq_pos-1);
	  lField = lField.substr(lField.find_first_not_of("\t"));
	  string lValue = lLine.substr(eq_pos+1);
	  if (diese_pos != string::npos) lValue = lLine.substr(eq_pos+1,diese_pos-1-eq_pos) ;
	  if (lField == "Dimension") lPulseSize=atoi(lValue.c_str());
	  if (lField == "Horloge") lPulse->SetSampling_ns( (ULong_t)atof(lValue.c_str())*1000000 ) ; // ms to ns
	  if (lField == "Filtre.nb") lPtsFiltre=atoi((lValue).c_str());
	  if (lField == "Base") lPulse->SetSambaBase(atof(lValue.c_str()));
	  if (lField == "Bruit") lPulse->SetSambaNoise(atof(lValue.c_str()));
	  if (lField == "Amplitude") lPulse->SetSambaAmpl(atof(lValue.c_str()));
	} // else cerr << "Strange line for pulse header: " << lLine << endl;
	if (!fEdwFile.good()) cerr << "Error reading pulse header" << endl;
      }
      if ( lPtsFiltre ) fEdwFile.ignore(8*lPtsFiltre); // filtered data = 8 bytes
      // First check if the pulse is here indeed!
      long lPos = fEdwFile.tellg();
      getline(fEdwFile,lLine);
      fEdwFile.seekg(lPos);
      if ( (!lLine.compare(0,kEnteteEvtSize,kEnteteEvt)) || 
	   (!lLine.compare(0,kEnteteVoieSize,kEnteteVoie))) {
	// No pulse, just a header.. need to record the pulse in the tree but with no trace?
      } else { // Read the pulse
	Short_t* lArray = new Short_t[lPulseSize];
	fEdwFile.read((char*)lArray,lPulseSize*2);
	if (fEdwFile.fail()) cerr << "SambaToRoot::FillRootFile: Error reading a pulse for event " <<Event->Header()->Num()<< endl;
	if ((fByteOrdering == kBigEndian && testbyteorder() == kLittleEndian) ||
	    (fByteOrdering == kLittleEndian && testbyteorder() == kBigEndian)) {
#ifdef DARWIN
	  for (Short_t i=0;i<lPulseSize;i++) lArray[i]=ntohs(lArray[i]);
#else
	  for (Short_t i=0;i<lPulseSize;i++) lArray[i]=R__bswap_16(lArray[i]);
#endif
	}
	lPulse->SetTrace(lPulseSize,lArray);
	delete[] lArray;
	Event->AddPulse(lPulse);
      }
      char toto; // Je comprends pas pourquoi il faut faire ca. Admettons...
      long pos=fEdwFile.tellg();
      fEdwFile.get(toto);
      if ( fEdwFile.good() ) {
	fEdwFile.seekg(pos);
	getline(fEdwFile,lLine);
	//	cout << lLine << endl;
	if ( ! lLine.compare(0,kEnteteEvtSize,kEnteteEvt) ) lNopulse=1;
      } else lNopulse=1;
      if (fEdwFile.bad()) cerr << "Error reading the edw file.." <<endl;
    }
    fRootFile->cd(); EdwTree.Fill();
  }
  fRootFile->cd(); EdwTree.Write("",kOverwrite);
  fRootFile->Close() ;

  return 0;
}
