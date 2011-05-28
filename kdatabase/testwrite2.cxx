#include "KJson.cxx"  //yeah, this is annoying... you have to include the .cxx file, which is a bunch of c functions
#include "KCurl.h"

using namespace std;

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
  
  
  KJson *doc = KJson_CreateObject();
    
  //add some key/value pairs. 
  KJson_AddItemToObject(doc, "author", KJson_CreateString("samba"));
  KJson_AddItemToObject(doc, "type", KJson_CreateString("samba run header"));
  KJson_AddItemToObject(doc, "Temperature", KJson_CreateNumber(0.02002));
  KJson_AddItemToObject(doc, "Year", KJson_CreateNumber(2011));
  
  //add a subdoc (a nested set of key/value pairs) to doc
  KJson *fmt;
  KJson_AddItemToObject(doc, "Bolo.reglages", fmt=KJson_CreateObject());
  KJson_AddItemToObject(fmt, "polar-centre", KJson_CreateNumber(4.0));
  KJson_AddItemToObject(fmt, "polar-garde", KJson_CreateNumber(1.5));
  
  //add an array to doc
  KJson *array;
  KJson_AddItemToObject(doc, "Date", array=KJson_CreateArray());
  KJson_AddItemToArray(array, KJson_CreateNumber(2011));
  KJson_AddItemToArray(array, KJson_CreateNumber(4));
  KJson_AddItemToArray(array, KJson_CreateNumber(2));
  
  
  //print out the json document just to check what it looks like
  char* buf = KJson_Print(doc);
  printf("\nHere is the document we're about to send.");
  cout << "The ID will be appended. _id : " << argv[5] << endl;
  printf("\n%s\n\n",buf);  
  unsigned int len = strlen(buf); //** I save the length here and use it later with curl call!
  printf("It has a length of %u\n\n", len);
  
 
  //now send the json document to the server with KCurl
  KCurl c;
  char myurl[1000];
  sprintf(myurl, "%s:%s@%s", argv[3], argv[4], argv[1]);
  
  string myitem = "";
  myitem += argv[2];
  myitem += "/";
  myitem += argv[5];
  
  printf("will call curl PUT %s\n\n", myurl);
  
  c.Put(myurl, myitem.data(), KJson_PrintUnformatted(doc));
  
  cout << "The Return " << endl;
  cout << c.GetReturn() << endl;
  
  return 0;
  
}
