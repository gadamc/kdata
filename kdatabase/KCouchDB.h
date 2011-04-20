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

class KCouchDB  { 

public:
  //Constructors
  KCouchDB(void);
  virtual ~KCouchDB(void);

  void* UploadMuonVetoDaqMap(const char* file, const char* uri, const char *db, const char* override = "True");
  
  //void* PUT();
  //void* GET();
  //void* POST()
  //void* DELETE();
  //void GetDoc(const char* docid);
  //void View(const char* view);
  
  
  //getters

  //setters
private:

  //void* fJsonObjectOutput  //or should i just store them in strings and use the jsansson.h object in the implementation when needed
  //completely hiding this stuff from the user. 
  //void* fJsonObjectInput
  
  //private methods
  //string fServer;
  //string fUrl; 
  //string fUserName;
  //string fPassword;
  
  
  ClassDef(KCouchDB,1);
};


#endif // __KCOUCHDB_H__
