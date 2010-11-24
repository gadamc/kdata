//______________________________________________________________________
//
// KSambaEvent.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBAEVENT_H__
#define __KSAMBAEVENT_H__

#include "KSambaDataObject.h"

class KSambaEvent : public KSambaDataObject { 

public:
  //Constructors
  KSambaEvent(void);
  virtual ~KSambaEvent(void);

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaEvent,1);

};


#endif // __KSAMBAEVENT_H__
