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
#include <vector>
#include "TObject.h"  //inherits from TObject so that it can be put in a ROOT container

using namespace std;

class KPsaProcessor : public TObject  { 

public:
	virtual void SetInputPulse(vector<Double_t> aPulse){fInputPulse = aPulse;}
	virtual vector<Double_t> GetOutputPulse(void){return fOutputPulse;}
	virtual Bool_t RunProcess(void) = 0;
	
  //getters
	virtual const char* GetName(void){return fProcessorName.c_str();}
  //setter
	
protected:
  //Constructors -- only derived classes can be instansiated. 
  KPsaProcessor(void);
  virtual ~KPsaProcessor(void);
	virtual void SetName(const char* aName){fProcessorName = aName;}
	
private:
	string fProcessorName;
	vector<Double_t> fInputPulse;
	vector<Double_t> fOutputPulse;
  //private methods
  void InitializeMembers(void);

  ClassDef(KPsaProcessor,1);
};


#endif // __KPSAPROCESSOR_H__
