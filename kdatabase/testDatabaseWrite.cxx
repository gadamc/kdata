

#include "KJson.h"  
#include "KCurl.h"

using namespace std;

int main (int /*argc*/, char** argv)
{
  //
  //usage:
  //testwrite2 <url> <database> <doc_id> 
  //
  // <url> -- don't include http://  use just 127.0.0.1:5984 or 134.158.176.27:5984
  // <database> = testdb
  // <doc_id> -- provide a unique document id for the json doc you will upload to the db. 
  //
  
  
  KJson *doc = KJson::CreateObject();
    
  //add some key/value pairs. 
  KJson::AddItemToObject(doc, "author", KJson::CreateString("samba"));
  KJson::AddItemToObject(doc, "type", KJson::CreateString("samba run header"));
  KJson::AddItemToObject(doc, "Temperature", KJson::CreateNumber(0.02002));
  KJson::AddItemToObject(doc, "Year", KJson::CreateNumber(2011));
  
  //add a subdoc (a nested set of key/value pairs) to doc
  KJson *fmt;
  KJson::AddItemToObject(doc, "Bolo.reglages", fmt=KJson::CreateObject());
  KJson::AddItemToObject(fmt, "polar-centre", KJson::CreateNumber(4.0));
  KJson::AddItemToObject(fmt, "polar-garde", KJson::CreateNumber(1.5));
  
  //add an array to doc
  KJson *array;
  KJson::AddItemToObject(doc, "Date", array=KJson::CreateArray());
  KJson::AddItemToArray(array, KJson::CreateNumber(2011));
  KJson::AddItemToArray(array, KJson::CreateNumber(4));
  KJson::AddItemToArray(array, KJson::CreateNumber(2));
  
  
  //print out the json document just to check what it looks like
  char* buf = KJson::Print(doc);
  printf("\nHere is the document we're about to send.");
  cout << "The ID will be appended. _id : " << argv[5] << endl;
  printf("\n%s\n\n",buf);  
  unsigned int len = strlen(buf); //** I save the length here and use it later with curl call!
  printf("It has a length of %u\n\n", len);
  
 
  //now send the json document to the server with KCurl
  KCurl c;
  char myurl[1000];
  sprintf(myurl, "%s", argv[1]);
  
  string myitem = "";
  myitem += argv[2];
  myitem += "/";
  myitem += argv[3];
  
  printf("will call curl PUT %s\n\n", myurl);
  
  c.Put(myurl, myitem.data(), KJson::PrintUnformatted(doc));
  
  cout << "The Return " << endl;
  cout << c.GetReturn() << endl;
  
  return 0;
  
}
