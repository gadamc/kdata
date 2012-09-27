//_____________________________________________
//
// KQDataReader.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/24/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "Rtypes.h"
#include "KQDataRecord.h"
#include "KBoloConfig.h"
#include "KDataReader.h"
#include "KHLABolometerRecord.h"
#include "KHLAEvent.h"
#include "KQUncertainty.h"
#include <iostream>
using namespace std;


#ifndef __KQDATAREADER_H__
#define __KQDATAREADER_H__

class KQDataReader {
  private:
    string fSourceFile; //source file with KHLAEvents
    string fBoloConfigFile; //ASCII file containing bolometer configuration data
    string fBoloName; //bolometer name
    Int_t fEventCategory; //event category 1
    Double_t fEnergyRecoilMin; //minimal recoil energy
    Double_t fEnergyRecoilMax; //maximal recoil energy;
    vector<KQDataRecord> fData;
  public:
    KQDataReader(const Char_t* aSourceFile = "",
                 const Char_t* aBoloConfigFile = "",
                 const Char_t* aBoloName = "ALL",
                 const Char_t* aCategoryName = "fiducial"); 
    Bool_t ReadEvents(const Char_t* aSourceFile = "",
                      const Char_t* aBoloConfigFile = "",
                      const Char_t* aBoloName = "ALL",
                      const Char_t* aCategoryName = "fiducial");
  ClassDef(KQDataReader,1);
    
    //setters
    void SetSourceFile(const Char_t* aSourceFile) { fSourceFile = aSourceFile; }
    void SetBoloConfigFile(const Char_t* aBoloConfigFile) { fBoloConfigFile = aBoloConfigFile; }
    void SetBoloName(const Char_t* aBoloName) { fBoloName = aBoloName; }
    void SetEventCategory(const Char_t* aCategoryName);
    void SetEnergyRecoilMin(Double_t anEnergyRecoilMin) { fEnergyRecoilMin = anEnergyRecoilMin; }
    void SetEnergyRecoilMax(Double_t anEnergyRecoilMax) { fEnergyRecoilMax = anEnergyRecoilMax; }
    
    //getters

    const Char_t* GetSourceFile() const { return(fSourceFile.c_str()); }
    const Char_t* GetBoloConfigFile() const { return(fBoloConfigFile.c_str()); }
    const Char_t* GetBoloName() const { return(fBoloName.c_str()); }
    const Char_t* GetEventCategory() const;
    Double_t GetEnergyRecoilMin() const { return fEnergyRecoilMin; }
    Double_t GetEnergyRecoilMax() const { return fEnergyRecoilMax; }
    KQDataRecord GetDataRecord(UInt_t anIndex) const;
    Double_t GetEnergyRecoil(UInt_t anIndex) const;
    Double_t GetEnergyIon(UInt_t anIndex) const;
    Double_t GetEnergyHeat(UInt_t anIndex) const;
    Double_t GetQvalue(UInt_t anIndex) const;
    Double_t GetSigmaIon(UInt_t anIndex) const;
    Double_t GetSigmaHeat(UInt_t anIndex) const;
    unsigned int GetSize() const {return fData.size();}
    
    friend class KQHistogramManager;
};

#endif //__KQDATAREADER_H__
