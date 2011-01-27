/*
 * KBoloConfig.cxx
 *
 * Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
 * Created on Thu Jan 27 14:54:11 2011
 * Copyright 2011 Karlsruhe Institute of Technology. All Rights reserved.
 *
 *
 */

#ifndef __KBOLOCONFIG_H__
#define __KBOLOCONFIG_H__



#include "Rtypes.h"
#include <string>
#include <vector>

class KBoloConfig {
  private:
    string fBoloName;
    Double_t fSigmaIonCalibration;
    Double_t fSigmaIonZero;
    Double_t fSigmaHeatCalibration;
    Double_t fSigmaHeatZero;
    Double_t fEnergyCalibration;
    Int_t fNumChannels;
    vector<Double_t> fChannelVoltages;

  public:
    KBoloConfig();

    const char* GetBoloName() const { return fBoloName.c_str(); }
    Double_t GetSigmaIonCalibration() const { return fSigmaIonCalibration; }
    Double_t GetSigmaIonZero() const { return fSigmaIonZero; }
    Double_t GetSigmaHeatCalibration() const { return fSigmaHeatCalibration; }
    Double_t GetSigmaHeatZero() const { return fSigmaHeatZero; }
    Double_t GetEnergyCalibration() const { return fEnergyCalibration; }
    Int_t GetNumChannel() const { return fNumChannel; }
    Double_t GetChannelVoltage(Int_t anIndex) const { return fChannelVoltages[anIndex]; }

    void SetBoloName(const char* aBoloName) { fBoloName=aBoloName; }
    void SetSigmaIonCalibration(Double_t aSigmaCalibration) { fSigmaIonCalibration=aSigmaIonCalibration; }
    void SetSigmaIonZero(Double_t aSigmaIonZero) { fSigmaIonZero=aSigmaIonZero; }
    void SetSigmaHeatCalibration(Double_t aSigmaHeatCalibration) { fSigmaHeatCalibration=aSigmaHeatCalibration; }
    void SetSigmaHeatZero(Double_t aSigmaHeatZero) { fSigmaHeatZero=aSigmaHeatZero; }
    void SetEnergyCalibration(Double_t aEnergyCalibration) { fEnergyCalibration=aEnergyCalibration; }
    void SetChannelVoltage(Int_t anIndex,Double_t aVoltage) { fChannelVoltages[index]=aVoltage; }
  ClassDef(KBoloConfig,0);
};

#endif

