/*
*  KDataFileIO.h
*  KDataStructure
*
*  Created by Adam Cox on 5/11/10.
*  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
*
*/

#ifndef __KDataFileIO_H__
#define __KDataFileIO_H__

#include "Rtypes.h"
#include "TChain.h"
#include <string>
#include "TFile.h"


using namespace std;

class KDataFileIO  { 

protected:
  //Constructors -- This class is the base for the KTreeWriter and KTreeReader classes
  KDataFileIO(void);
  virtual ~KDataFileIO(void);
  //virtual void Delete(void);
  // 
  string fTreeName;
  string fTreeTitle;
  string fBranchName;
  
public:

  string GetTreeName(void) const {return fTreeName;}
  string GetTreeTitle(void) const {return fTreeTitle;}
  string GetBranchName(void) const {return fBranchName;}
  

private:
  

  //private methods
  void InitializeMembers(void);

  ClassDef(KDataFileIO,2);
};


#endif // __KDataFileIO_H__
