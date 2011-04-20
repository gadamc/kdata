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
#include "TSystem.h"
#include "TString.h"
#include "jansson.h"

ClassImp(KCouchDB);

KCouchDB::KCouchDB(void)
{

}

KCouchDB::~KCouchDB(void)
{
  

}

void KCouchDB::UploadMuonVetoDaqMap(const char* file, const char* uri, const char *db, const char* override)
{
  TString command = "python $KDATA_ROOT/lib/KDataPy/kdatabase/uploadMuonVetoDaqMapCsvToCouch.py";
  command += " ";
  command += file;
  command += " ";
  command += uri;
  command += " ";
  command += db;
  command += " ";
  command += override;
  gSystem->Exec(command.Data());
  
  
}