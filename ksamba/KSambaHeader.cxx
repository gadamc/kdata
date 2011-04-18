//______________________________________________________________________
//
// KSambaHeader.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaHeader.h"

ClassImp(KSambaHeader);

KSambaHeader::KSambaHeader(void)
{
  InitializeMembers();
}

KSambaHeader::~KSambaHeader(void)
{
  for (vector<KSambaDetector*>::iterator it = fDetectors.begin();
       it != fDetectors.end(); it++)
  {
    delete (*it);
  }
  fDetectors.clear();
}

void KSambaHeader::InitializeMembers(void)
{
  fEndian = true; //equals true if big, false if little
  fRunName = "";
  fVersion = "0";
  fRelease = "0";
  fEchantillonage = 0;
  fNumBolos = 0;
  fNumChannels = 0;
  fDate = "";
  fIntitule = "";
}

KSambaDetector* KSambaHeader::GetDetectorFromList(const char* detname)
{
  for (vector<KSambaDetector*>::iterator it = fDetectors.begin();
       it != fDetectors.end(); it++)
  {
    if ( strcmp((*it)->GetName(), detname) == 0) {
      return *it;
    };
  }  
  return NULL;
}

KSambaDetector* KSambaHeader::GetDetectorFromList(UInt_t i)
{
  if (i < fDetectors.size())
    return fDetectors.at(i);
  else return NULL;
}

Bool_t KSambaHeader::IsInDetectorList(const char *detname)
{
  
  return (GetDetectorFromList(detname) != NULL) ? true : false;
}

KSambaDetector* KSambaHeader::AddDetector(void)
{
  KSambaDetector *det = new KSambaDetector;
  fDetectors.push_back(det);
  return det;
};

