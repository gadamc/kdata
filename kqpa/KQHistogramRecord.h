//_____________________________________________
//
// KQHistogramRecord.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/10/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

#include "Rtypes.h"
#include "TH2D.h"
#include <iostream>
using namespace std;

#ifndef __KQHISTOGRAMRECORD_H__
#define __KQHISTOGRAMRECORD_H__


class KQHistogramRecord {
  private:
    TH2D* fHistogram;
    Double_t fEnergyRecoilMean;
    Double_t fEnergyRecoilError;
    Double_t fQNeutron;
    Double_t fQNeutronError;
    Double_t fQGamma;
    Double_t fQGammaError;
    Double_t fQNeutronTheo;
    Double_t fQNeutronTheoError;
    Double_t fQGammaTheo;
    Double_t fQGammaTheoError;
    
  public:
    KQHistogramRecord();
    virtual ~KQHistogramRecord();
    
    //getters
    TH2D* GetHistogram() { return fHistogram; }
    Double_t GetEnergyRecoilMean() const { return fEnergyRecoilMean; }
    Double_t GetEnergyRecoilError() const { return fEnergyRecoilError; }
    Double_t GetQNeutron() const { return fQNeutron; }
    Double_t GetQNeutronError() const { return fQNeutronError; }
    Double_t GetQGamma() const { return fQGamma; }
    Double_t GetQGammaError() const { return fQGammaError; }
    Double_t GetQNeutronTheo() const { return fQNeutronTheo; }
    Double_t GetQNeutronTheoError() const { return fQNeutronTheoError; }
    Double_t GetQGammaTheo() const { return fQGammaTheo; }
    Double_t GetQGammaTheoError() const { return fQGammaTheoError; }
    
    //setters
    void SetHistogram(TH2D* aTH2D) { fHistogram = aTH2D; }
    void SetEnergyRecoilMean(Double_t anEnergyRecoilMean)
    { fEnergyRecoilMean = anEnergyRecoilMean; }
    void SetEnergyRecoilError(Double_t anEnergyRecoilError)
    { fEnergyRecoilError = anEnergyRecoilError; }
    void SetQNeutron(Double_t aQNeutron)
    { fQNeutron = aQNeutron; }
    void SetQNeutronError(Double_t aQNeutronError)
    { fQNeutronError = aQNeutronError; }
    void SetQGamma(Double_t aQGamma) { fQGamma = aQGamma; }
    void SetQGammaError(Double_t aQGammaError) { fQGammaError = aQGammaError; }
    void SetQNeutronTheo(Double_t aQNeutronTheo) { fQNeutronTheo = aQNeutronTheo; }
    void SetQNeutronTheoError(Double_t aQNeutronTheoError)
    { fQNeutronTheoError = aQNeutronTheoError; }
    void SetQGammaTheo(Double_t aQGammaTheo) { fQGammaTheo = aQGammaTheo; }
    void SetQGammaTheoError(Double_t aQGammaTheoError) 
    { fQGammaTheoError = aQGammaTheoError; }
    
    friend class KQHistogramManager;
  ClassDef(KQHistogramRecord,0);
};

#endif // __KQHISTOGRAMRECORD_H__