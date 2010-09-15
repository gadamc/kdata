//______________________________________________________________________
//
// KEventDisplay.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KEventDisplay.h"
#include "TCanvas.h"
#include "TH1D.h"
#include <stdexcept>
#include <iostream>
#include <vector>

using namespace std;
ClassImp(KEventDisplay);

KEventDisplay::KEventDisplay(void)
{
  fStatCanvas = 0;
  fPulseCanvas = 0;
  fPulseHists = 0;
  fEdwEvent = 0;
  fBolo = 0;
  InitializeMembers();
}


KEventDisplay::KEventDisplay(EdwEvent *e, KHLABolometerRecord *b)
{
  fStatCanvas = 0;
  fPulseCanvas = 0;
  fPulseHists = 0;
  fEdwEvent = 0;
  fBolo = 0;
  InitializeMembers();
  SetEvent(e,b);
}


KEventDisplay::~KEventDisplay(void)
{
  if(fStatCanvas != 0)
    delete fStatCanvas;

  if(fPulseCanvas != 0)
    delete fPulseCanvas;

  if(fPulseHists != 0)
    delete[] fPulseHists;
}

void KEventDisplay::InitializeMembers(void)
{
  fPulseIndex.clear();
  fNumPulseHists = 0;
}

void KEventDisplay::DisplayEvent()
{
  if(fBolo == 0 || fEdwEvent == 0) {
    cout << "KEventDisplay::DisplayEvent. You must first call SetEvent." << endl;
    return;
  }

  if(fPulseHists == 0){
    cout << "KEventDisplay::DisplayEvent. No Pulses To Draw." << endl;
    return;
  }

  SetUpCanvas();

  if(fPulseCanvas == 0 || fStatCanvas == 0){
    cout << "KEventDisplay::DisplayEvent. Unable to Create Canvases." << endl;
    return;
  }

  for(UInt_t i = 1; i <= 9 && i <= fNumPulseHists; i++){ //9 is the size of the Canvas
    fPulseCanvas->cd(i);
    fPulseHists[i-1].Draw();
  }
  fPulseCanvas->Draw();

  fStatCanvas->cd();
  //cout << "Ready to draw to the Stat Canvas" << endl;
  //Fill the Canvas with statistics from the Event.


}

void KEventDisplay::SetEvent(EdwEvent *e, KHLABolometerRecord *b)
{

  fEdwEvent = e;
  fBolo = b;

  //need to find the appropriate pulses
  if(!SetUpPulses()){
    cout << "KEventDisplay - Unable to SetUpPulses()" << endl;
    return;
  }

}

void KEventDisplay::SetUpCanvas(void)
{
  if(fStatCanvas == 0){
    fStatCanvas = new TCanvas("KED_fStatCanvas", "KEventDisplay - Stat Canvas", 915, 10, 250, 500);

  }

  if(fPulseCanvas == 0){
    fPulseCanvas = new TCanvas("KED_fPulseCanvas", "KEventDisplay - Pulse Canvas", 10, 10, 700, 700);
    fPulseCanvas->Divide(3,3);
    //fPulseCanvas->cd(1);
  }

}

Bool_t KEventDisplay::SetUpPulses(void) //should I make this some sort of static function?
{
  fNumPulseHists = 0;

  if(fBolo == 0)
    return false;

  if(fEdwEvent == 0)
    return false;

  if(fPulseHists != 0)
    delete[] fPulseHists;

  if(fBolo->GetNumPulseRecords() <= 0) {
    cout << "KHLABolometerRecord is not associated with any PulseRecords." << endl;
    return false;
  }

  try {

    fNumPulseHists = fBolo->GetNumPulseRecords();

    TString boloName = fBolo->GetDetectorName();
    TString pulseName;

    fPulseIndex.clear();
    EdwPulse *pulse = 0;

    for(UInt_t i = 0; i < fEdwEvent->NbPulses(); i++){
      pulse = fEdwEvent->Pulse(i);
      if(pulse == 0) {
        cout << "EdwEvent returned a NULL Pulse! : " << i << endl;
        return false;
      }
      pulseName = pulse->Channel();

      /*
      if (pulseName.Contains(boloName)) {
        //cout << "KEventDisplay - found pulse: " << pulseName << " contains " << boloName << " index " << i << endl;
        fPulseIndex.push_back(i);
      }
      */

      Char_t detectorSuffix[pulseName.Length()];
      Char_t detectorNum[pulseName.Length()];
      sscanf(pulseName,"%[A-Z]%[0-9]%*[A-Z]",detectorSuffix,detectorNum);
      TString subPulseName = "";
      subPulseName += detectorSuffix;
      subPulseName += detectorNum;
      if(boloName == subPulseName) {
        //cout << "KEventDisplay - found pulse: " << pulseName << " contains " << boloName << " index " << i << endl;
        fPulseIndex.push_back(i);
      }
    }

    if(fPulseIndex.size() != fNumPulseHists){
      cout << "KEventDisplay - Number of Pulses Mismatch." << endl;
      cout << "EdwEvent Found " << fPulseIndex.size() << endl;
      cout << "KHLABolometrRecord Found " << fNumPulseHists << endl;
    }
    else {
      //cout << "Found " << fNumPulseHists << " pulses " << endl;
    }

    Int_t numHists = (fPulseIndex.size() >= fNumPulseHists) ? fPulseIndex.size() : fNumPulseHists;

    fPulseHists = new TH1D[ numHists];

    for(UInt_t i = 0; i < numHists; i++){
      //cout << "Loading " << i << "th Pulse at Index " << fPulseIndex.at(i) << endl;
      pulse = fEdwEvent->Pulse(fPulseIndex.at(i));
      fPulseHists[i].AddDirectory(0); //don't let these be deleted by any TFiles that get deleted.
      fPulseHists[i].SetBins(pulse->TraceSize(), 0, pulse->TraceSize());
      fPulseHists[i].SetTitle(pulse->Channel().c_str());
      fPulseHists[i].SetName(pulse->Channel().c_str());
      for(Short_t bin = 1; bin <= pulse->TraceSize(); bin++)
        fPulseHists[i].SetBinContent(bin, pulse->Trace(bin-1));  //need the bin-1 because the data is stored in vector with first index 0
      //cout << "Done Loading into hist." << endl;
    }


  }
  catch (exception &e) {
    cout << "KEventDisplay::SetUpPulses - caught exception : " << e.what() << endl;
    return false;
  }


  return true;
}

TH1D KEventDisplay::GetPulseHistogram(UInt_t i) const
{
  TH1D theHist;

  if(i < fNumPulseHists)
    theHist = fPulseHists[i];

  return theHist;
}

