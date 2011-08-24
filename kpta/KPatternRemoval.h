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
  
 // ClassDef(KPatternRemoval,1);
  

};


#endif // __KPATTERNREMOVAL_H__
