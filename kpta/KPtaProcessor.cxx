//______________________________________________________________________

// KPtaProcessor.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPtaProcessor.h"
#include <fstream>
#include <sstream>

KPtaProcessor::KPtaProcessor(void)
{

  InitializeMembers();
}

KPtaProcessor::~KPtaProcessor(void)
{
  
}

void KPtaProcessor::InitializeMembers(void)
{
  fProcessorName = "";
}

template<class T> void KPtaProcessor::SetThisToInputPulse(const vector<T> &aPulse)
{
	fInputPulse.resize(aPulse.size(),0);
	
		try {
			for(unsigned int i = 0; i < fInputPulse.size(); i++){
				fInputPulse.at(i) = aPulse.at(i);
			}
		}
		catch (out_of_range& e) {
			//I think this should be impossible... 
			cerr << "KPtaProcessor::SetThisToInputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
		}
}

void KPtaProcessor::GetOutputPulse(vector<float> &myPulse) const
{
	//Demote the precision and obtain a copy of the output pulse in single precision.
	//Note that working with floats is slower (because the vector has to be copied 
	//from double precision to single precision) and more error prone. 
	//It's better just to use a vector of doubles.
	
	myPulse.resize(fOutputPulse.size(),0);
	
	try {
		for(unsigned int i = 0; i < myPulse.size(); i++){
			myPulse.at(i) = fOutputPulse.at(i);
		}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "vector<float> KPtaProcessor::GetOutputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
	}

}

void KPtaProcessor::GetInputPulse(vector<float> &myPulse) const
{
	//Demote the precision and obtain a copy of the input pulse in single precision.
	//Note that working with floats is slower (because the vector has to be copied 
	//from double precision to single precision) and more error prone. 
	//It's better just to use a vector of doubles.
	
	myPulse.resize(fInputPulse.size(),0);
	
	try {
		for(unsigned int i = 0; i < myPulse.size(); i++){
			myPulse.at(i) = fInputPulse.at(i);
		}
	}
	catch (out_of_range& e) {
		//I think this should be impossible... 
		cerr << "vector<float> KPtaProcessor::GetOutputPulse. exception caught: " << e.what() << " ending the copy of the pulse." << endl;
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
    cerr << "No ascii trace file found: " << aFile << endl; 
		return;
  }
	
	double theValue = 0;
	string line;
	fInputPulse.clear();
	
  while (getline(mFile,line)) {
    istringstream ss(line);
		ss >> theValue; 
		fInputPulse.push_back(theValue);
  }
	
  mFile.close();
	
}
