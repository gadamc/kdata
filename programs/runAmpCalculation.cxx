#include "KDataReader.h"
#include "KDataWriter.h"
#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KEvent.h"
#include "KLinearRemoval.h"
#include "KPatternRemoval.h"
#include "KFIRFilter.h"
#include "KRawMuonVetoSysRecord.h"
#include "KRawBolometerRecord.h"
#include "KAmpBolometerRecord.h"
#include "KRawSambaRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KSambaRecord.h"
#include "KIIRFirstOrder.h"
#include "KPulseAnalysisChain.h"
#include "KCurl.h"
#include "KJson.h"
#include "KWindow.h"
#include "KWindowDesign.h"
#include "KIIRFourthOrder.h"
#include "KRealToHalfComplexDFT.h"
#include "KHalfComplexPower.h"
#include "KPeakFinder.h"
#include "KOrderFilter.h"
#include <string>
#include <cstring>

using namespace std;
//this is a series of functions that take a raw KData file and process each pulse
//and create an 'amp' KData file. each process is defined and coded here using
//the classes available in kpta. 
//
//the starting point for reading this code is the function: runAmpCalculation
//void copyBasicData(KAmpBoloPulseRecord* pAmp, KRawBoloPulseRecord* pRaw);
//void moveSambaData(KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, 
//  KRawBoloPulseRecord* pRaw);
//void runHeatAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, 
//  KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name);
//void runIonAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, 
//  KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name);
//void runAmpCalculation(int argc, char* argv[]);

void moveSambaData(KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw)
{
  //move the samba data into a pulse analysis record. 
  KPulseAnalysisRecord *sambarec = ee->AddPulseAnalysisRecord(); 
  sambarec->SetBolometerRecord(boloAmp);  //link the TRefs in the different objects. This is important
  sambarec->SetBoloPulseRecord(pAmp);
  pAmp->AddPulseAnalysisRecord(sambarec);

  sambarec->SetAmp(pRaw->GetAmplitude());
  sambarec->SetName("samba");
  sambarec->SetIsBaseline(false);
  sambarec->SetBaselineAmplitudeWidth(pRaw->GetAmplitudeBaselineNoise());
  sambarec->SetUnit(0);

  sambarec = ee->AddPulseAnalysisRecord(); //add a new analysis record
  sambarec->SetBolometerRecord(boloAmp); //make the proper TRef links
  sambarec->SetBoloPulseRecord(pAmp);
  pAmp->AddPulseAnalysisRecord(sambarec);

  sambarec->SetAmp(pRaw->GetAmplitudeBaseline());
  sambarec->SetName("samba");
  sambarec->SetIsBaseline(true);
  sambarec->SetBaselineAmplitudeWidth(pRaw->GetAmplitudeBaselineNoise());
  sambarec->SetUnit(0);
}


void copyBasicData(KAmpBoloPulseRecord* pAmp, KRawBoloPulseRecord* pRaw)
{
   //copy the relevant information from the raw bolo pulse record to the amp pulse record.
    //one day, if i can get some template classes defined, this stuff could be more automatic. 
    //or, perhaps it could go inside of the KAmpEvent::AddBoloPulse method! It would be overloaded
    //with KAmpEvent::AddBoloPulse(KRawBoloPulseRecord *)
  
  pAmp->SetChannelName(pRaw->GetChannelName());
  pAmp->SetPulseTimeWidth(pRaw->GetPulseTimeWidth());
  pAmp->SetPretriggerSize(pRaw->GetPretriggerSize());
  pAmp->SetFilterSize(pRaw->GetFilterSize());
  pAmp->SetHeatPulseStampWidth(pRaw->GetHeatPulseStampWidth());
  pAmp->SetCryoPosition(pRaw->GetCryoPosition());
  pAmp->SetPolarFet(pRaw->GetPolarFet());
  pAmp->SetCorrPied(pRaw->GetCorrPied());
  pAmp->SetCompModul(pRaw->GetCompModul());
  pAmp->SetCorrTrngl(pRaw->GetCorrTrngl());
  pAmp->SetAmplModul(pRaw->GetAmplModul());
  pAmp->SetIsHeatPulse(pRaw->GetIsHeatPulse());
  pAmp->SetPulseLength(pRaw->GetPulseLength());
  pAmp->SetCorrPied(pRaw->GetCorrPied());
}


void runHeatAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, bool smartMem = true)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak(-1);
    start = 245; stop = 300;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    double maxValue = last->GetOutputPulse()[start];
    for(int b = start+1; b < stop; b++){
      if ( *(last->GetOutputPulse()+b) < maxValue){
        maxValue = *(last->GetOutputPulse()+b);
        peak = b;
      }
    }
    //now add the pulse analysis record and link the TRefs
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();  //add a new analysis record
    rec->SetBolometerRecord(boloAmp);
    rec->SetBoloPulseRecord(pAmp);
    pAmp->AddPulseAnalysisRecord(rec);

    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    //get the amplitude of the baseline
    peak = 100;
    maxValue = *(last->GetOutputPulse()+peak);
    rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    
    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    
  }
  else cout << "heat 1 processor failed" << endl;
  
}

void runIonAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, int polarity = 1, bool smartMem = true)
{
  bool theRet = false;
  if (polarity == 0) return;
  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak(-1);
    start = 6400; stop = 7000;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    double maxValue = last->GetOutputPulse()[start];
    if(polarity == -1){
      for(int b = start+1; b < stop; b++){
        if ( *(last->GetOutputPulse()+b) < maxValue){
          maxValue = *(last->GetOutputPulse()+b);
          peak = b;
        }
      }
    }
    else if (polarity == 1){
      for(int b = start+1; b < stop; b++){
        if ( *(last->GetOutputPulse()+b) > maxValue){
          maxValue = *(last->GetOutputPulse()+b);
          peak = b;
        }
      }
    }

    //now add the pulse analysis record and link the TRefs
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();  //add a new analysis record
    rec->SetBolometerRecord(boloAmp);  //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef

    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    //add another pulse analysis record to hold the estimate of this 
    //processor at the beginning of the pulse (where no event should occur)
    
    //get the amplitude of the baseline
    peak = 1600;
    maxValue = *(last->GetOutputPulse()+peak);
    rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    
    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
  }
  else cout << "ion 1 processor failed" << endl;
  
}

void runHeatAna2Pulse(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, bool smartMem = true)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak(-1);
    start = 245; stop = 300;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    double maxValue = last->GetOutputPulse()[start];
    for(int b = start+1; b < stop; b++){
      if ( *(last->GetOutputPulse()+b) < maxValue){
        maxValue = *(last->GetOutputPulse()+b);
        peak = b;
      }
    }
    
    //now add the pulse analysis record and link the TRefs
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();  //add a new analysis record
    rec->SetBolometerRecord(boloAmp);
    rec->SetBoloPulseRecord(pAmp);
    pAmp->AddPulseAnalysisRecord(rec);

    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
  }
  else cout << "heat 2 pulse processor failed" << endl;
  
}


void runHeatAna2Baseline(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, bool smartMem = true)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int peak = 100;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    double maxvalue = *(last->GetOutputPulse()+peak);
    rec->SetAmp(maxvalue);
    rec->SetName(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
   
    
  }
  else cout << "heat 2 baseline processor failed" << endl;
  
}



void runIonAna2Pulse(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, int polarity = 1, bool smartMem = true)
{
  bool theRet = false;
  if (polarity == 0) return;
  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  //set pattern removal
  KPatternRemoval *pat = (KPatternRemoval *)anaChain.GetProcessor(1);
  pat->SetPatternLength((Int_t)pRaw->GetHeatPulseStampWidth());
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak(-1);
    start = 6400; stop = 7000;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    double maxValue = last->GetOutputPulse()[start];
    if(polarity == -1){
      for(int b = start+1; b < stop; b++){
        if ( *(last->GetOutputPulse()+b) < maxValue){
          maxValue = *(last->GetOutputPulse()+b);
          peak = b;
        }
      }
    }
    else if (polarity == 1){
      for(int b = start+1; b < stop; b++){
        if ( *(last->GetOutputPulse()+b) > maxValue){
          maxValue = *(last->GetOutputPulse()+b);
          peak = b;
        }
      }
    }
    //now add the pulse analysis record and link the TRefs
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();  //add a new analysis record
    rec->SetBolometerRecord(boloAmp);
    rec->SetBoloPulseRecord(pAmp);
    pAmp->AddPulseAnalysisRecord(rec);

    rec->SetAmp(maxValue);
    rec->SetName(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
  }
  else cout << "heat 2 pulse processor failed" << endl;
  
}


void runIonAna2Baseline(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name, bool smartMem = true)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  //set pattern removal
  KPatternRemoval *pat = (KPatternRemoval *)anaChain.GetProcessor(1);
  pat->SetPatternLength((Int_t)pRaw->GetHeatPulseStampWidth());
  
  theRet =  anaChain.RunProcess(smartMem); //true says to use smart memory.
  if(theRet){
    int peak = 1600;
    KPtaProcessor *last = anaChain.GetProcessor(anaChain.GetNumProcessors()-1);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    double maxvalue = *(last->GetOutputPulse()+peak);
    
    rec->SetAmp(maxvalue);
    rec->SetName(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
   
    
  }
  else cout << "heat 2 baseline processor failed" << endl;
  
}


