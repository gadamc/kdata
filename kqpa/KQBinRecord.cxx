//_____________________________________________
//
// KQBinRecord.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/5/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KQBinRecord.h"

ClassImp(KQBinRecord);

Bool_t operator<(const KQBinRecord& aBinRecord,const KQBinRecord& anotherBinRecord)
{
  return aBinRecord.fBinContent<anotherBinRecord.fBinContent;
}

KQBinRecord::KQBinRecord(Int_t anX,
                         Int_t anY,
                         Double_t aBinContent)
                         : fX(anX), fY(anY), fBinContent(aBinContent)
{
}