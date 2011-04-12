//______________________________________________________________________
//
// KSambaHeader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAHEADER_H__
#define __KSAMBAHEADER_H__

#include "KSambaDataObject.h"

class KSambaHeader : public KSambaDataObject { 

public:
  //Constructors
  KSambaHeader(void);
  virtual ~KSambaHeader(void);
  virtual void Clear(Option_t *opt = "C");

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaHeader,1);
};


#endif // __KSAMBAHEADER_H__
