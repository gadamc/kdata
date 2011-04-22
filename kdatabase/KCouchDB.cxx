//______________________________________________________________________
//
// KCouchDB.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Friday 04. February 2011
//
//
// This is a wrapper class for using the python modules in 
// KData that access the edelweiss.cloudant.com database.
//


#include "KCouchDB.h"
#include "TSystem.h"
#include "TString.h"
#include "jansson.h"
#include <curl/curl.h>

ClassImp(KCouchDB);


KCouchDB::KCouchDB(void)
{
  fReturn = 0;
  handle = 0;
  ReSizeReturn(256*1024);
  
}

KCouchDB::~KCouchDB(void)
{
  DeleteReturn();
  curl_global_cleanup();
}

void KCouchDB::DeleteReturn(void)
{
  if (fReturn != 0){
    delete [] fReturn;
    fReturn = 0;
  }
}

void KCouchDB::ReSizeReturn(UInt_t size)
{
  if (fReturn != 0){
    DeleteReturn();
  }
  fPos = 0;
  fReturnLength = size;
  fReturn = new char[size];
}


char* KCouchDB::get(const char* url, const char* db,  const char* docid)
{
  
  char errorBuf[CURL_ERROR_SIZE+1];  //use this buffer to save any curl errors.
  CURL *curlhandle = (CURL *)handle;
  curlhandle = curl_easy_init();
  CURLcode res;
  
  if(curlhandle) {
    
    char myurl[1000];
    
    sprintf(myurl, "http://%s/%s/%s",  url, db, docid);
    
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl);
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, handleJsonReturn); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, this);  //checkCouchDBReturn will pass the output to myReturn
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    
    //using verbose output for debugging. 
    //curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);
    
    //perform the transaction
    res = curl_easy_perform(curlhandle);
    //printf("The callback was called %d times\n", count);
    //check for errors
    if(res){ //if res == 0, everything went well
      printf("Curl returned an error.\n");
      printf("Error code: %u \n", res);
      printf("Error: %s\n\n",errorBuf);
    }
    
    curl_easy_cleanup(curlhandle); //always cleanup curl handles

  }
  
  
  return GetReturn();
}
