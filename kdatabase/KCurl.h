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
#include <sstream>
#include <cstring>


using namespace std;


class KCurl  { 

public:
  //Constructors
  KCurl(void);
  virtual ~KCurl(void);
    
  //void* POST()
  //void* DELETE();
  Int_t Put(const char* url, const char* item, const char* opt = "-a", const char* thedoc = 0);
  Int_t Get(const char* url, const char* item, const char* opt = "-a");
 
  const char* GetReturn(void) const {return fReturn.c_str();}
  size_t GetReturnSize(void) const {return fReturn.size();}
  Bool_t GetVerbose(void) const { return fVerbose;}
  void SetVerbose(Bool_t aVal = true) {fVerbose = aVal;}
  
private:

  string fReturn;
  typedef struct{
    const char* buf;
    int len;
    int pos;
  } readarg_t;
  
  Bool_t fVerbose;

  static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
  { KCurl *mem = (KCurl *)data;
    const char* stringdata = (const char*)ptr;
    if(size*nmemb > 0)
      mem->fReturn.append(stringdata, size*nmemb);
    return size * nmemb; 
  }

  static size_t ReadBufferCallback(char *bufptr, size_t size, size_t nitems, void *userp)
  { 
    readarg_t *rarg = (readarg_t *)userp;
    size_t len = rarg->len - rarg->pos;
    if (len > size * nitems)
      len = size * nitems;
    memcpy(bufptr, rarg->buf + rarg->pos, len);
    rarg->pos += len;
    return len;
  }
  
  bool DoAcceptSSLCertificate(const char* opt);
  //bool DoUseSSL(const char* opt);

  
  ClassDef(KCurl,1);
  
};


#endif // __KCURL_H__
