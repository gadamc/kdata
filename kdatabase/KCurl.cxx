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

Int_t KCurl::Get(const char* url, const char* item, const char* opt)
{
  //Gets the response from the following HTTP GET command  
  //HTTP GET url/item
  //If you need to use authentication, then you should 
  //set url to be "http://username:password@address", for example
  //"http://user:secret@127.0.0.1:5984"
  //This will call 
  //HTTP GET http://user:secret@127.0.0.1:5984/item"
  //
  //This is exactly the same as using the UNIX program 'curl' from the 
  //command line
  //
  // $> curl url/item
  //
  // If you want to use SSL with HTTP, use https:// 
  // For CouchDB, the SSL port is 6984. In order to ignore self-signed SSL
  // certificates, use the -a option. 
  //
  //For Edelweiss CouchDB access, typical values are
  // url = https://edwdbik.fzk.de:6984 (the database is readable by all, but you must use a username/password to write)
  // item = (a docid, a view, or even _all_docs). 
  //     If you are accessing a database, your item should look something like
  //     "databasename/item"
  //     see the couchdb documentation
  //
  // opt is a string of characters separated by a space (' ') and preceded with a dash ('-') 
  //   -a  = accept the SSL certificate without a Certified Authorization. This is needed for self-signed certificates.
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
    myurl.append(url);
    myurl.append("/");
    myurl.append(item);
        
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl.c_str());
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, this);  //checkCouchDBReturn will pass the output to this object
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    
    if(opt){
      if(DoAcceptSSLCertificate(opt)){
        curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYPEER, 0L);
      }
    }
    
    //using verbose output for debugging. 
    if(GetVerbose()) curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);
    
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


Int_t KCurl::Put(const char* url, const char* item, const char* thedoc, const char* opt)
{
  //calls the following 
  //HTTP PUT url/item -d "thedoc"
  //If you need to use authentication, then you should 
  //set url to be "username:password@url", for example
  //"http://user:secret@127.0.0.1:5984"
  //This will call
  //HTTP PUT http://user:secret@url/item 
  //
  //If thedoc == 0, then the ' -d "thedoc" ' is excluded.
  //
  //
  // If you want to use SSL with HTTP, use https:// 
  // For CouchDB, the SSL port is 6984. In order to ignore self-signed SSL
  // certificates, use the -a option. 
  //
  //For Edelweiss CouchDB access, typical values are
  // url = https://edwdbik.fzk.de:6984 (the database is readable by all, but you must use a username/password to write)
  // item = (a docid, a view, or even _all_docs). 
  //     If you are putting a file on the database, your item should look something like
  //     "databasename/docid".
  //     If you are trying to create a new database, your item should be
  //     "databasename" and the "thedoc" should be a NULL pointer.
  //     see the couchdb documentation
  //
  // thedoc = a JSON document that you will likely be puting on a couchdb.
  //          NOTE: You must be sure that 'thedoc' is in proper JSON format
  //          This method does not check the format for you. If it is not in proper
  //          JSON format, then you will see an error in the returned string (KCurl::GetReturn())
  //
  // opt = currently this function just supports one option. 
  //   -a  = accept the SSL certificate without a Certified Authorization. This is needed for self-signed certificates.
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
    myurl.append(url);
    myurl.append("/");
    myurl.append(item);;
    
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

    
    if(opt){
      if(DoAcceptSSLCertificate(opt)){
        curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYPEER, 0L);
      }
    }
    
    
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


bool KCurl::DoAcceptSSLCertificate(const char* opt)
{
  string sOpts = opt;
  string token;
  istringstream iss(sOpts);
  while ( getline(iss, token, ' ') )
  {
    if (token.compare("-a")==0){
      return true;
    }
  }
  return false;
}
/*
 bool KCurl::DoUseSSL(const char* opt)
 {
 string sOpts = opt;
 string token;
 istringstream iss(sOpts);
 while ( getline(iss, token, ' ') )
 {
 cout << token << endl;
 if (token.compare("-s")==0){
 return true;
 }
 }
 return false;
 }
 */



