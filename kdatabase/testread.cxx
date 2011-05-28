/*
 *  testread.c
 *  kData
 *
 *  Created by Adam Cox on 4/3/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include "KJson.cxx"  //yeah, this is annoying... you have to include the .cxx file, which is a bunch of c functions
#include "KCurl.h"

using namespace std;
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
  
  KCurl c;
  char myurl[1000];
  sprintf(myurl, "%s:%s@%s", argv[3], argv[4], argv[1]);
  string myitem = "";
  myitem += argv[2];
  myitem += "/";
  myitem += argv[5];
  cout << myurl << endl;
  cout << myitem << endl;
  
  c.Get(myurl, myitem.data());
  
  string myreturn = c.GetReturn();  
  KJson *doc = KJson_Parse( myreturn.data() );
  char *rendered=KJson_Print(doc);
  
  cout << "The Return " << endl;
  cout << rendered << endl;
  
  
  return 0;
}
  
  
