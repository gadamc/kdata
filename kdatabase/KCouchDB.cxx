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
#include "TPython.h"

ClassImp(KCouchDB);

KCouchDB::KCouchDB(void)
{

}

KCouchDB::~KCouchDB(void)
{
  

}

void* KCouchDB::PythonTest(void)
{
  //holy shit this works
  
  void *b;
  TPython py;
  b = (void*)py.Eval("ROOT.TBrowser()");
  py.Bind((TBrowser *)b,"b");
  return b;
}
