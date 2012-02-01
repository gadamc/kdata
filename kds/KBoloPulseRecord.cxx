//_____________________________________________
//
// KBoloPulseRecord.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for all Pulse Sub Records. This class holds the channel number (1 or 2)
// and the pulse type information (heat, collectrode, veto, guard). Typically,
// one will work with a KHLABoloPulseRecord or KRawBoloPulseRecord, which are
// derived from this class.
// 
// So you don't have to memorize the numerical value that corresponds
// to each pulse type, the methods GetHeatType(), GetCollectrodeType(),
// GetVetoType() and GetGuardType() are available to return the numeric value
// stored in a local enum structure. 
// however, for your information, the structure is
//
// enum kPulseChannelType {
//   kCollectrodeType = 1,
//   kVetoType = 2,
//   kGuardType = 3,
//   kHeatType = 4
//  };
//
// The channel number should be either 1 or 2, corresponding to both channels
// in the bolometer data. Note, there is no channel 0. In principle, there could
// be 3 or more channels of a particular type as well (in the case of a future
// bolometer design?). 

#include "KBoloPulseRecord.h"
#include <iostream>
#include <cstring>

ClassImp(KBoloPulseRecord);

KBoloPulseRecord::KBoloPulseRecord(void)
{	
  //standard constructors 

  InitializeMembers();

  //we must take care of members on the heap differently
  //than members created on the stack.

}

KBoloPulseRecord::KBoloPulseRecord(const KBoloPulseRecord &aRec)
  : KSubRecord(aRec)
{
  //copy constructor 

  CopyLocalMembers(aRec);

}

KBoloPulseRecord& KBoloPulseRecord::operator=(const KBoloPulseRecord &aRec)
{
  //assignment operator

  if(&aRec == this) return *this;

  this->KSubRecord::operator=(aRec);
  CopyLocalMembers(aRec);

  return *this;
}

void KBoloPulseRecord::CopyLocalMembers(const KBoloPulseRecord &aRec)
{
  //used by the assignment operator to copy local members (fPulseType and
  //fChanneNumber)
  fChannelName = aRec.fChannelName;  
  fPositiveTriggerAmp = aRec.fPositiveTriggerAmp;
  fNegativeTriggerAmp = aRec.fNegativeTriggerAmp;
  fState = aRec.fState; 
  fPolarity = aRec.fPolarity;
  fGain = aRec.fGain;

  fBoloBoxVersion = aRec.fBoloBoxVersion;
  fConvergencePeriod = aRec.fConvergencePeriod;
  fRelay1Status = aRec.fRelay1Status;
  fRelay2Status = aRec.fRelay2Status;
  fFetDac = aRec.fFetDac;
  fBoloGain = aRec.fBoloGain;
}

KBoloPulseRecord::~KBoloPulseRecord(void)
{
  //destructor

  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KBoloPulseRecord::Clear(Option_t *anopt)
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

void KBoloPulseRecord::InitializeMembers(void)
{
  //init local members to default values (-99)

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
  fChannelName.resize(0);  //the name of the channel. for example: "chaleur ID4"
  fPositiveTriggerAmp = -99999;
  fNegativeTriggerAmp = -99999;
  fState.resize(0);
  fPolarity = -99999;
  fGain = -999999;
  
  fBoloBoxVersion = 1.0;  //assume its bbv1 unless otherwise informed (because there is no such information in the samba header)
  fConvergencePeriod = -99999;
  fRelay1Status.resize(0);
  fRelay2Status.resize(0);
  fFetDac = -99999;
  fBoloGain = -99999;
}


Bool_t KBoloPulseRecord::IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint) const
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

  if(fChannelName != aRec.fChannelName){
    bIsEqual = false;
    if (bPrint) 
      cout << "KRawBoloPulseRecord fChannelName Not Equal. lhs: " 
      << fChannelName << " != rhs " << aRec.fChannelName << endl;		
    else
      return false;  
  }

  if(fGain != aRec.fGain){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fGain Not Equal. lhs: " 
       << fGain << " != rhs " << aRec.fGain << endl;		
     else
       return false;  
   }
   
  if(fPolarity != aRec.fPolarity){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fPolarity Not Equal. lhs: " 
       << fPolarity << " != rhs " << aRec.fPolarity << endl;		
     else
       return false;  
   }
   
  if(fState != aRec.fState){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fState Not Equal. lhs: " 
       << fState << " != rhs " << aRec.fState << endl;		
     else
       return false;  
   }
  
  if(fNegativeTriggerAmp != aRec.fNegativeTriggerAmp){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fNegativeTriggerAmp Not Equal. lhs: " 
       << fNegativeTriggerAmp << " != rhs " << aRec.fNegativeTriggerAmp << endl;		
     else
       return false;  
   }
  
  if(fPositiveTriggerAmp != aRec.fPositiveTriggerAmp){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fPositiveTriggerAmp Not Equal. lhs: " 
       << fPositiveTriggerAmp << " != rhs " << aRec.fPositiveTriggerAmp << endl;		
     else
       return false;  
   }
   
   
  if(fBoloBoxVersion != aRec.fBoloBoxVersion){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fBoloBoxVersion Not Equal. lhs: " 
       << fBoloBoxVersion << " != rhs " << aRec.fBoloBoxVersion << endl;		
     else
       return false;  
   }

   if(fConvergencePeriod != aRec.fConvergencePeriod){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fConvergencePeriod Not Equal. lhs: " 
       << fConvergencePeriod << " != rhs " << aRec.fConvergencePeriod << endl;		
     else
       return false;  
   }

   if(fRelay1Status != aRec.fRelay1Status){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fRelay1Status Not Equal. lhs: " 
       << fRelay1Status << " != rhs " << aRec.fRelay1Status << endl;		
     else
       return false;  
   }

   if(fRelay2Status != aRec.fRelay2Status){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fRelay2Status Not Equal. lhs: " 
       << fRelay2Status << " != rhs " << aRec.fRelay2Status << endl;		
     else
       return false;  
   }

   if(fFetDac != aRec.fFetDac){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fFetDac Not Equal. lhs: " 
       << fFetDac << " != rhs " << aRec.fFetDac << endl;		
     else
       return false;  
   }
   
   if(fBoloGain != aRec.fBoloGain){
      bIsEqual = false;
      if (bPrint) 
        cout << "KBoloPulseRecord fBoloGain Not Equal. lhs: " 
        << fBoloGain << " != rhs " << aRec.fBoloGain << endl;		
      else
        return false;  
    }
   
  return bIsEqual;
}


void KBoloPulseRecord::Compact(void)
{
  //make the event class as small as possible. this calls 'Compact' for all member
  //variables that are KDS classes, member variables that can be compacted (such as TBits)
  //and base classes

  KSubRecord::Compact();
}