int main(int /*argc*/, char* argv[]){
  KDataReader f(argv[1]);
  KDataWriter ff(argv[2],"KAmpEvent");
  KAmpEvent *ee = (KAmpEvent *)ff.GetEvent();
  KRawEvent *e = (KRawEvent *)f.GetEvent();


  KCurl c;
  string json;
  KJson *doc;
  KJson *heatpulse;
  KJson *ionpulse;
  //get the heat template for chaleur FID804AB. For testing purposes,
  //we'll use just this template for all heat pulses.
  c.Get("https://edwdbik.fzk.de:6984", "/analysis/run13_templatepulse_chaleur_FID804AB");
  json = c.GetReturn();
  doc = KJson::Parse(json.data());
  heatpulse = KJson::GetObjectItem(doc, "pulse");
  vector<double> heatTemplate;
  for(int i = 0; i < KJson::GetArraySize(heatpulse); i++)
    heatTemplate.push_back(KJson::GetArrayItem(heatpulse, i)->valuedouble);
  
  //and use this for all ionization pulses for now. 
  c.Get("https://edwdbik.fzk.de:6984", "/analysis/run13_templatepulse_centre_FID804AB");
  json = c.GetReturn();
  doc = KJson::Parse(json.data());
  ionpulse = KJson::GetObjectItem(doc, "pulse");
  vector<double> ionTemplate;
  for(int i = 0; i < KJson::GetArraySize(ionpulse); i++)
    ionTemplate.push_back(KJson::GetArrayItem(ionpulse, i)->valuedouble);


  //set up some memory location for data processing
  unsigned int heatSize = 512;
  unsigned int ionSize = 8196;
  double* pulseIon1 = new double[ionSize];
  double* pulseIon2 = new double[ionSize];
  double* pulseHeat1 = new double[heatSize];
  double* pulseHeat2 = new double[heatSize];
 
 
  //set up the analysis processing chains.
  
  //analysis 1 heat
  KPulseAnalysisChain heatAna1;
  KLinearRemoval *basHeat1 = new KLinearRemoval;
  KIIRFirstOrder *iirHeat5_1= new KIIRFirstOrder(-0.96906742, 0.98453371,  0.98453371); //this is a butterworth, high pass filter at 500 Hz for the 100kHz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basHeat1->SetInputPulse(pulseHeat1); basHeat1->SetInputPulseSize(heatSize);
  basHeat1->SetOutputPulse(pulseHeat1); basHeat1->SetOutputPulseSize(heatSize);
  iirHeat5_1->SetInputPulse(pulseHeat1); iirHeat5_1->SetInputPulseSize(heatSize);
  iirHeat5_1->SetOutputPulse(pulseHeat2); iirHeat5_1->SetOutputPulseSize(heatSize);
    //add processors to the chain, in order.
  heatAna1.AddProcessor(basHeat1);
  heatAna1.AddProcessor(iirHeat5_1);
  
  
  //analysis 1 ion.
  KPulseAnalysisChain ionAna1;
  KLinearRemoval *basIon1 = new KLinearRemoval;
  KPatternRemoval *patIon1 = new KPatternRemoval;
  KIIRFirstOrder *iirIon500_1= new KIIRFirstOrder(-0.96906742, 0.98453371,  0.98453371); //this is a butterworth, high pass filter at 500 Hz for the 100kHz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basIon1->SetInputPulse(pulseIon1); basIon1->SetInputPulseSize(ionSize);  //notice how basIon1 and patIon1 point to the same memory
  basIon1->SetOutputPulse(pulseIon1); basIon1->SetOutputPulseSize(ionSize); //this is because these processors can be done in-place.
  patIon1->SetInputPulse(pulseIon1); patIon1->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  patIon1->SetOutputPulse(pulseIon1); patIon1->SetOutputPulseSize(ionSize);  
  iirIon500_1->SetInputPulse(pulseIon1); iirIon500_1->SetInputPulseSize(ionSize);
  iirIon500_1->SetOutputPulse(pulseIon2); iirIon500_1->SetOutputPulseSize(ionSize);
    //add processors to the chain, in order
  ionAna1.AddProcessor(basIon1);
  ionAna1.AddProcessor(patIon1);
  ionAna1.AddProcessor(iirIon500_1);
  
  
  //analysis 2 heat
  KPulseAnalysisChain heatAna2Pulse;
  KLinearRemoval *basHeat2 = new KLinearRemoval;
  KWindow *winHeat2 = new KWindow;
  double *mWin = KWindowDesign::GetTukeyWindow(512,0.50); 
  winHeat2->SetInputPulseSize(heatSize);
  winHeat2->SetOutputPulseSize(heatSize);
  winHeat2->SetWindow(mWin, heatSize);
  delete [] mWin; //have to clean this up.
  //KIIRFirstOrder *iirHeat5_2= new KIIRFirstOrder(-0.96906742, 0.98453371,  0.98453371);
  KIIRFourthOrder *iirHeat5_2= new KIIRFourthOrder(-3.58623981,  4.8462898 , -2.93042722,  0.6704579, 0.01658193,  0. , -0.03316386,  0. ,  0.01658193); //this is a butterworth, band pass filter between 5 Hz to 50 Hz
    //set up the memory allocation
  basHeat2->SetInputPulse(pulseHeat1); basHeat2->SetInputPulseSize(heatSize); 
  basHeat2->SetOutputPulse(pulseHeat1); basHeat2->SetOutputPulseSize(heatSize); //calculation done in-place
  winHeat2->SetInputPulse(pulseHeat1); winHeat2->SetInputPulseSize(heatSize);
  winHeat2->SetOutputPulse(pulseHeat1); winHeat2->SetOutputPulseSize(heatSize); //calculation done in-place
  iirHeat5_2->SetInputPulse(pulseHeat1); iirHeat5_2->SetInputPulseSize(heatSize);
  iirHeat5_2->SetOutputPulse(pulseHeat2); iirHeat5_2->SetOutputPulseSize(heatSize);
    //add processors to the chain, in order.
  heatAna2Pulse.AddProcessor(basHeat2);
  heatAna2Pulse.AddProcessor(winHeat2);
  heatAna2Pulse.AddProcessor(iirHeat5_2);
  
  
  //analysis 2 heat - baseline
  KPulseAnalysisChain heatAna2Baseline;
  KLinearRemoval *basHeat2b = new KLinearRemoval;
  double *mWinb = KWindowDesign::GetTukeyWindow(200,0.25);
  double *mFullWinb = new double[heatSize]; 
  memset(mFullWinb, 0, heatSize * sizeof(double));
  memcpy(mFullWinb, mWinb, 200*sizeof(double) );
  KWindow *winHeat2b = new KWindow;
  winHeat2b->SetInputPulseSize(heatSize);
  winHeat2b->SetOutputPulseSize(heatSize);
  winHeat2b->SetWindow(mFullWinb, heatSize);
  delete [] mWinb; //have to clean this up.
  delete [] mFullWinb;
  //KIIRFirstOrder *iirHeat5b_2= new KIIRFirstOrder(-0.96906742, 0.98453371,  0.98453371);
  KIIRFourthOrder *iirHeat5b_2= new KIIRFourthOrder(-3.58623981,  4.8462898 , -2.93042722,  0.6704579, 0.01658193,  0. , -0.03316386,  0. ,  0.01658193); //this is a butterworth, band pass filter between 5 Hz to 50 Hz
    //set up the memory allocation
  basHeat2b->SetInputPulse(pulseHeat1); basHeat2b->SetInputPulseSize(heatSize);
  basHeat2b->SetOutputPulse(pulseHeat1); basHeat2b->SetOutputPulseSize(heatSize);
  winHeat2b->SetInputPulse(pulseHeat1); winHeat2b->SetInputPulseSize(heatSize);
  winHeat2b->SetOutputPulse(pulseHeat1); winHeat2b->SetOutputPulseSize(heatSize); //calculation done in-place
  iirHeat5b_2->SetInputPulse(pulseHeat1); iirHeat5b_2->SetInputPulseSize(heatSize);
  iirHeat5b_2->SetOutputPulse(pulseHeat2); iirHeat5b_2->SetOutputPulseSize(heatSize);
    //add processors to the chain, in order.
  heatAna2Baseline.AddProcessor(basHeat2b);
  heatAna2Baseline.AddProcessor(winHeat2b);
  heatAna2Baseline.AddProcessor(iirHeat5b_2);
  
  
  
  //analysis 2 ion. pulse
  KPulseAnalysisChain ionAna2Pulse;
  KLinearRemoval *basIon2 = new KLinearRemoval;
  KPatternRemoval *patIon2 = new KPatternRemoval;
  mWin = KWindowDesign::GetBlackmanWindow(3200);
  double *mFullWin = new double[ionSize];
  memset(mFullWin, 0, ionSize * sizeof(double));
  memcpy(mFullWin+ionSize-3200, mWin, 3200*sizeof(double) );
  KWindow *winIonPulse2 = new KWindow;
  winIonPulse2->SetInputPulseSize(ionSize);
  winIonPulse2->SetOutputPulseSize(ionSize);
  winIonPulse2->SetWindow(mFullWin, ionSize);
  delete [] mFullWin;
  delete [] mWin;
  
  KIIRFourthOrder *iirIon50_2= new KIIRFourthOrder(-3.95982379,  5.88045441, -3.88143355,  0.96080294, 0.00019593, 0.        , -0.00039186,  0.        ,  0.00019593); //this is a butterworth, band pass filter between 50 Hz to the 500Hz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basIon2->SetInputPulse(pulseIon1); basIon2->SetInputPulseSize(ionSize);  //notice how basIon2 and patIon1 point to the same memory
  basIon2->SetOutputPulse(pulseIon1); basIon2->SetOutputPulseSize(ionSize); //this is because these processors can be done in-place.
  patIon2->SetInputPulse(pulseIon1); patIon2->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  patIon2->SetOutputPulse(pulseIon1); patIon2->SetOutputPulseSize(ionSize);  
  winIonPulse2->SetInputPulse(pulseIon1); winIonPulse2->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  winIonPulse2->SetOutputPulse(pulseIon1); winIonPulse2->SetOutputPulseSize(ionSize);
  iirIon50_2->SetInputPulse(pulseIon1); iirIon50_2->SetInputPulseSize(ionSize);
  iirIon50_2->SetOutputPulse(pulseIon2); iirIon50_2->SetOutputPulseSize(ionSize);
    //add processors to the chain, in order
  ionAna2Pulse.AddProcessor(basIon2);
  ionAna2Pulse.AddProcessor(patIon2);
  ionAna2Pulse.AddProcessor(winIonPulse2);
  ionAna2Pulse.AddProcessor(iirIon50_2);
  
  
  //anlaysis ion 2 baseline
  KPulseAnalysisChain ionAna2Baseline;
  KLinearRemoval *basIon2b = new KLinearRemoval;
  KPatternRemoval *patIon2b = new KPatternRemoval;
  mWin = KWindowDesign::GetBlackmanWindow(3200);
  mFullWin = new double[ionSize];
  memset(mFullWin, 0, ionSize * sizeof(double));
  memcpy(mFullWin, mWin, 3200*sizeof(double) );
  KWindow *winIonBaseline2b = new KWindow;
  winIonBaseline2b->SetInputPulseSize(ionSize);
  winIonBaseline2b->SetOutputPulseSize(ionSize);
  winIonBaseline2b->SetWindow(mFullWin, ionSize);
  delete [] mFullWin;
  delete [] mWin;
  
  KIIRFourthOrder *iirIon50_2b= new KIIRFourthOrder(-3.95982379,  5.88045441, -3.88143355,  0.96080294,  0.00019593, 0.        , -0.00039186,  0.        ,  0.00019593); //this is a butterworth, band pass filter between 50 Hz to the 500kHz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basIon2b->SetInputPulse(pulseIon1); basIon2b->SetInputPulseSize(ionSize);  //notice how basIon2 and patIon1 point to the same memory
  basIon2b->SetOutputPulse(pulseIon1); basIon2b->SetOutputPulseSize(ionSize); //this is because these processors can be done in-place.
  patIon2b->SetInputPulse(pulseIon1); patIon2b->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  patIon2b->SetOutputPulse(pulseIon1); patIon2b->SetOutputPulseSize(ionSize);  
  winIonBaseline2b->SetInputPulse(pulseIon1); winIonBaseline2b->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  winIonBaseline2b->SetOutputPulse(pulseIon1); winIonBaseline2b->SetOutputPulseSize(ionSize);
  iirIon50_2b->SetInputPulse(pulseIon1); iirIon50_2b->SetInputPulseSize(ionSize);
  iirIon50_2b->SetOutputPulse(pulseIon2); iirIon50_2b->SetOutputPulseSize(ionSize);
    //add processors to the chain, in order
  ionAna2Baseline.AddProcessor(basIon2b);
  ionAna2Baseline.AddProcessor(patIon2b);
  ionAna2Baseline.AddProcessor(winIonBaseline2b);
  ionAna2Baseline.AddProcessor(iirIon50_2b);
  
  
  
  
  //analysis 3 ion. pulse
  KPulseAnalysisChain ionAna3Pulse;
  KLinearRemoval *basIon3 = new KLinearRemoval;
  KPatternRemoval *patIon3 = new KPatternRemoval;
  mWin = KWindowDesign::GetBlackmanWindow(3200);
  mFullWin = new double[ionSize];
  memset(mFullWin, 0, ionSize * sizeof(double));
  memcpy(mFullWin+ionSize-3200, mWin, 3200*sizeof(double) );
  KWindow *winIonPulse3 = new KWindow;
  winIonPulse3->SetInputPulseSize(ionSize);
  winIonPulse3->SetOutputPulseSize(ionSize);
  winIonPulse3->SetWindow(mFullWin, ionSize);
  delete [] mFullWin;
  delete [] mWin;
  //KIIRFourthOrder *iirIon50_3= new KIIRFourthOrder(-3.95982379,  5.88045441, -3.88143355,  0.96080294,  0.00019593, 0.        , -0.00039186,  0.        ,  0.00019593); //this is a butterworth, band pass filter between 50 Hz to the 500kHz ion channels and 5 Hz for the 1kHz heat channels. 
  
  KIIRFourthOrder *iirIon50_3= new KIIRFourthOrder(-3.99599944,  5.98800827, -3.98801822,  0.99600939, 1.99460576e-06, 0.        , -3.98921151e-06,  0.        ,  1.99460576e-06); //this is a butterworth, band pass filter between 5 Hz to the 50Hz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basIon3->SetInputPulse(pulseIon1); basIon3->SetInputPulseSize(ionSize);  //notice how basIon2 and patIon1 point to the same memory
  basIon3->SetOutputPulse(pulseIon1); basIon3->SetOutputPulseSize(ionSize); //this is because these processors can be done in-place.
  patIon3->SetInputPulse(pulseIon1); patIon3->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  patIon3->SetOutputPulse(pulseIon1); patIon3->SetOutputPulseSize(ionSize);  
  winIonPulse3->SetInputPulse(pulseIon1); winIonPulse3->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  winIonPulse3->SetOutputPulse(pulseIon1); winIonPulse3->SetOutputPulseSize(ionSize);
  iirIon50_3->SetInputPulse(pulseIon1); iirIon50_3->SetInputPulseSize(ionSize);
  iirIon50_3->SetOutputPulse(pulseIon2); iirIon50_3->SetOutputPulseSize(ionSize);
    //add processors to the chain, in order
  ionAna3Pulse.AddProcessor(basIon3);
  ionAna3Pulse.AddProcessor(patIon3);
  ionAna3Pulse.AddProcessor(winIonPulse3);
  ionAna3Pulse.AddProcessor(iirIon50_3);
  
  
  //anlaysis ion 3 baseline
  KPulseAnalysisChain ionAna3Baseline;
  KLinearRemoval *basIon3b = new KLinearRemoval;
  KPatternRemoval *patIon3b = new KPatternRemoval;
  mWin = KWindowDesign::GetBlackmanWindow(3200);
  mFullWin = new double[ionSize];
  memset(mFullWin, 0, ionSize * sizeof(double));
  memcpy(mFullWin, mWin, 3200*sizeof(double) );
  KWindow *winIonBaseline3b = new KWindow;
  winIonBaseline3b->SetInputPulseSize(ionSize);
  winIonBaseline3b->SetOutputPulseSize(ionSize);
  winIonBaseline3b->SetWindow(mFullWin, ionSize);
  delete [] mFullWin;
  delete [] mWin;
  //KIIRFourthOrder *iirIon50_3b= new KIIRFourthOrder(-3.95982379,  5.88045441, -3.88143355,  0.96080294,  0.00019593, 0.        , -0.00039186,  0.        ,  0.00019593); //this is a butterworth, band pass filter between 50 Hz to the 500kHz ion channels and 5 Hz for the 1kHz heat channels. 
  
  KIIRFourthOrder *iirIon50_3b= new KIIRFourthOrder(-3.99599944,  5.98800827, -3.98801822,  0.99600939, 1.99460576e-06, 0.        , -3.98921151e-06,  0.        ,  1.99460576e-06); //this is a butterworth, band pass filter between 5 Hz to the 50kHz ion channels and 5 Hz for the 1kHz heat channels. 
    //set up the memory allocation
  basIon3b->SetInputPulse(pulseIon1); basIon3b->SetInputPulseSize(ionSize);  //notice how basIon2 and patIon1 point to the same memory
  basIon3b->SetOutputPulse(pulseIon1); basIon3b->SetOutputPulseSize(ionSize); //this is because these processors can be done in-place.
  patIon3b->SetInputPulse(pulseIon1); patIon3b->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  patIon3b->SetOutputPulse(pulseIon1); patIon3b->SetOutputPulseSize(ionSize);  
  winIonBaseline3b->SetInputPulse(pulseIon1); winIonBaseline3b->SetInputPulseSize(ionSize);  //further optimizations could be included if these processors could be done within the same loop over the pulse
  winIonBaseline3b->SetOutputPulse(pulseIon1); winIonBaseline3b->SetOutputPulseSize(ionSize);
  iirIon50_3b->SetInputPulse(pulseIon1); iirIon50_3b->SetInputPulseSize(ionSize);
  iirIon50_3b->SetOutputPulse(pulseIon2); iirIon50_3b->SetOutputPulseSize(ionSize);
    //add processors to the chain, in order
  ionAna3Baseline.AddProcessor(basIon3b);
  ionAna3Baseline.AddProcessor(patIon3b);
  ionAna3Baseline.AddProcessor(winIonBaseline3b);
  ionAna3Baseline.AddProcessor(iirIon50_3b);
  
  
  
  //int numEvents = 1000;
  int numEvents = f.GetEntries();
  
  
  
  //build noise power spectrum - just chaleur FID804AB to start with!!!
  double *avePower = 0;
  int numberOfNoiseEvents = 0;
  KLinearRemoval *npLin = new KLinearRemoval;
  double *mOptWin = KWindowDesign::GetTukeyWindow(heatSize,0.10);
  KWindow *mWinHeat2 = new KWindow;
  mWinHeat2->SetWindow(mOptWin, heatSize);
  delete [] mOptWin; mOptWin = 0;
  KRealToHalfComplexDFT *r2hc = new KRealToHalfComplexDFT;
  KHalfComplexPower *hcp = new KHalfComplexPower;
  KPeakFinder *pfinder = new KPeakFinder;  //era default for heat weak - sensitivity of 4.5 sigma... could change to 5.5 sigma to make sure
    //strict is 3.5 sigma
  KOrderFilter *ordr = new KOrderFilter;  //use ERA default of heat weak - order 3  ... strict is order 6
  pfinder->SetHeatDefault();
  pfinder->SetPolarity(0);
  ordr->SetOrder(3);
  KPulseAnalysisChain heatNoise;
  heatNoise.AddProcessor(npLin);
  //heatNoise.AddProcessor(mWinHeat);
  heatNoise.AddProcessor(ordr);
  heatNoise.AddProcessor(pfinder);
  
  //KLinearRemoval *npLin2 = new KLinearRemoval;
  //will need to loop through the data here in order to build up the noise power spectrum
  //to be used in the optimal filter
  for(int i = 0; i < numEvents; i++){
    f.GetEntry(i);
    for(int j = 0; j < e->GetNumBoloPulses(); j++){
      KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
      if(strcmp(pRaw->GetChannelName(),"chaleur FID804AB") == 0){
        vector<short> pulse = pRaw->GetTrace();
        if(pulse.size() > 0){
          heatNoise.SetInputPulse(pulse);
          if(heatNoise.RunProcess()){
            KPeakFinder *pf = (KPeakFinder *)heatNoise.GetProcessor(heatNoise.GetNumProcessors()-1);
            //cout << "number of peaks: weak, xtraweak, strict  " << pf->GetNumWeakPeaks() << " " << pf->GetNumExtraWeakPeaks() << " " << pf->GetNumStrictPeaks() << endl;
            if(pf->GetNumExtraWeakPeaks()==0){
              mWinHeat2->SetInputPulse(npLin->GetOutputPulse(), npLin->GetOutputPulseSize());
              mWinHeat2->RunProcess();
              r2hc->SetInputPulse(mWinHeat2->GetOutputPulse(), mWinHeat2->GetOutputPulseSize());  //get this from the window stage
              if(r2hc->RunProcess()){
                hcp->SetInputPulse(r2hc->GetOutputPulse(), r2hc->GetOutputPulseSize());
                if(hcp->RunProcess()){
                  cout << "we have a power spectrum! " << hcp->GetOutputPulse() << " " << hcp->GetOutputPulseSize() << endl;
                  if(avePower == 0){
                    avePower = new double[hcp->GetOutputPulseSize()];
                    memset(avePower, 0, hcp->GetOutputPulseSize() * sizeof(double));
                  }
                  numberOfNoiseEvents++;
                  for(unsigned int k = 0; k < hcp->GetOutputPulseSize(); k++)
                    avePower[k] += hcp->GetOutputPulse()[k];
                }
              }
            }
          }
        }
      }
    }
  }
  for(unsigned int i = 0; i < hcp->GetOutputPulseSize(); i++)
    avePower[i] = avePower[i] / (double)numberOfNoiseEvents;
    
  //cout << "[";
  //for(unsigned int i = 0; i < hcp->GetOutputPulseSize(); i++)
  //  cout << avePower[i] << ",";
  //cout << "]" << endl;
  //return 0;
  
  //optimal filter for chaleur FID804AB
  KPulseAnalysisChain heatOptimal;
  KLinearRemoval *optLin = new KLinearRemoval;
  mOptWin = KWindowDesign::GetTukeyWindow(heatSize,0.10);
  KWindow *mWinHeat = new KWindow;
  mWinHeat->SetWindow(mOptWin, heatSize);
  delete [] mOptWin; mOptWin = 0;
  
  
  
  
  
  
  
  
  
  //this isn't going to be the MOST efficient way, however, because some processes
  //will be repeated, such as the linear removal process, for example. 
  //There is a significant amount of optimization that can eventually be done if we 'map' out 
  //a set of analysis processors, being careful not to re-run any process that we don't need. 
  
  
  
  
  //loop through the data, copying the raw information that is needed
  //for the amp-level data, and then applying the various analysis processing chains
  //that are defined above.
  for(int i = 0; i < numEvents; i++){
    f.GetEntry(i);
    ee->Clear("C");
    *(KEvent *)ee = *e; //copy the base-class stuff
    
    if(i % 100 == 0) cout << "entry " << i << endl;
    
    //normally, one would copy the muon veto system record information,
    //but since this data hasn't even been applied at the raw level (unfortunately)
    //let's not bother for now
    //KRawMuonVetoSysRecord *muonRaw = (KRawMuonVetoSysRecord *)e->GetMuonVetoSystemRecord();
    //KRawMuonVetoSysRecord *muonAmp = (KRawMuonVetoSysRecord *)ee->GetMuonVetoSystemRecord();
    //*muonAmp = *muonRaw;

    for(int j = 0; j < e->GetNumBolos(); j++){
      if(i % 100 == 0) cout << "     bolo " << j << endl;
      KRawBolometerRecord *boloRaw = (KRawBolometerRecord *)e->GetBolo(j);
      KAmpBolometerRecord *boloAmp = ee->AddBolo();
      boloAmp->SetMass(boloRaw->GetMass());
      boloAmp->SetDetectorName(boloRaw->GetDetectorName());

      //copy the samba DAQ record and set the TRef
      KRawSambaRecord *samRaw = (KRawSambaRecord *)boloRaw->GetSambaRecord();
      //check to see if we already have this samba
      KRawSambaRecord *samAmp = 0;
      for(int ss = 0; ss < ee->GetNumSambas(); ss++){
        if(*(KRawSambaRecord *)ee->GetSamba(ss) == *samRaw) { //compare the samba records
          samAmp = (KRawSambaRecord *)ee->GetSamba(ss); //if they are equal, we've found a match
          break;  //stop the loop
          } 
      }
      
      if(samAmp == 0){ //if we didn't find a samba match, make a new one.
        samAmp = ee->AddSamba();
        *samAmp = *samRaw; //copy the data
      }
      
      boloAmp->SetSambaRecord(samAmp); //set the TRef


      for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
        if(i % 100 == 0) cout << "          pulse " << k;
        KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);

        if(pRaw->GetPulseLength() == 0)
          continue;  //apparently, sometimes there are events where there is no pulse data!

        //for each pulse record, we add a bolo amp pulse record
        KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse();
        boloAmp->AddPulseRecord(pAmp); //link the TRef
        pAmp->SetBolometerRecord(boloAmp); //link the TRef
        
        *(KBoloPulseRecord *)pAmp = *pRaw;  //copies the base class stuff.
        copyBasicData(pAmp, pRaw); //copy basic data
        moveSambaData(ee, boloAmp, pAmp, pRaw); //move the samba analysis to a pulse analysis record
        
        //now apply the different analysis chains.
        if(pRaw->GetIsHeatPulse()){
          //heat analysis 1
          runHeatAna1(heatAna1, ee, boloAmp, pAmp, pRaw, "iir1hp5Hz");
          runHeatAna2Pulse(heatAna2Pulse, ee, boloAmp, pAmp, pRaw, "iir4bp5to50HzWindow");
          runHeatAna2Baseline(heatAna2Baseline, ee, boloAmp, pAmp, pRaw, "iir4bp5to50HzWindow");
        }
        else{
          //ion analysis 1
          int pol(0);
          pol = pRaw->GetPolarity() > 0 ? -1 : 1;
          runIonAna1(ionAna1, ee, boloAmp, pAmp, pRaw,  "iir1hp500Hz", pol);
          runIonAna2Pulse(ionAna2Pulse, ee, boloAmp, pAmp, pRaw, "iir4bp50to500HzBlackmanWindow", pol);
          runIonAna2Baseline(ionAna2Baseline, ee, boloAmp, pAmp, pRaw, "iir4bp50to500HzBlackmanWindow");
          //I can re-use these functions
          runIonAna2Pulse(ionAna3Pulse, ee, boloAmp, pAmp, pRaw, "iir4bp5to50HzBlackmanWindow", pol);
          runIonAna2Baseline(ionAna3Baseline, ee, boloAmp, pAmp, pRaw, "iir4bp5to50HzBlackmanWindow");
          
        }
        
        
      }//for pulse loop
      if(i %100 == 0) cout << endl;
    }//for bolo loop
    ff.Fill();
  }
  
  int writeReturn = ff.Write();
  ff.Close();
  f.Close();
  return writeReturn;
}
