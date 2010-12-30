/**
   @file    EdwEvent.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwEvent class. See the .h file
*/

#include "EdwEvent.h"

ClassImp(EdwEvent);

EdwEvent::EdwEvent() : TObject() {
  fHeader = new EdwEventHeader() ;
  fPulses = new TObjArray() ;
}

void EdwEvent::Clear() {
  fPulses->Delete();
  fHeader->Clear() ;
}

EdwEvent::~EdwEvent() {

}

EdwPulse* EdwEvent::Pulse(string aChannel) {

  EdwPulse* lPulse = NULL;
  for (unsigned int i=0; i<this->NbPulses(); i++) {
    if ( this->Pulse(i)->Channel() == aChannel) {
      if (this->Pulse(i)->TraceSize() != 0) // pulse de longueur nulle donne rien
	lPulse = this->Pulse(i);
    }
  }
  // (we don't check that there could be several pulses with same channel..)
  // When using this function must check if it gives NULL!
  return lPulse;
}

void EdwEvent::PlotTraces(string aPlotName, Bool_t aPerBolo) {

  if (aPerBolo) {
    // todo!
  } else {
    Int_t lNb = this->NbPulses();
    Int_t nx = 3 ; // By choice
    Int_t ny = (Int_t)((Float_t)lNb/(Float_t)nx) ;
    if (nx*ny != lNb) ny+=1;
    TCanvas* c = new TCanvas("c","canvas",350*nx,250*ny);
    c->Divide(nx,ny);
    for (Int_t i=0; i<lNb; i++) {
      EdwPulse* lPulse = this->Pulse(i);
      vector<Short_t> toto = lPulse->Trace() ;
      TH1F* lTrace = new TH1F("toto",(lPulse->Channel()).c_str(),lPulse->TraceSize(),-0.5,lPulse->TraceSize()-0.5) ;
      for (Int_t j=0;j<lTrace->GetNbinsX();j++) lTrace->SetBinContent(j+1,(Float_t)toto.at(j)); // Root binning convention
      c->cd(i+1);
      lTrace->Draw();
    }
    c->SaveAs(aPlotName.c_str());
    delete c;
  }

}

void EdwEvent::Dump(string aFile, string aChannel) {

  if (aFile == "None") {
    cout << "----------------------------------------" << endl;
    cout << "-- EDW Raw Event " << fHeader->Num() << " from run " << fHeader->Run() << endl;
    cout << "Samba delay: " << fHeader->SambaDelay() << endl;
    cout << "Date(s): " << fHeader->DateSec() << " - Date(mus): " << fHeader->DateMuSec() << endl;
    for (UInt_t i=0; i<this->NbPulses(); i++) {
      EdwPulse* lPulse = this->Pulse((Int_t)i);
      if (aChannel == "all" || aChannel == lPulse->Channel()) {
	cout << "------ Channel " << lPulse->Channel()<<" ("<<lPulse->TraceSize()<<" points) ------"<<endl;
	for (UInt_t j=0; j<(UInt_t)(lPulse->TraceSize()); j++) cout << lPulse->Trace(j) << " ";
	cout << endl;
      }
    }
    cout << "----------------------------------------" << endl;

  } else {
    ofstream lFile(aFile.c_str(),ios::out);
    lFile << "----------------------------------------" << endl;
    lFile << "-- EDW Raw Event " << fHeader->Num() << " from run " << fHeader->Run() << endl;
    lFile << "Date(s): " << fHeader->DateSec() << " - Date(mus): " << fHeader->DateMuSec() << endl;
    for (UInt_t i=0; i<this->NbPulses(); i++) {
      EdwPulse* lPulse = this->Pulse((Int_t)i);
      if (aChannel == "all" || aChannel == lPulse->Channel()) {
	lFile << "------ Channel " << lPulse->Channel()<<endl;
	for (UInt_t j=0; j<(UInt_t)(lPulse->TraceSize()); j++) 
	  lFile << lPulse->Trace(j) << endl;
      }
    }
    lFile << "----------------------------------------" << endl;
    lFile.close();
  }

}

vector<time_t> GetMinMaxTimes(TChain* aEvtChain, Bool_t aFast) {
  // a optimiser: mettre des setbranchstatus pour ne lire que les entetes
  time_t tmin = time(NULL);
  time_t tmax = 0;
  EdwEvent* lEvt=new EdwEvent();
  aEvtChain->SetBranchAddress("Event",&lEvt);
  if (!aFast) {
    for (UInt_t i=0; i<aEvtChain->GetEntries(); i++) {
      aEvtChain->GetEntry(i);
      time_t lTime = lEvt->Header()->DateSec();
      if (lTime < tmin) tmin = lTime;
      if (lTime > tmax) tmax = lTime;
      lEvt->Clear();
    }
  } else {
    aEvtChain->GetEntry(0);
    tmin = lEvt->Header()->DateSec();
    lEvt->Clear();
    aEvtChain->GetEntry(aEvtChain->GetEntries()-1);
    tmax = lEvt->Header()->DateSec();
    lEvt->Clear();
  }
  delete lEvt;
  
  vector<time_t> lMinmaxtimes;
  lMinmaxtimes.push_back(tmin); lMinmaxtimes.push_back(tmax);

  return lMinmaxtimes;
}

ULong_t GetFirstEventAfter(TChain* aEvtChain, time_t aT0) {

  EdwEvent* lEvt=new EdwEvent();
  aEvtChain->SetBranchAddress("Event",&lEvt);
  ULong_t a = 0;
  ULong_t b = aEvtChain->GetEntries()-1;
  aEvtChain->GetEntry(b);
  // Case aT0 > all events: by convention, return "N".
  if (lEvt->Header()->DateSec() < aT0) {
    b = aEvtChain->GetEntries(); // CONVENTION
  } else {
    lEvt->Clear();
    aEvtChain->GetEntry(0);
    // Case aT0 <= all events: return 0;
    if (lEvt->Header()->DateSec() >= aT0) {
      b = 0;
    } else {
      // Dichotomy loop. Rmk: for ULong_t, 5/2 = 2
      do {
	lEvt->Clear();
	ULong_t c = (a+b)/2 ;
	aEvtChain->GetEntry(c);
	time_t lTime = lEvt->Header()->DateSec();
	if (lTime >= aT0) b = c;
	else a = c;
	lEvt->Clear();
      } while (b != (a+1));
    }
  }
  delete lEvt;

  return b;
}

ULong_t GetLastEventBefore(TChain* aEvtChain, time_t aT0) {

  EdwEvent* lEvt=new EdwEvent();
  aEvtChain->SetBranchAddress("Event",&lEvt);
  ULong_t a = 0;
  ULong_t b = aEvtChain->GetEntries()-1;
  aEvtChain->GetEntry(b);
  // Case aT0 > all events: return last event.
  if (lEvt->Header()->DateSec() < aT0) {
    a = aEvtChain->GetEntries()-1;
  } else {
    lEvt->Clear();
    aEvtChain->GetEntry(0);
    // Case aT0 <= all events: return "N" by convention;
    if (lEvt->Header()->DateSec() >= aT0) {
      a = aEvtChain->GetEntries();
    } else {
      // Dichotomy loop. Rmk: for ULong_t, 5/2 = 2
      do {
	lEvt->Clear();
	ULong_t c = (a+b)/2 ;
	aEvtChain->GetEntry(c);
	time_t lTime = lEvt->Header()->DateSec();
	if (lTime >= aT0) b = c;
	else a = c;
	lEvt->Clear();
      } while (b != (a+1));
    }
  }
  delete lEvt;

  return a;
}

