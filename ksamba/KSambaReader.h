//______________________________________________________________________
//
// KSambaReader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAREADER_H__
#define __KSAMBAREADER_H__

#include "Rtypes.h"
#include "KSambaHeader.h"
#include "KSambaEvent.h"
#include <fstream>

using namespace std;

class KSambaReader  { 

public:
  //Constructors
  KSambaReader(void);
  KSambaReader(const char* sambaFile);
  virtual ~KSambaReader(void);
  virtual void Clear(Option_t *opt = "C");

  Bool_t OpenFile(const char* sambaFile);
  void CloseFile(void);
  Bool_t ReadHeader(void);
  Bool_t ReadNextEvent(void);
  
  //getters
  
  //Bool_t GetNextEvent(void) { return true;  } //yay, i compile!
  KSambaHeader* GetSambaHeader(void) const { return fHeader;  }
  KSambaEvent* GetSambaEvent(void) const { return fEvent;  }
  
  //setters
private:

  //private methods
  void InitializeMembers(void);

  ifstream fSambaFile;
  KSambaHeader *fHeader;
  KSambaEvent *fEvent;
  
  
  
  //Bool_t ReadNextObject(void);
  //Bool_t ReadNextLine(void);
  //Bool_t IsSambaDataObject(void);
  
  ClassDef(KSambaReader,1);
};


#endif // __KSAMBAREADER_H__
