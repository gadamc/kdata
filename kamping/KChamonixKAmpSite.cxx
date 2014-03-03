//_____________________________________________
// KChamonixKAmpSite.cxx  
// KDataStructure
//
// Created by Adam Cox
// Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
//
//
// analyzes both heat and ionization pulses using an optimal filter.
//
// this kampsite assumes that the analysis is being performed on raw data where the settings
// for the the DAQ and characteristics of pulses on each channel are the same throughout the set of data it analyzes.
// 
// The kampsite works in the following way.
// 
// First, the noise spectra for each channel must be determined. This kampsite currently uses the ERA style of pulse
// detector to find "noise only" events. However, this class will likely employ some wavelet decomposition for noise event 
// decisions. Additionally, the user may turn off noise spectrum determination (NeedScout(false)) and provide
// a noise spectrum directly to an instance of this class. 
//
// Second, the windowing processors must be set up by providing the pulse size for heat channels and ionization channels,
// There is only one heat window
// and one ionization channel window. So, all channels should have the same data length for each type.
// This is a bit of a drawback of this KAmpSite, but so far, this has always been true for the data taken 
// in Edelweiss. When you supply the pulse template, you don't need to supply the windowing. The windowing is done
// automatically.
//
// Third, a numerical pulse template for each channel must be supplied for each channel. 
// When the pulse template is supplied, it is first windowed, then should be shifted such that the start of the pulse
// is at the first position in the template. The shift time is supplied by the user... which is typically equal to the
// size of the pretrigger. The reason that the template pulse has a start-time that is set to the middle position of the full
// length of the template pulse is so that windowing can first occur, then it is shifted. This is especially important for 
// wide signals such as the heat and bbv2 channels. 
// 
// Important. This KAmpSite, in order to minimize its memory footprint, requires that the lengths of all ionization pulses and 
// all heat pulses are the same. IF you need to handle data where the heat pulses are different for different detectors, simply
// create a new KChamonixKAmpSite for each pulse length that you need. 
//
// *** The pulse lengths of the ionization channel and heat channels are defined when you call SetTemplate ** So, you MUST set
// your numerical pulse template to have the same length as the length of the signals in the data ****
// 
// The data stored in the KPulseAnalysisRecords from this processor can be directly read from the source code in the
// FillResults method. Almost all of the data in the KPulseAnalysisRecord comes from the results produced by the
// KOptimalKamper class. 
//

#include "KChamonixKAmpSite.h"

#include "KAmpEvent.h"
#include "KRawEvent.h"
#include "KAmpBolometerRecord.h"
#include "KAmpBoloPulseRecord.h"
#include "KRawBolometerRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KPulseAnalysisRecord.h"
#include "KWindowDesign.h"
#include "KPulseAnalysisChain.h"
#include "KLinearRemoval.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KNullProcessor.h"
#include "KPulsePolarityCalculator.h"
#include "TH1D.h"
#include <algorithm>
#include <iostream>

using namespace std;

KChamonixKAmpSite::KChamonixKAmpSite(void): fPulseTemplateShifter(0,0,0,0)  //set the fPulseTemplateShifter pulse locations to NULL so we set them later.
{
  SetName("KChamonixKAmpSite");

  fEraPeakFinderOrderHeat_default = 3;
  fEraPeakFinderNumRmsHeat_default = 5.0;
  fHeatPeakDetector.SetOrder( fEraPeakFinderOrderHeat_default );
  fHeatPeakDetector.SetNumRms( fEraPeakFinderNumRmsHeat_default );
  
  
  fEraPeakFinderOrderIon_default = 3;
  fEraPeakFinderNumRmsIon_default = 5.0;
  fIonPeakDetector.SetOrder( fEraPeakFinderOrderIon_default );
  fIonPeakDetector.SetNumRms( fEraPeakFinderNumRmsIon_default );

  //need something completely different here!

  fHeatPreProcessor = new KPulseAnalysisChain();
  fHeatPreProcessor->SetIsOwner();
  //fHeatPreProcessor->AddProcessor( new KBaselineRemoval() );
  fHeatPreProcessor->AddProcessor( new KNullProcessor() );

  fBBv1IonPreProcessor  = new KPulseAnalysisChain();
  fBBv1IonPreProcessor->SetIsOwner();
  fBBv1IonPreProcessor->AddProcessor( new KBaselineRemoval() );
  fBBv1IonPreProcessor->AddProcessor( new KPatternRemoval() ); 
  fBBv1IonPreProcessor->AddProcessor( new KPatternRemoval() ); //yes, i make two...  see below.

  fBBv2IonPreProcessor  = new KPulseAnalysisChain();
  fBBv2IonPreProcessor->SetIsOwner();
  fBBv2IonPreProcessor->AddProcessor( new KLinearRemoval() );
  KPatternRemoval *pta = new KPatternRemoval();
  pta->SetPatternLength(10);  //just fixed for now!!!
  fBBv2IonPreProcessor->AddProcessor( pta ); 
  pta = new KPatternRemoval();
  pta->SetPatternLength(20);  //just fixed for now!!!
  fBBv2IonPreProcessor->AddProcessor( pta ); 

  fHeatWindow = 0;
  fIonWindow = 0;
  // fFirstPulse = true;
  fScoutData = true;
  fOperaGlitchNumBinsThreshold = 5;
  fWriteExtraData = true;

  fHeatPulsePeakSearchRangeMin = -100;
  fHeatPulsePeakSearchRangeMax = 100;
  fIonPulsePeakSearchRangeMin = -1000;
  fIonPulsePeakSearchRangeMax = 3000;

}

