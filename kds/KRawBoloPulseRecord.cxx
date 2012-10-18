//______________________________________________________________________
//
// KRawBoloPulseRecord.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Holds the Raw-level pulse information, including the pulse trace
// This class is based on KBoloPulseRecord (which stores the channel
// number and type). 
//


#include "KRawBoloPulseRecord.h"
#include "TH1.h"
#include "TGraph.h"
#include "TH1D.h"
#include <iostream>
#include <typeinfo>

using namespace std;

ClassImp(KRawBoloPulseRecord);

KRawBoloPulseRecord::KRawBoloPulseRecord(void)
{
  //standard constructor
  
  InitializeMembers();
}


KRawBoloPulseRecord::KRawBoloPulseRecord(const KRawBoloPulseRecord &aRec)
: KBoloPulseRecord(aRec)
{
  //copy constructor 
  
	CopyLocalMembers(aRec);
	
}

KRawBoloPulseRecord& KRawBoloPulseRecord::operator=(const KRawBoloPulseRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KBoloPulseRecord::operator=(aRec);
	
	CopyLocalMembers(aRec);
	
	return *this;
}

void KRawBoloPulseRecord::CopyLocalMembers(const KRawBoloPulseRecord &aRec)
{
	//used in the assignment operator method, this copies over the local 
  //data members. It also set sets fBolometerRecord (the TRef pointer) to zero.
  	
  fTrace = aRec.fTrace;   
  fDaqAmplitude = aRec.fDaqAmplitude;   
  fDaqAmplitudeBaseline = aRec.fDaqAmplitudeBaseline;   
  fDaqAmplitudeBaselineNoise = aRec.fDaqAmplitudeBaselineNoise;   
  fPulseRiseTime = aRec.fPulseRiseTime;
  
}


KRawBoloPulseRecord::~KRawBoloPulseRecord(void)
{
  //destructor

}

void KRawBoloPulseRecord::Clear(Option_t* /* opt */)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KRawBoloPulseRecord::InitializeMembers(void)
{
  //init local members.
  
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  
  fTrace.resize(0); //the raw trace.
  fDaqAmplitude = -99; //the pulse amplitude calcuated by the DAQ
  fDaqAmplitudeBaseline = -99; //the baseline amplitude calculated by DAQ
  fDaqAmplitudeBaselineNoise = -99; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  fPulseRiseTime = -99;



  
}

Bool_t KRawBoloPulseRecord::IsSame(const KRawBoloPulseRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KBoloPulseRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
                     //the operator== method uses this functionality.
	}
	
	//HMM...if the following lines of code produce a lot of Not Equal messages to cout, its probably
	//because I'm asking if two floating point values are exactly the same!
	//this is inherently dangerous.  its probably better to specify an allowed 
	//range that's on the order of machine error. 
	

	
  
  if(fTrace != aRec.fTrace){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fTrace Not Equal. " << endl;
		else
			return false;  
	}
  
	
	if(fDaqAmplitude != aRec.fDaqAmplitude){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fDaqAmplitude Not Equal. lhs: " 
			<< fDaqAmplitude << " != rhs " << aRec.fDaqAmplitude << endl;		
		else
			return false;  
	}
	
  if(fDaqAmplitudeBaseline != aRec.fDaqAmplitudeBaseline){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fDaqAmplitudeBaseline Not Equal. lhs: " 
			<< fDaqAmplitudeBaseline << " != rhs " << aRec.fDaqAmplitudeBaseline << endl;		
		else
			return false;  
	}
  
  if(fDaqAmplitudeBaselineNoise != aRec.fDaqAmplitudeBaselineNoise){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fDaqAmplitudeBaselineNoise Not Equal. lhs: " 
			<< fDaqAmplitudeBaselineNoise << " != rhs " << aRec.fDaqAmplitudeBaselineNoise << endl;		
		else
			return false;  
	}
  
  if(fPulseRiseTime != aRec.fPulseRiseTime){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawBoloPulseRecord fPulseRiseTime Not Equal. lhs: " 
      << fPulseRiseTime << " != rhs " << aRec.fPulseRiseTime << endl;   
    else
      return false;  
  }
  
  
	
	return bIsEqual;
}


void KRawBoloPulseRecord::Compact(void)
{
	//make the event class as small as possible. this calls 'Compact' for all member
	//variables that are KDS classes, member variables that can be compacted (such as TBits)
	//and base classes
	
	KBoloPulseRecord::Compact();
  
}

void KRawBoloPulseRecord::SetTrace(UInt_t size, const Short_t* aData)
{
  fTrace.resize(0);
  for(UInt_t i = 0; i < size; i++) fTrace.push_back(aData[i]);
}

void KRawBoloPulseRecord::FillHistogram(TH1 &hist)
{
  //Fills hist with data found from the pulse trace. Then you can more easily plot it.
  //The TH1 must be a TH1I, S, F or D.
  //
  //developers note: This method shouldn't be a part of this class
  //Rather, there should be some sort of data manipulation / data visualization
  //set of classes to perform these types of tasks. Otherwise, data storage object
  //could become flooded with data visualization tools. 
  //I leave this here for now, but heed this warning.
  //
  //Of course, the other option is the leave this entirely up to the user. ROOT already 
  //has the TH1 histograms and its trivial to fill the histogram or a TGraph with 
  //a vector of shorts
  //
  //
  
  //test to make sure hist is either a TH1I, TH1S, TH1F, or TH1D. 
  //isn't there a better way to do this? the problem is that TH2 and TH3 all derive from TH1
  //
  try {
    TH1I testHist = dynamic_cast<TH1I &>(hist);
  }
  catch (bad_cast& a) {
    try {
      TH1S testHist = dynamic_cast<TH1S &>(hist);
    }
    catch (bad_cast& b) {
      try {
        TH1F testHist = dynamic_cast<TH1F &>(hist);
      }
      catch (bad_cast& c) {
        try {
          TH1D testHist = dynamic_cast<TH1D &>(hist);
        }
        catch (bad_cast& e) {
          cout << "KRawBoloPulseRecord::FillHistogram. Only supports TH1I, TH1S, TH1F or TH1D" << endl;
          return;
        }
      }
    }
  }
 
  hist.SetBins((Int_t)fTrace.size(), 0, (Double_t)fTrace.size());
  for(UInt_t i = 0; i < fTrace.size(); i++)  hist.SetBinContent(i+1, fTrace[i]);
}

void KRawBoloPulseRecord::FillGraph(TGraph &graph)
{
  //Fills hist with data found from the pulse trace. Then you can more easily plot it.
  //
  //developers note: This method shouldn't probably be a part of this class
  //Rather, there should be some sort of data manipulation / data visualization
  //set of classes to perform these types of tasks. Otherwise, data storage object
  //could become flooded with data visualization tools. 
  //I leave this here for now, but heed this warning.
  
  graph.Set((Int_t)fTrace.size());
  for(UInt_t i = 0; i < fTrace.size(); i++) graph.SetPoint(i, i, fTrace[i]);
}

TH1D* KRawBoloPulseRecord::GetHist(void)
{
  //create and return a pointer to a TH1D histogram filled with the pulse trace
  //you own the memory, so you MUST delete.

  TH1D *h = new TH1D();
  h->SetTitle(GetChannelName());
  h->SetName(GetChannelName());
  FillHistogram(*h);
  return h;
}

TGraph* KRawBoloPulseRecord::GetGraph()
{
  //create and return a pointer to a TGraph filled with the pulse trace
  //you own the memory, so you MUST delete.
  
  TGraph *g = new TGraph();
  g->SetName(GetChannelName());
  FillGraph(*g);
  return g;
}

