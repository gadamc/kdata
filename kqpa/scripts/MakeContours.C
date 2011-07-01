//_____________________________________________
//
// MakeContours.C
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 6/30/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This script shows the usage of the KQContourPointList class
// Single bolometer events (KHLABolometerRecord) are loaded from a KHLAEvent
// ROOT file and the corresponding quantities (E_Ion, EHeat, Sigma_E_Ion,
// Sigma_E_Heat with specified restrictions are written to an ASCII file,
// which is then loaded by the KQContourPointList and then the events with
// 90% CL contour lines with markers for central values are drawn

#ifndef __CINT__
#include "KQContourPointList.h"
#include "KDataReader.h"
#include "KHLAEvent.h"
#include "KHLABolometerRecord.h"
#include "TFile.h"
#include "TCut.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
using namespace std;
#else
class KQContourPointList;
class KDataReader;
class KHLAEvent;
class KHLABolometerRecord;
#endif






KDataReader* aReader = 0;
KHLAEvent* anHLAEvent = 0;
KHLABolometerRecord* aBoloEvent = 0;
KQContourPointList* aList = 0;


void MakeContours(const Char_t* aKEventROOTFile,
                  const Char_t* aDetectorName ="FID401",
                  Int_t anEventCategory = 2,
                  Double_t anEnergyIonMin = 0,
                  Double_t anEnergyIonMax = 100,
                  Double_t anEnergyHeatMin = 0,
                  Double_t anEnergyHeatMax = 100
                  )
{
  Double_t anEnergyIon;
  Double_t anEnergyHeat;
  Double_t aSigmaEnergyIon;
  Double_t aSigmaEnergyHeat;
  Double_t aQvalue;
  Double_t anEnergyRecoil;
  aReader = new KDataReader(aKEventROOTFile);
  ofstream os("events.txt");
  anHLAEvent = (KHLAEvent*)aReader->GetEvent();
  Int_t anEventCounter = 0;
  cout << "KHLAEvents total: " << aReader->GetEntries() << endl;
  for(Int_t k = 0; k<aReader->GetEntries(); ++k) {
    aReader->GetEntry(k);
    for(Int_t l = 0; l< anHLAEvent->GetNumBolos(); ++l) {
      aBoloEvent = static_cast<KHLABolometerRecord*>(anHLAEvent->GetBolo(l));
      anEnergyIon = aBoloEvent->GetEnergyIon();
      anEnergyHeat = aBoloEvent->GetEnergyHeat(1);
      aSigmaEnergyIon = aBoloEvent->GetBaselineNoiseIonFiducial();
      aSigmaEnergyHeat = aBoloEvent->GetBaselineNoiseHeat(1);
      aQvalue = aBoloEvent->GetQvalue();
      anEnergyRecoil = aBoloEvent->GetEnergyRecoil();
      
      if(aBoloEvent->GetEventFlag()==anEventCategory)
        if(aBoloEvent->GetDetectorName()==aDetectorName)
          if(anEnergyIonMin <= anEnergyRecoil && anEnergyRecoil <= anEnergyIonMax
          && anEnergyHeatMin <= aQvalue && aQvalue <= anEnergyHeatMax) {
        os << aQvalue << " ";
        os << anEnergyRecoil << " ";
        os << aSigmaEnergyIon << " ";
        os << TMath::Sqrt(anEnergyRecoil) << " ";
        os << endl;
        cout << aQvalue << " ";
        cout << anEnergyRecoil << " ";
        cout << aSigmaEnergyIon << " ";
        cout << TMath::Sqrt(anEnergyRecoil) << " ";
        cout << endl;
        ++anEventCounter;
      } //if   
    } //for  
  } //for
  cout << "events: " << anEventCounter << endl;
  aList = new KQContourPointList("QErecoil","events.txt");
  aList->ReadASCIIFile("events.txt","");
  aList->Draw();
}