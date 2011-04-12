//______________________________________________________________________
//
// KSambaEventReader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAEVENTREADER_H__
#define __KSAMBAEVENTREADER_H__

#include <fstream>
#include "Rtypes.h"

using namespace std;

class KSambaEvent;

class KSambaEventReader  { 
  
public:
  //Constructors
  KSambaEventReader(void);
  virtual ~KSambaEventReader(void);
  
  Bool_t ReadEvent(ifstream& aSambaFile, KSambaEvent* aheader = 0);
  
  //getters
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSambaEventReader,1);

};


#endif // __KSAMBAEVENTREADER_H__
