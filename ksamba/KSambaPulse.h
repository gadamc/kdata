//______________________________________________________________________
//
// KSambaPulse.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAPULSE_H__
#define __KSAMBAPULSE_H__

#include "KSambaDataObject.h"

class KSambaPulse : public KSambaDataObject  { 

public:
  //Constructors
  KSambaPulse(void);
  virtual ~KSambaPulse(void);
  virtual void Clear(Option_t *opt = "C");

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaPulse,1);
};


#endif // __KSAMBAPULSE_H__
