//_____________________________________________
//
// KDataFileIO.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 5/11/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The base class for the KDataReader and KDataWriter classes. This class
// does the basic tasks of opening the TFile and setting the branch addresses
// The user should not interact directly with this class, except for the public
// methods that KDataReader/Writer inherit. However, using the methods in this 
// class shouldn't be necessary to perform an analysis of the data stored in an
// KData file. 
// This is an abstract class. 

#include "KDataFileIO.h"
#include <exception>
#include <iostream>
#include <string>
#include "TBranchElement.h"

using namespace std;

ClassImp(KDataFileIO);

KDataFileIO::KDataFileIO(void)
{
  InitializeMembers();

  //fFile = 0;
  //fChain = 0;
}

KDataFileIO::~KDataFileIO(void)
{
  //if (fChain != 0) delete fChain; //

  //Delete(); //out commented due to python script chrash
  //I have to test if Delete() (delete fFile) takes care of the Tchain itself; 
  //maybe the TChain is also associated to the current File and deleted once delete fFile is called
}

void KDataFileIO::InitializeMembers(void)
{
  //
  //

  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES

  //THESE ARE DEFAULT VALUES AND SHOULD NEVER BE CHANGED!
  fTreeName = "t";
  fTreeTitle = "KDSTree";
  //fChainTitle = "KDSChain";
  fBranchName = "event";

}

/*
void KDataFileIO::Delete(void)
{

  Close();
  if(fFile != 0) {
    delete fFile; 
    fFile = 0;
  }

}*/


/*
void KDataFileIO::CreateTree(void)
{

  fFile->cd();

  
  if(fChain != 0) {
    delete fChain; fChain = 0;
  }

  fChain = new TChain(fTreeName.c_str(), fChainTitle.c_str());


  if(fChain->IsZombie())
    cout << "KDataFileIO::CreateTree TChain is Zombie" << endl;  
}
*/

/*
void KDataFileIO::GetTreePointerInFile(const Char_t* name)
{

  if (fFile == 0){
    cout << "KDataFileIO - File Pointer is NULL" << endl;
    return;
  }

  fFile->cd();


  //fTree = (TChain *)fFile->Get(fTreeName.c_str());
  if(fChain!=0){
    cout << "Calling fChain " << fChain << " AddFile(\"" << name << "\");" << endl,
      fChain->AddFile(name);
  }
  else{
    if(fChain==0){
      CreateTree();
      fChain->AddFile(name); //Create Tree does print some error messegas to screen if it fails, but 
                             // it does not catch any exception, use an if statement to check again?
    }
    if(fChain->IsZombie()){
      cout << "KDataFileIO::GetTreePointerInFile. TChain is Zombie" << endl; 
    }
}

*/



/*
Bool_t KDataFileIO::Close(Option_t *opt)
{
  //

  //every time we close the file, we delete the Event pointer. 

  if(fFile == 0){
    fChain=0; //the chain should have been deleted
    return true;
  }

  if(fFile->IsOpen())
    fFile->Close(opt);

  if (fFile->IsOpen()) {
    return false;
    //return false if we couldn't close the file.
  }
  else {
    delete fFile; 
    fFile = 0;
    fChain= 0; //the chain is deleted by the TFile
    return true;
  }

}
*/
/*


TFile* KDataFileIO::OpenFileForWriting(const Char_t* name, const Char_t* option, 
  const Char_t* title)
{

  fFile =  TFile::Open(name, option, title); 
  //MUST do this after initializing the TFile
  CreateTree();

  return fFile;
}

TFile* KDataFileIO::OpenFileForReading(const Char_t* name)
{

  fFile = TFile::Open(name, "read");
  if (fFile == 0){
    cout << "KDataFileIO - Unable to Open file: " << name << endl;
    return fFile;
  }
  GetTreePointerInFile(name);

  return fFile;
}

Int_t KDataFileIO::GetEntries(void) const 
{
  if(fChain!=0) return fChain->GetEntries();
  else return -1;
}

const char* KDataFileIO::GetFileName(void) const
{
  if (fFile != 0) {
    return fFile->GetName();
  }
  else return "";

}

const char* KDataFileIO::GetEventClassName(void) const
{
  if(fChain != 0){
    TBranchElement *branch = (TBranchElement *)fTree->GetBranch(fBranchName.c_str());
    if(branch != 0)
      return branch->GetClassName();

    else return "";
  }
  else return "";
}


void KDataFileIO::ls(Option_t *anOpt) const
{
  if(fFile != 0){
    fFile->ls(anOpt);
  }

}

Bool_t KDataFileIO::cd(const char *path)
{
  if(fFile != 0){
    return fFile->cd(path);
  }
  else return false;
}


*/
