//_____________________________________________
//
// KQContourPoint.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


#include "Rtypes.h"
#include "TF2.h"
#include "TMath.h"
#include "TMarker.h"
#include "KQContour.h"

#ifndef __KQCONTOURPOINT_H__
#define __KQCONTOURPOINT_H__

class KQContourPoint {
  private:
    Double_t fQvalue;
    Double_t fEnergyRecoil;
    Double_t fSigmaEnergyIon;
    Double_t fSigmaEnergyHeat;
    Double_t fSigmaEnergyIonHeat;
    Double_t fVoltageBias;
    Double_t fEpsilon;
    Double_t fConfidenceLevel;
    Double_t fConfidenceLevelError;
    TF2* fFunction;
    TMarker* fMarker;
  public:
    KQContourPoint(Double_t aQvalue = 0,
                   Double_t anEnergyRecoil = 0,
                   Double_t aSigmaIon = 0,
                   Double_t aSigmaHeat = 0,
                   Double_t aSigmaIonHeat = 0,
                   Double_t aConfidenceLevel = 0.9,
                   Double_t aVoltageBias = 8.0,
                   Double_t anEpsilon = 3.0,
                   Double_t anNpx = 1000,
                   Double_t anNpy = 1000,
                   Double_t aNumSigmas = 10
                  );
    ~KQContourPoint();

    void SetQvalue(Double_t aQvalue) { fQvalue = aQvalue; }
    void SetEnergyRecoil(Double_t anEnergyRecoil) { fEnergyRecoil = anEnergyRecoil; }
    void SetSigmaEnergyHeat(Double_t aSigmaEnergyHeat) { fSigmaEnergyHeat = aSigmaEnergyHeat; }
    void SetSigmaEnergyIon(Double_t aSigmaEnergyIon) { fSigmaEnergyIon = aSigmaEnergyIon; }
    void SetSigmaIonHeat(Double_t aSigmaEnergyIonHeat) { fSigmaEnergyIonHeat = aSigmaEnergyIonHeat; }
    void SetVoltageBias(Double_t aVoltageBias) { fVoltageBias = aVoltageBias; }
    void SetEpsilon(Double_t anEpsilon) { fEpsilon = anEpsilon; }
    void SetConfidenceLevel(Double_t aConfidenceLevel);
    void SetRange(Double_t xmin, Double_t ymin,
                  Double_t xmax, Double_t ymax);

    Double_t GetQvalue() const { return fQvalue; }
    Double_t GetEnergyRecoil() const { return fEnergyRecoil; }
    Double_t GetSigmaEnergyHeat() const { return fSigmaEnergyHeat; }
    Double_t GetSigmaEnergyIon() const { return fSigmaEnergyIon; }
    Double_t GetSigmaEnergyIonHeat() const { return fSigmaEnergyIonHeat; }
    Double_t GetVoltageBias() const { return fVoltageBias; }
    Double_t GetEpsilon() const { return fEpsilon; }
    Double_t GetConfidenceLevel() const { return fConfidenceLevel; }
    Double_t GetConfidenceLevelError() const { return fConfidenceLevelError; }
    TMarker* GetMarker() const { return fMarker; }
    TH2D* GetHistogram();
    TH2D* GetContourHistogram();
    
    
    TF2* GetFunctionClone() const { return (TF2*)(fFunction->Clone()); }
    TF2* GetFunction() const { return fFunction; }
    
    void Draw(Option_t* anOption = "");

  ClassDef(KQContourPoint,0);
};

#endif