KChamonixKAmpSite::~KChamonixKAmpSite(void)
{
  // SetHeatPreProcessor(0);
  // SetBBv1IonPreProcessor(0);
  // SetBBv2IonPreProcessor(0);
  // SetHeatWindow(0);
  // SetIonWindow(0);

  if(fHeatPreProcessor){
    delete fHeatPreProcessor;
    fHeatPreProcessor = 0;
  }
  if(fBBv1IonPreProcessor){
    delete fBBv1IonPreProcessor;
    fBBv1IonPreProcessor = 0;
  }
  if(fBBv2IonPreProcessor){
    delete fBBv2IonPreProcessor;
    fBBv2IonPreProcessor = 0;
  }
  if(fHeatWindow){
    delete fHeatWindow;
    fHeatWindow = 0;
  }
  if(fIonWindow){
    delete fIonWindow;
    fIonWindow = 0;
  }


  //clean up kampers
  std::map<std::string, KOptimalKamper* >::iterator  it;
  for( it = fOptKampers.begin(); it != fOptKampers.end(); it++)
    delete it->second;


}


// void KChamonixKAmpSite::SetHeatPreProcessor(KPtaProcessor* pta)
// {
//   //if you call this method, this object owns the memory of the pointer you give it. it will delete the object when
//   //its destructor is called!!!

//   if(fHeatPreProcessor){
//     delete fHeatPreProcessor;
//     fHeatPreProcessor = 0;
//   }
//   fHeatPreProcessor = pta;

// }

// void KChamonixKAmpSite::SetBBv1IonPreProcessor(KPtaProcessor* pta)
// {
//   //if you call this method, this object owns the memory of the pointer you give it. it will delete the object when
//   //its destructor is called!!!
//   if(fBBv1IonPreProcessor){
//     delete fBBv1IonPreProcessor;
//     fBBv1IonPreProcessor = 0;
//   }
//   fBBv1IonPreProcessor = pta;
// }

// void KChamonixKAmpSite::SetBBv2IonPreProcessor(KPtaProcessor* pta)
// {
//   //if you call this method, this object owns the memory of the pointer you give it. it will delete the object when
//   //its destructor is called!!!

//   if(fBBv2IonPreProcessor){
//     delete fBBv2IonPreProcessor;
//     fBBv2IonPreProcessor = 0;
//   }
//   fBBv2IonPreProcessor = pta;
// }


// void KChamonixKAmpSite::SetHeatWindow(KPtaProcessor* pta)
// {
//   //if you call this method, this object owns the memory of the pointer you give it. it will delete the object when
//   //its destructor is called!!!

//   if(fHeatWindow){
//     delete fHeatWindow;
//     fHeatWindow = 0;
//   }
//   fHeatWindow = pta;
// }

// void KChamonixKAmpSite::SetIonWindow(KPtaProcessor* pta)
// {
//   //if you call this method, this object owns the memory of the pointer you give it. it will delete the object when
//   //its destructor is called!!!

//   if(fIonWindow){
//     delete fIonWindow;
//     fIonWindow = 0;
//   }
//   fIonWindow = pta;
// }


void KChamonixKAmpSite::CreateHeatWindow(unsigned int pulseSize, double tukeyWindowParam)
{
  if(fHeatWindow) {delete fHeatWindow; fHeatWindow = 0;}
  fHeatWindow = new KWindow();
  fHeatWindow->SetWindow( KWindowDesign::GetTukeyWindow(pulseSize,tukeyWindowParam), pulseSize);
}

void KChamonixKAmpSite::CreateIonWindow(unsigned int pulseSize, double tukeyWindowParam)
{
  if(fIonWindow) {delete fIonWindow; fIonWindow = 0;}
  fIonWindow = new KWindow();
  fIonWindow->SetWindow( KWindowDesign::GetTukeyWindow(pulseSize,tukeyWindowParam), pulseSize);
}



