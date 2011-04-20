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

size_t passJsonDoc(char *bufptr, size_t size, size_t nitems, void *userp)
{
  json_t * mydata = (json_t *)userp;
  char *buf = json_dumps(mydata , JSON_INDENT(0));
  printf("size * nitems = %lu  ",size*nitems);
  printf("json object size = %u\n", (unsigned int)strlen(buf));
  size_t thesize = (strlen(buf) >= size*nitems) ? size*nitems-1 : strlen(buf);
  printf("copy %u bytes of %s\n\n",(unsigned int)thesize, buf);
  memcpy(bufptr, buf, thesize);
  bufptr[thesize+1] = '\0';  //add null terminating character.
  
  free(buf);
  count += 1;
  return thesize;
}

size_t passBuffer(char *bufptr, size_t size, size_t nitems, void *userp)
{
  char* mydata = (char *)userp;
  size_t thesize = (strlen(mydata) >= size*nitems) ? size*nitems-1 : strlen(mydata);
  memcpy(bufptr, mydata, thesize);
  bufptr[thesize+1] = '\0';  //add null terminating character.
  count += 1;
  return thesize;
}

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
  
  return size * nmemb;
}

int main (int /*argc*/, char** argv)
{
  //
  //usage:
  //testwrite2 <url> <database> <username> <password> <doc_id> 
  //
  // <url> -- don't include http://  use just 127.0.0.1:5984 or 134.158.176.27:5984
  // <database> = testdb
  // <doc_id> -- provide a unique document id for the json doc you will upload to the db. 
  //
  
  
  //create json document
  json_t *doc = json_object();
  
  //add some key/value pairs. 
  json_object_set_new(doc, "_id", json_string(argv[5]));
  json_object_set_new(doc, "author", json_string("samba"));
  json_object_set_new(doc, "type", json_string("samba run header"));
  json_object_set_new(doc, "Temperature", json_real(0.02002));
  json_object_set_new(doc, "Year", json_integer(2011));
  
  //add a subdoc (a nested set of key/value pairs) to doc
  json_t *subdoc = json_object();
  json_object_set_new(subdoc, "polar-centre", json_real(4.0));
  json_object_set_new(subdoc, "polar-garde", json_real(1.5));
  json_object_set(doc, "Bolo.reglages", subdoc);
  json_decref(subdoc);  //have to deal with the garbage collector
  
  //add an array to doc
  json_t *array = json_array();
  json_array_append(array, json_integer(2011));
  json_array_append(array, json_integer(4));
  json_array_append(array, json_integer(2));
  json_object_set(doc, "Date", array);
  json_decref(array); //deal with garbage collector
      
  
  //print out the json document just to check what it looks like
  char* buf = json_dumps(doc, JSON_INDENT(0));
  printf("\nHere is the document we're about to send.");
  printf("\n%s\n\n",buf);  
  unsigned int len = strlen(buf); //** I save the length here and use it later with curl call!
  printf("It has a length of %u\n\n", (curl_off_t)len);
  
  
  
  //check to make sure we built the json object properly
  json_error_t jsonerror;
  json_t *root = json_loads(buf, 0, &jsonerror);
  
  //if root is not a json object, this returns NULL
  if(!json_is_object(root)){
    printf("Error: %u:%u: %s\n", jsonerror.line, jsonerror.column, jsonerror.text);
    return -1;  
  }
  json_decref(root);  //garbage collect.
  
  
  /** note **/
  //i suppose i could pass the json_object *doc
  //to curl via the callback functios, but 
  //this example passes the buffer, char* buf, instead. 
  //see below. 
  
  //now send the json document to the server with libcurl
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
    json_t *id = json_object_get(doc,"_id");  //check to make sure that the _id key exists. 
    if(id== NULL){
      printf("error. no id\n");
      return -1;  //only send a document that has the _id key. 
                  //we could send the document, in which case couch will create an id for you. 
                  //but I think its good policy to avoid this. 
    }

    sprintf(myurl, "http://%s:%s@%s/%s/%s", argv[3], argv[4], argv[1], argv[2], json_string_value(id));
    printf("will call curl PUT %s\n\n", myurl);
    
    curl_easy_setopt(curlhandle, CURLOPT_URL, myurl);
    
    //use appropriate json header for curl
    struct curl_slist *jsonheader = NULL;
    jsonheader = curl_slist_append(jsonheader, "Content-Type: application/json");
    curl_easy_setopt(curlhandle, CURLOPT_HTTPHEADER, jsonheader);
    
    //set the curl options for this transaction
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, handleJsonReturn); //catch the output from couch and deal with it with this function
    curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &myresult);  //checkCouchDBReturn will pass the output to myReturn
    curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, passBuffer); //calls this function to get data to PUT to the couchdb server
    curl_easy_setopt(curlhandle, CURLOPT_READDATA, buf);//passJsonString will get buf data using this pointer
    curl_easy_setopt(curlhandle, CURLOPT_INFILESIZE_LARGE, (curl_off_t)len);  //the length of buf that will be PUT
    curl_easy_setopt(curlhandle, CURLOPT_ERRORBUFFER, errorBuf);  //hold any errors here.
    curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1); //tell curl to upload the contents that passJsonString tells it to.
    
    //using verbose output for debugging. 
    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);
   
    //perform the transaction
    res = curl_easy_perform(curlhandle);
    //check for errors
    if(res){ //if res == 0, everything went well
      printf("Curl returned an error.\n");
      printf("Error code: %u \n", res);
      printf("Error: %s\n\n",errorBuf);
    }
    
    //now read the return from the curl call, which is stored
    //in myresult and pass it to the json parser, which will 
    //unpack the string into a json_t object.
    //
    //note:  root and jsonerror were declared above (line ~84)
    //
    root = json_loads(myresult.data, 0, &jsonerror);
    
    printf("The callback was called %d times\n", count);
    
    printf("Here is the response from the http request.\n\n");
    printf("%s\n\n",myresult.data);
    
    if(json_is_object(root)) {
      printf("Here is what the response looks like when json_dumps prints in pretty form.\n\n");
      buf = json_dumps(root, JSON_INDENT(2));
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
