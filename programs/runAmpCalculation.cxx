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
#include <string>

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
  sambarec->SetCalculationType("samba");
  sambarec->SetIsBaseline(false);
  sambarec->SetBaselineAmplitudeWidth(pRaw->GetAmplitudeBaselineNoise());
  sambarec->SetUnit(0);

  sambarec = ee->AddPulseAnalysisRecord(); //add a new analysis record
  sambarec->SetBolometerRecord(boloAmp); //make the proper TRef links
  sambarec->SetBoloPulseRecord(pAmp);
  pAmp->AddPulseAnalysisRecord(sambarec);

  sambarec->SetAmp(pRaw->GetAmplitudeBaseline());
  sambarec->SetCalculationType("samba");
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


void runHeatAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(true); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak;
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
    rec->SetCalculationType(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    //get the amplitude of the baseline
    peak = 50;
    maxValue = *(last->GetOutputPulse()+peak);
    rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    
    rec->SetAmp(maxValue);
    rec->SetCalculationType(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    
  }
  else cout << "heat 1 processor failed" << endl;
  
}


void runIonAna1(KPulseAnalysisChain &anaChain, KAmpEvent *ee, KAmpBolometerRecord *boloAmp, KAmpBoloPulseRecord *pAmp, KRawBoloPulseRecord* pRaw, const char* name)
{
  bool theRet = false;

  pRaw->GetTrace(anaChain.GetProcessor(0)->GetInputPulse()); //copies the pulse from the event to our local memory location
  theRet =  anaChain.RunProcess(true); //true says to use smart memory.
  if(theRet){
    int start, stop;
    int peak;
    start = 6500; stop = 7000;
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
    rec->SetBolometerRecord(boloAmp);  //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef

    rec->SetAmp(maxValue);
    rec->SetCalculationType(name);
    rec->SetIsBaseline(false);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    KLinearRemoval *lin = (KLinearRemoval *)anaChain.GetProcessor(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
    //add another pulse analysis record to hold the estimate of this 
    //processor at the beginning of the pulse (where no event should occur)
    
    //get the amplitude of the baseline
    peak = 500;
    maxValue = *(last->GetOutputPulse()+peak);
    rec = ee->AddPulseAnalysisRecord();
    rec->SetBolometerRecord(boloAmp); //link TRef
    rec->SetBoloPulseRecord(pAmp); //link TRef
    pAmp->AddPulseAnalysisRecord(rec); //link TRef
    
    rec->SetAmp(maxValue);
    rec->SetCalculationType(name);
    rec->SetIsBaseline(true);
    rec->SetPeakPosition(peak);
    rec->SetUnit(0);
    rec->SetBaselineRemoved(lin->GetOffset());
    rec->SetSlopeRemoved(lin->GetSlope());
    
  }
  else cout << "ion 1 processor failed" << endl;
  
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
  c.Get("https://localhost:6984", "/analysis/run13_templatepulse_chaleur_FID804AB");
  json = c.GetReturn();
  doc = KJson::Parse(json.data());
  heatpulse = KJson::GetObjectItem(doc, "pulse");
  vector<double> heatTemplate;
  for(unsigned int i = 0; i < KJson::GetArraySize(heatpulse); i++)
    heatTemplate.push_back(KJson::GetArrayItem(heatpulse, i)->valuedouble);
  
  //and use this for all ionization pulses for now. 
  c.Get("https://localhost:6984", "/analysis/run13_templatepulse_centre_FID804AB");
  json = c.GetReturn();
  doc = KJson::Parse(json.data());
  ionpulse = KJson::GetObjectItem(doc, "pulse");
  vector<double> ionTemplate;
  for(unsigned int i = 0; i < KJson::GetArraySize(ionpulse); i++)
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
  
  
  //this isn't going to be the MOST efficient way, however, because some processes
  //will be repeated, such as the linear removal process, for example. 
  
  //int numEvents = 1000;
  int numEvents = f.GetEntries();

  //will need to loop through the data here in order to build up the noise power spectrum
  //to be used in the optimal filter
  for(int i = 0; i < numEvents; i++){
    
    
  }
  
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
          
        }
        else{
          //ion analysis 1
          runIonAna1(ionAna1, ee, boloAmp, pAmp, pRaw, "iir1hp500Hz");
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