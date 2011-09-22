//______________________________________________________________________
//
// KPeakFinder.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPEAKFINDER_H__
#define __KPEAKFINDER_H__

#include "KPtaProcessor.h"
#include "KRootMeanSquare.h"
#include <vector>

class KPeakFinder : public KPtaProcessor { 
  
public:
  //Constructors
  KPeakFinder(void);
  //for the memory-savy programmers
  KPeakFinder(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KPeakFinder(void);
  
  virtual bool RunProcess(void);
  
  virtual void SetExtraWeakAmp(double a){fExtraWeakAmp = a;}
  virtual void SetWeakAmp(double a){fWeakAmp = a;}
  virtual void SetStrictAmp(double a){fStrictAmp = a;}
  virtual void SetPolarity(bool a) {fPolarity = a;}
  
  virtual double GetExtraWeakAmp(void) const {return fExtraWeakAmp;}
  virtual double GetWeakAmp(void) const {return fWeakAmp;}
  virtual double GetStrictAmp(void) const {return fStrictAmp;}
  virtual bool GetPolarity(void) const {return fPolarity;}
  
  virtual unsigned int GetNumExtraWeakPeaks(void) const {return fExtraWeakPeakPositions.size();}
  virtual unsigned int GetNumWeakPeaks(void) const {return fWeakPeakPositions.size();}
  virtual unsigned int GetNumStrictPeaks(void) const {return fStrictPeakPositions.size();}
  
  virtual unsigned int GetExtraWeakPosition(unsigned int i) const {return fExtraWeakPeakPositions[i];}
  virtual unsigned int GetWeakPosition(unsigned int i) const {return fWeakPeakPositions[i];}
  virtual unsigned int GetStrictPosition(unsigned int i) const {return fStrictPeakPositions[i];}
  
  virtual void SetHeatDefault(void);
  virtual void SetIonDefault(void);
  
  
protected:
  double fExtraWeakAmp;  //in units of the number of std dev of the baseline (RMS)
  double fWeakAmp; //in units of the number of std dev of the baseline (RMS)
  double fStrictAmp;  //in units of the number of std dev of the baseline (RMS)
  bool fPolarity; //false is negative polarity (negative-going pulse), and true is positive-going pulse
  
  std::vector<unsigned int> fExtraWeakPeakPositions;
  std::vector<unsigned int> fWeakPeakPositions;
  std::vector<unsigned int> fStrictPeakPositions;
  
  KRootMeanSquare fRms;  
  
  
  virtual void EraseVectors(void);
  
private:
  //private methods
  void InitializeMembers(void);
  
  
};


#endif
