/*
 *  testread.c
 *  kData
 *
 *  Created by Adam Cox on 4/3/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include "KJson.h"  
#include "KCurl.h"



using namespace std;
int main (int /*argc*/, char** argv)
{
  //
  //usage:
  //testread <url> <item>
  //
  // <url>  -- the CouchDB URL (https://edelweiss.cloudant.com)
  //
  // <item> --  the CouchDB API call.
  //      so, something like this. <database name>/<doc_id> 
  //      
  //      "analysis/run12analysisconstants_ID6"
  //         or
  //      "fabdb/detector_FID402"
  //
  //         or
  //      "datadb/_design/proc/_view/proc0?reduce=false&include_docs=true"
  
  KCurl c;
  char myurl[1000];
  sprintf(myurl, "%s", argv[1]);
  string myitem = "";
  myitem += argv[2];
  
  cout << myurl << endl;
  cout << myitem << endl;
  
  c.Get(myurl, myitem.data());
  
  string myreturn = c.GetReturn();  
  KJson *doc = KJson::Parse( myreturn.data() );
  char *rendered=KJson::Print(doc);
  
  cout << "The Return " << endl;
  cout << rendered << endl;
  
  
  return 0;
}
  
  
