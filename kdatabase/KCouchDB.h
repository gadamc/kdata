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
    
  //void* PUT();
  //void* GET();
  //void* POST()
  //void* DELETE();
  //void GetDoc(const char* docid);
  //void View(const char* view);
  
  char* get(const char* url, const char* db, const char* docid);
  
  
private:
  
  
  //void* fJsonObjectOutput  //or should i just store them in strings and use the jsansson.h object in the implementation when needed
  //completely hiding this stuff from the user. 
  //void* fJsonObjectInput
  
  //private methods
  //string fServer;
  //string fUrl; 
  //string fUserName;
  //string fPassword;
  
  UInt_t fReturnLength;
  char* fReturn;
  UInt_t fPos;
  void *handle;
  
  void DeleteReturn(void);
  void ReSizeReturn(UInt_t size);
  UInt_t GetPos(void){ return fPos;}
  void SetPos(UInt_t i){fPos = i;  }
  UInt_t GetLength(void){return fReturnLength;}
  void SetLength(UInt_t i){fReturnLength= i;}
  char* GetReturn(void) {return fReturn;}
  
  static size_t handleJsonReturn(void *ptr, size_t size, size_t nmemb, void *data)
  {
    KCouchDB *result = (KCouchDB *)data;
    
    if(result->GetPos() + size * nmemb >= result->GetLength() - 1)
    { //make sure the buffer is large enough. if not, make it bigger.
      //printf("error, too small of buffer. %u * %u = %lu", (unsigned int)size, (unsigned int)nmemb, (unsigned int)size*nmemb);
      result->DeleteReturn();
      result->ReSizeReturn(size*nmemb+1);
    }
    
    //this is done because curl can return data in chunks instead of 
    //full strings. so we have to keep track of where we have to 
    //add to the data string. 
    memcpy(result->GetReturn() + result->GetPos(), ptr, size * nmemb);
    result->SetPos( result->GetPos() + size * nmemb );
    return size * nmemb;
  }
  
  ClassDef(KCouchDB,1);
  
};


#endif // __KCOUCHDB_H__
