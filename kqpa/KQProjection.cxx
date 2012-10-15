//_____________________________________________
//
// KQProjection.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/10/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class fills a TH2D histogram with (ERecoil, Q)-tuples of bolometer events
// The events are read from a ROOT file with KEvents, and only those filled in
// the histogram which are in the range of the histogram [Emin,Emax]
// 
// By calling the ReadData() method the user can fill
// respectively refill the  histogram
// Then with the methods GetHistogram(), GetProjection() the TH2D or its
// projection on the Q axis can be retrieved
//
// example:
//
// KQProjection aQProjection("/kalinka/storage/edelweiss/Bolo/Run12/Eds/Final/Gamma/Kds_Run12_v3.0_skim.root",
//                           "FID401",
//                           "fiducial");
//
// Double_t anEnergyRecoilMin = 0;
// Double_t anEnergyRecoilMax = 1000;
// aQProjection.ReadData(anEnergyRecoilMin,anEnergyRecoilMax);
// aQProjection.MakeHistogram();
// TH1D* aHistogram = aQProjection.GetProjection();
// aHistogram->Draw();


#include "KQProjection.h"

ClassImp(KQProjection);



KQProjection::KQProjection()
{
  fEnergyRecoilMin = 0;
  fEnergyRecoilMax = 1000;
  fNumBinsEnergyRecoil = 1000;
  fQMin = 0;
  fQMax = 0.6;
  fNumBinsQ = 1000;
  
  
  fHistogram = new TH2D("hist", //name
                        "hist", //title
                        fNumBinsEnergyRecoil, //recoil energy bins
                        fEnergyRecoilMin, // minimal recoil energy
                        fEnergyRecoilMax, //maximal recoil energy
                        fNumBinsQ, // Q bins
                        fQMin, // minimal Q
                        fQMax); // maximal Q
}

KQProjection::KQProjection(const KQProjection& anotherKQProjection)
{
  fIsDataRead = anotherKQProjection.fIsDataRead;
  fVerbose = anotherKQProjection.fVerbose;
  fHistogram = new TH2D(*(anotherKQProjection.fHistogram));
  fEnergyRecoilMin = anotherKQProjection.fEnergyRecoilMin;
  fEnergyRecoilMax = anotherKQProjection.fEnergyRecoilMax;
  fNumBinsEnergyRecoil = anotherKQProjection.fNumBinsEnergyRecoil;
  fEventCategory = anotherKQProjection.fEventCategory;
  fQMin = anotherKQProjection.fQMin;
  fQMax = anotherKQProjection.fQMax;
  fNumBinsQ = anotherKQProjection.fNumBinsQ;
  fSourceFile = anotherKQProjection.fSourceFile;
  fBoloName = anotherKQProjection.fBoloName;
  fData = anotherKQProjection.fData;
  
}



KQProjection::KQProjection(const Char_t* aSourceFile,
                           const Char_t* aBoloName,
                           const Char_t* aCategoryName,
                           Double_t aNumBinsEnergyRecoil,
                           Double_t anEnergyRecoilMin,
                           Double_t anEnergyRecoilMax,
                           Double_t aNumBinsQ,
                           Double_t aQMin,
                           Double_t aQMax,
                           const Char_t* aHistogramName)
{
  fVerbose = true;
  fSourceFile = aSourceFile;
  fBoloName = aBoloName;
  SetEventCategory(aCategoryName);
  fEnergyRecoilMin = anEnergyRecoilMin;
  fEnergyRecoilMax = anEnergyRecoilMax;
  fNumBinsEnergyRecoil = aNumBinsEnergyRecoil;
  fNumBinsQ = aNumBinsQ;
  fQMin = aQMin;
  fQMax = aQMax;
  
  fHistogram = new TH2D(aHistogramName,
                        TString::Format("E_{Recoil}: %lf .. %lf",fEnergyRecoilMin,
                                fEnergyRecoilMax).Data(),
                        fNumBinsEnergyRecoil,fEnergyRecoilMin,
                        fEnergyRecoilMax, fNumBinsQ,fQMin,fQMax);
  

}

KQProjection::~KQProjection() 
{
  if(fHistogram)
    delete fHistogram;
}


Bool_t KQProjection::ReadData(Double_t anEnergyRecoilMin,
                              Double_t anEnergyRecoilMax)
{
  fEnergyRecoilMin = anEnergyRecoilMin;
  fEnergyRecoilMax = anEnergyRecoilMax;
  fHistogram->SetTitle(TString::Format("E_{Recoil}: %lf .. %lf",fEnergyRecoilMin,
                                       fEnergyRecoilMax).Data());
  fHistogram->Reset();
  fHistogram->GetXaxis()->SetRangeUser(anEnergyRecoilMin,
                                       anEnergyRecoilMax);
                                       
  fData.clear();
  

  
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
    cout << "minimal recoil energy: " << fEnergyRecoilMin << endl;
    cout << "maximal recoil energy: " << fEnergyRecoilMax << endl;
    cout << "recoil energy bins: " << fNumBinsEnergyRecoil << endl;
    cout << "Q bins: " << fNumBinsQ << endl;
    cout << "event category: " << GetEventCategoryName() << endl;
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
        aBoloRecord = static_cast<KHLABolometerRecord*>(aKHLAEvent->GetBolo(l));
        if(aBoloRecord->GetEventFlag()==fEventCategory &&
          aBoloRecord->GetDetectorName()==fBoloName &&
          aBoloRecord->GetEnergyRecoil()>= fEnergyRecoilMin &&
          aBoloRecord->GetEnergyRecoil()<= fEnergyRecoilMax &&
          aBoloRecord->GetQvalue()<=fQMax &&
          aBoloRecord->GetQvalue()>=fQMin) {   
            fData.push_back(KQDataRecord(aBoloRecord->GetEnergyRecoil(),
                                         aBoloRecord->GetQvalue()));
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
        aBoloRecord = static_cast<KHLABolometerRecord*>(aKHLAEvent->GetBolo(l));
        if(aBoloRecord->GetEventFlag()==fEventCategory &&
          aBoloRecord->GetEnergyRecoil()>= fEnergyRecoilMin &&
          aBoloRecord->GetEnergyRecoil()<= fEnergyRecoilMax &&
          aBoloRecord->GetQvalue()<=fQMax &&
          aBoloRecord->GetQvalue()>=fQMin) {
            fData.push_back(KQDataRecord(aBoloRecord->GetEnergyRecoil(),
                                         aBoloRecord->GetQvalue()));                         
        }
      }
    }

  if(fVerbose) {
    cout << "All bolometer events: " << aNumBoloEvents << endl;
    cout << GetEventCategoryName() << " events in detector " << fBoloName << " in specified range: "
    << fData.size() << endl;
  }
  
  sort(fData.begin(),fData.end());
  
  fIsDataRead = true;
   
  return true;
} 

Bool_t KQProjection::MakeHistogram()
{
  if(!fIsDataRead)
    ReadData(fEnergyRecoilMin,fEnergyRecoilMax);
  
  fHistogram->Reset();
  for(UInt_t k = 0; k< fData.size(); ++k)
    fHistogram->Fill(fData[k].GetEnergyRecoil(),fData[k].GetQvalue());

  return true;
}
  
  


void KQProjection::SetEventCategory(const Char_t* anEventCategory) 
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



const Char_t* KQProjection::GetEventCategoryName() const
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

void KQProjection::Fit(TF1* aFunction) {
  fHistogram->ProjectionY()->Fit(aFunction);
}

Int_t KQProjection::Fill(Double_t x,Double_t y,Double_t w) 
{
  return fHistogram->Fill(x,y,w);
}