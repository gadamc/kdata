//______________________________________________________________________
//
// KCurl.cxx
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


#include "KCurl.h"
#include <curl/curl.h>


ClassImp(KCurl);

KCurl::KCurl(void)
{
  
}

KCurl::~KCurl(void)
{
  curl_global_cleanup();
}

const char* KCurl::GetReturn(void) const
{
  string b;
  for(UInt_t i = 0; i < fReturn.size(); i++){
    b += fReturn[i];
  }
  return b.c_str();
}

Int_t KCurl::GetReturnSize(void) const
{
  //gives the full size of the response from the server (in bytes)
  //
  
  Int_t size;
  for(UInt_t i = 0; i < fReturn.size(); i++){
    size += fReturn[i].size();
  }
  return size;
}

Int_t KCurl::Get(const char* url, const char* db,  const char* item,
                    const char* user, const char* pword)
{
  //Gets the response from the following HTTP GET command
  //HTTP GET http://user:pword@url/db/item
  //If user/pword are not set, then it calls
  //HTTP GEt http://url/db/item
  //
  //For CouchDB access, typical values are
  // url = edwdbik.fzk.de:5984
  // db = edwdb
  // item = (a docid, a view, or even _all_docs). see the couchdb documentation
  // user / pword - obtain this from the database czar. 
  //
  //The return is GetReturnSize. You can get the full
  //string by calling GetReturn. 
  
  char errorBuf[CURL_ERROR_SIZE+1];  //use this buffer to save any curl errors.
  CURL *curlhandle = 0;
  curlhandle = curl_easy_init();
  CURLcode res;
  
  fReturn.resize(0);
  
  if(curlhandle) {
    
    string myurl;
    myurl.append("http://");
    if(user != 0 && pword != 0){
      myurl.append(user);
      myurl.append(":");
      myurl.append(pword);
    }
    myurl.append(url);
    myurl.append("/");
    myurl.append(db);
    myurl.append("/");
    myurl.append(item);
        
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl.c_str());
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, this);  //checkCouchDBReturn will pass the output to this object
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
      return -1;
    }
    
    curl_easy_cleanup(curlhandle); //always cleanup curl handles
  }
  
  
  return GetReturnSize();
}
