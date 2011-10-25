//_____________________________________________
//
// KPulseAnalysisRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// 

#include "KPulseAnalysisRecord.h"
#include <iostream>
#include <cstring>

ClassImp(KPulseAnalysisRecord);

KPulseAnalysisRecord::KPulseAnalysisRecord(void)
{	
  //standard constructors 

  InitializeMembers();

  //we must take care of members on the heap differently
  //than members created on the stack.

}

KPulseAnalysisRecord::KPulseAnalysisRecord(const KPulseAnalysisRecord &aRec)
  : KSubRecord(aRec)
{
  //copy constructor 

  CopyLocalMembers(aRec);

}

KPulseAnalysisRecord& KPulseAnalysisRecord::operator=(const KPulseAnalysisRecord &aRec)
{
  //assignment operator

  if(&aRec == this) return *this;

  this->KSubRecord::operator=(aRec);
  CopyLocalMembers(aRec);

  return *this;
}

void KPulseAnalysisRecord::CopyLocalMembers(const KPulseAnalysisRecord &aRec)
{
  //used by the assignment operator to copy local members (fPulseType and
  //fChanneNumber)
  fAmp = aRec.fAmp;
  fName = aRec.fName;
  fPeakPosition = aRec.fPeakPosition; 
  fIsBaseline = aRec.fIsBaseline;
  fUnit = aRec.fUnit;
  fChiSq = aRec.fChiSq;
  fBaselineAmplitudeWidth = aRec.fBaselineAmplitudeWidth;
  fRiseTime = aRec.fRiseTime;
  fPulseWidth = aRec.fPulseWidth;
  fBaselineRemoved = aRec.fBaselineRemoved;
  fSlopeRemoved = aRec.fSlopeRemoved;
  memcpy(fExtra, aRec.fExtra, KPULSEANARECORD_EXTRA_SIZE * sizeof(Double32_t));

}

KPulseAnalysisRecord::~KPulseAnalysisRecord(void)
{
  //destructor

  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KPulseAnalysisRecord::Clear(Option_t *anopt)
{

  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
  KSubRecord::Clear(anopt);

  //Clear and delete local objects here. 


  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KPulseAnalysisRecord::InitializeMembers(void)
{
  //init local members to default values (-99)

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

  fAmp = -9999;
  fName = "";  
  fPeakPosition = -1;
  fIsBaseline = 0;  
  fUnit = -1;  
  fChiSq = -1;
  fBaselineAmplitudeWidth = -1;
  fRiseTime = -1;
  fPulseWidth = -1;
  fBaselineRemoved = 0;
  fSlopeRemoved = 0;
  memset(fExtra, 0, KPULSEANARECORD_EXTRA_SIZE * sizeof(Double32_t));
}


Bool_t KPulseAnalysisRecord::IsSame(const KPulseAnalysisRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.

  Bool_t bIsEqual = true; //assume its true, then test for differences

  //call the base class's IsSame methods
  if(!this->KSubRecord::IsSame(aRec,bPrint)){
    bIsEqual = false;
    if(!bPrint)
      return false;  //if we're not printing out, just return false at first failure
    //the operator== method uses this functionality.
  }

  if(fAmp != aRec.fAmp){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fAmp Not Equal." 
      << fAmp << " != rhs " << aRec.fAmp << endl;		
    else
      return false;  
  }
  if(fName != aRec.fName){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fName Not Equal. " 
      << fName << " != rhs " << aRec.fName << endl;		
    else
      return false;  
  }
  if(fPeakPosition != aRec.fPeakPosition){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fPeakPosition Not Equal. " 
      << fPeakPosition << " != rhs " << aRec.fPeakPosition << endl;		
    else
      return false;  
  }
  if(fIsBaseline != aRec.fIsBaseline){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fIsBaseline Not Equal. " 
      << fIsBaseline << " != rhs " << aRec.fIsBaseline << endl;		
    else
      return false;  
  }
  if(fUnit != aRec.fUnit){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fUnit Not Equal." 
      << fUnit << " != rhs " << aRec.fUnit << endl;		
    else
      return false;  
  }
  if(fChiSq != aRec.fChiSq){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fChiSq Not Equal." 
      << fChiSq << " != rhs " << aRec.fChiSq << endl;		
    else
      return false;  
  }

  if(fBaselineAmplitudeWidth != aRec.fBaselineAmplitudeWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fBaselineAmplitudeWidth Not Equal." 
      << fBaselineAmplitudeWidth << " != rhs " << aRec.fBaselineAmplitudeWidth << endl;		
    else
      return false;  
  }

  if(fRiseTime != aRec.fRiseTime){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fRiseTime Not Equal." 
      << fRiseTime << " != rhs " << aRec.fRiseTime << endl;		
    else
      return false;  
  }
  
  if(fPulseWidth != aRec.fPulseWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fPulseWidth Not Equal." 
      << fPulseWidth << " != rhs " << aRec.fPulseWidth << endl;		
    else
      return false;  
  }
  
  if(fBaselineRemoved != aRec.fBaselineRemoved){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fBaselineRemoved Not Equal." 
      << fBaselineRemoved << " != rhs " << aRec.fBaselineRemoved << endl;		
    else
      return false;  
  }
  
  if(fSlopeRemoved != aRec.fSlopeRemoved){
    bIsEqual = false;
    if (bPrint) 
      cout << "KPulseAnalysisRecord fSlopeRemoved Not Equal." 
      << fSlopeRemoved << " != rhs " << aRec.fSlopeRemoved << endl;		
    else
      return false;  
  }
  for(int xtr = 0; xtr < KPULSEANARECORD_EXTRA_SIZE; xtr++){
    if(fExtra[xtr] != aRec.fExtra[xtr]){
      bIsEqual = false;
      if (bPrint) 
        cout << "KPulseAnalysisRecord fExtra["<< xtr << "] Not Equal." 
        << fExtra[xtr] << " != rhs " << aRec.fExtra[xtr] << endl;		
      else
        return false;  
    }
  }
  
  return bIsEqual;
}


void KPulseAnalysisRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes

  KSubRecord::Compact();
}

void KPulseAnalysisRecord::SetExtra(Double32_t aVal, UInt_t index)
{
  if (index < KPULSEANARECORD_EXTRA_SIZE)
    fExtra[index] = aVal;
}

Double32_t KPulseAnalysisRecord::GetExtra(UInt_t index) const
{
  if (index < KPULSEANARECORD_EXTRA_SIZE)
    return fExtra[index];
    
  else return -99999.;
}




