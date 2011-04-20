/*
 *  testread.c
 *  kData
 *
 *  Created by Adam Cox on 4/3/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include <jansson.h>
#include <curl/curl.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define BUFFER_SIZE (256 * 1024) //256 kb. initial buffer size. 

using namespace std;
int count = 0;
struct write_result  //use this struct to store the result from a curl call
{
  unsigned int len; 
  char *data;
  unsigned int pos;
};

static size_t handleJsonReturn(void *ptr, size_t size, size_t nmemb, void *data)
{
  struct write_result *result = (struct write_result *)data;
  
  if(result->pos + size * nmemb >= result->len - 1)
  { //make sure the buffer is large enough. if not, make it bigger.
    printf("error, too small of buffer. %u * %u = %lu", (unsigned int)size, (unsigned int)nmemb, (unsigned int)size*nmemb);
    free(result->data);
    result->data = (char*)malloc((size*nmemb+1)*sizeof(char));
  }
  
  //this is done because curl can return data in chunks instead of 
  //full strings. so we have to keep track of where we have to 
  //add to the data string. 
  memcpy(result->data + result->pos, ptr, size * nmemb);
  result->pos += size * nmemb;
  count += 1;
  return size * nmemb;
}

int main (int /*argc*/, char** argv)
{
  //
  //usage:
  //testread <url> <database> <username> <password> <doc_id> 
  //
  // <url> -- don't include http://  use just 127.0.0.1:5984 or 134.158.176.27:5984
  // <database> = testdb
  // <doc_id> -- provide a unique document id for the json doc you will get from the db. 
  //
  
  
  CURL *curlhandle;
  CURLcode res;
  
  write_result myresult;  //use this to save the return from the curl request
  myresult.data = (char*)malloc(BUFFER_SIZE*sizeof(char));;  //set the pointer to allocated space.
  myresult.len = BUFFER_SIZE;
  myresult.pos = 0;
  
  curlhandle = curl_easy_init();
  char errorBuf[CURL_ERROR_SIZE+1];  //use this buffer to save any curl errors.
  
  if(curlhandle) {
    
    char myurl[1000];
    
    sprintf(myurl, "http://%s:%s@%s/%s/%s", argv[3], argv[4], argv[1], argv[2], argv[5]);
    printf("will call curl GET %s\n\n", myurl);
    
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl);
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, handleJsonReturn); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &myresult);  //checkCouchDBReturn will pass the output to myReturn
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    
    //using verbose output for debugging. 
    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);
    
    //perform the transaction
    res = curl_easy_perform(curlhandle);
    printf("The callback was called %d times\n", count);
    //check for errors
    if(res){ //if res == 0, everything went well
      printf("Curl returned an error.\n");
      printf("Error code: %u \n", res);
      printf("Error: %s\n\n",errorBuf);
    }
    
    //now read the return from the curl call, which is stored
    //in myresult and pass it to the json parser, which will 
    //unpack the string into a json_t object.
    json_error_t jsonerror;
    json_t *root = json_loads(myresult.data, 0, &jsonerror);
    
    printf("Here is the response from the http request.\n\n");
    printf("%s\n\n",myresult.data);
    
    if(json_is_object(root)) {
      printf("Here is what the response looks like when json_dumps prints in pretty form.\n\n");
      char *buf = json_dumps(root, JSON_INDENT(2));
      printf("%s\n\n",buf);  //print the return as pretty json
      free(buf);
    }
    else {
      printf("This response is not a proper json object.\n");
      printf("Error: %u:%u: %s\n", jsonerror.line, jsonerror.column, jsonerror.text);
    }
    
    curl_easy_cleanup(curlhandle); //always cleanup curl handles
    
  }
  
  curl_global_cleanup();
  return 0;
}
  
  
