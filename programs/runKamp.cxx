#include "KDataReader.h"
#include "KDataWriter.h"
#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KEvent.h"
#include "KRawMuonVetoSysRecord.h"
#include "KRawBolometerRecord.h"
#include "KAmpBolometerRecord.h"
#include "KRawSambaRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KSambaRecord.h"

#include "KBackyardKampSite.h"

#include <string>
#include <cstring>
#include <cmath>

using namespace std;

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


int main(int /*argc*/, char* argv[]){
  KDataReader f(argv[1]);
  KDataWriter ff(argv[2],"KAmpEvent");
  KAmpEvent *ee = (KAmpEvent *)ff.GetEvent();
  KRawEvent *e = (KRawEvent *)f.GetEvent();


  //set up some memory location for data processing
  //each kampsite should specify their memory locations
  //which then are communicated to all of their kampers.
  //unsigned int heatSize = 512;
  //unsigned int ionSize = 8196;
  //double* pulseIon1 = new double[ionSize];
  //double* pulseIon2 = new double[ionSize];
  //double* pulseHeat1 = new double[heatSize];
  //double* pulseHeat2 = new double[heatSize];
 
  KBackyardKampSite mBackYard;  
  
  int numEvents = f.GetEntries();
  
  //Run through the the raw data and pass it to the 
  //kampsites so they may KAmpSite::PrepareKAmpSite()
  

  //KLinearRemoval *npLin2 = new KLinearRemoval;
  //will need to loop through the data here in order to build up the noise power spectrum
  //to be used in the optimal filter
  //for(int i = 0; i < numEvents; i++){
  //  f.GetEntry(i);
  //  for(int j = 0; j < e->GetNumBoloPulses(); j++){
  //    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
  //    mKarlruheSite.ScoutKampSite(pRaw, e);
  //    }
  //  }
  //}
  
  
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
        
        mBackYard.RunKampSite(ee, boloAmp, pAmp, pRaw);
        
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
