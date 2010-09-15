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
#include "TText.h"
#include "TPad.h"

using namespace std;
ClassImp(KEventDisplay);

KEventDisplay::KEventDisplay(void)
{
  fStatCanvas = 0;
  fPulseCanvas = 0;
  fPulseHists = 0;
  fEdwEvent = 0;
  fBolo = 0;
  fApplyBasicPulseProcessing = true;
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
  
  TText *aText = (TText *)gPad->GetPrimitive("detectorName");
  
  TString aString;
  string EUnit = "keV";
  
  if(aText) {
    aString = fBolo->GetDetectorName();
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  } 
  
  aText = (TText *)gPad->GetPrimitive("qvalue");
  if(aText){
    aString.Form("%s %.2f", "Q value ", fBolo->GetQvalue());

    aString += fBolo->GetQvalue();
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("erecoil");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Recoil ", fBolo->GetEnergyRecoil(), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Ecollectrode1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Collectrode 1 ", fBolo->GetEnergyCollectrode(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBasecollectrode1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Collectrode 1 ", fBolo->GetEnergyBaselineCollectrode(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoisecollectrode1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Collectrode 1 ", fBolo->GetBaselineNoiseCollectrode(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Ecollectrode2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Collectrod 2 ", fBolo->GetEnergyCollectrode(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBasecollectrode2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Collectrode 2 ", fBolo->GetEnergyBaselineCollectrode(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoisecollectrode2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Collectrode 2 ", fBolo->GetBaselineNoiseCollectrode(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eveto1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Veto 1 ", fBolo->GetEnergyVeto(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseveto1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Veto 1 ", fBolo->GetEnergyBaselineVeto(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseveto1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Veto 1 ", fBolo->GetBaselineNoiseVeto(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eveto2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Veto 2 ", fBolo->GetEnergyVeto(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseveto2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Veto 2 ", fBolo->GetEnergyBaselineVeto(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseveto2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Veto 2 ", fBolo->GetBaselineNoiseVeto(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eguard1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Guard 1 ", fBolo->GetEnergyGuard(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseguard1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Guard 1 ", fBolo->GetEnergyBaselineGuard(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseguard1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Guard 1 ", fBolo->GetBaselineNoiseGuard(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eguard2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Guard 2 ", fBolo->GetEnergyGuard(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseguard2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Guard 2 ", fBolo->GetEnergyBaselineGuard(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseguard2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Guard 2 ", fBolo->GetBaselineNoiseGuard(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eheat1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Heat 1 ", fBolo->GetEnergyHeat(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseheat1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Heat 1 ", fBolo->GetEnergyBaselineHeat(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseheat1");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Heat 1 ", fBolo->GetBaselineNoiseHeat(1), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("Eheat2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Heat 2 ", fBolo->GetEnergyHeat(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("EBaseheat2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Baseline Heat 2 ", fBolo->GetEnergyBaselineHeat(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
  
  aText = (TText *)gPad->GetPrimitive("ENoiseheat2");
  if(aText){
    aString.Form("%s %.2f %s", "Energy Nosie Heat 2 ", fBolo->GetBaselineNoiseHeat(2), EUnit.c_str());
    aText->SetText(aText->GetX(), aText->GetY(), aString.Data()); 
    aText->Draw();
  }
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
    fStatCanvas = new TCanvas("KED_fStatCanvas", "KEventDisplay - Stat Canvas", 715, 10, 300, 600);
    fStatCanvas->cd();
    Double_t ySpace = 0.03;
    Double_t yPos = 0.98;
    Double_t xLeft = 0.05;
    TText *myText;
    myText = new TText(xLeft, yPos-=ySpace, "A Detector");
    myText->SetName("detectorName");
    myText->Draw();

    myText = new TText(xLeft, yPos-=ySpace, "QValue");
    myText->SetName("qvalue");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "Erecoil");
    myText->SetName("erecoil");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Ecollectrode1");
    myText->SetName("Ecollectrode1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBasecollectrode1");
    myText->SetName("EBasecollectrode1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoisecollectrode1");
    myText->SetName("ENoisecollectrode1");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Ecollectrode2");
    myText->SetName("Ecollectrode2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBasecollectrode2");
    myText->SetName("EBasecollectrode2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoisecollectrode2");
    myText->SetName("ENoisecollectrode2");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eveto1");
    myText->SetName("Eveto1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseveto1");
    myText->SetName("EBaseveto1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseveto1");
    myText->SetName("ENoiseveto1");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eveto2");
    myText->SetName("Eveto2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseveto2");
    myText->SetName("EBaseveto2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseveto2");
    myText->SetName("ENoiseveto2");
    myText->Draw();

    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eguard1");
    myText->SetName("Eguard1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseguard1");
    myText->SetName("EBaseguard1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseguard1");
    myText->SetName("ENoiseguard1");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eguard2");
    myText->SetName("Eguard2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseguard2");
    myText->SetName("EBaseguard2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseguard2");
    myText->SetName("ENoiseguard2");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eheat1");
    myText->SetName("Eheat1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseheat1");
    myText->SetName("EBaseheat1");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseheat1");
    myText->SetName("ENoiseheat1");
    myText->Draw();
    
    yPos-= 0.01;
    
    myText = new TText(xLeft, yPos-=ySpace, "Eheat2");
    myText->SetName("Eheat2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "EBaseheat2");
    myText->SetName("EBaseheat2");
    myText->Draw();
    
    myText = new TText(xLeft, yPos-=ySpace, "ENoiseheat2");
    myText->SetName("ENoiseheat2");
    myText->Draw();

    
    
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
    string pulseName;

    fPulseIndex.clear();
    EdwPulse *pulse = 0;

    for(UInt_t i = 0; i < fEdwEvent->NbPulses(); i++){
      pulse = fEdwEvent->Pulse(i);
      if(pulse == 0) {
        cout << "EdwEvent returned a NULL Pulse! : " << i << endl;
        return false;
      }
      pulseName = pulse->Channel();


      Char_t* detectorSuffix = new Char_t[pulseName.size()];
      Char_t* detectorNum = new Char_t[pulseName.size()];
      sscanf(pulseName.c_str(),"%*s %[A-Z]%[0-9]%*[A-Z]",detectorSuffix,detectorNum);
      TString subPulseName = "";
      subPulseName += detectorSuffix;
      subPulseName += detectorNum;
      if(boloName == subPulseName) {
        //cout << "KEventDisplay - found pulse: " << pulseName << " contains " << boloName << " index " << i << endl;
        fPulseIndex.push_back(i);
      }
      if(detectorSuffix != 0)
        delete [] detectorSuffix;
      
      if(detectorNum != 0)
        delete [] detectorNum;
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
      
      //if(fApplyBasicPulseProcessing)
        
        
        
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

