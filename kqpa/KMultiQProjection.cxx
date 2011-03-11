//_____________________________________________
//
// KMultiQProjection.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/11/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class stores multiple KQProjection objects created from the same
// source file and with same bolometer name and event category, but different
// recoil energy ranges
// 
// example:
//
// theEnergyRecoilMins[] = { 0, 100, 200, 300, 400, 500 ,600 };
// theEnergyRecoilMaxs[] = { 100, 200, 300, 400, 500, 600, 700 };
// Int_t aNumProjections = 7;
//
// KMultiQProjection aMultiQProjection(
//    "/kalinka/storage/edelweiss/Bolo/Run12/Eds/Final/Gamma/Kds_Run12_v.3.0_skim.root"
//    "FID401",
//    aNumProjections,
//    theEnergyRecoilMins,
//    theEnergyRecoilMaxs,
//    "fiducial");
//
// TH1D* hist = aMultiQProjection.GetProjection(5);
// hist->Draw();

#include "KMultiQProjection.h"

ClassImp(KMultiQProjection);

KMultiQProjection::KMultiQProjection(const Char_t* aSourceFile,
                      const Char_t* aBoloName,
                      Int_t aNumProjections,
                      Double_t* theEnergyRecoilMins,
                      Double_t* theEnergyRecoilMaxs,
                      const Char_t* anEventCategory)
{
  fNumBinsEnergyRecoil = 1000;
  fNumBinsQ = 1000;
  fQMin = 0;
  fQMax = 2;
  
  ReadData(aSourceFile,aBoloName,aNumProjections,theEnergyRecoilMins,
           theEnergyRecoilMaxs,anEventCategory);
  /*
  for(Int_t k = 0; k<aNumProjections; ++k) {
    fProjections.push_back(KQProjection(aSourceFile,
                                        aBoloName,
                                        theEnergyRecoilMins[k],
                                        theEnergyRecoilMaxs[k],
                                        anEventCategory,
                                        TString::Format("hist %i",k).Data()));
  }
  */
}

KMultiQProjection::~KMultiQProjection() 
{
  for(UInt_t k = 0; k<fHistograms.size(); ++k)
    if(fHistograms[k])
      delete fHistograms[k];
  fHistograms.clear();
}

Bool_t KMultiQProjection::ReadData(const Char_t* aSourceFile,
                                   const Char_t* aBoloName,
                                   Int_t aNumProjections,
                                   Double_t* theEnergyRecoilMins,
                                   Double_t* theEnergyRecoilMaxs,
                                   const Char_t* anEventCategory)
{
  for(UInt_t k = 0; k<fHistograms.size(); ++k)
    if(fHistograms[k])
      delete fHistograms[k];
  fHistograms.clear();
    
  fNumProjections = aNumProjections;

  for(Int_t k = 0; k<fNumProjections; ++k) {
    fHistograms.push_back(new TH2D(TString::Format("hist %i",k).Data(),
                                    TString::Format("Hist %i",k).Data(),
                                    fNumBinsEnergyRecoil,
                                    theEnergyRecoilMins[k],
                                    theEnergyRecoilMaxs[k],
                                    fNumBinsQ,
                                    fQMin,
                                    fQMax));
  }

  if(fSourceFile!=aSourceFile && strcmp(aSourceFile,""))
    fSourceFile = aSourceFile;
  if(fBoloName!=aBoloName && strcmp(aBoloName,""))
    fBoloName = aBoloName;
  SetEventCategory(anEventCategory); //setting the event category
    
  cout << "Reading events ... " << endl;
  

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
  cout << "recoil energy bins: " << fNumBinsEnergyRecoil << endl;
  cout << "Q bins: " << fNumBinsQ << endl;
  cout << "event category: " << GetEventCategory() << endl;
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
        aBoloRecord = aKHLAEvent->GetBolo(l);
        if(aBoloRecord->GetEventFlag()==fEventCategory &&
          aBoloRecord->GetDetectorName()==fBoloName)
            for(UInt_t m = 0; m<fHistograms.size(); ++m)
              if(aBoloRecord->GetEnergyRecoil()>= theEnergyRecoilMins[m] &&
                 aBoloRecord->GetEnergyRecoil()<= theEnergyRecoilMaxs[m]) {                                                            
                  fHistograms[m]->Fill(aBoloRecord->GetEnergyRecoil(),
                                       aBoloRecord->GetQvalue());
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
        aBoloRecord = aKHLAEvent->GetBolo(l);
        if(aBoloRecord->GetEventFlag()==fEventCategory)
            for(UInt_t m = 0; m<fHistograms.size(); ++m)
              if(aBoloRecord->GetEnergyRecoil()>= theEnergyRecoilMins[m] &&
                 aBoloRecord->GetEnergyRecoil()<= theEnergyRecoilMaxs[m]) {                                                            
                  fHistograms[m]->Fill(aBoloRecord->GetEnergyRecoil(),
                                       aBoloRecord->GetQvalue());
                                                                     
        }
      }
    }

  cout << "All bolometer events: " << aNumBoloEvents << endl;
  for(UInt_t k = 0; k<fHistograms.size(); ++k)
    cout << GetEventCategory() << " events in detector " << fBoloName << " in range [" << theEnergyRecoilMins[k] << "," << theEnergyRecoilMaxs[k] << "] : "
    << fHistograms[k]->GetEntries() << endl;
  return true;
} 

const Char_t* KMultiQProjection::GetEventCategory() const
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

void KMultiQProjection::SetEventCategory(const Char_t* anEventCategory) 
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


