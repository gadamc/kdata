//______________________________________________________________________
//
// KSambaDataObject.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBADATAOBJECT_H__
#define __KSAMBADATAOBJECT_H__

#include "Rtypes.h"
#include <string>

using namespace std;

class KSambaDataObject  { 

public:
  //Constructors
  KSambaDataObject(void);
  virtual ~KSambaDataObject(void);

  //getters
  
  const char* GetRecordWord(void){ return fRecordWord.c_str(); }
  
  //setters

  void SetRecordWord(const char* aWord){ fRecordWord = aWord;  }
private:

  string fRecordWord;
  
  //private methods
  void InitializeMembers(void);

  ClassDef(KSambaDataObject,1);

};


#endif // __KSAMBADATAOBJECT_H__
