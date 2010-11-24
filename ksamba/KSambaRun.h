//______________________________________________________________________
//
// KSambaRun.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBARUN_H__
#define __KSAMBARUN_H__

#include "KSambaDataObject.h"

class KSambaRun : public KSambaDataObject { 
  
public:
  //Constructors
  KSambaRun(void);
  virtual ~KSambaRun(void);
  
  //getters
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSambaRun,1);

};


#endif // __KSAMBARUN_H__
