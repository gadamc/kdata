//_____________________________________________
// KOptimalKamper.h
// KDataStructure
//
// Created by Adam Cox
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KOPTIMALKAMPER_H_
#define __KOPTIMALKAMPER_H_

#include "KAmper.h"
#include "KOptimalFilter.h"
#include "KRealToHalfComplexDFT.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

class KWindow;
class KPtaProcessor;


class KOptimalKamper : KAmper {

public:
  KOptimalKamper(void);
  virtual ~KOptimalKamper(void);
  
  virtual std::map<std::string, KResult>&  MakeKamp(KRawBoloPulseRecord * rawPulseRecord);
  virtual std::map<std::string, KResult>&  MakeKamp(KRawBoloPulseRecord * rawPulseRecord, double fixPeakPosition);


  KOptimalFilter& GetOptimalFilter(void){return fOptimalFilter;}
  void SetWindow(KWindow *pta){fWindow = pta;}
  void SetPreProcessor(KPtaProcessor *pta){fPreProcessor = pta;}
  KWindow* GetWindow(void){return fWindow;}
  KPtaProcessor* GetPreProcessor(void){return fPreProcessor;}
  KRealToHalfComplexDFT& GetRealToHalfComplexDFT(void){return fR2Hc;}
   
  void SetPulseAmplitudeShift(int a){fPulseAmplitudeShift = a;}
  void SetPulseTemplateShiftFromPreTrigger(double aVal){fPulseTemplateShift = -1*aVal;}
  double GetPulseTemplateShiftFromPreTrigger(void) const {return fPulseTemplateShift;}

  void SetAmplitudeEstimatorSearchRangeMin(double aVal){fAmplitudeEstimatorSearchRangeMin = aVal;}
  void SetAmplitudeEstimatorSearchRangeMax(double aVal){fAmplitudeEstimatorSearchRangeMax = aVal;}
  double GetAmplitudeEstimatorSearchRangeMin(void) const {return fAmplitudeEstimatorSearchRangeMin;}
  double GetAmplitudeEstimatorSearchRangeMax(void) const {return fAmplitudeEstimatorSearchRangeMax;}

  int GetPulseAmplitudeShift(void){return fPulseAmplitudeShift;}
  void SetIonPulseStartTime(double peakPos){fIonPulsePeakPos = peakPos;}


  //these methods just call the same methods defined in the optimal filter. this is just to make
  //user's code more clear -- see the KChamonixKAmpSite for example.
  virtual void SetToRecalculate(bool option = true){fOptimalFilter.SetToRecalculate(option);}  
  template <class T> void SetNoiseSpectrum(std::vector<T> &r);
  template <class T> void SetNoiseSpectrum(const T* resp, unsigned int size);
  template <class T> void SetTemplateDFT(std::vector<T> &r);
  template <class T> void SetTemplateDFT(const T* resp, unsigned int size);
  virtual void SetNoiseSpectrum(double* resp){fOptimalFilter.SetNoiseSpectrum(resp);}
  virtual void SetNoiseSpectrumSize(unsigned int size){fOptimalFilter.SetNoiseSpectrumSize(size);}
  virtual void SetTemplateDFT(double* resp){fOptimalFilter.SetTemplateDFT(resp);}
  virtual void SetTemplateDFTSize(unsigned int size){fOptimalFilter.SetTemplateDFTSize(size);}


  //void SetFineTuneAmpEstimatorSubBinSize(double value){fFineTuneAmpEstimatorSubBinSize = value;}
  //double GetFineTuneAmpEstimatorSubBinSize(void){return fFineTuneAmpEstimatorSubBinSize;}


  void SetUseMinimizer(bool opt=true){fUseMinimizer = opt;}
  bool GetUseMinimizer(void){return fUseMinimizer;}
  void SetMinimizerTimeStepValue(double value){fMinimizerTimeStepValue = value;}
  double GetMinimizerTimeStepValue(void){return fMinimizerTimeStepValue;}
  void SetMinimizerAmpStepValue(double value){fMinimizerAmpStepValue = value;}
  double GetMinimizerAmpStepValue(void){return fMinimizerAmpStepValue;}
  double Chi2Functor(const double* xx) {return fOptimalFilter.GetChiSquared(xx[0], xx[1]);}

private:
 
  KOptimalFilter fOptimalFilter;  //the optimal filter KPtaProcessor class used internally
  KPtaProcessor *fPreProcessor; //this should probably be baseline removal will always happen before windowing. Its a KPtaProcessor in case you want to pass in a KLinearRemoval instead.
  KWindow *fWindow; //set the window pointer - this lets you choose different windows via KWindowDesign

  double fPulseTemplateShift;
  double fAmplitudeEstimatorSearchRangeMin;  //minimum value of pulse amplitude search range, relative to the pulse shift. that is, if you set -10, then the fPulseTemplateShift-10 is the minimum value of the range searched
  double fAmplitudeEstimatorSearchRangeMax;  //maximum value of pulse amplitude search range, relative to the pulse shift. that is, if you set 100, then the fPulseTemplateShift+100 is the maximum value of the range searched
  int fPulseAmplitudeShift; //the position, relative to the pulse trigger, from which the pulse amplitude is always estimated. This is useful for the measurement of the baseline resolution. This IS NOT fPulseTemplateShift
  double fIonPulsePeakPos;
  
  KRealToHalfComplexDFT fR2Hc;

  //double fFineTuneAmpEstimatorSubBinSize; //should be a number between 0 and 1. but i dont check this explicitly, so the code may blow up if you give it values outside of this range.

  //attempt at using a real minimization routine to find the minimum chi2
  ROOT::Math::Minimizer*  fMinimizer; 
  ROOT::Math::IMultiGenFunction * fFunc;
  bool fUseMinimizer;  //turn mimization on/off. maybe its to cpu intensive to be useful. code removed. always off.
  double fMinimizerTimeStepValue;  //used with the mimizer to define the step size in time when minimizing chi2. not used.
  double fMinimizerAmpStepValue;   //also defines the step size for the minimizer when searching for amp to minimize chi2. not used.
  
};
 
template <class T> void KOptimalKamper::SetNoiseSpectrum(std::vector<T> &resp)
{
  fOptimalFilter.SetNoiseSpectrum(resp);
}

template <class T> void KOptimalKamper::SetNoiseSpectrum(const T* resp, unsigned int size)
{
  fOptimalFilter.SetNoiseSpectrum(resp, size);
}

template <class T> void KOptimalKamper::SetTemplateDFT(std::vector<T> &resp)
{
  fOptimalFilter.SetTemplateDFT(resp);
}

template <class T> void KOptimalKamper::SetTemplateDFT(const T* resp, unsigned int size)
{
  fOptimalFilter.SetTemplateDFT(resp, size);
}



#endif // __KOPTIMALKAMPER_H_
