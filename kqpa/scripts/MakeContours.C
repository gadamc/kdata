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

//#ifndef __CINT__
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
#include <set>
#include <string>
using namespace std;

/*#else
class KQContourPointList;
class KDataReader;
class KHLAEvent;
class KHLABolometerRecord;
#endif

*/




KDataReader* aReader = 0;
KHLAEvent* anHLAEvent = 0;
KHLABolometerRecord* aBoloEvent = 0;
KQContourPointList* aList = 0;


void ReadData(const Char_t* aKEventROOTFile,
              const Char_t* outfile,
                  set<string> aSetOfDetectors,
                  Int_t anEventCategory = 2,
                  Double_t anEnergyRecoilMin = 20,
                  Double_t anEnergyRecoilMax = 200,
                  Double_t aQvalueMin = 0,
                  Double_t aQvalueMax = 1.4
                  )
{
  //This method reads bolo events from a ROOT file and writes 
  // lines <Q> <E_recoil> <sigma_ion> <sigma_heat> <voltage bias> 
  // to an ASCII file "events.txt" for all  events with specified restrictions
  // The heat and ion uncertainties are calculated by interpolating
  // between the uncertainties at noise level read from the ROOT file
  // and the uncertainties at 133Ba calibration level E = 356 keV
  // read from an ASCII file "BoloConfigFile.txt" containing calibration data
  // for run 12
  // For the latter there might be a more sophisticated way to store and retrieve
  // calibration data for future runs and additional detectors
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
  Double_t aVoltageBias;
  aReader = new KDataReader(aKEventROOTFile);
  ofstream os(outfile);
  os << "# Q E_recoil sigma_ion sigma_heat voltage_bias" << endl;
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
      aVoltageBias = calib.GetVoltageBias(aDetectorIndex);
      //aSigmaEnergyIon = aSigmaEnergyIon356;
      //aSigmaEnergyHeat = aSigmaEnergyHeat356;
      if(aSetOfDetectors.size()) {
        if(aBoloEvent->GetEventFlag()==anEventCategory)
         if(aSetOfDetectors.find(aBoloEvent->GetDetectorName())!=aSetOfDetectors.end())
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
          os << aVoltageBias;
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
          cout << aVoltageBias << " ";
          cout << "\t\t" << aSigmaEnergyHeat << "\t" << aSigmaEnergyHeat356 << endl;
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
          os << aVoltageBias;
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
          cout << aVoltageBias << " ";
          cout << "\t\t" << aSigmaEnergyHeat << "\t" << aSigmaEnergyHeat356 << endl;
       
          cout << endl;
          ++anEventCounter;
          } //if 
      }

          
    } //for  
  } //for
  cout << "events: " << anEventCounter << endl;
}

void DrawEvents(const Char_t* outfile,
                Int_t aNumber = 10,
                TF1* aFunction = 0)
{
  aList = new KQContourPointList("QErecoil",outfile);
  aList->ReadASCIIFile(outfile,"QErecoil",aNumber,aFunction);
  aList->Draw();
}

void MakeContours()
{
  set<string> detectors;
  detectors.insert("ID401");
  detectors.insert("ID402");
  detectors.insert("ID403");
  detectors.insert("ID404");
  detectors.insert("ID405");
  detectors.insert("ID2");
  detectors.insert("ID3");
  detectors.insert("ID5");
  detectors.insert("ID6");
  detectors.insert("FID401");
  detectors.insert("FID402");
  
  ReadData("/kalinka/storage/edelweiss/Bolo/Run12/Eds/Merge/Bckgd/Kds_AllBolos.root",
           "bckgd.txt",
           detectors,
           2, // event category: fiducial
           20, // E_recoil,min
           200, // E_ recoil,max
           0, // Q_min
           1.4); // Q_max
  //DrawEvents("gamma.txt",10000);
}

