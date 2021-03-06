//_____________________________________________
//
// KQContourPoint.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 4/28/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


#include "Rtypes.h"
#include "TObject.h"
#include "TF2.h"
#include "TMath.h"
#include "TMarker.h"
#include "Math/WrappedMultiTF1.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "Math/GSLMCIntegrator.h"
#include "KQContour.h"
#include "KErecoilQDensity.h"
#include "KLindhard.h"


#ifndef __KQCONTOURPOINT_H__
#define __KQCONTOURPOINT_H__

class KQContourPoint : public TObject {
  private:
    KQContourPoint* fPreviousVersion; // copy of this
    Double_t fQvalue; // Q value
    Double_t fEnergyRecoil; // recoil energy
    Double_t fEnergyIon; // ion energy
    Double_t fEnergyHeat; // heat energy
    Double_t fSigmaEnergyIon; // uncertainty on the ion energy
    Double_t fSigmaEnergyHeat; // uncertainty on the  heat energy
    Double_t fSigmaEnergyIonHeat; // root of covariance between the ion and
    // heat energy
    Double_t fVoltageBias; // voltage bias
    Double_t fEpsilon; // epsilon_gamma
    Double_t fConfidenceLevel; // confidence level
    Double_t fConfidenceLevelError; // uncertainty on the confidence level
    Double_t fConfidenceFunctionValue; // function value corresponding to the confidence line
    TF2* fFunction; //! function representing the pdf g(E_recoil,Q)
    TMarker* fMarker; // marker representing the modal value (E_recoil,Q)
    Int_t fNumBinsX; // number of bins in X direction of the
    // distribution histogram
    Int_t fNumBinsY; // number of bins in Y direction of the 
    // distribution histogram
    Int_t fNumSigmas; // number of sigmas of the recoil energy and Q value
    // to build the region taken into regard to calculate the contour line
    

    
  public:
    KQContourPoint(Double_t aQvalueOrEnergyIon = 0,
                   Double_t anEnergyRecoilOrEnergyHeat = 0,
                   const Char_t* aMode = "QErecoil",
                   Double_t aSigmaIon = 0,
                   Double_t aSigmaHeat = 0,
                   Double_t aSigmaIonHeat = 0,
                   Double_t aConfidenceLevel = 0.9,
                   Double_t aVoltageBias = 8.0,
                   Double_t anEpsilon = 3.0,
                   Double_t aNumBinsX = 500,
                   Double_t aNumBinsY = 500,
                   Double_t aNumSigmas = 10
                  );
    KQContourPoint(const KQContourPoint& anotherPoint);
    virtual ~KQContourPoint();

    void CalculateContour();
    void ResetMarker();
    void SetQvalue(Double_t aQvalue);
    void SetEnergyRecoil(Double_t anEnergyRecoil);
    void SetEnergyIon(Double_t anEnergyIon);
    void SetEnergyHeat(Double_t anEnergyHeat);
    void SetResolutionX(Int_t aNumBinsX);
    void SetResolutionY(Int_t aNumBinsY);
    void SetFunction();
    void SetNpx(Int_t anNpx);
    void SetNpy(Int_t anNpy);
    void SetNumSigmas(Double_t aNumSigmas);
    void SetSigmaEnergyHeat(Double_t aSigmaEnergyHeat);
    void SetSigmaEnergyIon(Double_t aSigmaEnergyIon);
    void SetSigmaIonHeat(Double_t aSigmaEnergyIonHeat);
    void SetVoltageBias(Double_t aVoltageBias);
    void SetEpsilon(Double_t anEpsilon);
    void SetConfidenceLevel(Double_t aConfidenceLevel);
    void SetRange(Double_t xmin, Double_t ymin,
                  Double_t xmax, Double_t ymax);

    Double_t GetQvalue() const { return fQvalue; }
    Double_t GetEnergyRecoil() const { return fEnergyRecoil; }
    Double_t GetEnergyIon() const { return fEnergyIon; }
    Double_t GetEnergyHeat() const { return fEnergyHeat; }
    Int_t GetResolutionX() const { return fNumBinsX; }
    Int_t GetResolutionY() const { return fNumBinsY; }
    Int_t GetNpx() const { return fFunction->GetNpx(); }
    Int_t GetNpy() const { return fFunction->GetNpy(); }
    Double_t GetNumSigmas() const { return fNumSigmas; }
    Double_t GetSigmaEnergyHeat() const { return fSigmaEnergyHeat; }
    Double_t GetSigmaEnergyIon() const { return fSigmaEnergyIon; }
    Double_t GetSigmaEnergyIonHeat() const { return fSigmaEnergyIonHeat; }
    Double_t GetVoltageBias() const { return fVoltageBias; }
    Double_t GetEpsilon() const { return fEpsilon; }
    Double_t GetConfidenceLevel() const { return fConfidenceLevel; }
    Double_t GetConfidenceLevelError() const { return fConfidenceLevelError; }
    Double_t GetConfidenceFunctionValue() const { return fConfidenceFunctionValue; }
    TMarker* GetMarker() const { return fMarker; }
    TH2D* GetHistogram();
    TH2D* GetContourHistogram();

    
    
    TF2* GetFunctionClone() const { return (TF2*)(fFunction->Clone()); }
    TF2* GetFunction() const { return fFunction; }
    
    void Draw(Option_t* anOption = "");
    Bool_t CutsALine(TF1* aFunction,
                     Int_t aNumPoints = 1000);
    Bool_t CutsLindhardLine(Int_t aNumPoints = 1000);
    Bool_t CutsOne(Int_t aNumPoints = 1000);
    Bool_t IsInConfidenceRegion(Double_t anEnergyRecoil,
				Double_t aQvalue);
    friend Bool_t operator!=(KQContourPoint& aPoint,
                             KQContourPoint& anotherPoint);
    friend Bool_t operator==(KQContourPoint& aPoint,
                             KQContourPoint& anotherPoint);

  ClassDef(KQContourPoint,1);
};

#endif

