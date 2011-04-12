//______________________________________________________________________
//
// KSambaDetectorChannel.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBADETECTORCHANNEL_H__
#define __KSAMBADETECTORCHANNEL_H__

#include "KSambaDataObject.h"

class KSambaDetectorChannel : public KSambaDataObject { 
  
public:
  //Constructors
  KSambaDetectorChannel(void);
  virtual ~KSambaDetectorChannel(void);
  
  //getters
  
  //setters
private:
  
  //private methods
  void InitializeMembers(void);
  
  ClassDef(KSambaDetectorChannel,1);

};


#endif // __KSAMBADETECTORCHANNEL_H__
