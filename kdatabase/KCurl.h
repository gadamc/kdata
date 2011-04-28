//______________________________________________________________________
//
// KCurl.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Created Friday 04. February 2011
//


#ifndef __KCURL_H__
#define __KCURL_H__

#include "Rtypes.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

using namespace std;


class KCurl  { 

public:
  //Constructors
  KCurl(void);
  virtual ~KCurl(void);
    
  //void* PUT();
  //void* GET();
  //void* POST()
  //void* DELETE();
  //void GetDoc(const char* docid);
  //void View(const char* view);
  //Int_t Put(const char* url, const char* db, const char* data, const char* user = 0, const char* pword = 0);
  Int_t Get(const char* url, const char* db, const char* item, const char* user = 0, const char* pword = 0);
  const char* GetReturn(void) const; 
  Int_t GetReturnSize(void) const;
  
public:

  vector<string> fReturn;

  static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
  {
    KCurl *mem = (KCurl *)data;
    if(size*nmemb > 0) {
      string s((char *)ptr, size * nmemb);
      mem->fReturn.push_back(s.data());
    }    
    return size * nmemb;
  }
  
  ClassDef(KCurl,1);
  
};


#endif // __KCURL_H__
