//_____________________________________________
//
// KQLindhardProjection.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/24/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//




#include "Rtypes.h"
#include "TH2D.h"
#include "TH1D.h"
#include "KQProjection.h"
#include "KLindhard.h"
#include <iostream>
#include <string>
using namespace std;

#ifndef __KQLINDHARDPROJECTION_H__
#define __KQLINDHARDPROJECTION_H__


class KQLindhardProjection : public KQProjection {
  private:
    string fMinimizer;
    string fMethod;
  public:
    KQLindhardProjection(const Char_t* aSourceFile,
                 const Char_t* aBoloName,
                 const Char_t* aCategoryName = "fiducial",
                 Double_t aNumBinsEnergyRecoil = 1000,
                 Double_t anEnergyRecoilMin = 0,
                 Double_t anEnergyRecoilMax = 1000,
                 Double_t aNumBinsQ = 1000,
                 Double_t aQMin = 0,
                 Double_t aQMax = 2,
                 const Char_t* aHistogramName = "hist",
                 const Char_t* aMinimizer = "Minuit2",
                 const Char_t* aMethod = "Combined");
                         
    virtual void MakeHistogram();

};

#endif