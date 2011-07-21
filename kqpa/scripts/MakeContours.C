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
#include "KQUncertainty.h"
#include "KRun12Temp.h"
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
                  Double_t anEnergyRecoilMin = 0,
                  Double_t anEnergyRecoilMax = 100,
                  Double_t aQvalueMin = 0,
                  Double_t aQvalueMax = 1
                  )
{
  KRun12Temp calib("BoloConfigFile.txt");
  calib.ReadCalibrationFile("BoloConfigFile.txt");
  Int_t aDetectorIndex;
  Double_t anEnergyIon;
  Double_t anEnergyHeat;
  Double_t aSigmaEnergyIon;
  Double_t aSigmaEnergyHeat;
  Double_t aSigmaEnergyIon356;
  Double_t aSigmaEnergyHeat356;
  Double_t aQvalue;
  Double_t anEnergyRecoil;
  aReader = new KDataReader(aKEventROOTFile);
  ofstream os("events.txt");
  anHLAEvent = (KHLAEvent*)aReader->GetEvent();
  Int_t anEventCounter = 0;
  cout << "KHLAEvents total: " << aReader->GetEntries() << endl;
  for(Int_t k = 0; k<aReader->GetEntries(); ++k) {
    aReader->GetEntry(k);
    if(!(k%100000))
      cout << "entry " << k << endl;
    for(Int_t l = 0; l< anHLAEvent->GetNumBolos(); ++l) {
      aBoloEvent = static_cast<KHLABolometerRecord*>(anHLAEvent->GetBolo(l));
      anEnergyIon = aBoloEvent->GetEnergyIon();
      anEnergyHeat = aBoloEvent->GetEnergyHeat(1);
      aSigmaEnergyIon = aBoloEvent->GetBaselineNoiseIonFiducial();
      aSigmaEnergyHeat = aBoloEvent->GetBaselineNoiseHeat(1);
      aQvalue = aBoloEvent->GetQvalue();
      anEnergyRecoil = aBoloEvent->GetEnergyRecoil();
      aDetectorIndex= calib.GetCalibrationEntry(aBoloEvent->GetDetectorName().c_str());
      aSigmaEnergyIon356 = calib.GetUncerIonCalib(aDetectorIndex);
      aSigmaEnergyHeat356 = calib.GetUncerHeatCalib(aDetectorIndex);
      aSigmaEnergyIon = aSigmaEnergyIon356;
      aSigmaEnergyHeat = aSigmaEnergyHeat356;
      
      if(strcmp(aDetectorName,"ALL")) {
        if(aBoloEvent->GetEventFlag()==anEventCategory)
          if(aBoloEvent->GetDetectorName()==aDetectorName)
            if(anEnergyRecoilMin <= anEnergyRecoil && anEnergyRecoil <= anEnergyRecoilMax
            && aQvalueMin <= aQvalue && aQvalue <= aQvalueMax) {
          os << aQvalue << " ";
          os << anEnergyRecoil << " ";
          os << KQUncertainty::GetChannelUncertainty(anEnergyIon,
                                                    aSigmaEnergyIon,
                                                    aSigmaEnergyIon356)
            << " ";
          os << KQUncertainty::GetChannelUncertainty(anEnergyHeat,
                                                    aSigmaEnergyHeat,
                                                    aSigmaEnergyHeat356)
            << " ";
          os << endl;
          cout << aQvalue << " ";
          cout << anEnergyRecoil << " ";
          cout << KQUncertainty::GetChannelUncertainty(anEnergyIon,
                                                    aSigmaEnergyIon,
                                                    aSigmaEnergyIon356)
            << " ";
          cout << KQUncertainty::GetChannelUncertainty(anEnergyHeat,
                                                    aSigmaEnergyHeat,
                                                    aSigmaEnergyHeat356)
            << " ";
          cout << "\t\t" << aSigmaEnergyHeat << " " << aSigmaEnergyHeat356 << endl;
          cout << endl;
          ++anEventCounter;
          } //if   
      }
      else  
      {
          if(aBoloEvent->GetEventFlag()==anEventCategory)
            if(anEnergyRecoilMin <= anEnergyRecoil && anEnergyRecoil <= anEnergyRecoilMax
            && aQvalueMin <= aQvalue && aQvalue <= aQvalueMax) {
          os << aQvalue << " ";
          os << anEnergyRecoil << " ";
          os << KQUncertainty::GetChannelUncertainty(anEnergyIon,
                                                    aSigmaEnergyIon,
                                                    aSigmaEnergyIon356)
            << " ";
          os << KQUncertainty::GetChannelUncertainty(anEnergyHeat,
                                                    aSigmaEnergyHeat,
                                                    aSigmaEnergyHeat356)
            << " ";
          os << endl;
          cout << aQvalue << " ";
          cout << anEnergyRecoil << " ";
          cout << KQUncertainty::GetChannelUncertainty(anEnergyIon,
                                                    aSigmaEnergyIon,
                                                    aSigmaEnergyIon356)
            << " ";
          cout << KQUncertainty::GetChannelUncertainty(anEnergyHeat,
                                                    aSigmaEnergyHeat,
                                                    aSigmaEnergyHeat356)
            << " ";
          cout << "\t\t" << aSigmaEnergyHeat << " " << aSigmaEnergyHeat356 << endl;
          cout << endl;
          ++anEventCounter;
          } //if 
      }
          
    } //for  
  } //for
  cout << "events: " << anEventCounter << endl;
  aList = new KQContourPointList("QErecoil","events.txt");
  aList->ReadASCIIFile("events.txt","");
  aList->Draw();
}