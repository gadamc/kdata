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

#include "KPsaProcessor.h"

class KPatternRemoval : public KPsaProcessor { 

public:
  //Constructors
  KPatternRemoval(void);
  virtual ~KPatternRemoval(void);
	virtual bool RunProcess(void);

	virtual bool CalculatePattern(void);
	virtual bool SubtractPattern(void);
	virtual bool SubtractPattern(vector<double> &aPattern);
	
  //getters
	virtual unsigned int GetPatternLength(void) const { return fPatternLength;}
	virtual unsigned int GetBaselineStart(void) const { return fBaselineStart;}
	virtual unsigned int GetBaselineStop(void) const { return fBaselineStop;}

	virtual vector<double> GetPattern(void) const { return fPattern;}

  //setters
	virtual void SetPatternLength(unsigned int aLength){ fPatternLength = aLength;}
	virtual void SetBaselineStart(unsigned int aVal);
	virtual void SetBaselineStop(unsigned int aVal);
	
	virtual bool SetPattern(vector<double> &aPattern);
	virtual bool SetPattern(vector<short> &aPulse){ return SetThisToPattern(aPulse);} //Set the pattern 
	virtual bool SetPattern(vector<float> &aPulse){ return SetThisToPattern(aPulse);} //Set the pattern
	virtual bool SetPattern(vector<int> &aPulse){ return SetThisToPattern(aPulse);} //Set the pattern
	
private:
	unsigned int fPatternLength;  //used primarily for the CalculatePattern routine. Whenever you call SetPattern, this value is changed to match the length of the pattern you passed in.
	unsigned int fBaselineStart;
	unsigned int fBaselineStop;
	vector<double> fPattern;
  //private methods
  void InitializeMembers(void);
	template<class T> bool SetThisToPattern(vector<T> &aPulse);

  //ClassDef(KPatternRemoval,1);
};


#endif // __KPATTERNREMOVAL_H__
