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
  virtual bool RunProcess(bool smartMemory);
  virtual void AddProcessor(KPtaProcessor *p);
  virtual void AddProcessorAt(KPtaProcessor *p, unsigned int index);
  virtual void DeleteProcessors(void);
  virtual void SetIsOwner(bool anOpt = true);

  KPtaProcessor* GetProcessor(unsigned int i) { return fProcessorList.at(i);}
  virtual unsigned int GetNumProcessors(void) const {return fProcessorList.size();}
  virtual void SetMyOutputPulse(const double* p, unsigned int s);

private:
  std::vector<KPtaProcessor*> fProcessorList;
  bool fIsOwner; 
  //private methods
  void InitializeMembers(void);


  //ClassDef(KPulseAnalysisChain,1);

};

//
#endif // __KPULSEANALYSISCHAIN_H__