Bool_t KChamonixKAmpSite::RunKampSite(KRawBolometerRecord *boloRaw, KAmpBolometerRecord *boloAmp, KAmpEvent *ee)
{
  //

  //loop first and do the ionization channels. use the result from the ionization channels
  //to inform the heat channel energy estimator. 

  double PeakPos = 0;
  double maxPeak = 0.;
  int precNum = -1;

  if(boloRaw->IsVetoRecord()){return true;}  //skip veto records!

  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    
    if(pRaw->GetPulseLength() == 0) 
      continue;

    if(pRaw->GetIsHeatPulse()) 
      continue;  //first loop through the ionization pulses...
    
    if(fTemplateSpectra.find(pRaw->GetChannelName()) == fTemplateSpectra.end()) 
      continue;   //we don't have a template spectra for this channel. skip it.

    if(fNoiseSpectra.find(pRaw->GetChannelName()) == fNoiseSpectra.end()) 
      continue; //also skip if we don't have a noise spectr
    
    

    KOptimalKamper* aKamper = GetOptimalKamper(pRaw);
    //perform the optimal filtering
    map<string, KResult> resMap = aKamper->MakeKamp(pRaw);
    

    //save the data into the output KAmpEvent (if it exists)
    if(ee && boloAmp){
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
      KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent
      
      FillResults(rec, resMap, pRaw);
    }  

    
    if((fabs(resMap["amp"].fValue) > maxPeak) and resMap["amp"].fValue!=-99999){
       maxPeak = fabs(resMap["amp"].fValue);
       PeakPos = (resMap["minChi2Pos"].fValue - pRaw->GetPretriggerSize() - 1 ) * pRaw->GetPulseTimeWidth()*1.e-9;
       precNum = k;
     }

  }


  //now for the heat pulses.

  for(int k = 0; k < boloRaw->GetNumPulseRecords(); k++){
    
    KRawBoloPulseRecord *pRaw = (KRawBoloPulseRecord *)boloRaw->GetPulseRecord(k);
    
    if(pRaw->GetPulseLength() == 0) 
      continue;

    if(!pRaw->GetIsHeatPulse()) 
      continue;
    
    if(fTemplateSpectra.find(pRaw->GetChannelName()) == fTemplateSpectra.end()) 
      continue; //we don't have a template spectra for this channel. skip it.

    if(fNoiseSpectra.find(pRaw->GetChannelName()) == fNoiseSpectra.end())
      continue; //also skip if we don't have a noise spectr
    
    if(fHeatWindow == 0) continue;

    
    KOptimalKamper* aKamper = GetOptimalKamper(pRaw);
    aKamper->SetIonPulseStartTime(PeakPos);    //set the ionization pulse peak position (in seconds from the trigger position)

    //perform the optimal filtering
    map<string, KResult> resMap = aKamper->MakeKamp(pRaw);
    
    //save the data into the output KAmpEvent (if it exists)
    if(ee && boloAmp){
      KAmpBoloPulseRecord *pAmp = ee->AddBoloPulse(pRaw, boloAmp); //for each pulse record, we add a bolo amp pulse record
      KPulseAnalysisRecord *rec = ee->AddPulseAnalysisRecord();
      SetTRefLinksForKAmpEvent(rec, boloAmp,pAmp);  //you MUST call this in order to set the TRef links and make a valid KAmpEvent

      FillResults(rec, resMap, pRaw );

    }

  }
  
  return true;
}  

