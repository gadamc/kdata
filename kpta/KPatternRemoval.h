//______________________________________________________________________
//
// KPatternRemoval.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPATTERNREMOVAL_H__
#define __KPATTERNREMOVAL_H__

#include "KPtaProcessor.h"

using namespace std;

class KPatternRemoval : public KPtaProcessor { 

public:
  //Constructors
  KPatternRemoval(void);
  //for the memory-savy programmers
  KPatternRemoval(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KPatternRemoval(void);

  virtual bool RunProcess(void);
  template<class T> bool RunProcess(vector<T> &aPattern);

  virtual bool CalculatePattern(void);
  virtual bool SubtractPattern(void);
  virtual void SubtractPattern(unsigned int i, unsigned int patternPos);

  virtual double* GetPattern(void) const { return fPattern;}
  template<class T> bool SetPattern(vector<T> &aPulse);

  virtual unsigned int GetPatternLength(void) const { return fPatternLength;}
  virtual unsigned int GetBaselineStart(void) const { return fBaselineStart;}
  virtual unsigned int GetBaselineStop(void) const { return fBaselineStop;}

  virtual bool SetPatternLength(unsigned int aLength);
  virtual void SetBaselineStart(unsigned int aVal);
  virtual void SetBaselineStop(unsigned int aVal);

  virtual void SetUseExternalPattern(bool opt){opt = fUseExternalPattern;}
  virtual bool GetUseExternalPattern(void) const {return fUseExternalPattern;}

private:
  unsigned int fPatternLength;  
  unsigned int fBaselineStart;
  unsigned int fBaselineStop;
  bool fUseExternalPattern;
  double* fPattern;
  //private methods
  void InitializeMembers(void);

  //ClassDef(KPatternRemoval,1);


};


template<class T> bool KPatternRemoval::SetPattern(std::vector<T> &aPattern)
{
  //sets the pattern and sets fUseExternalPattern to true if
  //the memory for the pattern is properly allocated.
  //otherwise, fUseExternalPattern is not changed. 
  //
  //If this methods returns true, then
  //this means, the following are equivalent.
  // SetPattern(aPattern);
  // RunProcess();
  //
  // and
  //
  // RunProcess(aPattern);
  //

  if(SetPatternLength(aPattern.size()))
    return false;

  for(unsigned int i = 0; i < fPatternLength; i++){
    *(fPattern+i) = aPattern.at(i);
  }
  fUseExternalPattern = true;
  return true;
}


template<class T> bool KPatternRemoval::RunProcess(std::vector<T> &aPattern)
{
  //Subtracts aPattern from the input pulse and assigns the result to the output pulse.

  return (SetPattern(aPattern) ? SubtractPattern() : false);

  /*
  if(SetPattern(aPattern))
  return SubtractPattern();
  else
  return false;
  */
}
#endif // __KPATTERNREMOVAL_H__
