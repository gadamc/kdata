//______________________________________________________________________
//
// KSambaDetector.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaDetector.h"

ClassImp(KSambaDetector);

KSambaDetector::KSambaDetector(void)
{
  InitializeMembers();
}

KSambaDetector::~KSambaDetector(void)
{
  for (vector<KSambaDetectorChannel*>::iterator it = fChannels.begin();
       it != fChannels.end(); it++)
  {
    delete (*it);
  }
  fChannels.clear();
}

void KSambaDetector::InitializeMembers(void)
{
  SetName("");
  for (vector<KSambaDetectorChannel*>::iterator it = fChannels.begin();
       it != fChannels.end(); it++)
  {
    delete (*it);
  }
  fChannels.clear();
}

KSambaDetectorChannel* KSambaDetector::AddChannel(void)
{
  KSambaDetectorChannel *chan = new KSambaDetectorChannel;
  fChannels.push_back(chan);
  return chan;
};

KSambaDetectorChannel* KSambaDetector::GetChannelFromList(const char* channame)
{
  for (vector<KSambaDetectorChannel*>::iterator it = fChannels.begin();
       it != fChannels.end(); it++)
  {
    if ( strcmp((*it)->GetName(), channame) == 0) {
      return *it;
    };
  }  
  return NULL;
}

Bool_t KSambaDetector::IsChannelInList(const char *channame)
{
  
  return (GetChannelFromList(channame) != NULL) ? true : false;
}

KSambaDetectorChannel* KSambaDetector::GetChannelFromList(UInt_t i)
{
  if (i < fChannels.size())
    return fChannels.at(i);
  else return NULL;
}
