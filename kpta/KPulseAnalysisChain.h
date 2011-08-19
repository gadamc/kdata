//______________________________________________________________________
//
// KPulseAnalysisChain.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPULSEANALYSISCHAIN_H__
#define __KPULSEANALYSISCHAIN_H__

#include "KPtaProcessor.h"
#include <vector>

class KPulseAnalysisChain : public KPtaProcessor { 

public:
  //Constructors
  KPulseAnalysisChain(void);
  virtual ~KPulseAnalysisChain(void);
  //virtual void Clear(Option_t *opt = "C");
	virtual bool RunProcess(void);
	virtual void AddProcessor(KPtaProcessor *p);
	virtual void AddProcessorAt(KPtaProcessor *p, unsigned int index);
	virtual void DeleteProcessors(void);
  virtual void SetIsOwner(bool anOpt = true);
  
  //KPtaProcessor* GetProcessor(Int_t i) { return fProcessorList.At(i);}
  
private:
	vector<KPtaProcessor*> fProcessorList;
  bool fIsOwner; 
  //private methods
  void InitializeMembers(void);
  void SetOutputPulse(const double* p, unsigned int s);
};

//
#endif // __KPULSEANALYSISCHAIN_H__
