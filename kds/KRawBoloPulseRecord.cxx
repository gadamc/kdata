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
  
	//fBolometerRecordNum = aRec.fBolometerRecordNum;  have to set this value manually
	fBolometerRecord = 0;
	
  fChannelName = aRec.fChannelName;  
  fPulseTimeWidth = aRec.fPulseTimeWidth;  
  fTrace = aRec.fTrace;   
  fAmplitude = aRec.fAmplitude;   
  fAmplitudeBaseline = aRec.fAmplitudeBaseline;   
  fAmplitudeBaselineNoise = aRec.fAmplitudeBaselineNoise;   
  fPretriggerSize = aRec.fPretriggerSize;
  fPulseRiseTime = aRec.fPulseRiseTime;
  fFilterSize = aRec.fFilterSize;
  fPulseLength = aRec.fPulseLength;
  fHeatPulseStampWidth = aRec.fHeatPulseStampWidth;
  fCryoPosition = aRec.fCryoPosition;
  fPolarFet = aRec.fPolarFet;
  fCorrPied = aRec.fCorrPied;
  fCompModul = aRec.fCompModul;
  fCorrTrngl = aRec.fCorrTrngl;
  fAmplModul = aRec.fAmplModul;
  fIsHeatPulse = aRec.fIsHeatPulse;
  
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
  
  fChannelName.resize(0);  //the name of the channel. for example: "chaleur ID4"
  fPulseTimeWidth = 10; //returns number of ns for each point.  = 1 / f
  fTrace.resize(0); //the raw trace.
  fAmplitude = -99; //the pulse amplitude calcuated by the DAQ
  fAmplitudeBaseline = -99; //the baseline amplitude calculated by DAQ
  fAmplitudeBaselineNoise = -99; //the width of the distribution of the baseline amplitude for this run period, calculated by DAQ
  fPretriggerSize = -99;
  fPulseRiseTime = -99;
  fFilterSize = -99;
  fPulseLength = 0;
  fHeatPulseStampWidth = -99;
  fCryoPosition = -99;
  fPolarFet.resize(0);
  fCorrPied = -99;
  fCompModul = -99;
  fCorrTrngl = -99;
  fAmplModul = -99;
  fIsHeatPulse = -99;
  
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
	
	if(fChannelName != aRec.fChannelName){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fChannelName Not Equal. lhs: " 
			<< fChannelName << " != rhs " << aRec.fChannelName << endl;		
		else
			return false;  
	}
	
	if(fPulseTimeWidth != aRec.fPulseTimeWidth){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fPulseTimeWidth Not Equal. lhs: " 
			<< fPulseTimeWidth << " != rhs " << aRec.fPulseTimeWidth << endl;		
		else
			return false;  
	}
  
  if(fTrace != aRec.fTrace){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fTrace Not Equal. " << endl;
		else
			return false;  
	}
  
	
	if(fAmplitude != aRec.fAmplitude){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitude Not Equal. lhs: " 
			<< fAmplitude << " != rhs " << aRec.fAmplitude << endl;		
		else
			return false;  
	}
	
  if(fAmplitudeBaseline != aRec.fAmplitudeBaseline){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitudeBaseline Not Equal. lhs: " 
			<< fAmplitudeBaseline << " != rhs " << aRec.fAmplitudeBaseline << endl;		
		else
			return false;  
	}
  
  if(fAmplitudeBaselineNoise != aRec.fAmplitudeBaselineNoise){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplitudeBaselineNoise Not Equal. lhs: " 
			<< fAmplitudeBaselineNoise << " != rhs " << aRec.fAmplitudeBaselineNoise << endl;		
		else
			return false;  
	}
  
  if(fPretriggerSize != aRec.fPretriggerSize){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fPretriggerSize Not Equal. lhs: " 
			<< fPretriggerSize << " != rhs " << aRec.fPretriggerSize << endl;		
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

  if(fFilterSize != aRec.fFilterSize){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fFilterSize Not Equal. lhs: " 
			<< fFilterSize << " != rhs " << aRec.fFilterSize << endl;		
		else
			return false;  
	}

  if(fPulseLength != aRec.fPulseLength){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fPulseLength Not Equal. lhs: " 
			<< fPulseLength << " != rhs " << aRec.fPulseLength << endl;		
		else
			return false;  
	}

  if(fHeatPulseStampWidth != aRec.fHeatPulseStampWidth){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fHeatPulseStampWidth Not Equal. lhs: " 
			<< fHeatPulseStampWidth << " != rhs " << aRec.fHeatPulseStampWidth << endl;		
		else
			return false;  
	}

  if(fCryoPosition != aRec.fCryoPosition){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fCryoPosition Not Equal. lhs: " 
			<< fCryoPosition << " != rhs " << aRec.fCryoPosition << endl;		
		else
			return false;  
	}

  if(fPolarFet != aRec.fPolarFet){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fPolarFet Not Equal. lhs: " 
			<< fPolarFet << " != rhs " << aRec.fPolarFet << endl;		
		else
			return false;  
	}

  if(fCorrPied != aRec.fCorrPied){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fCorrPied Not Equal. lhs: " 
			<< fCorrPied << " != rhs " << aRec.fCorrPied << endl;		
		else
			return false;  
	}

  if(fCompModul != aRec.fCompModul){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fCompModul Not Equal. lhs: " 
			<< fCompModul << " != rhs " << aRec.fCompModul << endl;		
		else
			return false;  
	}

  if(fCorrTrngl != aRec.fCorrTrngl){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fCorrTrngl Not Equal. lhs: " 
			<< fCorrTrngl << " != rhs " << aRec.fCorrTrngl << endl;		
		else
			return false;  
	}

  if(fAmplModul != aRec.fAmplModul){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fAmplModul Not Equal. lhs: " 
			<< fAmplModul << " != rhs " << aRec.fAmplModul << endl;		
		else
			return false;  
	}

  if(fIsHeatPulse != aRec.fIsHeatPulse){
		bIsEqual = false;
		if (bPrint) 
			cout << "KRawBoloPulseRecord fIsHeatPulse Not Equal. lhs: " 
			<< fIsHeatPulse << " != rhs " << aRec.fIsHeatPulse << endl;		
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


