//______________________________________________________________________
//
// KPulseAnalysisChain.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPulseAnalysisChain.h"
#include <stdexcept> 
#include <iostream>

using namespace std;

//ClassImp(KPulseAnalysisChain);

KPulseAnalysisChain::KPulseAnalysisChain(void)
{
	
  InitializeMembers();
}

KPulseAnalysisChain::~KPulseAnalysisChain(void)
{
  //Does calling clear at destruction take too much computing time?
  //Clear("C");

}

//void KPulseAnalysisChain::Clear(Option_t* /* opt */)
//{
	//Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  //InitializeMembers();

//}


void KPulseAnalysisChain::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	fProcessorList.resize(0);
}

int KPulseAnalysisChain::RunAll(void)
{
	//call RunProcess for each KPsaProcessor in the list.
	//returns the number of processors that return "true".
	//if a processor fails (returns "false"), will return a -X, 
	//where X is the Xth processor in the list. 
	//
	int theReturn = 0;
	
	for(unsigned int i = 0; i < fProcessorList.size(); i++){
		
		try {
			KPsaProcessor *p = fProcessorList.at(i);
			if(p != 0){
				if(p->RunProcess()){
					theReturn++;
					if(i+1 < fProcessorList.size()){
						KPsaProcessor *pnext = fProcessorList.at(i+1);
						if(pnext != 0) //if this fails here, then this will break... the next processor won't get the pulse
							pnext->SetInputPulse(p->GetOutputPulse());
					}
				}
				else {
					i = fProcessorList.size(); //end the loop
					theReturn = -1*theReturn;
				}	
				
			}
			
		}
		catch (out_of_range& e) {
			//I think this should be impossible... 
			cerr << "KPulseAnalysisChain::RunAll. i = " << i << " exception caught: " << e.what() << endl;
		}
		
					
	}
	
	return theReturn;
}

void KPulseAnalysisChain::AddProcessor(KPsaProcessor *p)
{
	fProcessorList.push_back(p);	
}

void KPulseAnalysisChain::AddProcessorAt(KPsaProcessor *p, unsigned int index)
{
	if(index >= fProcessorList.size())
		AddProcessor(p);
	
	else 
		fProcessorList.insert(fProcessorList.begin() + index, p);
}

/*void KPulseAnalysisChain::GetNumProcessorsInChain(void)
{
	
}*/