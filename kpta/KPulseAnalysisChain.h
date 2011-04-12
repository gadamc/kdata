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
  
  //getters
	//virtual unsigned int GetNumProcessorsInChain(void);
	//PrintProcessorList
	
	//Access to processors after they've been added to the list!?!
	
  //setters
private:
	vector<KPtaProcessor*> fProcessorList;
  bool fIsOwner; 
  //private methods
  void InitializeMembers(void);
  void SetOutputPulse(const vector<double>);
};

//
#endif // __KPULSEANALYSISCHAIN_H__
