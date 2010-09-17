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

#include <stdexcept> 
#include <iostream>
#include <vector>

using namespace std;

class KPsaProcessor  { 

public:
  KPsaProcessor(void);
  virtual ~KPsaProcessor(void);
  
	virtual void SetInputPulse(const vector<double> &aPulse){fInputPulse = aPulse;} //Set the input pulse
	virtual void SetInputPulse(const vector<short> &aPulse){ SetThisToInputPulse(aPulse);} //Set the input pulse 
	virtual void SetInputPulse(const vector<float> &aPulse){ SetThisToInputPulse(aPulse);} //Set the input pulse
	virtual void SetInputPulse(const vector<int> &aPulse){ SetThisToInputPulse(aPulse);} //Set the input pulse
	virtual void SetInputPulse(const char* aFile);
	
	virtual vector<double> GetOutputPulse(void) const {return fOutputPulse;}  //obtain a copy of the output pulse with double precision
	virtual void GetOutputPulse(vector<float> &myPulse) const; //demote the precision and obtian a copy of the output pulse in single precision
	
	virtual vector<double> GetInputPulse(void) const {return fInputPulse;}  //obtain a copy of the input pulse
	virtual void GetInputPulse(vector<float> &myPulse) const; //demote the precision and obtian a copy of the output pulse in single precision

	virtual bool RunProcess(void) = 0;
  //getters
	virtual const char* GetName(void) const {return fProcessorName.c_str();}
  //setter
	virtual void SetName(const char* aName){fProcessorName = aName;}
	
protected:

	string fProcessorName;
	vector<double> fInputPulse;
	vector<double> fOutputPulse;
	
	
private:
	
	template<class T> void SetThisToInputPulse(const vector<T> &aPulse);
  //private methods
  void InitializeMembers(void);
	
};


#endif // __KPSAPROCESSOR_H__
