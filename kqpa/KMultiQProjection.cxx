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
//    "fiducial");
// 
// aMultiQProjection.ReadData(aNumProjections,
//                            theEnergyRecoilMins,
//                            theEnergyRecoilMaxs);
//
// TH1D* hist = aMultiQProjection.GetProjection(5);
// hist->Draw();

#include "KMultiQProjection.h"

ClassImp(KMultiQProjection);

KMultiQProjection::KMultiQProjection(const Char_t* aSourceFile,
                                     const Char_t* aBoloName,
                                     const Char_t* anEventCategory,
                                     Double_t aNumBinsEnergyRecoil,
                                     Double_t aNumBinsQ,
                                     Double_t aQMin,
                                     Double_t aQMax)
{
  fVerbose = false;
  fSourceFile = aSourceFile;
  fBoloName = aBoloName;
  SetEventCategory(anEventCategory);
  fNumBinsEnergyRecoil = aNumBinsEnergyRecoil;
  fNumBinsQ = aNumBinsQ;
  fQMin = aQMin;
  fQMax = aQMax;

}

KMultiQProjection::~KMultiQProjection() 
{
  for(UInt_t k = 0; k<fHistograms.size(); ++k)
    if(fHistograms[k])
      delete fHistograms[k];
  fHistograms.clear();
}

Bool_t KMultiQProjection::ReadData(Int_t aNumProjections,
                                   Double_t* theEnergyRecoilMins,
                                   Double_t* theEnergyRecoilMaxs)
{
  for(UInt_t k = 0; k<fHistograms.size(); ++k)
    if(fHistograms[k])
      delete fHistograms[k];
  fHistograms.clear();
    
  fNumProjections = aNumProjections;

  for(Int_t k = 0; k<fNumProjections; ++k) {
    fHistograms.push_back(new KQProjection(fSourceFile.c_str(),
                                           fBoloName.c_str(),
                                           GetEventCategoryName(),
                                           theEnergyRecoilMins[k],
                                           theEnergyRecoilMaxs[k],
                                           fNumBinsEnergyRecoil,
                                           fNumBinsQ,
                                           fQMin,
                                           fQMax,
                                           TString::Format("hist %i",k).Data()));
  }

  
  if(fVerbose)
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

  if(fVerbose) {
    cout << "Open " << fSourceFile << endl;
    cout << "with " << aNumEntries << " entries " << endl;
  }
  
  
  KHLABolometerRecord* aBoloRecord;
  if(fBoloName!="ALL")
    for(int k = 0; k<aNumEntries; ++k) {
      aKDataReader.GetEntry(k);
      if(!(k%100000)&&fVerbose)
        cout << "Entry " << k << endl;
      aNumBoloEvents += aKHLAEvent->GetNumBolos(); 
      for(int l = 0; l<aKHLAEvent->GetNumBolos(); ++l)  {
        aBoloRecord = aKHLAEvent->GetBolo(l);
        for(UInt_t m = 0; m<fHistograms.size(); ++m)
          if(aBoloRecord->GetEventFlag()==fHistograms[m]->GetEventCategory() &&
            aBoloRecord->GetDetectorName()==fHistograms[m]->GetBoloName())
              if(aBoloRecord->GetEnergyRecoil()>= fHistograms[m]->GetEnergyRecoilMin() &&
                 aBoloRecord->GetEnergyRecoil()<= fHistograms[m]->GetEnergyRecoilMax()) {                                                            
                  fHistograms[m]->Fill(aBoloRecord->GetEnergyRecoil(),
                                       aBoloRecord->GetQvalue());
        }
      }
    }
    else
    for(int k = 0; k<aNumEntries; ++k) {
      aKDataReader.GetEntry(k);
      if(!(k%100000)&&fVerbose)
  cout << "Entry " << k << endl;
      aNumBoloEvents += aKHLAEvent->GetNumBolos();
      for(int l = 0; l<aKHLAEvent->GetNumBolos(); ++l) {
        aBoloRecord = aKHLAEvent->GetBolo(l);
        for(UInt_t m = 0; m<fHistograms.size(); ++m)
          if(aBoloRecord->GetEventFlag()==fHistograms[m]->GetEventCategory())
            if(aBoloRecord->GetEnergyRecoil()>= fHistograms[m]->GetEnergyRecoilMin() &&
               aBoloRecord->GetEnergyRecoil()<= fHistograms[m]->GetEnergyRecoilMax()) {                                                            
                  fHistograms[m]->Fill(aBoloRecord->GetEnergyRecoil(),
                                       aBoloRecord->GetQvalue());
                                                                     
        }
      }
    }

  if(fVerbose) {
    cout << "All bolometer events: " << aNumBoloEvents << endl;
    for(UInt_t k = 0; k<fHistograms.size(); ++k)
      cout << fHistograms[k]->GetEntries() << " " << 
      fHistograms[k]->GetEventCategoryName() << " events in detector " <<
      fHistograms[k]->GetBoloName() <<  " in range [" << 
      fHistograms[k]->GetEnergyRecoilMin() << "," << 
      fHistograms[k]->GetEnergyRecoilMax() <<
      "] " << endl;
  }
  return true;
} 

const Char_t* KMultiQProjection::GetEventCategoryName() const
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


