//_____________________________________________
//
// KQBinRecord.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/5/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// THis class stores information about a bin and is used to build a sorted list
// of bins with ascending order of bin contents

#include "Rtypes.h"

#ifndef KQBinRecord_H__
#define KQBinRecord_H__

class KQBinRecord {
  private:
    Int_t fX;
    Int_t fY;
    Double_t fBinContent;
  public:
    KQBinRecord(Int_t anX = 0,
                Int_t anY = 0,
                Double_t aBinContent = 0);
    //setters
    void SetX(Int_t anX) { fX = anX; }
    void SetY(Int_t anY) { fY = anY; }
    void SetBinContent(Double_t aBinContent) { fBinContent = aBinContent; }
    
    //getters
    Int_t GetX() { return fX; }
    Int_t GetY() { return fY; }
    Double_t GetBinContent() { return fBinContent; }
    friend Bool_t operator<(const KQBinRecord& aBinRecord, 
                            const KQBinRecord& anotherBinRecord);
    ClassDef(KQBinRecord,0);
};

#endif