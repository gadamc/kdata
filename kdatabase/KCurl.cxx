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
// This is a wrapper class for using simple libcurl commands in 
// KData in order to facilitate using the edelweiss couchdb.
//


#include "KCurl.h"
#include <curl/curl.h>


ClassImp(KCurl);

KCurl::KCurl(void)
{
  SetVerbose(0);
  fReturn = "";
}

KCurl::~KCurl(void)
{
  curl_global_cleanup();
}


Int_t KCurl::Get(const char* url, const char* item)
{
  //Gets the response from the following HTTP GET command  
  //HTTP GET http://url/item
  //If you need to use authentication, then you should 
  //set url to be "username:password@url", for example
  //"user:secret@127.0.0.1:5984"
  //This will call 
  //HTTP GET http://user:secret@127.0.0.1:5984/item"
  //
  //This is exactly the same as using the UNIX program 'curl' from the 
  //command line
  //
  // $> curl http://url/item
  //
  //For CouchDB access, typical values are
  // url = edwdbik.fzk.de:5984 (or for your own local db 127.0.0.1:5984)
  // item = (a docid, a view, or even _all_docs). 
  //     If you are accessing a database, your item should look something like
  //     "databasename/item", where "edwdb" is the standard database name
  //     and item is the docid, view, _all_docs, etc....
  //     see the couchdb documentation
  //
  //The return value is the value received from libcurl. You can get the full
  //string response by calling GetReturn. If the return is 0, then
  //the curl transaction seems to have worked without error.
  //
  
  char errorBuf[CURL_ERROR_SIZE+1];  //use this buffer to save any curl errors.
  CURL *curlhandle = 0;
  curlhandle = curl_easy_init();
  CURLcode res;
  
  fReturn = "";
  if(curlhandle) {
    
    string myurl;
    myurl.append("http://");
    myurl.append(url);
    myurl.append("/");
    myurl.append(item);
        
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl.c_str());
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, this);  //checkCouchDBReturn will pass the output to this object
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    
    //using verbose output for debugging. 
    if(GetVerbose()) curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);
    
    //perform the transaction
    res = curl_easy_perform(curlhandle);
    //printf("The callback was called %d times\n", count);
    //check for errors
    if(res){ //if res == 0, everything went well
      cerr << "Curl returned an error." << endl;
      cerr << "Error code: " <<  res << endl;
      cerr << "Error: " << errorBuf << endl;
    }
    
    curl_easy_cleanup(curlhandle); //always cleanup curl handles
  }
  
  
  return res;
}


Int_t KCurl::Put(const char* url, const char* item, const char* thedoc)
{
  //calls the following 
  //HTTP PUT http://url/item -d "thedoc"
  //If you need to use authentication, then you should 
  //set url to be "username:password@url", for example
  //"user:secret@127.0.0.1:5984"
  //This will call
  //HTTP PUT http://user:secret@url/item 
  //
  //If thedoc == 0, then the ' -d "thedoc" ' is excluded.
  //
  //For CouchDB access, typical values are
  // url = edwdbik.fzk.de:5984 (don't include the http://)
  // item = (a docid, a view, or even _all_docs). 
  //     If you are putting a file on the database, your item should look something like
  //     "databasename/docid", where "edwdb" is the standard database name
  //     and item is the docid.
  //     If you are trying to create a new database, your item should be
  //     "databasename" and the "thedoc" should be a NULL pointer.
  //     see the couchdb documentation
  //
  // thedoc = a JSON document that you will likely be puting on a couchdb.
  //          NOTE: You must be sure that 'thedoc' is in proper JSON format
  //          This method does not check the format for you. If it is not in proper
  //          JSON format, then you will see an error in the returned string (KCurl::GetReturn())
  //
  //The return value is the value received from libcurl. 
  //You can get the full string by calling GetReturn. If the return is 0, then
  //the curl transaction seems to have worked without error.
  //
  
  char errorBuf[CURL_ERROR_SIZE+1];  //use this buffer to save any curl errors.
  CURL *curlhandle = 0;
  curlhandle = curl_easy_init();
  CURLcode res;
  
  fReturn = "";
  
  if(curlhandle) {
    
    string myurl;
    myurl.append("http://");
    myurl.append(url);
    myurl.append("/");
    myurl.append(item);
        
    readarg_t mydoc;
    mydoc.len = 0;
    mydoc.pos = 0;
    mydoc.buf = 0;
    
    if(thedoc) {
      mydoc.len = strlen(thedoc);
      mydoc.pos = 0;
      mydoc.buf = thedoc;
    }
    
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl.c_str());
    
    //use appropriate json header for curl
    if(thedoc){
      struct curl_slist *jsonheader = NULL;
      jsonheader = curl_slist_append(jsonheader, "Content-Type: application/json");
      curl_easy_setopt(curlhandle, CURLOPT_HTTPHEADER, jsonheader);
    }
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, this);  
    if(thedoc) {
      curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, ReadBufferCallback); //calls this function to get data to PUT to the couchdb server
      curl_easy_setopt(curlhandle, CURLOPT_READDATA, &mydoc);//passJsonString will get buf data using this pointer
      curl_easy_setopt(curlhandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)mydoc.len);  //the length of buf that will be PUT
    }
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1); //tell curl to upload the contents that passJsonString tells it to.

    
    //using verbose output for debugging. 
    if(GetVerbose()) curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);
    
    //perform the transaction
    res = curl_easy_perform(curlhandle);
    //printf("The callback was called %d times\n", count);
    //check for errors
    if(res){ //if res == 0, everything went well
      cerr << "Curl returned an error." << endl;
      cerr << "Error code: " <<  res << endl;
      cerr << "Error: " << errorBuf << endl;
      cerr << "Error String: " << curl_easy_strerror(res);
    }
    
    curl_easy_cleanup(curlhandle); //always cleanup curl handles
  }
  
  
  return res;
}


