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

KPtaProcessor::KPtaProcessor(void)
{

  InitializeMembers();
}

KPtaProcessor::~KPtaProcessor(void)
{
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

void KPtaProcessor::InitializeMembers(void)
{
  fProcessorName = "";
  fInputPulse = 0;
  fOutputPulse = 0;
  fInputSize = 0;
  fOutputSize = 0;
}

void KPtaProcessor::AllocateArrays(unsigned int size)
{
  if(fInputPulse) delete [] fInputPulse;
  if(fOutputPulse) delete [] fOutputPulse;
  fInputPulse = new double[ size ];
  fOutputPulse = new double[ size ];
  fInputSize = fOutputSize = size;
  
}

template <class T> void KPtaProcessor::SetTheInputPulse(const T* aPulse, unsigned int size)
{
  if(size != fInputSize)
    AllocateArrays(size);
  
  for(unsigned int i = 0; i < size; i++)
    *(fInputPulse + i) = *(aPulse + i);
  
}


template <class T> void KPtaProcessor::SetTheInputPulse(const vector<T> &aPulse)
{
  if(aPulse.size() != fInputSize)
    AllocateArrays(aPulse.size());  
	
		try {
			for(unsigned int i = 0; i < fInputSize; i++){
				*(fInputPulse+i) = aPulse.at(i);
			}
		}
		catch (out_of_range& e) {
			//I think this should be impossible... 
			cerr << "KPtaProcessor::SetThisToInputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
		}
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
