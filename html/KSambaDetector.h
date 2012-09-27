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

#include "KSambaDetectorChannel.h"
#include "Rtypes.h"
#include "TObject.h"
#include <string>
#include <vector>

using namespace std;

class KSambaDetector : public TObject  { 

public:
  //Constructors
  KSambaDetector(void);
  virtual ~KSambaDetector(void);

  const char* GetName(void) const { return fName.c_str();}

  void SetName(const char* aVal) {fName = aVal;}
  KSambaDetectorChannel* AddChannel();
  KSambaDetectorChannel* GetChannelFromList(const char* channame);
  Bool_t IsChannelInList(const char *channame);
  KSambaDetectorChannel* GetChannelFromList(UInt_t i);
  UInt_t GetChannelListSize(void) const { return fChannels.size();  }

private:

  //private methods
  string fName;
  vector<KSambaDetectorChannel *> fChannels;
  
  void InitializeMembers(void);

  ClassDef(KSambaDetector,1);

};


#endif // __KSAMBADETECTOR_H__
