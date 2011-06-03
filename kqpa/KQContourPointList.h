//_____________________________________________
//
// KQContourPointList.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/26/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


#include "Rtypes.h"
#include "TF2.h"
#include "TMath.h"
#include "TMarker.h"
#include "TObjString.h"
#include "KQContourPoint.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <string.h>
using namespace std;

#ifndef __KQCONTOURPOINTLIST_H__
#define __KQCONTOURPOINTLIST_H__

class KQContourPointList {
  private:
    string fMode;
    string fFileName;
    vector<KQContourPoint*> fPoints;
    Double_t fEnergyRecoilMin;
    Double_t fEnergyRecoilMax;
    Double_t fQvalueMin;
    Double_t fQvalueMax;
    TF2* fEmptyFrame;   
    void UpdateEmptyFrame();
    
  public:
    KQContourPointList(const Char_t* aFileName = "",
                       const Char_t* aMode ="QERecoil",
                       Double_t  anEnergyRecoilMin = 0,
                       Double_t anEnergyRecoilMax = 1000,
                       Double_t aQvalueMin = 0,
                       Double_t aQvalueMax = 2
    );
    ~KQContourPointList();
    
    //setters
    void SetEnergyRecoilMin(Double_t anEnergyRecoilMin)
    { 
      fEnergyRecoilMin = anEnergyRecoilMin; 
      UpdateEmptyFrame();
    }
    void SetEnergyRecoilMax(Double_t anEnergyRecoilMax)
    { 
      fEnergyRecoilMax = anEnergyRecoilMax;
      UpdateEmptyFrame();
    }
    void SetQvalueMin(Double_t aQvalueMin)
    { 
      fQvalueMin = aQvalueMin;
      UpdateEmptyFrame();
    }
    void SetQvalueMax(Double_t aQvalueMax)
    { 
      fQvalueMax = aQvalueMax;
      UpdateEmptyFrame();
    }
    
    //getters
    Double_t GetEnergyRecoilMin() const { return fEnergyRecoilMin; }
    Double_t GetEnergyRecoilMax() const { return fEnergyRecoilMax; }
    Double_t GetQvalueMin() const { return fQvalueMin; }
    Double_t GetQvalueMax() const { return fQvalueMax; }
    UInt_t GetEntries() const { return fPoints.size(); }
    TF2* GetEmptyFrame() const { return fEmptyFrame; }
    KQContourPoint* GetElement(UInt_t anIndex) const 
    {
      if(anIndex >= fPoints.size()) {
        cout << "KQContourPoint* GetElement(UInt_t anIndex): invalid index,"
        << " must be <" << fPoints.size() << endl;
        return 0;
      }
      else
        return fPoints[anIndex];
    }
    
    void ReadASCIIFile(const Char_t* aFileName = "");
    void Draw(Option_t* anOption = "");
    void AddPoint(Double_t aQvalue,
              Double_t anEnergyRecoil,
              Double_t aSigmaIon,
              Double_t aSigmaHeat);
    void RemovePoint(UInt_t anINdex = 0);
    void ClearPoints();
    void ShowPoints();
  ClassDef(KQContourPointList,0);
};

#endif