void KChamonixKAmpSite::FillResults(KPulseAnalysisRecord* rec, map<string, KResult> &resMap, KRawBoloPulseRecord *pRaw )
{
  //fill results
  if(resMap.find("amp") != resMap.end())                    rec->SetAmp(resMap["amp"].fValue);
  if(resMap.find("peakPosition") != resMap.end())           rec->SetPeakPosition(resMap["peakPosition"].fValue);
  if(resMap.find("chi2AtPeakPosition") != resMap.end())     rec->SetChiSq(resMap["chi2AtPeakPosition"].fValue);
  if(resMap.find("baselineRemoved") != resMap.end())        rec->SetBaselineRemoved(resMap["baselineRemoved"].fValue);

  if(resMap.find("ampAtTemplatePulseAmplitudeShift") != resMap.end())     rec->SetExtra(resMap["ampAtTemplatePulseAmplitudeShift"].fValue, 0);
  if(resMap.find("minChi2") != resMap.end())                              rec->SetExtra(resMap["minChi2"].fValue, 1);
  if(resMap.find("minChi2Pos") != resMap.end())                           rec->SetExtra(resMap["minChi2Pos"].fValue, 2);
  if(resMap.find("optAmpAtMinChi2") != resMap.end())                      rec->SetExtra(resMap["optAmpAtMinChi2"].fValue, 3);
  if(resMap.find("pulseAmpAtOptimalAmpPeakPosition") != resMap.end())     rec->SetExtra(resMap["pulseAmpAtOptimalAmpPeakPosition"].fValue, 4);
  if(resMap.find("pulseAmpAtMinChi2Position") != resMap.end())            rec->SetExtra(resMap["pulseAmpAtMinChi2Position"].fValue, 5);

  //fill heat channel specific results...
  if(pRaw->GetIsHeatPulse()){
    fHeatPeakDetector.SetInputPulse( fHeatWindow);
    if(fHeatPeakDetector.RunProcess() )    rec->SetExtra(fHeatPeakDetector.GetPeakBins().size(), 6); 
  }
  else{
    fIonPeakDetector.SetInputPulse( fHeatWindow);
    if(fIonPeakDetector.RunProcess() )    rec->SetExtra(fIonPeakDetector.GetPeakBins().size(), 6); 

  }

  if(resMap.find("risetime") != resMap.end())       rec->SetRisetime(resMap["risetime"].fValue);
  if(resMap.find("pulseWidth") != resMap.end())     rec->SetPulseWidth(resMap["pulseWidth"].fValue);

  if(resMap.find("pulseTemplateShift") != resMap.end())         rec->SetExtra(resMap["pulseTemplateShift"].fValue, 7);
  if(resMap.find("amplitudeShift") != resMap.end())             rec->SetExtra(resMap["amplitudeShift"].fValue, 8);
  if(resMap.find("fixPeakPosition") != resMap.end())            rec->SetExtra(resMap["fixPeakPosition"].fValue, 9);
  if(resMap.find("ionPulseStartTime") != resMap.end())          rec->SetExtra(resMap["ionPulseStartTime"].fValue, 10);
  if(resMap.find("ampAtIonPulseStartTime") != resMap.end())     rec->SetExtra(resMap["ampAtIonPulseStartTime"].fValue, 11);
  if(resMap.find("chi2AtIonPulseStartTime") != resMap.end())    rec->SetExtra(resMap["chi2AtIonPulseStartTime"].fValue, 12);
  if(resMap.find("slope") != resMap.end())                      rec->SetExtra(resMap["slope"].fValue, 13);


  //this following chunk of code looks to see if this event was used
  //in the calculation of the noise spectrum for this channel.
  //It searches the vector in the fNoiseEventSambaEventNumberList for this channel name
  //and if it finds the samba event number, it sets the Extra[14] field to be 1.
  // if it does not find the samba event number in this list, it sets the Extra[14]  to be 0.
  // if, for some reason, it can't find the channel name in the fNoiseEventSambaEventNumberList,
  // it sets Extra[14] to be -1, which is the default value, to indicate an unknown value.
  KRawBolometerRecord *bolo = pRaw->GetBolometerRecord();
  KRawSambaRecord *samba = bolo->GetSambaRecord();
  std::map<std::string, std::vector<Int_t> >::iterator it;
  it = fNoiseEventSambaEventNumberList.find(pRaw->GetChannelName());

  int noiseIndicator = -1;
  if (it != fNoiseEventSambaEventNumberList.end()){

    if ( find( it->second.begin(), it->second.end(), samba->GetSambaEventNumber()) != it->second.end() )
      noiseIndicator = 1;   //  
    else
      noiseIndicator = 0;
  }
  rec->SetExtra(noiseIndicator, 14); 


  if( !pRaw->GetIsHeatPulse()){
    fOrderThresholdFinder.SetInputPulse( (std::vector<short> &)pRaw->GetTrace() );
    if(fOrderThresholdFinder.RunProcess())
      rec->SetExtra(fOrderThresholdFinder.GetPeakBins().size(), 15);
  }

  rec->SetName(GetName());
}


