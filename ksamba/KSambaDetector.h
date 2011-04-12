//______________________________________________________________________
//
// KSambaDetector.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBADETECTOR_H__
#define __KSAMBADETECTOR_H__

#include "KSambaDataObject.h"

class KSambaDetector  : public KSambaDataObject { 

public:
  //Constructors
  KSambaDetector(void);
  virtual ~KSambaDetector(void);

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaDetector,1);

};


#endif // __KSAMBADETECTOR_H__
