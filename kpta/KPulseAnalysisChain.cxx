//______________________________________________________________________
//
// KPulseAnalysisChain.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// This object can hold (and own) pointers to objects that derive
// from KPtaProcessor in order to create a chain of pulse processing steps.
// In addition, this class is also derived from KPtaProcessor, which means
// that one can create a nested chain of chains. 
//
// By default, it does not own the memory of the objects in its list
// but that can be set via the SetIsOwner method.
//
// When KPulseAnalysisChain::RunProcess is called, for each 
// KPtaProcessor-derived object in the list it calls SetInputPulse(), 
// RunProcess() and then copies the result from GetOutputPulse() into 
// the SetInputPulse for the next KPtaProcessor in the list. 
//

#include "KPulseAnalysisChain.h"
#include <stdexcept> 
#include <iostream>

using namespace std;

//ClassImp(KPulseAnalysisChain);


KPulseAnalysisChain::KPulseAnalysisChain(void)
{
  SetName("KPulseAnalysisChain");

  InitializeMembers();
  fIsOwner = 0;
}

KPulseAnalysisChain::~KPulseAnalysisChain(void)
{

  DeleteProcessors();
}

//void KPulseAnalysisChain::Clear(Option_t* /* opt */)
//{
  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  //InitializeMembers();

//}
void KPulseAnalysisChain::DeleteProcessors(void)
{
  //deletes all of the KPtaProcessors in the list if its
  //the owner. If fIsOwner is false, this method does nothing. 

  if(fIsOwner){

    for(unsigned int i = 0; i < fProcessorList.size(); i++){
      try {
        KPtaProcessor *p = fProcessorList.at(i);
        if(p != 0){
          delete p;
          p = 0;
        }
      }
      catch (out_of_range& e) {
        //I think this should be impossible... 
        cerr << "KPulseAnalysisChain::DeleteProcessors. i = " << i << " exception caught: " << e.what() << endl;
        cerr << "  The array was NOT cleared. Possible Memory Leak. Please submit a bug report at https://edwdev-ik.fzk.de/bugs" << endl;
        return;
      }
    }
    fProcessorList.clear();
  }
}


void KPulseAnalysisChain::SetIsOwner(bool anOpt)
{ 
  //if anOpt is set to TRUE, then the analysis chain "owns"
  //all of the processors that were added to the chain via
  //AddProcessor or AddProcessorAt. 
  //If KPulseAnalysisChain::DeleteProcessors() is called, it will 
  //delete all of the processors if its the owner. Otherwise,
  //it won't delete any of them.
  //Also, when this object's destructor is called, it will 
  //delete the Processors that it owns. 

  fIsOwner = anOpt;  
}



void KPulseAnalysisChain::InitializeMembers(void)
{
  //clears the list of processors and initilizes any local data members
  //to their default values.

  DeleteProcessors();
  fProcessorList.resize(0);
  fIsOwner = false;
}


bool KPulseAnalysisChain::RunProcess(void)
{
  return RunProcess(false);
}
/*
void KPulseAnalysisChain::SetPulsePointer(KPtaProcessor* p, double* pulse, unsigned int i, bool memopt)
{
if(memopt)
p->SetInputPulse(pulse); //this call just sets the pointer, reusing the same space in memory
else
p->SetIntputPulse(pulse, i); //this call will copy the values from this to the memory for p
}
*/


bool KPulseAnalysisChain::RunProcess(bool smartMemory)
{
  //call RunProcess for each KPtaProcessor in the list.
  //returns true if all processors run and no errors are reported.
  //Otherwise it returns false
  //
  //future feature request:
  //returns the number of processors that return "true".
  //if a processor fails (returns "false"), will return a -X, 
  //where X is the Xth processor in the list. 
  //
  //The KPulseAnalysisChain does not play well if you're trying to be fancy
  //with memory usage. It will probably cause a memory leak. 

  //
  //if smartMemory == true, then the processors in the chain will only use
  //the memory allocated for the input and output pulses allocated by this KPulseAnalysisChain.
  //Otherwise, each processor in the chain will have its own input/output pulses and will copy data
  //to their respective memory before performing their operation.
  //The user is responsible for setting the pointers in each element of the chain.
  //
  //This class is then just an organizer of processors and all memory management is done
  //by the user.
  //
  //Note that this might break with the FFT-related processors if you try to use FFTW's
  //parallel processing tools, so be careful with that.
  //
  //

  if( (fInputPulse == 0 || fOutputPulse == 0) && !smartMemory) {
    cerr << "input and output pulses are not allocated." << endl;
    return false;
  }


  int theReturn = 0;
  KPtaProcessor *p_prev = 0;

  for(unsigned int i = 0; i < fProcessorList.size(); i++){
    //cout << "Pulse Analysis Chain Processor: " << i << endl;
    try {
      KPtaProcessor *p = fProcessorList.at(i);
      
      //set the input pulse of the previous processor. 
      if(!smartMemory) {
        if(i == 0)
          p->SetInputPulse(GetInputPulse(), GetInputPulseSize());
        else 
          p->SetInputPulse(p_prev->GetOutputPulse(), p_prev->GetOutputPulseSize());
      } 
      if(p->RunProcess()){
        theReturn++;

      }
      else {
        cerr << p->GetName() << " Processor Failed. Stopping Subsequent Processes for this Chain. " << endl;
        cerr << "   The output pulse for this chain should be the same as the input pulse." << endl;
        cerr << "   If you're sure this is a bug, submit a report at https://edwdev-ik.fzk.de/bugs" << endl;
        //fOutputPulse = fInputPulse;  //make the output equal the input.
        break;
      }
      
      p_prev = p;
      
      if(i == fProcessorList.size() - 1  && !smartMemory)
        SetMyOutputPulse(p->GetOutputPulse(), p->GetOutputPulseSize());
        
    }
    catch (out_of_range& e) {
      //I think this should be impossible... 
      cerr << "KPulseAnalysisChain::RunAll. i = " << i << " exception caught: " << e.what() << endl;
    }

  }
  

  if(theReturn) 
    return true;
  else return false;
}

void KPulseAnalysisChain::AddProcessor(KPtaProcessor *p)
{
  //Add a processor to the end of the list.
  //If p is NULL, then nothing happens. 
  
  if(p == 0) return;
  fProcessorList.push_back(p);	
}

void KPulseAnalysisChain::AddProcessorAt(KPtaProcessor *p, unsigned int index)
{
  //Add a processor at a particular position in the list.
  //If p is NULL then nothing happens
  
  if(p == 0) return;
  
  if(index >= fProcessorList.size())
    AddProcessor(p);

  else 
    fProcessorList.insert(fProcessorList.begin() + index, p);
}

void KPulseAnalysisChain::SetMyOutputPulse(const double* p, unsigned int s)
{
  if(s != fOutputSize){
    if(fOutputPulse) delete [] fOutputPulse;
    fOutputPulse = new double[s];
  }

  memcpy(fOutputPulse, p, s*sizeof(double));  
}

/*void KPulseAnalysisChain::GetNumProcessorsInChain(void)
{

}*/