Bool_t KChamonixKAmpSite::ScoutKampSite(KRawBoloPulseRecord* pRaw, KRawEvent* /*e*/)
{
  //run through the data, look for noise events, find their power spectra and then find the average
  //noise power. use these noise spectra for the optimal filter.
  //
  //If the Ion/Heat channel Window has not been created before you call this method, it will be created for you
  //automatically with the default parameters. If you wish to have different parameters (such as a different alpha 
  //parameter for the Tukey Window), you should first call CreateHeat/IonWindow(size, alpha). 
  //
  //This method skips channels that do not have a pulse template set (see SetTemplate())
  //

  KRawBolometerRecord *bolo = pRaw->GetBolometerRecord();
  if (bolo == 0){
    cout << pRaw->GetChannelName() << endl;
    cout << "KChamonixKAmpSite::ScoutKampSite. This is really bad. Check to make sure the file you are processing is a raw ROOT file.  The KRawBoloPulseRecord object returned a null pointer to the KRawBolometerRecord" << endl;
    return false;
  }
  if(bolo->IsVetoRecord())
    return true;

  if(pRaw->GetPulseLength() == 0) { return false;}

  if(fTemplateSpectra.find(pRaw->GetChannelName()) == fTemplateSpectra.end()) {
      return true;
    };  //skip if we don't have a noise spectr

  //the preprocessing, windowing, and peak detection tools all depend upon if this channel
  //is a heat channel, or an ionization channel and its bb version number
  //check this information and setup accordingly

  KPtaProcessor *thePreProc = 0;
  KPtaProcessor *theWindow = 0;
  KEraPeakFinder* thePeakDetector = 0;
  KPtaProcessor* theProcToTheFFT = 0;

  Int_t eraPFOrder = GetEraPeakFinderOrder(pRaw->GetChannelName());
  Double_t eraPFRms = GetEraPeakFinderNumRms(pRaw->GetChannelName());

  if(pRaw->GetIsHeatPulse()){
    thePreProc = fHeatPreProcessor;
    if (fHeatWindow == 0) CreateHeatWindow(pRaw->GetPulseLength());
    theWindow = theProcToTheFFT = fHeatWindow;
    thePeakDetector = &fHeatPeakDetector;

    if(eraPFRms < 0)
      eraPFRms = fEraPeakFinderNumRmsHeat_default;
    if(eraPFOrder < 0)
      eraPFOrder = fEraPeakFinderOrderHeat_default;
  }
  else{

    if(eraPFRms < 0)
      eraPFRms = fEraPeakFinderNumRmsIon_default;
    if(eraPFOrder < 0)
      eraPFOrder = fEraPeakFinderOrderIon_default;

    if(pRaw->GetBoloBoxVersion() > 2.1)
      return false;  //only support bbv2 and bbv1 at the moment

    if (fIonWindow == 0) CreateIonWindow(pRaw->GetPulseLength());

    else if(pRaw->GetBoloBoxVersion() > 1.99){  //avoid direct comparison of floating point number  

      thePreProc = fBBv2IonPreProcessor;
      
      //why did I have windowing turned off for bbv2 at a previous commit to the
      //repository? what's wrong with windowing bbv2?
      theWindow = theProcToTheFFT = fIonWindow;
      thePeakDetector = &fIonPeakDetector;

    }
    else if(pRaw->GetBoloBoxVersion() > 0.9){
      thePreProc = fBBv1IonPreProcessor;
      theWindow =  theProcToTheFFT = fIonWindow;
      thePeakDetector = &fIonPeakDetector;
    }
  }


  //the following code:
  //  checks ionization channels for 'opera glitches'
  //  calls the preprocessor - usually baseline/linear removal and pattern removal
  //  looks for noise with the era peak finder
  //  if it is noise:
  //    windows the event
  //    calculates the power spectrum 
  //    adds it to the running average for that channel

  if( !pRaw->GetIsHeatPulse()){
    fOrderThresholdFinder.SetInputPulse( (std::vector<short> &)pRaw->GetTrace() );
    if(!fOrderThresholdFinder.RunProcess()){
      cout << "KChamonixKAmpSite::ScoutKampSite. the fOrderThresholdFinder failed. pulse channel: " << pRaw->GetChannelName() << endl;
      return false;
    }

    if(fOrderThresholdFinder.GetPeakBins().size() >= fOperaGlitchNumBinsThreshold) {return false;} //we found a "glitch", so this is not noise.
  }


  thePreProc->SetInputPulse( (std::vector<short> &)pRaw->GetTrace());
  if(!thePreProc->RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. the preprocessor failed. pulse channel: " << pRaw->GetChannelName() << endl;
    return false;
  }
  
 
  thePeakDetector->SetPolarity( 0 ); //always search for pulse in both polarities

  thePeakDetector->SetInputPulse(thePreProc);
  if( !thePeakDetector->RunProcess()){
      cout << "KChamonixKAmpSite::ScoutKampSite. thePeakDetector failed. pulse channel: " << pRaw->GetChannelName() << endl;
      return false;
  }
  if(thePeakDetector->GetPeakBins().size() > 0) {return false;} //we found a peak, so this is not noise.
      
  //we found noise, now window it, find its power spectrum and then add it to the running average power spectrum  
  if(theWindow){

    theWindow->SetInputPulse(thePreProc);
    if(!theWindow->RunProcess()){
      cout << "KChamonixKAmpSite::ScoutKampSite. theWindow failed. pulse channel: " << pRaw->GetChannelName() << endl; return false;
    }
  }

  //calculate the power spectrum - fft, then |fft|^2
  fR2Hc.SetInputPulse(theProcToTheFFT);
  if(!fR2Hc.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fR2Hc failed. pulse channel: " << pRaw->GetChannelName() << endl; return false;
  }

  fHc2P.SetInputPulse(fR2Hc);
  if(!fHc2P.RunProcess()){
    cout << "KChamonixKAmpSite::ScoutKampSite. fHc2P failed. pulse channel: " << pRaw->GetChannelName()<< endl; return false;
  }

  if(fHc2P.GetOutputPulse() == 0){
    cout << "KChamonixKAmpSite::ScoutKampSite. fHc2P null pointer. pulse channel: " << pRaw->GetChannelName() << endl; return false;
  }
  
  if(fNoiseEventCounts.find(pRaw->GetChannelName()) == fNoiseEventCounts.end()){
    fNoiseEventCounts[pRaw->GetChannelName()] = 1;
  }
  else fNoiseEventCounts[pRaw->GetChannelName()]++;
    
  //I'm assuming that the length of the pulse will never change during a data file!
  //if the map doesn't find a vector for this channel,then we create one

  if(fNoiseSpectra.find(pRaw->GetChannelName()) == fNoiseSpectra.end()){
    vector<double> power;
    power.resize(fHc2P.GetOutputPulseSize());
    std::copy(fHc2P.GetOutputPulse(), fHc2P.GetOutputPulse()+fHc2P.GetOutputPulseSize(), power.begin());
    fNoiseSpectra[pRaw->GetChannelName()] = power;
  }
  else{ //add noise spectrum to the running average
    for(unsigned int i = 0; i < fNoiseSpectra[pRaw->GetChannelName()].size(); i++ ){
      fNoiseSpectra[pRaw->GetChannelName()][i] = fNoiseSpectra[pRaw->GetChannelName()][i] * (fNoiseEventCounts[pRaw->GetChannelName()]-1.0)/fNoiseEventCounts[pRaw->GetChannelName()] + fHc2P.GetOutputPulse()[i]/fNoiseEventCounts[pRaw->GetChannelName()];
    }
  }

  //also, record the samba event number of this noise event
  KRawSambaRecord *samba = bolo->GetSambaRecord();
  fNoiseEventSambaEventNumberList[pRaw->GetChannelName()].push_back( samba->GetSambaEventNumber());

  return true;
}  


