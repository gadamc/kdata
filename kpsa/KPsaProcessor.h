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

#include <iostream>
#include <vector>

using namespace std;

class KPsaProcessor  { 

public:

	virtual void SetInputPulse(vector<double> aPulse){fInputPulse = aPulse;}
	virtual vector<double> GetOutputPulse(void){return fOutputPulse;}
	virtual bool RunProcess(void) = 0;
  //getters
	virtual const char* GetName(void){return fProcessorName.c_str();}
  //setter
	virtual void SetName(const char* aName){fProcessorName = aName;}
	
protected:

	string fProcessorName;
	vector<double> fInputPulse;
	vector<double> fOutputPulse;
	
	KPsaProcessor(void);
  virtual ~KPsaProcessor(void);
	
private:
	

  //private methods
  void InitializeMembers(void);

  //ClassDef(KPsaProcessor,1);
};


#endif // __KPSAPROCESSOR_H__
