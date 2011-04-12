/**
   @file    EdwDB.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwDB class. See the .h file
*/

#include "EdwDB.h"

ClassImp(EdwDB); /**< Root dictionnary */

EdwDB::EdwDB() : TObject() {
  fFileName = "edwdb.root" ;
  fFile = new TFile(fFileName.c_str(),"RECREATE");

}

EdwDB::EdwDB(string aFile, string aRead ) : TObject() {
  if (aRead != "READ" && aRead != "RECREATE" && aRead != "UPDATE") {
    cerr << "EdwDB: wrong aRead flag" << endl; exit(-1);
  }
  fFileName = aFile ;
  fFile = new TFile(fFileName.c_str(),aRead.c_str());

  if (fFile->IsZombie()) {
    cerr << "EdwDB: fFile is zombie. Exiting." << endl; exit(-1);
  }
}

EdwDB::~EdwDB() {
  if (fFile->IsZombie()) cerr << "EdwDB destructor: fFile is zombie." <<endl;
  if (fFile->IsOpen()) fFile->Close() ;
  delete fFile ;
  // delete fTree ;//done actually when closing the file because the file owns the tree

}

void EdwDB::CloseFile() {
  if (fFile->IsZombie()) cerr << "EdwDB::CloseFile: fFile is zombie." <<endl;
  if (fFile->IsOpen()) fFile->Close() ;
}
