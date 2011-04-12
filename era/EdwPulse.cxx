/**
   @file    EdwPulse.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwPulse class. See the .h file
*/

#include "EdwPulse.h"

ClassImp(EdwPulse); /**< Root dictionnary */

EdwPulse::EdwPulse() : Pulse() {
}

EdwPulse::~EdwPulse() {
  this->ClearTrace();
}
