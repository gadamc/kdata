/*
 * KRun12Temp.h
 *
 * Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
 * Created on Thu Jan 27 14:54:11 2011
 * Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
 *
 * This is a temporary class reading FWDMs for the heat and ion channels for a particular detector from an ASCII file
 */

#ifndef __KRUN12TEMP_H__
#define __KRUN12TEMP_H__



#include "Rtypes.h"
#include "TTree.h"
#include <math.h>
#include <string>
using namespace std;

class KRun12Temp {
  private:
    TTree* fTree; //tree storing bolo data

  public:
    KRun12Temp(string aFileName);
    
    void ReadCalibrationFile(string aFileName);
    Int_t GetCalibrationEntry(string aBoloName);
    Int_t GetDetectorNumber(Int_t anEntry);
    Double_t GetUncerIonZero(Int_t anEntry);
    Double_t GetUncerIonCalib(Int_t anEntry);
    Double_t GetUncerHeatZero(Int_t anEntry);
    Double_t GetUncerHeatCalib(Int_t anEntry);
    Double_t GetVoltageBias(Int_t anEntry);
    Double_t GetRadius(Int_t anEntry);


  ClassDef(KRun12Temp,0);
};

#endif