Bool_t KChamonixKAmpSite::SetTemplate(const char* channelName,  std::vector<double>& pulse, int pulseShift, unsigned int pulseType)
{
  //pulsetype, 0 = heat, 1 = ion
  //
  //pulseShift no longer does anything because I calculate it automatically. Its left here so it doesn't break anything.
  // 
  //If the Ion/Heat channel Window has not been created before you call this method, it will be created for you
  //automatically with the default parameters. If you wish to have different parameters (such as a different alpha 
  //parameter for the Tukey Window), you should first call CreateHeat/IonWindow(size, alpha). 
  //

  KPtaProcessor *theWindowProcessor = 0;
  switch (pulseType) {
    case 0:
    if(fHeatWindow == 0)
      CreateHeatWindow(pulse.size());
    theWindowProcessor = fHeatWindow;
    break;
    case 1:
    if(fIonWindow == 0)
      CreateIonWindow(pulse.size());
    theWindowProcessor = fIonWindow;

    break;
    default:
    cout << "unknown pulse type" << endl;
    return false;
  }

  if(theWindowProcessor == 0){
    cout << "theWindowProcessor pointer is null for this pulse type " << pulseType << endl;
    return false;
  }

  theWindowProcessor->SetInputPulse(pulse);
  if(!theWindowProcessor->RunProcess()){
    cout << "theWindowProcessor failed" << endl; 
    cout << "your pulse template was not set" << endl;
    return false;
  }
   

  pulseShift = pulse.size()/2; 
  for(unsigned int i = 0; i < pulse.size(); i++){
    if (pulse[i] != 0){
      pulseShift = -1*i;
      break;
    }
  }
  //cout << "pulse shift set to " << pulseShift << endl;

  SetTemplateShift(channelName, pulseShift);

  fPulseTemplateShifter.SetShift(pulseShift);
  fPulseTemplateShifter.SetInputPulse(theWindowProcessor->GetOutputPulse());
  fPulseTemplateShifter.SetInputPulseSize(theWindowProcessor->GetOutputPulseSize());
  fPulseTemplateShifter.SetOutputPulse(theWindowProcessor->GetOutputPulse());
  fPulseTemplateShifter.SetOutputPulseSize(theWindowProcessor->GetOutputPulseSize());


  fPulseTemplateShifter.SetMode(2);
  fPulseTemplateShifter.RunProcess();

  //for(int iii = 0; iii < 10; iii++)
  //  cout << "pulse tempate post shift, bin " << iii <<  ":  " << fPulseTemplateShifter.GetOutputPulse()[iii] << endl;
    
  fR2Hc.SetInputPulse( theWindowProcessor);
  if(!fR2Hc.RunProcess()){
    cout << "fR2Hc failed" << endl; return false;
  }
  
  if(fR2Hc.GetOutputPulse() == 0){
    cout << "fR2Hc null pointer" << endl; return false;
  }
  
  vector<double> dft;
  dft.resize(fR2Hc.GetOutputPulseSize());
  
  std::copy(fR2Hc.GetOutputPulse(), fR2Hc.GetOutputPulse()+fR2Hc.GetOutputPulseSize(), dft.begin());
  fTemplateSpectra[channelName] = dft;
  return true;
}

set<int>& KChamonixKAmpSite::GetHeatPulseStampWidths(KRawBoloPulseRecord *pRaw)
{
  //returns the set of unique heat pulse stamp widths for the bolometer associated with the KRawBoloPulseRecord. 
  //The reason for a set is that there can 
  //be two NTDs per bolometer and they could have different or the same heat pulse widths. Using a set
  //means that its not necessary to check if the NTDs have the same pulse widths. 
  //so, this is general enough to return all of the unique heat pulse
  //widths for each NTD connected to each bolometer. This even works, of course, if there are more than
  //two NTDs per bolometer.... (future R&D?)

  //assume that the data does NOT change throughout the data file!!!

  if(GetHeatPulseStampWidthsSize(pRaw->GetChannelName()) != 0) 
    return GetHeatPulseStampWidths(pRaw->GetChannelName());

  set<int> stampwidths;

  KRawBolometerRecord *bolo = pRaw->GetBolometerRecord();

  for(int i = 0; i < bolo->GetNumPulseRecords(); i++){
    KRawBoloPulseRecord *p = bolo->GetPulseRecord(i);
    if(p->GetIsHeatPulse())
      stampwidths.insert( (int)(p->GetHeatPulseStampWidth()) );  
  }

  fHeatPulseStampWidths[pRaw->GetChannelName()] = stampwidths;
  
  return GetHeatPulseStampWidths(pRaw->GetChannelName());
}

