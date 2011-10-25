//______________________________________________________________________________
//
// KDataProcessingInfo
//
// Class that holds information about the processing of a run.  This can be
// stored in a TTree, or in a TTree::GetUserInfo().  In particular, it stores
// the following information: 
//
//   1. Processing Time (Date)
//   2. Modules used in processing 
//   3. Commands used in processing 
//   4. SVN revision of KData
//   4. Whether calibration data used in this processing came from the database
// 
// This is stored by KDataWriter in the TTree::GetUserInfo() list.
// This means that it accessible by doing: 
// 
//    KDataProcessingInfo* proc_info = (KDataProcessingInfo*)tree->GetUserInfo()->First();
// 
// Processors (such as rootification of the data, KSamba2KData, and estimation of the pulse
// ampitudes that are managed by the KAmpKounselor) should add and append information to 
// this object before closing out the TFile.
//
// Processing information will be chained so that previous processing
// information will also be saved, see GetPrevProcInfo(); 
//
// Created by Mike G. Marino 2011. Part of the EXO offline analysis code base.
// Some minor modifications were made by Adam Cox (such as changing the name)
//
#include "KDataProcessingInfo.h"
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;

ClassImp( KDataProcessingInfo )
//______________________________________________________________________________
void KDataProcessingInfo::Print(Option_t * opt) const
{
  // Dump out information to the screen.  This will also print out information
  // from previous processing.
  if (fPrevProc) fPrevProc->Print(opt);
  cout << "********************************************************" << endl;
  cout << "*****************KDataProcessingInfo Dump*****************" << endl;
  cout << "SVN Revision: " << GetSVNRev() << endl; 
  cout << "Start Date of Processing: " << GetStartTimeOfProcess().AsString(opt) << endl; 
  if(fCalibrationsFromDatabase) cout << "All calibration data came from the database." << endl;
  else cout << "At least one calibration handler used default values." << endl;
  cout << endl;
  cout << "Modules used:" << endl;
  for (size_t i=0;i<GetModulesUsed().size();i++) {
    cout << "  " << GetModulesUsed()[i] << endl;
  }
  cout << endl;
  cout << "Commands called:" << endl;
  for (size_t i=0;i<GetCommandsCalled().size();i++) {
    cout << "  " << GetCommandsCalled()[i] << endl;
  }
  cout << endl;
  cout << "Host Information:" << endl;
  cout << "  " << GetHostName() << endl;
  cout << "  " << GetHostInfo() << endl;
  cout << endl;
  cout << "Processed by:" << endl;
  cout << "  " << fUser << endl;
  cout << "********************************************************" << endl;
}

//______________________________________________________________________________
void KDataProcessingInfo::SetPrevProcInfo( const KDataProcessingInfo& prev ) 
{
  // Set processing information from a previous run.  This copies the data.
  delete fPrevProc;
  fPrevProc = new KDataProcessingInfo(prev);
}

//______________________________________________________________________________
KDataProcessingInfo::KDataProcessingInfo(const KDataProcessingInfo& other) : 
  TObject(other), fPrevProc(0)
{
  // Copy constructor
  fSVNRevision = other.fSVNRevision;
  fVectorOfModules = other.fVectorOfModules;
  fCommandsCalled = other.fCommandsCalled;
  fStartTimeOfProcess = other.fStartTimeOfProcess;
  fCalibrationsFromDatabase = other.fCalibrationsFromDatabase;
  if (other.fPrevProc) {
    fPrevProc = new KDataProcessingInfo(*other.fPrevProc);
  } 
}
//______________________________________________________________________________
KDataProcessingInfo& KDataProcessingInfo::operator=(const KDataProcessingInfo& other)
{
  // Copy operator 
  fSVNRevision = other.fSVNRevision;
  fVectorOfModules = other.fVectorOfModules;
  fCommandsCalled = other.fCommandsCalled;
  fStartTimeOfProcess = other.fStartTimeOfProcess;
  fCalibrationsFromDatabase = other.fCalibrationsFromDatabase;
  if(fPrevProc) delete fPrevProc;
  if (other.fPrevProc) {
    fPrevProc = new KDataProcessingInfo(*other.fPrevProc);
  } else fPrevProc = NULL; 
  return *this;
}
//______________________________________________________________________________
void KDataProcessingInfo::SetStartTimeOfProcess(TTimeStamp &aTimeStamp)
{
  fStartTimeOfProcess = aTimeStamp;
} 

//______________________________________________________________________________
void KDataProcessingInfo::SetAllModules( const std::string& mods )
{
  // Overload to handle a white-space separated list of modules.
  std::istringstream os(mods);
  std::string temp;
  fVectorOfModules.clear();
  while (os >> temp) fVectorOfModules.push_back(temp);
}
//______________________________________________________________________________
void KDataProcessingInfo::SetAllCommands( const std::string& cmds )
{
  // Overload to handle a list separated by endlines.
  std::istringstream os(cmds);
  std::string temp;
  fCommandsCalled.clear();
  while (std::getline(os, temp)) fCommandsCalled.push_back(temp);
}