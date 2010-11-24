//______________________________________________________________________
//
// KSambaHeaderReader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAHEADERREADER_H__
#define __KSAMBAHEADERREADER_H__

#include <fstream>
#include "Rtypes.h"

using namespace std;

class KSambaHeader;

class KSambaHeaderReader  { 
  
public:
  //Constructors
  KSambaHeaderReader(void);
  //KSambaHeaderReader(ifstream& aSambaFile, KSambaHeader* aheader = 0);
  virtual ~KSambaHeaderReader(void);
  //getters
  
  Bool_t ReadHeader(ifstream& aSambaFile, KSambaHeader* aheader = 0);
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSambaHeaderReader,1);

};


#endif // __KSAMBAHEADERREADER_H__
