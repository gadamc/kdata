/**
   @file    EdwAutomateData.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwAutomateData class. See the .h file
*/

#include "EdwAutomateData.h"

ClassImp(EdwAutomateData); /**< Root dictionnary */

EdwAutomateData::EdwAutomateData() : TObject() {
}

EdwAutomateData::~EdwAutomateData() {
}

void EdwAutomateData::Clear() {
  fDate = 0;
  fTBolo = 0;
  fBaSource1 = 0;
  fBaSource2 = 0;
  fCoSource1 = 0;
  fCoSource2 = 0;

}
