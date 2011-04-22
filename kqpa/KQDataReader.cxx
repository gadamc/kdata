//_____________________________________________
//
// KQDataReader.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/24/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is supposed to read data from bolometer events,
// store it internally in a vector of KQDataRecords, which can then
// individually be retrieved by get methods

#include "KQDataReader.h"
#include <string.h>
#include <algorithm>

ClassImp(KQDataReader);

KQDataReader::KQDataReader(const Char_t* aSourceFile,
                           const Char_t* aBoloConfigFile,
                           const Char_t* aBoloName,
                           const Char_t* aCategoryName) 
{
  //constructor
  fSourceFile = aSourceFile;
  fBoloConfigFile = aBoloConfigFile;
  fBoloName = aBoloName;
  SetEventCategory(aCategoryName);
  fEnergyRecoilMin = 0;
  fEnergyRecoilMax = 1000;
}

Bool_t KQDataReader::ReadEvents(const Char_t* aSourceFile,
                                const Char_t* aBoloConfigFile,
                                const Char_t* aBoloName,
                                const Char_t* aCategoryName) 
{ 
  //This method reads the bolometer events in the specified source file
  //For each bolometer event, a KQDataRecord is filled 
  //which is appended to the vector<KQDataRecord> "fData".
  
  if(fSourceFile!=aSourceFile && strcmp(aSourceFile,""))
    fSourceFile = aSourceFile;
  if(fBoloConfigFile!=aBoloConfigFile && strcmp(aBoloConfigFile,""))
    fBoloConfigFile = aBoloConfigFile;
  if(fBoloName!=aBoloName && strcmp(aBoloName,""))
    fBoloName = aBoloName;
  SetEventCategory(aCategoryName); //setting the event category
    
  cout << "Reading events ... " << endl;
  
  KBoloConfig aBoloConfig(fBoloName.c_str(),fBoloConfigFile.c_str()); //bolometer configuration file
  if(!aBoloConfig.Load()) {
    cout << "KQDataReader::ReadEvents(): bolometer configuration file could \
    not be loaded" << endl;
    return false;
  }
  KDataReader aKDataReader(fSourceFile.c_str()); // source file
  KHLAEvent* aKHLAEvent = (KHLAEvent*)aKDataReader.GetEvent();
  if(!aKHLAEvent) {
    cout << "KQDataReader::ReadEvents(): no KHLAEvent found in source file "
    << endl;
    return false;
  }
  
  Int_t aNumEntries = aKDataReader.GetEntries();
  Int_t aNumBoloEvents = 0;
  
  if(aNumEntries==0) {
    cout << "KQDataReader::ReadEvents(): " <<  fSourceFile << ".root has no entries! " << endl;
    return false;
  }
  
  cout << "Open " << fSourceFile << endl;
  cout << "with " << aNumEntries << " entries " << endl;
  
  KHLABolometerRecord* aBoloRecord;
  if(fBoloName!="ALL")
    for(int k = 0; k<aNumEntries; ++k) {
      aKDataReader.GetEntry(k);
      if(!(k%100000))
        cout << "Entry " << k << endl;
      aNumBoloEvents += aKHLAEvent->GetNumBolos(); 
      for(int l = 0; l<aKHLAEvent->GetNumBolos(); ++l)  {
        aBoloRecord = static_cast<KHLABolometerRecord*>(aKHLAEvent->GetBolo(l));
        if(aBoloRecord->GetEventFlag()==fEventCategory &&
          aBoloRecord->GetDetectorName()==fBoloName &&
          aBoloRecord->GetEnergyRecoil()>= fEnergyRecoilMin &&
          aBoloRecord->GetEnergyRecoil()<= fEnergyRecoilMax) {
          Double_t aHeatUncertainty = KQUncertainty::GetChannelUncertainty(aBoloRecord->GetEnergyRecoil(),
                                                                     aBoloConfig.GetSigmaHeatZero(),
                                                                     aBoloConfig.GetSigmaHeatCalibration(),
                                                                     aBoloConfig.GetEnergyCalibration());
          Double_t anIonUncertainty = KQUncertainty::GetChannelUncertainty(aBoloRecord->GetEnergyRecoil(),
                                                                     aBoloConfig.GetSigmaIonZero(),
                                                                     aBoloConfig.GetSigmaIonCalibration(),
                                                                     aBoloConfig.GetEnergyCalibration());
                                                                     
            fData.push_back(KQDataRecord(aBoloRecord->GetEnergyRecoil(),
                                         aBoloRecord->GetQvalue(),
                                         aBoloRecord->GetEnergyIon(),
                                         aBoloRecord->GetEnergyHeat(0),
                                         anIonUncertainty,
                                         aHeatUncertainty));
        }
      }
    }
    else
    for(int k = 0; k<aNumEntries; ++k) {
      aKDataReader.GetEntry(k);
      if(!(k%100000))
  cout << "Entry " << k << endl;
      aNumBoloEvents += aKHLAEvent->GetNumBolos();
      for(int l = 0; l<aKHLAEvent->GetNumBolos(); ++l) {
        aBoloRecord = static_cast<KHLABolometerRecord*>(aKHLAEvent->GetBolo(l));
        if(aBoloRecord->GetEventFlag()==fEventCategory &&
          aBoloRecord->GetEnergyRecoil()>= fEnergyRecoilMin &&
          aBoloRecord->GetEnergyRecoil()<= fEnergyRecoilMax) {
          Double_t aHeatUncertainty = KQUncertainty::GetChannelUncertainty(aBoloRecord->GetEnergyRecoil(),
                                                                     aBoloConfig.GetSigmaHeatZero(),
                                                                     aBoloConfig.GetSigmaHeatCalibration(),
                                                                     aBoloConfig.GetEnergyCalibration());
          Double_t anIonUncertainty = KQUncertainty::GetChannelUncertainty(aBoloRecord->GetEnergyRecoil(),
                                                                     aBoloConfig.GetSigmaIonZero(),
                                                                     aBoloConfig.GetSigmaIonCalibration(),
                                                                     aBoloConfig.GetEnergyCalibration());
          fData.push_back(KQDataRecord(aBoloRecord->GetEnergyRecoil(),
                                       aBoloRecord->GetQvalue(),
                                       aBoloRecord->GetEnergyIon(),
                                       aBoloRecord->GetEnergyHeat(0),
                                       anIonUncertainty,
                                       aHeatUncertainty));

                                                                     
        }
      }
    }

  cout << "All bolometer events: " << aNumBoloEvents << endl;
  cout << GetEventCategory() << " events in detector " << fBoloName << ": " << fData.size() << endl;
  
  if(!fData.size()) {
    cout << "KQDataReader::ReadEvents(): " << "no bolometer events found, nothing to be done! " << endl;
    return false;
  }
  
  sort(fData.begin(),fData.end());
  return true;
} 