set<int>& KChamonixKAmpSite::GetHeatPulseStampWidths(const char* channelName)
{
  //returns a reference to the internal set of known heat pulse stamps for a particular channel. If GetHeatPulseStampWidths(KRawBoloPulseRecord*)
  //has not yet been called for this channel name, then this will return an empty set because this object doesn't yet
  //know about this channel. Therefore, its better to use GetHeatPulseStampWidths(KRawBoloPulseRecord*) instead.
  //

  if (fHeatPulseStampWidths.find(channelName) != fHeatPulseStampWidths.end())
    return fHeatPulseStampWidths.find(channelName)->second;

  else { //instantiate an empty set for this channel
    fHeatPulseStampWidths[channelName] = set<int>();
    return fHeatPulseStampWidths[channelName];
  }

}

unsigned int KChamonixKAmpSite::GetHeatPulseStampWidthsSize(const char* channelName) const
{
  if (fHeatPulseStampWidths.find(channelName) != fHeatPulseStampWidths.end())
    return fHeatPulseStampWidths.find(channelName)->second.size();

  else {
    return 0;
  }

}

vector<double> KChamonixKAmpSite::GetTemplateSpectrum(const char* channelName) const
{
  if (fTemplateSpectra.find(channelName) != fTemplateSpectra.end()){
    return fTemplateSpectra.find(channelName)->second;
  }
  else {
    vector<double> empty;  //just return an empty vector
    return empty;
  }
    
}

unsigned int KChamonixKAmpSite::GetNumNoiseEventsFound(const char* channelName) const
{
  if (fNoiseEventCounts.find(channelName) != fNoiseEventCounts.end()){
    return fNoiseEventCounts.find(channelName)->second;
  }
  else return 0;
}

int KChamonixKAmpSite::GetTemplateShift(const char* channelName) const
{
  if (fTemplateShift.find(channelName) != fTemplateShift.end()){
    return fTemplateShift.find(channelName)->second;
  }
  else return -9999;
}


vector<double> KChamonixKAmpSite::GetNoisePower(const char* channelName) const
{
  if (fNoiseSpectra.find(channelName) != fNoiseSpectra.end()){
    return fNoiseSpectra.find(channelName)->second;
  }
  else {
    vector<double> empty;  //just return an empty vector
    return empty;
  }
    
}

Int_t KChamonixKAmpSite::GetEraPeakFinderOrder(const char* channelName) const
{
  if (fEraPeakFinderOrder.find(channelName) != fEraPeakFinderOrder.end()){
    return fEraPeakFinderOrder.find(channelName)->second;
  }
  else return -1;
}
  
Double_t KChamonixKAmpSite::GetEraPeakFinderNumRms(const char* channelName) const
{
  if (fEraPeakFinderNumRms.find(channelName) != fEraPeakFinderNumRms.end()){
    return fEraPeakFinderNumRms.find(channelName)->second;
  }
  else return -1;
}

KOptimalKamper* KChamonixKAmpSite::GetOptimalKamper(const char* channelName)
{
  if (fOptKampers.find(channelName) != fOptKampers.end()){
    return fOptKampers.find(channelName)->second;
  }
  else 
    return 0;
 
}

