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
#include "KHLABolometerRecord.h"
#include <vector>

class TCanvas;
class TH1D;
class TObject;

class KEventDisplay  { 

public:
  //Constructors
  KEventDisplay(void);
  KEventDisplay(EdwEvent *e, KHLABolometerRecord *b);
  virtual ~KEventDisplay(void);
  
  void DisplayEvent(void);
  
  //getters
  TH1D GetPulseHistogram(UInt_t i) const;
  UInt_t GetNumPulses(void){return fNumPulseHists;}
  
  //setters
  void SetEvent(EdwEvent *e, KHLABolometerRecord *b);
  void SetApplyBasicPulseProcessing(Bool_t anOpt = true){ fApplyBasicPulseProcessing = anOpt;  }
  
private:

  TCanvas* fStatCanvas; 
  TCanvas* fPulseCanvas;
  TH1D *fPulseHists; //an array of histograms holding pulses
  EdwEvent *fEdwEvent;
  KHLABolometerRecord *fBolo;
  vector<Int_t> fPulseIndex;
  UInt_t fNumPulseHists; //number of fPulseHists.
  
  Bool_t fApplyBasicPulseProcessing;
  
  //private methods
  void InitializeMembers(void);
  Bool_t SetUpPulses(void);
  void SetUpCanvas(void);

  ClassDef(KEventDisplay,1);
};


#endif // __KEVENTDISPLAY_H__
