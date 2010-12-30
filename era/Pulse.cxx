/**
   @file    Pulse.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the Pulse class. See the .h file
*/

#include "Pulse.h"

ClassImp(Pulse); /**< Root dictionnary */

Pulse::Pulse() : TObject() {
  fChannel="channel-name";
  fSampling_ns=0;
  fTrace.clear();
  fIsHeat = 0;
}

Pulse::~Pulse() {
  fTrace.clear();
}

void Pulse::SetTrace(UShort_t n, const Short_t* aData) {
  for (unsigned int i=0; i<n; i++) fTrace.push_back(aData[i]);
}

void Pulse::SetChannel(string aChannel) {
  fChannel = aChannel;
  fIsHeat = isheat(aChannel);
}

Bool_t Pulse::IsSaturated() {
  Bool_t lIsSat = 0;
  for (UInt_t i=0; i<fTrace.size(); i++) {
    if (fTrace[i] >= SHRT_MAX || fTrace[i] <= SHRT_MIN) lIsSat = 1;
  }
  return lIsSat;
}

void Pulse::PlotRawTrace(string aPlotName) {
  plotvector(fTrace,aPlotName,0,"Raw trace");
}
