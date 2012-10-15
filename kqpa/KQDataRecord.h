//_____________________________________________
//
// KQDataRecord.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/18/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "Rtypes.h"

#ifndef __KQDATARECORD_H__
#define __KQDATARECORD_H__

class KQDataRecord {
  private:
    Double_t fEnergyRecoil;
    Double_t fEnergyIon;
    Double_t fEnergyHeat;
    Double_t fQ;
    Double_t fSigmaIon;
    Double_t fSigmaHeat;
    
  public:
    KQDataRecord(Double_t anEnergyRecoil = 0,Double_t aQ = 0,Double_t anEnergyIon = 0,
               Double_t anEnergyHeat = 0,Double_t aSigmaIon = 0,
               Double_t aSigmaHeat = 0)
                 : fEnergyRecoil(anEnergyRecoil),fEnergyIon(anEnergyIon),
                 fEnergyHeat(anEnergyHeat), fQ(aQ),fSigmaIon(aSigmaIon),
                 fSigmaHeat(aSigmaHeat) { }
    virtual ~KQDataRecord(void){}

    //getters
    Double_t GetEnergyRecoil() const { return fEnergyRecoil; }
    Double_t GetEnergyIon() const { return fEnergyIon; }
    Double_t GetEnergyHeat() const { return fEnergyHeat; }
    Double_t GetQvalue() const { return fQ; }
    Double_t GetSigmaIon() const { return fSigmaIon; }
    Double_t GetSigmaHeat() const { return fSigmaHeat; }
    
    //setters
    void SetEnergyRecoil(Double_t anEnergyRecoil) { fEnergyRecoil = anEnergyRecoil; }
    void SetEnergyIon(Double_t anEnergyIon) { fEnergyIon = anEnergyIon; }
    void SetEnergyHeat(Double_t anEnergyHeat) { fEnergyHeat = anEnergyHeat; }
    void SetQvalue(Double_t aQ) { fQ = aQ; }
    void SetSigmaIon(Double_t aSigmaIon) { fSigmaIon = aSigmaIon; }
    void SetSigmaHeat(Double_t aSigmaHeat) { fSigmaHeat = aSigmaHeat; }
    
    friend Bool_t operator<(const KQDataRecord& aFirstDataRecord,
                            const KQDataRecord& aSecondDataRecord);
    friend class KQDataReader;
    
  ClassDef(KQDataRecord,1);
};

#endif //__KQDATARECORD_H__
