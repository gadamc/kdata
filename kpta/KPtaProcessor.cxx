//______________________________________________________________________

// KPtaProcessor.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This is the abstract base class for all Pulse Trace Analysis Processors
// When you write a new class that derives from KPtaProcesser, you 
// must define a RunProcess method. 
// This class provides a number of input/output methods to set the input
// pulse and get the output result. You can also give each process a name


#include "KPtaProcessor.h"
#include <fstream>
#include <sstream>

using namespace std;


//ClassImp(KPtaProcessor)

KPtaProcessor::KPtaProcessor(void)
{

  InitializeMembers();
}

KPtaProcessor::~KPtaProcessor(void)
{
  if(!fDoNotDelete){
    if(fInputPulse) {
      delete[] fInputPulse;
      fInputPulse = 0;
      fInputSize =0;
    }

    if(fOutputPulse) {
      delete[] fOutputPulse;
      fOutputPulse = 0;
      fOutputSize =0;
    }
  }
  
}

void KPtaProcessor::InitializeMembers(void)
{
  fProcessorName = "";
  fInputPulse = 0;
  fOutputPulse = 0;
  fInputSize = 0;
  fOutputSize = 0;
  fDoNotDelete = false;
}

void KPtaProcessor::AllocateArrays(unsigned int size)
{
  if(fInputPulse) delete [] fInputPulse;
  if(fOutputPulse) delete [] fOutputPulse;
  fInputPulse = new double[ size ];
  fOutputPulse = new double[ size ];
  memset(fOutputPulse, 0, size*sizeof(double));
  fInputSize = fOutputSize = size;
  
}

void KPtaProcessor::SetInputPulse(double *aPulse)
{
  //This is a relatively dangerous method. Use it only if you know what you're doing.
  //However, using this method to set the pointer to the intput pulse is a clever way
  //of reducing the amount of CPU time spent copying pulses around. Instead, you can use
  //this to reuse memory.
  //
  //Make sure that you set the output pulse pointer and the size of the pulses. 
  //
  //Also, note that you probably shouldn't use this for the FFT-related methods since
  //those use a special fftw memory allocation and deallocation function. 
  //However, it might work as well - see the fftw documentation. 
  //
  //This automatically calls "DoNotDeletePulses" which tells the KPtaProcessor
  //not to delete the pulse in the destructor. 
  //
  fInputPulse = aPulse;
  fDoNotDelete = true;
}


void KPtaProcessor::SetOutputPulse(double *aPulse)
{
  //This is a relatively dangerous method. Use it only if you know what you're doing.
  //However, using this method to set the pointer to the intput pulse is a clever way
  //of reducing the amount of CPU time spent copying pulses around. Instead, you can use
  //this to reuse memory.
  //
  //Make sure that you set the output pulse pointer and the size of the pulses. 
  //
  //Also, note that you probably shouldn't use this for the FFT-related methods since
  //those use a special fftw memory allocation and deallocation function. 
  //However, it might work as well - see the fftw documentation. 
  //
  //This automatically calls "DoNotDeletePulses" which tells the KPtaProcessor
  //not to delete the pulse in the destructor. 
  //
  fOutputPulse = aPulse;
  fDoNotDelete = true;
}


void KPtaProcessor::SetInputPulse(const char* aFile)
{
	//Load a pulse from a file. This is especially useful for loading
	//a template that is already in a file format.
	//The file should be a simple list of values in each line
	//representing one point in the file. 
	
  ifstream mFile(aFile,ios::in);
  if (!mFile) {
    cerr << "No file found: " << aFile << endl; 
		return;
  }
	
	double theValue = 0;
	string line;
  vector<double> inputPulse;
	
  while (getline(mFile,line)) {
    istringstream ss(line);
		ss >> theValue; 
		inputPulse.push_back(theValue);
  }
  mFile.close();
	
  SetInputPulse(inputPulse);
}
