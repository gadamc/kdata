//______________________________________________________________________
//
// KCouchDB.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Thursday 28. April 2011
//


#ifndef __KCOUCHDB_H__
#define __KCOUCHDB_H__

#include "Rtypes.h"
#include <string>

class KJson;

class KCouchDB  { 

public:
  //Constructors
  KCouchDB();//(const char* server = 0, const char* db = 0, const char* user = 0, const char* pass = 0);
  virtual ~KCouchDB(void);

  // KJson* Getdoc(const char* docId) const;
  //   KJson* View(const char* viewName, const char* options) const;
  //   //savedoc
  //   //bulksave
  // 
  //   void SetServer(const char* name){fServer = name;}
  //   void SetDb(const char* name){fDb = name;}
  //   void SetUser(const char* name){fUser = name;}
  //   void SetPassword(const char* name){fPass = name;}
  
  
private:

  //private methods
  // std::string fServer;
  //   std::string fDb;
  //   std::string fUser;
  //   std::string fPass;
  
  ClassDef(KCouchDB,1);
};


#endif // __KCOUCHDB_H__