void KQDataReader::SetEventCategory(const Char_t* anEventCategory) 
{
  // sets an event category
  
  if(strcmp(anEventCategory,"noise")==0)
    fEventCategory = 0;
  else
    if(strcmp(anEventCategory,"heat_only")==0)
      fEventCategory = 1;
    else
      if(strcmp(anEventCategory,"fiducial")==0)
        fEventCategory = 2;
      else
        if(strcmp(anEventCategory,"surface")==0)
          fEventCategory = 3;
        else
          if(strcmp(anEventCategory,"pure_guard")==0)
            fEventCategory = 4;
          else
            if(strcmp(anEventCategory,"other")==0)
              fEventCategory = 5;
            else
            {
              cout << "KQDataReader::SetEventCategory(): invalid event category " << endl;
              cout << "valid values are " << endl;
              cout << "\"noise\"" << endl;
              cout << "\"heat_only\"" << endl;
              cout << "\"fiducial\"" << endl;
              cout << "\"surface\"" << endl;
              cout << "\"pure_guard\"" << endl;
              cout << "\"other\"" << endl;
            }
}



const Char_t* KQDataReader::GetEventCategory() const
{
  // gets a C string for current event category
  
  switch(fEventCategory) {
    case 0: return("noise"); break;
    case 1: return("heat_only"); break;
    case 2: return("fiducial"); break;
    case 3: return("surface"); break;
    case 4: return("pure_guard"); break;
    case 5: return("other"); break;
    default: return(""); break;
  }
}

KQDataRecord KQDataReader::GetDataRecord(UInt_t anIndex) const
{
  //gets the data record at the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataRecord::GetDataRecord(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return KQDataRecord();
  }
  else
    return(fData[anIndex]);
}

Double_t KQDataReader::GetEnergyRecoil(UInt_t anIndex) const 
{
  //gets the recoil energy for the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataReader::GetEnergyRecoil(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fEnergyRecoil);
}

Double_t KQDataReader::GetEnergyHeat(UInt_t anIndex) const 
{
  //gets the heat energy for the specified entry
  if(anIndex >= fData.size()) {
    cout <<  "KQDataReader::GetEnergyHeat(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fEnergyHeat);
}

Double_t KQDataReader::GetEnergyIon(UInt_t anIndex) const
{
  //gets the ion energy for the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataReader::GetEnergyIon(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fEnergyIon);
}

Double_t KQDataReader::GetQvalue(UInt_t anIndex) const 
{
  //gets the Q value for the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataReader::GetQvalue(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fQ);
}

Double_t KQDataReader::GetSigmaIon(UInt_t anIndex) const 
{
  //gets the Q value for the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataReader::GetSigmaIon(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fSigmaIon);
}

Double_t KQDataReader::GetSigmaHeat(UInt_t anIndex) const 
{
  //gets the uncertainty on the heat channel for the specified entry
  if(anIndex >= fData.size()) {
    cout << "KQDataReader::GetSigmaHeat(): invalid index, must be in [0,"
    << fData.size()-1 << endl;
    return -1;
  }
  else
    return(fData[anIndex].fSigmaHeat);
}


    

