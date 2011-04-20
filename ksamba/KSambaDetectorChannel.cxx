//______________________________________________________________________
//
// KSambaDetectorChannel.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSambaDetectorChannel.h"

ClassImp(KSambaDetectorChannel);

KSambaDetectorChannel::KSambaDetectorChannel(void)
{
  InitializeMembers();
}

KSambaDetectorChannel::~KSambaDetectorChannel(void)
{
  
}

void KSambaDetectorChannel::InitializeMembers(void)
{
  SetName("");
  SetState("");
  SetPosition(0);
  SetMass(0);
  SetMac("");
  SetDiviseurD2(0);
  SetDiviseurD3(0);
  SetPolarCentre(0);
  SetPolarGarde(0);
  SetGainCentre(0);
  SetGainGarde(0);
  SetPolarFet("");
  SetCorrPied(0);
  SetCompModul(0);
  SetCorrTrngl(0);
  SetAmplModul(0);
  SetGainChaleur(0);
}
