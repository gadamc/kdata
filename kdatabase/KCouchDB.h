//______________________________________________________________________
//
// KCouchDB.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Friday 04. February 2011
//


#ifndef __KCOUCHDB_H__
#define __KCOUCHDB_H__

#include "Rtypes.h"
#include "TBrowser.h"

class KCouchDB  { 

public:
  //Constructors
  KCouchDB(void);
  virtual ~KCouchDB(void);

  void* PythonTest(void);
  //getters

  //setters
private:

  //private methods

  ClassDef(KCouchDB,1);
};


#endif // __KCOUCHDB_H__