KOptimalKamper* KChamonixKAmpSite::GetOptimalKamper(KRawBoloPulseRecord *rawBoloPulse)
{

  KOptimalKamper* myKamper = GetOptimalKamper(rawBoloPulse->GetChannelName());
  
  if (myKamper == 0){
    // return NULL if can't find noise power spectrum, template spectra or template shift
    if( fTemplateSpectra.find(rawBoloPulse->GetChannelName()) == fTemplateSpectra.end() ||
        fNoiseSpectra.find(rawBoloPulse->GetChannelName()) == fNoiseSpectra.end() ||
        fTemplateShift.find(rawBoloPulse->GetChannelName()) == fTemplateShift.end())

      return 0;

    cout << "Creating new KOptimalKamper for channel: " << rawBoloPulse->GetChannelName() << endl;

    myKamper = new KOptimalKamper();
    myKamper->SetTemplateDFT( fTemplateSpectra.find(rawBoloPulse->GetChannelName())->second );
    myKamper->SetNoiseSpectrum( fNoiseSpectra.find(rawBoloPulse->GetChannelName())->second );
    myKamper->SetPulseTemplateShiftFromPreTrigger( GetTemplateShift(rawBoloPulse->GetChannelName()) );

    myKamper->SetToRecalculate(); //tell the filter to recalculate its kernel


    //set up a kamper for a heat pulse
    if(rawBoloPulse->GetIsHeatPulse()){
    
      myKamper->SetWindow( fHeatWindow ); //tell the optimal filter kamper to use this window function.
      myKamper->SetPreProcessor( fHeatPreProcessor );
      myKamper->SetAmplitudeEstimatorSearchRangeMax(fHeatPulsePeakSearchRangeMax); 
      myKamper->SetAmplitudeEstimatorSearchRangeMin(fHeatPulsePeakSearchRangeMin);
    

    }
    //set up the kamper for an ionization pulse
    else{
      myKamper->SetWindow( fIonWindow ); //tell the optimal filter kamper to use this window function.

      KPtaProcessor *preProc;    

      if(rawBoloPulse->GetBoloBoxVersion() >= 1.99){
        preProc = fBBv2IonPreProcessor;
      }
      else {  //BBv1 ionization needs two separate pattern removal processors
        std::set<int> heatwidths = GetHeatPulseStampWidths(rawBoloPulse);
        std::set<int>::iterator stampIter;  
        stampIter = heatwidths.begin();
        preProc = fBBv1IonPreProcessor;

        try{
          KPatternRemoval& mPat1 = dynamic_cast<KPatternRemoval &>( *preProc->GetProcessor(1) );
          mPat1.SetPatternLength( *stampIter );
        }
        catch(std::bad_cast){} //just do nothing.

        try {
          KPatternRemoval& mPat2 = dynamic_cast<KPatternRemoval &>( *preProc->GetProcessor(2) );
          stampIter++;
          if ( stampIter != heatwidths.end() )
            mPat2.SetPatternLength( *stampIter );
          else {
            stampIter = heatwidths.begin();
            mPat2.SetPatternLength( 2 * (*stampIter) );
          }
        }
        catch(std::bad_cast){} //just do nothing.
      }
     
      myKamper->SetPreProcessor( preProc );
      myKamper->SetAmplitudeEstimatorSearchRangeMax(fIonPulsePeakSearchRangeMax); 
      myKamper->SetAmplitudeEstimatorSearchRangeMin(fIonPulsePeakSearchRangeMin);

    }

    fOptKampers[rawBoloPulse->GetChannelName()] = myKamper;
    return myKamper;
  }
  
  //return myKamper if it already wasn't NULL
  return myKamper;

}



void KChamonixKAmpSite::SetNoisePower(const char* channelName, vector<double> power)
{
  fNoiseSpectra[channelName] = power;
}

void KChamonixKAmpSite::ReportResults(void)
{
  //report on the number of power spectra found
  std::map<std::string, unsigned int>::iterator it;
  cout << "Report on the number of Noise Events Found" << endl;
  cout << "Default ERA Peak finder order/num rms:" << endl;
  cout << "Heat (order/num rms): " << fEraPeakFinderOrderHeat_default << "/" << fEraPeakFinderNumRmsHeat_default << endl;
  cout << "Ion (order/num rms): " << fEraPeakFinderOrderIon_default << "/" << fEraPeakFinderNumRmsIon_default << endl;
  for( it = fNoiseEventCounts.begin(); it != fNoiseEventCounts.end(); it++){
    cout << (*it).first.c_str() << endl;
    cout << "  era order: ";
    if(GetEraPeakFinderOrder((*it).first.c_str()) > 0)
      cout << GetEraPeakFinderOrder((*it).first.c_str()) << endl;
    else
      cout << "default" << endl;
    cout << "  era num rms: ";
    if(GetEraPeakFinderNumRms((*it).first.c_str()) > 0)
      cout << GetEraPeakFinderNumRms((*it).first.c_str()) << endl;
    else
      cout << "default" << endl;

    cout << "  number of noise events: " << (*it).second << endl;
  }

}

void KChamonixKAmpSite::WriteExtraData(TDirectory *dd)
{
  if(dd == 0){
    cerr << "KChamonixKAmpSite::WriteExtraData received null pointer." << endl;
    return;
  }

  //extract the noise spectra and place them into histograms
  std::map<std::string, std::vector<double> >::iterator it;

  TDirectory *nd = dd->mkdir("noisespectra");

  if(nd == 0){
    cerr << "unable to make noisespectra directory in KChamonixKAmpSite::WriteExtraData" << endl;
    return;
  }

  nd->cd();
  for(it = fNoiseSpectra.begin(); it != fNoiseSpectra.end(); it++){
    string histname = it->first;

    TH1D h(histname.c_str(), histname.c_str(), it->second.size(), 0, it->second.size());
    for (unsigned int i = 0; i <  it->second.size(); i++)
      h.SetBinContent(i+1, it->second[i]);

    h.SetEntries( GetNumNoiseEventsFound(it->first.c_str()) );
    h.Write(histname.c_str(), TH1D::kWriteDelete);

  }
    
  nd = dd->mkdir("templatefft");

  if(nd == 0){
    cerr << "unable to make templatefft directory in KChamonixKAmpSite::WriteExtraData" << endl;
    return;
  }
  nd->cd();
  for(it = fTemplateSpectra.begin(); it != fTemplateSpectra.end(); it++){
    string histname = it->first;

    TH1D h(histname.c_str(), histname.c_str(), it->second.size(), 0, it->second.size());
    for (unsigned int i = 0; i <  it->second.size(); i++)
      h.SetBinContent(i+1, it->second[i]);

    h.SetEntries( 1 );
    h.Write(histname.c_str(), TH1D::kWriteDelete);

  }

  

}


