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
  :TObject()
{
  //copy constructor 

  CopyLocalMembers(aRec);

}

KBoloPulseRecord& KBoloPulseRecord::operator=(const KBoloPulseRecord &aRec)
{
  //assignment operator

  if(&aRec == this) return *this;

  CopyLocalMembers(aRec);

  return *this;
}

void KBoloPulseRecord::CopyLocalMembers(const KBoloPulseRecord &aRec)
{
  //used by the assignment operator to copy local members (fPulseType and
  //fChanneNumber)

  fBolometerRecord = 0;
  fChannelName = aRec.fChannelName;  
  fPositiveTriggerAmp = aRec.fPositiveTriggerAmp;
  fNegativeTriggerAmp = aRec.fNegativeTriggerAmp;
  fState = aRec.fState; 
  fVoltage = aRec.fVoltage;
  fGain = aRec.fGain;

  fBoloBoxVersion = aRec.fBoloBoxVersion;
  fConvergencePeriod = aRec.fConvergencePeriod;
  fRelay1Status = aRec.fRelay1Status;
  fRelay2Status = aRec.fRelay2Status;
  fFetDac = aRec.fFetDac;
  fBoloGain = aRec.fBoloGain;

  fPulseTimeWidth = aRec.fPulseTimeWidth;  

  fPretriggerSize = aRec.fPretriggerSize;
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

KBoloPulseRecord::~KBoloPulseRecord(void)
{
  //destructor

  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KBoloPulseRecord::Clear(Option_t * /*anopt*/)
{

  //Clear the base classes and then clear/delete any local
  //members. 

  //Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

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
  fVoltage = -99999;
  fGain = -999999;
  
  fBoloBoxVersion = 1.0;  //assume its bbv1 unless otherwise informed (because there is no such information in the samba header)
  fConvergencePeriod = -99999;
  fRelay1Status.resize(0);
  fRelay2Status.resize(0);
  fFetDac = -99999;
  fBoloGain = -99999;

  fPulseTimeWidth = -99; 

  fPretriggerSize = 0;
  fFilterSize = 0;
  fPulseLength = 0;
  fHeatPulseStampWidth = -99;
  fCryoPosition = -99;
  fPolarFet.resize(0);
  fCorrPied = -99;
  fCompModul = -99;
  fCorrTrngl = -99;
  fAmplModul = -99;
  fIsHeatPulse = 0;
}


Bool_t KBoloPulseRecord::IsSame(const KBoloPulseRecord &aRec, Bool_t bPrint) const
{
  //Compares two objects and their member variables to test for equality.
  //If bPrint is set to true, then a message for each member variable that is different
  //will print to standard out. Otherwise, this method will return false and quit
  //checking member variables as soon as it finds a unequal data member.

  Bool_t bIsEqual = true; //assume its true, then test for differences

  
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
   
  if(fVoltage != aRec.fVoltage){
     bIsEqual = false;
     if (bPrint) 
       cout << "KBoloPulseRecord fVoltage Not Equal. lhs: " 
       << fVoltage << " != rhs " << aRec.fVoltage << endl;    
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
   
   if(fPulseTimeWidth != aRec.fPulseTimeWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fPulseTimeWidth Not Equal. lhs: " 
      << fPulseTimeWidth << " != rhs " << aRec.fPulseTimeWidth << endl;   
    else
      return false;  
  }

  if(fPretriggerSize != aRec.fPretriggerSize){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fPretriggerSize Not Equal. lhs: " 
      << fPretriggerSize << " != rhs " << aRec.fPretriggerSize << endl;   
    else
      return false;  
  }

  

  if(fFilterSize != aRec.fFilterSize){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fFilterSize Not Equal. lhs: " 
      << fFilterSize << " != rhs " << aRec.fFilterSize << endl;   
    else
      return false;  
  }

  if(fPulseLength != aRec.fPulseLength){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fPulseLength Not Equal. lhs: " 
      << fPulseLength << " != rhs " << aRec.fPulseLength << endl;   
    else
      return false;  
  }

  if(fHeatPulseStampWidth != aRec.fHeatPulseStampWidth){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fHeatPulseStampWidth Not Equal. lhs: " 
      << fHeatPulseStampWidth << " != rhs " << aRec.fHeatPulseStampWidth << endl;   
    else
      return false;  
  }

  if(fCryoPosition != aRec.fCryoPosition){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fCryoPosition Not Equal. lhs: " 
      << fCryoPosition << " != rhs " << aRec.fCryoPosition << endl;   
    else
      return false;  
  }

  if(fPolarFet != aRec.fPolarFet){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fPolarFet Not Equal. lhs: " 
      << fPolarFet << " != rhs " << aRec.fPolarFet << endl;   
    else
      return false;  
  }

  if(fCorrPied != aRec.fCorrPied){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fCorrPied Not Equal. lhs: " 
      << fCorrPied << " != rhs " << aRec.fCorrPied << endl;   
    else
      return false;  
  }

  if(fCompModul != aRec.fCompModul){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fCompModul Not Equal. lhs: " 
      << fCompModul << " != rhs " << aRec.fCompModul << endl;   
    else
      return false;  
  }

  if(fCorrTrngl != aRec.fCorrTrngl){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fCorrTrngl Not Equal. lhs: " 
      << fCorrTrngl << " != rhs " << aRec.fCorrTrngl << endl;   
    else
      return false;  
  }

  if(fAmplModul != aRec.fAmplModul){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fAmplModul Not Equal. lhs: " 
      << fAmplModul << " != rhs " << aRec.fAmplModul << endl;   
    else
      return false;  
  }

  if(fIsHeatPulse != aRec.fIsHeatPulse){
    bIsEqual = false;
    if (bPrint) 
      cout << "KBoloPulseRecord fIsHeatPulse Not Equal. lhs: " 
      << fIsHeatPulse << " != rhs " << aRec.fIsHeatPulse << endl;   
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

}
