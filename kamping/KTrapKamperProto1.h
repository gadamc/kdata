//
// KTrapKamperProto1.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KTRAPKAMPERPROTO1_H_
#define __KTRAPKAMPERPROTO1_H_

#include "KAmper.h"
#include "KBaselineRemoval.h"
#include "KPatternRemoval.h"
#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include "KPeakDetectorProto.h"
#include "KRootMeanSquare.h"
#include <string>
#include <vector>
#include <map>

class KTrapKamperProto1 : KAmper {

public:
  KTrapKamperProto1(void);
  virtual ~KTrapKamperProto1(void);
  
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual Bool_t MakeKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec, double fixPeakPosition);
  virtual Bool_t MakeBaseKamp(KRawBoloPulseRecord * rawPulseRecord, KPulseAnalysisRecord *rec);
  virtual void SetName(const char* name){fName = name;}
  virtual const char* GetName(void){return fName.c_str();}

  virtual std::vector<KTrapezoidalFilter*>& GetTrapHeatTime( void ){return fTrapHeatTime;}  
  virtual std::vector<KTrapezoidalFilter*>& GetTrapIonTime( void ){return fTrapIonTime;}
  
  virtual KTrapezoidalFilter* AddTrapHeatTime(double decay, unsigned int rise, unsigned int flat); 
  virtual KTrapezoidalFilter* AddTrapIonTime(double decay, unsigned int rise, unsigned int flat); 
  
  virtual KTrapezoidalFilter* GetTrapAmplitude(void){return &fTrapAmplitude;}
  
  virtual KBaselineRemoval* GetBaselineRemovalHeat(void){return &fBaseRemovalHeat;}
  virtual KBaselineRemoval* GetBaselineRemovalIon(void){return &fBaseRemovalIon;}
  virtual KPatternRemoval* GetPatternRemoval(void){return &fPatRemoval;}
  
  virtual std::vector<double>& GetPeakPositionResult(void){return fPeakPositionResult;}

  virtual double GetPeakPositionSearchAmplifier(void){return fPeakPositionSearchAmplifier;}
  virtual void SetPeakPositionSearchAmplifier(double aval){ fPeakPositionSearchAmplifier = aval;}


  //methods to set internal values for the various parameters for specific channels.
  //these parameters should be available from the database 
  virtual void SetTrapAmplitudeDecayConstant(const char* channelName, double value){ fTrapAmplitudeDecayConstants[channelName] = value;}
  virtual double GetTrapAmplitudeDecayConstant(const char* channelName) const;
  

  //temporarily public!
  void FillPeakPositionResult(KOrderFilter& fOrderFilter, KTrapezoidalFilter* trap, int polarity);
  void ClearPeakPositionResult(void) {fPeakPositionResult.clear();}
  void ResizePeakPositionResult(unsigned int size){fPeakPositionResult.resize(size);}
  //unsigned int PileUpDetection(KRawBoloPulseRecord * pRec, KPulseAnalysisRecord *rec);
  
  std::vector<int>& GetHeatPulseStampWidths(KRawBoloPulseRecord * pRec);
  
private:
  std::string fName;
  std::vector<double> fPeakPositionResult;
  std::vector<int> fHeatPulseStampWidths;
  
  std::vector<KTrapezoidalFilter* > fTrapIonTime;
  std::vector<KTrapezoidalFilter* > fTrapHeatTime;

  KBaselineRemoval fBaseRemovalHeat;
  KBaselineRemoval fBaseRemovalIon;
  KPatternRemoval fPatRemoval;
  KRootMeanSquare fRms;
  
  KTrapezoidalFilter fTrapAmplitude;

  KOrderFilter fOrderFilter1Heat;
  KOrderFilter fOrderFilter2Heat;
  KOrderFilter fOrderFilter1Ion;
  KOrderFilter fOrderFilter2Ion;
  
  KPeakDetectorProto fMaxIonPeakDetector;
  KPeakDetectorProto fMultipleIonPeakDetector;
  
  KPeakDetectorProto fMaxHeatPeakDetector;
  KPeakDetectorProto fMultipleHeatPeakDetector;
  
  double fPeakPositionSearchAmplifier;
  
  std::map<std::string, double> fTrapAmplitudeDecayConstants;
  std::vector<double> fDefaultTrapHeatAmplitudeParameters;
  std::vector<double> fDefaultTrapIonAmplitudeParameters;
  void FillTrapAmplitudeParameters(const char* channelName, bool isHeatPulse);
  
  
  unsigned int FindPeak(vector<double>& pulse, unsigned int maxPosition);
    
  double GetMean(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity);
  double GetMax(unsigned int first, unsigned int last, double *pulse, unsigned int pulseLength, int polarity);
  
  unsigned int RunPulseStartTime(vector<KTrapezoidalFilter *>& trapVec, KOrderFilter& ord1, KOrderFilter& ord2, 
    KPtaProcessor& fromProcessor, int polarity);
  unsigned int RunIonPulseStartTime(int polarity);
  unsigned int RunHeatPulseStartTime(void);
  
  virtual KTrapezoidalFilter* AddTrapTime(vector<KTrapezoidalFilter *>& trapVec, double decay, 
                                unsigned int rise, unsigned int flat);
  
  
  
};


#endif // __KTRAPKAMPERPROTO1_H_
