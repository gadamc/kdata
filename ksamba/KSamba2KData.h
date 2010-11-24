//______________________________________________________________________
//
// KSamba2KData.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBA2KDATA_H__
#define __KSAMBA2KDATA_H__

#include "Rtypes.h"

class KSamba2KData  { 
  
public:
  //Constructors
  KSamba2KData(void);
  //KSamba2KData(const char* inputSambaFile, const char* outputKRawEventFile);
  virtual ~KSamba2KData(void);
  
  //Bool_t ConvertFile(const char* inputSamba, const char* outputKRawEventFile);
  
  //Is this possible???
 // Bool_t ConvertStream(ifstream* inputSambaStream, const char* outputKRawEventFile);
  
  //getters
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSamba2KData,1);

};


#endif // __KSAMBA2KDATA_H__
