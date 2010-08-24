/*
 *  KPsaProcessor.h
 *  kData
 *
 *  Created by Adam Cox on 8/24/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KPSAPROCESSOR_H__
#define __KPSAPROCESSOR_H__

#include "Rtypes.h"
#include <iostream>

class KPsaProcessor  { 

public:
	virtual void SetInputPulse(vector<Double_t> aPulse){fInputPulse = aPulse;}
	virtual vector<Double_t> GetOutputPulse(void){return fOutputPulse;}
	virtual Bool_t RunProcess(void) = 0;
	
  //getters
	virtual char* GetName(void){return fProcessorName.c_str();}
  //setter
	
protected:
  //Constructors -- only derived classes can be instansiated. 
  KPsaProcessor(void);
  virtual ~KPsaProcessor(void);
	virtual SetName(const char* aName){fProcessorName = aName;}
	
private:
	string fProcessorName;
	vector<Double_t> fInputPulse;
	vector<Double_t> fOutputPulse;
  //private methods
  void InitializeMembers(void);

  ClassDef(KPsaProcessor,1);
};


#endif // __KPSAPROCESSOR_H__
