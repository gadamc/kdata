//______________________________________________________________________
//
// KSambaReader.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaReader.h"
#include <iostream>
#include "KSambaHeaderReader.h"
#include "KSambaEventReader.h"

ClassImp(KSambaReader);


KSambaReader::KSambaReader(void)
{
  fHeader = new KSambaHeader;
  fEvent = new KSambaEvent;
  InitializeMembers();
}

KSambaReader::KSambaReader(const char* sambaFile)
{
  fHeader = new KSambaHeader;
  fEvent = new KSambaEvent;
  InitializeMembers();
  OpenFile(sambaFile);
  ReadHeader();
}

KSambaReader::~KSambaReader(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");
  CloseFile();
  if(fHeader)
    delete fHeader;
  if(fEvent)
    delete fEvent;
}


Bool_t KSambaReader::OpenFile(const char* sambaFile)
{
  fSambaFile.open(sambaFile,ios::in) ;
  if (!fSambaFile.is_open()) {
    cerr << "KSambaReader::OpenFile. error opening stream from " << sambaFile << endl;
    return false;
  }
  else return true;
}

void KSambaReader::CloseFile(void)
{
  fSambaFile.close();
}

void KSambaReader::Clear(Option_t* /* opt */)
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

void KSambaReader::InitializeMembers(void)
{

  
}

Bool_t KSambaReader::ReadHeader(void)
{
  if(!fSambaFile.is_open()) return false;
  
  KSambaHeaderReader mHeaderReader;
  return mHeaderReader.ReadHeader(fSambaFile, fHeader);
}


Bool_t KSambaReader::ReadNextEvent(void)
{
  if(!fSambaFile.is_open()) return false;
  
  KSambaEventReader mEventReader;
  
  return mEventReader.ReadEvent(fSambaFile, fEvent);
}


