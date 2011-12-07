//______________________________________________________________________
//
// KAmpBoloPulseRecord.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Holds the Raw-level pulse information, including the pulse trace
// This class is based on KBoloPulseRecord (which stores the channel
// number and type). 
//


#include "KAmpBoloPulseRecord.h"
#include "KRawSambaRecord.h"
#include "KPulseAnalysisRecord.h"
#include "TH1.h"
#include "TGraph.h"
#include <iostream>
#include <typeinfo>
#include <cstring>

using namespace std;

ClassImp(KAmpBoloPulseRecord);

KAmpBoloPulseRecord::KAmpBoloPulseRecord(void)
{
  //standard constructor

  InitializeMembers();
}


KAmpBoloPulseRecord::KAmpBoloPulseRecord(const KAmpBoloPulseRecord &aRec)
  : KBoloPulseRecord(aRec)
{
  //copy constructor 

  CopyLocalMembers(aRec);

}

KAmpBoloPulseRecord& KAmpBoloPulseRecord::operator=(const KAmpBoloPulseRecord &aRec)
{
  if(&aRec == this) return *this;

  this->KBoloPulseRecord::operator=(aRec);

  CopyLocalMembers(aRec);

  return *this;
}

void KAmpBoloPulseRecord::CopyLocalMembers(const KAmpBoloPulseRecord &aRec)
{
  //used in the assignment operator method, this copies over the local 
  //data members. It also set sets fBolometerRecord (the TRef pointer) to zero.

  //fBolometerRecordNum = aRec.fBolometerRecordNum;  have to set this value manually
  fBolometerRecord = 0;

  fChannelName = aRec.fChannelName;  
  fPulseTimeWidth = aRec.fPulseTimeWidth;  
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

  fPulseAnaRecords.Delete();


}


KAmpBoloPulseRecord::~KAmpBoloPulseRecord(void)
{
  //destructor

}

void KAmpBoloPulseRecord::Clear(Option_t* /* opt */)
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

void KAmpBoloPulseRecord::InitializeMembers(void)
{
  //init local members.

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

  fChannelName.resize(0);  //the name of the channel. for example: "chaleur ID4"
  fPulseTimeWidth = 10; //returns number of ns for each point.  = 1 / f
  fPretriggerSize = -99;
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
  fBolometerRecord = 0;
  fPulseAnaRecords.Delete();
}

Bool_t KAmpBoloPulseRecord::IsSame(const KAmpBoloPulseRecord &aRec, Bool_t bPrint) const
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
      cout << "KAmpBoloPulseRecord fChannelName Not Equal. lhs: " 
      << fChannelName << " != rhs " << aRec.fChannelName << endl;		
    else
      return false;  
  }

  if(fPulseTimeWidth != aRec.fPulseTimeWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fPulseTimeWidth Not Equal. lhs: " 
      << fPulseTimeWidth << " != rhs " << aRec.fPulseTimeWidth << endl;		
    else
      return false;  
  }

  if(fPretriggerSize != aRec.fPretriggerSize){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fPretriggerSize Not Equal. lhs: " 
      << fPretriggerSize << " != rhs " << aRec.fPretriggerSize << endl;		
    else
      return false;  
  }

  if(fFilterSize != aRec.fFilterSize){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fFilterSize Not Equal. lhs: " 
      << fFilterSize << " != rhs " << aRec.fFilterSize << endl;		
    else
      return false;  
  }

  if(fPulseLength != aRec.fPulseLength){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fPulseLength Not Equal. lhs: " 
      << fPulseLength << " != rhs " << aRec.fPulseLength << endl;		
    else
      return false;  
  }

  if(fHeatPulseStampWidth != aRec.fHeatPulseStampWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fHeatPulseStampWidth Not Equal. lhs: " 
      << fHeatPulseStampWidth << " != rhs " << aRec.fHeatPulseStampWidth << endl;		
    else
      return false;  
  }

  if(fCryoPosition != aRec.fCryoPosition){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fCryoPosition Not Equal. lhs: " 
      << fCryoPosition << " != rhs " << aRec.fCryoPosition << endl;		
    else
      return false;  
  }

  if(fPolarFet != aRec.fPolarFet){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fPolarFet Not Equal. lhs: " 
      << fPolarFet << " != rhs " << aRec.fPolarFet << endl;		
    else
      return false;  
  }

  if(fCorrPied != aRec.fCorrPied){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fCorrPied Not Equal. lhs: " 
      << fCorrPied << " != rhs " << aRec.fCorrPied << endl;		
    else
      return false;  
  }

  if(fCompModul != aRec.fCompModul){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fCompModul Not Equal. lhs: " 
      << fCompModul << " != rhs " << aRec.fCompModul << endl;		
    else
      return false;  
  }

  if(fCorrTrngl != aRec.fCorrTrngl){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fCorrTrngl Not Equal. lhs: " 
      << fCorrTrngl << " != rhs " << aRec.fCorrTrngl << endl;		
    else
      return false;  
  }

  if(fAmplModul != aRec.fAmplModul){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fAmplModul Not Equal. lhs: " 
      << fAmplModul << " != rhs " << aRec.fAmplModul << endl;		
    else
      return false;  
  }

  if(fIsHeatPulse != aRec.fIsHeatPulse){
    bIsEqual = false;
    if (bPrint) 
      cout << "KAmpBoloPulseRecord fIsHeatPulse Not Equal. lhs: " 
      << fIsHeatPulse << " != rhs " << aRec.fIsHeatPulse << endl;		
    else
      return false;  
  }


  return bIsEqual;
}

void KAmpBoloPulseRecord::AddPulseAnalysisRecord(KPulseAnalysisRecord* aPulseRecord)
{
  //add a pulse analysis record to this object

  //fPulseAnaRecords.Add((TObject *)aPulseRecord);
  fPulseAnaRecords.Add((TObject *)aPulseRecord);
}

KPulseAnalysisRecord* KAmpBoloPulseRecord::GetPulseAnalysisRecord(Int_t i) const
{
  //this returns the i^th pulse analysis record for this particular bolometer record.
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
  return (KPulseAnalysisRecord*) fPulseAnaRecords.At(i);
}

KPulseAnalysisRecord* KAmpBoloPulseRecord::GetPulseAnalysisRecord(const char* name) const
{
  //this returns the pulse analysis record with a particular name
  //regardless of channel number, or pulse record type (collectrode, veto, guard, heat)
  //
  for (int i = 0; i < GetNumPulseAnalysisRecords(); i++){
    KPulseAnalysisRecord* r = GetPulseAnalysisRecord(i);
    if (strcmp(r->GetName(), name) == 0)
      return r;
  }
  return 0;
}


void KAmpBoloPulseRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes

  KBoloPulseRecord::Compact();
  fPulseAnaRecords.Compress();
}
