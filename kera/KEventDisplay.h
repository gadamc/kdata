//______________________________________________________________________
//
// KEventDisplay.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KEVENTDISPLAY_H__
#define __KEVENTDISPLAY_H__

#include "Rtypes.h"
#include "EdwEvent.h"
#include <vector>
#include "KHLABolometerRecord.h"
#include "KPulseAnalysisChain.h"

class TCanvas;
class TH1D;
class TObject;
class TString;

class KEventDisplay  { 

public:
  //Constructors
  KEventDisplay(void);
  KEventDisplay(EdwEvent *e, KHLABolometerRecord *b);
  virtual ~KEventDisplay(void);
  
  void DisplayEvent(void);
  void DisplayEvent(EdwEvent *e, KHLABolometerRecord *b);
  void DisplayEvent(EdwEvent *e, const char* boloName);
 
  void DisplayPower(void);
  void DisplayPower(EdwEvent *e, KHLABolometerRecord *b);
  void DisplayPower(EdwEvent *e, const char* boloName);
  
  //getters
  TH1D GetPulseHistogram(UInt_t i) const;
  UInt_t GetNumPulses(void){return fNumPulseHists;}
  
  //setters
  void SetEvent(EdwEvent *e, KHLABolometerRecord *b);
  void SetEvent(EdwEvent *e, const char* boloName);
  
  void SetApplyPulseProcessing(Bool_t anOpt = true){ fApplyPulseProcessing = anOpt;  }
  
  
private:

  TCanvas* fStatCanvas; 
  TCanvas* fPulseCanvas;
  TH1D *fPulseHists; //an array of histograms holding pulses
  EdwEvent *fEdwEvent;
  KHLABolometerRecord *fBolo;
  vector<Int_t> fPulseIndex;
  UInt_t fNumPulseHists; //number of fPulseHists.
  string fBoloName;
  
  Bool_t fApplyPulseProcessing;
  Bool_t fResizeStatWindow;
  Bool_t fDisplayPower;
  
  KPulseAnalysisChain fIonPulseAnalysisChain;
  KPulseAnalysisChain fHeatPulseAnalysisChain;
  KPulseAnalysisChain fPowerSpectrumChain;
  
  //private methods
  void InitializeMembers(void);
  Bool_t SetUpPulses(Bool_t displayPower = false);
  void SetUpCanvas(void);
  void DrawStatsCanvas(void);
  void SetUpPulseAnalysisChains(void);

  ClassDef(KEventDisplay,1);
};


#endif // __KEVENTDISPLAY_H__
