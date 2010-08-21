/**
   @file    EdwEventHeader.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwEventHeader class. See the .h file
*/

#include "EdwEventHeader.h"

ClassImp(EdwEventHeader); /**< Root dictionnary */

EdwEventHeader::EdwEventHeader() : TObject() {
  fRun="run-name";
  fNum = 0 ;
  this->SetDateSec(0);
  this->SetDateMuSec(0);
  fSambaDelay = 0;
}

void EdwEventHeader::Clear() {
  fRun="run-name";
  fNum = 0 ;
  this->SetDateSec(0);
  this->SetDateMuSec(0);
  fSambaDelay = 0;
}

UInt_t EdwEventHeader::TriggerBit(Int_t num) const {
  UInt_t bit = 0;
  if (num == 1) bit = fTriggerBit1;
  else if (num == 2) bit = fTriggerBit2;
  else cerr << "TriggerBit: wrong bit trigger asked"<<endl;
  return bit;
}

void EdwEventHeader::SetTriggerBit(UInt_t aNum, UInt_t aBit) {
  if (aNum == 1) fTriggerBit1 = aBit;
  else if (aNum == 2) fTriggerBit2 = aBit;
  else cerr <<"TriggerBit: wrong asked" << endl;
}
