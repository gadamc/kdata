//_____________________________________________
//
// KQBinRecord.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/5/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//


#include "Rtypes.h"

#ifndef KQBinRecord_H__
#define KQBinRecord_H__

class KQBinRecord {
  private:
    Int_t fX; //  bin index on the Y axis
    Int_t fY; // bin index on the X axis
    Double_t fBinContent; // bin content
  public:
    KQBinRecord(Int_t anX = 0,
                Int_t anY = 0,
                Double_t aBinContent = 0);

    void SetX(Int_t anX) { fX = anX; }
    void SetY(Int_t anY) { fY = anY; }
    void SetBinContent(Double_t aBinContent) { fBinContent = aBinContent; }
    

    Int_t GetX() { return fX; }
    Int_t GetY() { return fY; }
    Double_t GetBinContent() { return fBinContent; }
    friend Bool_t operator<(const KQBinRecord& aBinRecord, 
                            const KQBinRecord& anotherBinRecord);
    ClassDef(KQBinRecord,0);
};

#endif