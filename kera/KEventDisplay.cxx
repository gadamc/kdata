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
#include "TString.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include "TText.h"
#include "TPad.h"

#include "KBaselineRemoval.h"
#include "KPulseAnalysisChain.h"
#include "KPatternRemoval.h"
using namespace std;
ClassImp(KEventDisplay);

const UInt_t __k_NumDisplayHists = 9;

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
  fResizeStatWindow = true;
}

void KEventDisplay::DisplayEvent(EdwEvent *e, KHLABolometerRecord *b)
{
  SetEvent(e,b);
  DisplayEvent();
}

void KEventDisplay::DisplayEvent(EdwEvent *e, const char* boloName)
{
  SetEvent(e,boloName);
  DisplayEvent();
}

void KEventDisplay::DisplayEvent()
{
  if(fEdwEvent == 0 && fBoloName.size() == 0) {
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

  //Draw the Pulse Canvas
  for(UInt_t i = 1; i <= __k_NumDisplayHists; i++){ //9 is the number of pads in the Canvas
    fPulseCanvas->cd(i);
    if(i <= fNumPulseHists)
      fPulseHists[i-1].Draw();
    else 
      gPad->Clear();
  }
  fPulseCanvas->Draw();

  DrawStatsCanvas();

}

void KEventDisplay::SetEvent(EdwEvent *e, KHLABolometerRecord *b)
{

  fEdwEvent = e;
  fBoloName = b->GetDetectorName();
  
  if(fBolo == 0)
    fResizeStatWindow = true;
  fBolo = b;
  
  
  //need to find the appropriate pulses
  if(!SetUpPulses()){
    cout << "KEventDisplay - Unable to SetUpPulses()" << endl;
    return;
  }

}

void KEventDisplay::SetEvent(EdwEvent *e, const char* boloName)
{
  
  fEdwEvent = e;
  fBoloName = boloName;
  if(fBolo != 0)
    fResizeStatWindow = true;
  
  fBolo = 0;
  //need to find the appropriate pulses
  if(!SetUpPulses()){
    cout << "KEventDisplay - Unable to SetUpPulses()" << endl;
    return;
  }
  
}

void KEventDisplay::SetUpCanvas(void)
{
  Int_t width = 250;
  Int_t height = 300;
  
  
  if(fStatCanvas == 0){

    Int_t textHeightInPixels = 14;
    Int_t textFont = 43;
 
    fStatCanvas = new TCanvas("KED_fStatCanvas", "Stat Canvas", 715, 10, width, height);
    fStatCanvas->cd();
    
    TText *myText;
    myText = new TText(0,0, "");
    myText->SetName("detectorName");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(30);
    myText->Draw();
    
    myText = new TText(0,0, "");
    myText->SetName("qvalue");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("erecoil");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("eventFlag");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("chi2Flag");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("voltageFlag");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ionFlag");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();

    myText = new TText(0, 0, "");
    myText->SetName("cutFlag");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ionPulseTimeOffset");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("Ecollectrode1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBasecollectrode1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoisecollectrode1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("Ecollectrode2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBasecollectrode2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoisecollectrode2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("Eveto1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseveto1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseveto1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
        
    myText = new TText(0, 0, "");
    myText->SetName("Eveto2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseveto2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseveto2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
        
    myText = new TText(0, 0, "");
    myText->SetName("Eguard1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseguard1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseguard1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
        
    myText = new TText(0, 0, "");
    myText->SetName("Eguard2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseguard2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseguard2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
        
    myText = new TText(0, 0, "");
    myText->SetName("Eheat1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseheat1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseheat1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
        
    myText = new TText(0, 0, "");
    myText->SetName("Eheat2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("EBaseheat2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("ENoiseheat2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();

    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderRun");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderSambaNumber");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderNum");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderDateSec");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
   
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderDateMuSec");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderHumanDate");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderHumanDateNotify");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderSambaDelay");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderTriggerBit1");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
    myText = new TText(0, 0, "");
    myText->SetName("edwHeaderTriggerBit2");
    myText->SetTextFont(textFont);
    myText->SetTextSizePixels(textHeightInPixels);
    myText->Draw();
    
  }
  
  if(fBolo != 0){
    width = 550;
    height = 700;
  }
  else {
    width = 250;
    height = 300;
  }
  if(fResizeStatWindow) fStatCanvas->SetWindowSize(width, height);
  fResizeStatWindow = false;  //if the user manipulates the size of the stat window
                              //then it won't change sizes unless the bolometer
                              //record pointer changes from 0 to non-zero or vice-versa.
  
  if(fPulseCanvas == 0){
    fPulseCanvas = new TCanvas("KED_fPulseCanvas", "Pulse Canvas", 10, 10, 700, 700);
    fPulseCanvas->Divide(3,3);
    //fPulseCanvas->cd(1);
  }

}

void KEventDisplay::DrawStatsCanvas(void)
{
  if(fStatCanvas == 0) 
    return;
  
  fStatCanvas->cd();
  
  Double_t yTop = 0.98;
  Double_t ySpace = 0.03;
  Double_t yPos = yTop;
  Double_t xLeft = 0.02;
  
  TString aString;
  string EUnit = "keV";
  
  TText *aText = (TText *)gPad->GetPrimitive("detectorName");
  if(aText) {
    aText->SetText(xLeft, yPos-=ySpace, fBoloName.c_str()); 
    aText->Draw();
  } 
  
  EdwEventHeader *header = fEdwEvent->Header();

  
  if(fEdwEvent != 0 && header != 0){
    //now draw the raw pulse stats.
    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderRun");
    if(aText){
      aString.Form("%s %s", "Run ", header->Run().c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderSambaNumber");
    if(aText){
      
      aString.Form("%s %d", "Samba DAQ ", header->SambaNum());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderNum");
    if(aText){

      aString.Form("%s %d", "Event Num ", header->Num());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderDateSec");
    if(aText){
      aString.Form("%s %d", "DateSec ", header->DateSec());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else  {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderDateMuSec");
    if(aText){
      aString.Form("%s %d", "DateMuSec ", header->DateMuSec());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderHumanDateNotify");
    if(aText){
      aString.Form("%s","Year-M-D HH:MM:SS.ns");
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderHumanDate");
    if(aText){
      TTimeStamp eventTime(header->DateSec(), header->DateMuSec()*1000.);
      aString.Form("%s",eventTime.AsString("c"));
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderSambaDelay");
    if(aText){
      aString.Form("%s %#.4f","Samba Delay ", header->SambaDelay());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderTriggerBit1");
    if(aText){
      aString.Form("%s 0x%X","Trigger Bit 1 ", header->TriggerBit(1));
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("edwHeaderTriggerBit2");
    if(aText){
      aString.Form("%s 0x%X","Trigger Bit 2 ", header->TriggerBit(2));
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    

  }
  
  if(fBolo != 0){
    yPos = yTop;
    xLeft = xLeft + 0.45 + 0.02;

    aText = (TText *)gPad->GetPrimitive("qvalue");
    if(aText){
      //double qval = fBolo->GetQvalue();
      aString.Form("%s %#.2g", "Q value ", fBolo->GetQvalue());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    aText = (TText *)gPad->GetPrimitive("erecoil");
    if(aText){
      aString.Form("%s %#.2f %s", "Recoil ", fBolo->GetEnergyRecoil(), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("eventFlag");
    if(aText){
      aString.Form("%s %d (= %s)", "ERA Event Flag ", fBolo->GetEventFlag(), fBolo->GetEventCategory().c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("chi2Flag");
    if(aText){
      aString.Form("%s %d", "ERA Chi2 Flag ", fBolo->GetChi2Flag());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("voltageFlag");
    if(aText){
      aString.Form("%s %d", "Voltage Flag ", fBolo->GetVoltageFlag());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ionFlag");
    if(aText){
      aString.Form("%s %d%d %d%d %d%d", "Ion Flags ", fBolo->TestIonFlag(0),
                   fBolo->TestIonFlag(1), fBolo->TestIonFlag(2), fBolo->TestIonFlag(3),
                   fBolo->TestIonFlag(4), fBolo->TestIonFlag(5));
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("cutFlag");
    if(aText){
      aString.Form("%s %d%d %d%d %d%d %d %d %d%d %d%d %d%d %d %d %d", "Cut Flags ", fBolo->TestCutsBit(0),
                   fBolo->TestCutsBit(1), fBolo->TestCutsBit(2), fBolo->TestCutsBit(3),
                   fBolo->TestCutsBit(4), fBolo->TestCutsBit(5), fBolo->TestCutsBit(6),
                   fBolo->TestCutsBit(7), fBolo->TestCutsBit(8), fBolo->TestCutsBit(9), 
                   fBolo->TestCutsBit(10), fBolo->TestCutsBit(11), fBolo->TestCutsBit(12),
                   fBolo->TestCutsBit(13), fBolo->TestCutsBit(14), fBolo->TestCutsBit(15),
                   fBolo->TestCutsBit(16));
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("ionPulseTimeOffset");
    if(aText){
      aString.Form("%s %d", "Ion Pulse Time Offset [micro-sec] ", fBolo->GetIonPulseTimeOffset()*10);
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }

    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eheat1");
    if(aText && fBolo->GetEnergyHeat(1) != -9999.){
      aString.Form("%s %#.2f %s", "Heat 1 ", fBolo->GetEnergyHeat(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseheat1");
    if(aText && fBolo->GetEnergyBaselineHeat(1) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Heat 1 ", fBolo->GetEnergyBaselineHeat(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseheat1");
    if(aText &&  fBolo->GetBaselineNoiseHeat(1) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Heat 1 ", fBolo->GetBaselineNoiseHeat(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eheat2");
    if(aText && fBolo->GetEnergyHeat(2) != -9999.){
      aString.Form("%s %#.2f %s", "Heat 2 ", fBolo->GetEnergyHeat(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseheat2");
    if(aText && fBolo->GetEnergyBaselineHeat(2) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Heat 2 ", fBolo->GetEnergyBaselineHeat(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseheat2");
    if(aText && fBolo->GetBaselineNoiseHeat(2) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Heat 2 ", fBolo->GetBaselineNoiseHeat(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos -= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Ecollectrode1");
    if(aText && fBolo->GetEnergyCollectrode(1) != -9999.){
      aString.Form("%s %#.2f %s", "Collectrode 1 ", fBolo->GetEnergyCollectrode(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBasecollectrode1");
    if(aText && fBolo->GetEnergyBaselineCollectrode(1) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Collectrode 1 ", fBolo->GetEnergyBaselineCollectrode(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoisecollectrode1");
    if(aText && fBolo->GetBaselineNoiseCollectrode(1) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Collectrode 1 ", fBolo->GetBaselineNoiseCollectrode(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
        
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Ecollectrode2");
    if(aText && fBolo->GetEnergyCollectrode(2) != -9999.){
      aString.Form("%s %#.2f %s", "Collectrode 2 ", fBolo->GetEnergyCollectrode(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBasecollectrode2");
    if(aText && fBolo->GetEnergyBaselineCollectrode(2) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Collectrode 2 ", fBolo->GetEnergyBaselineCollectrode(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoisecollectrode2");
    if(aText && fBolo->GetBaselineNoiseCollectrode(2) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Collectrode 2 ", fBolo->GetBaselineNoiseCollectrode(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    } 
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eveto1");
    if(aText && fBolo->GetEnergyVeto(1) != -9999.){
      aString.Form("%s %#.2f %s", "Veto 1 ", fBolo->GetEnergyVeto(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseveto1");
    if(aText && fBolo->GetEnergyBaselineVeto(1) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Veto 1 ", fBolo->GetEnergyBaselineVeto(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseveto1");
    if(aText && fBolo->GetBaselineNoiseVeto(1) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Veto 1 ", fBolo->GetBaselineNoiseVeto(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eveto2");
    if(aText && fBolo->GetEnergyVeto(2) != -9999.){
      aString.Form("%s %#.2f %s", "Veto 2 ", fBolo->GetEnergyVeto(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseveto2");
    if(aText && fBolo->GetEnergyBaselineVeto(2) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Veto 2 ", fBolo->GetEnergyBaselineVeto(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseveto2");
    if(aText && fBolo->GetBaselineNoiseVeto(2) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Veto 2 ", fBolo->GetBaselineNoiseVeto(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eguard1");
    if(aText && fBolo->GetEnergyGuard(1) != -9999.){
      aString.Form("%s %#.2f %s", "Guard 1 ", fBolo->GetEnergyGuard(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseguard1");
    if(aText && fBolo->GetEnergyBaselineGuard(1) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Guard 1 ", fBolo->GetEnergyBaselineGuard(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseguard1");
    if(aText && fBolo->GetBaselineNoiseGuard(1) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Guard 1 ", fBolo->GetBaselineNoiseGuard(1), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    yPos-= 0.01;
    
    aText = (TText *)gPad->GetPrimitive("Eguard2");
    if(aText && fBolo->GetEnergyGuard(2) != -9999.){
      aString.Form("%s %#.2f %s", "Guard 2 ", fBolo->GetEnergyGuard(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("EBaseguard2");
    if(aText && fBolo->GetEnergyBaselineGuard(2) != -9999.){
      aString.Form("%s %#.2f %s", "Baseline Guard 2 ", fBolo->GetEnergyBaselineGuard(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }
    
    aText = (TText *)gPad->GetPrimitive("ENoiseguard2");
    if(aText && fBolo->GetBaselineNoiseGuard(2) != -9999.){
      aString.Form("%s %#.2f %s", "Noise Guard 2 ", fBolo->GetBaselineNoiseGuard(2), EUnit.c_str());
      aText->SetText(xLeft, yPos-=ySpace, aString.Data()); 
      aText->Draw();
    }
    else if(aText) {
      aText->SetText(0, 0, ""); 
      aText->Draw();
    }

    yPos-= 0.01;
    
    
  }

  
  
    
  
}

Bool_t KEventDisplay::SetUpPulses(void) //should I make this some sort of static function?
{
  

  if(fBoloName.size() == 0)
    return false;

  if(fEdwEvent == 0)
    return false;

  if(fPulseHists == 0){
    fPulseHists = new TH1D[__k_NumDisplayHists];
    for(UInt_t i = 0; i < __k_NumDisplayHists; i++)
      fPulseHists[i].AddDirectory(0); //don't let these be deleted by any TFiles that get deleted.
  }

  if(fBolo != 0) {
    if(fBolo->GetNumPulseRecords() <= 0) {
      cout << "KHLABolometerRecord is not associated with any PulseRecords." << endl;
      //return false;
    }
  }

  try {

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
      if(fBoloName == subPulseName) {
        //cout << "KEventDisplay - found pulse: " << pulseName << " contains " << boloName << " index " << i << endl;
        fPulseIndex.push_back(i);
      }
      if(detectorSuffix != 0)
        delete [] detectorSuffix;
      
      if(detectorNum != 0)
        delete [] detectorNum;
    }
    
    fNumPulseHists = 0;
    
    if(fBolo != 0){
      if((Int_t)fPulseIndex.size() != fBolo->GetNumPulseRecords()){
        cout << "KEventDisplay - Number of Pulses Mismatch. Please submit a bug report to https://edwdev-ik.fzk.de/bugs" << endl;
        cout << "EdwEvent Found " << fPulseIndex.size() << endl;
        cout << "KHLABolometrRecord Found " << fBolo->GetNumPulseRecords() << endl;
      }
      fNumPulseHists = ((Int_t)fPulseIndex.size() >= fBolo->GetNumPulseRecords()) ? fPulseIndex.size() : fBolo->GetNumPulseRecords();
    }
    else {
      fNumPulseHists = fPulseIndex.size();
    }
    
    //MAKE SURE WE DON'T EXCEED THE MAXIMUM NUMBER OF HISTS ALLOWED 
    //CHECK WITH THE __k_NumDisplayHists variable and choose the smaller of the two.
    if(fNumPulseHists > __k_NumDisplayHists){
      cerr << "KEventDisplay::SetUpPulses. There are more histograms found for this bolometer than is possible to display." << endl;
      cerr << "        This is due to lazy programming. You'll need to fix the code so that the display canvas adapts to " << endl;
      cerr << "        the number of pulses in an event." << endl;
      fNumPulseHists = __k_NumDisplayHists;
    }
    
    for(UInt_t i = 0; i < fNumPulseHists; i++){
      //cout << "Loading " << i << "th Pulse at Index " << fPulseIndex.at(i) << endl;
      pulse = fEdwEvent->Pulse(fPulseIndex.at(i));
      fPulseHists[i].Reset();
      fPulseHists[i].SetBins(pulse->TraceSize(), 0, pulse->TraceSize());
      fPulseHists[i].SetTitle(pulse->Channel().c_str());
      fPulseHists[i].SetName(pulse->Channel().c_str());
      
      
      if(fApplyBasicPulseProcessing) {
        //cout << "Processing pulse: " << pulse->Channel() << endl;
        KPulseAnalysisChain chain;
        chain.SetInputPulse(pulse->Trace());
        chain.SetIsOwner(true);  //the chain will delete all of the processors for me.
        
        KPatternRemoval *remove1 = 0;
        KPatternRemoval *remove2 = 0;
        KBaselineRemoval *baseline = 0;
        
        baseline = new KBaselineRemoval;
        chain.AddProcessor(baseline);
        
        if(!pulse->IsHeat()){
          remove1 = new KPatternRemoval;
          remove1->SetPatternLength(100);
          chain.AddProcessor(remove1);
          remove2 = new KPatternRemoval;
          remove2->SetPatternLength(200);
          chain.AddProcessor(remove2);
        }
        
        chain.RunProcess();
        
        vector<double> outPulse = chain.GetOutputPulse();
        
        for(UInt_t bin = 1; bin <= outPulse.size(); bin++)
          fPulseHists[i].SetBinContent(bin, outPulse.at(bin-1)); 
        
      }
      
      else{
        for(Short_t bin = 1; bin <= pulse->TraceSize(); bin++)
          fPulseHists[i].SetBinContent(bin, pulse->Trace(bin-1));  //need the bin-1 because the data is stored in vector with first index 0
        //cout << "Done Loading into hist." << endl;
      }
        
     
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

