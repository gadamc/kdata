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
#include "KQContourPoint.h"
#include <fstream>
#include <string>
#include <string.h>
using namespace std;

#ifndef __KQCONTOURPOINTLIST_H__
#define __KQCONTOURPOINTLIST_H__

class KQContourPointList {
  private:
    string fMode;
    string fFileName;
    vector<KQContourPoint*> fPoints;
  public:
    KQContourPointList(const Char_t* aFileName = "",
                       const Char_t* aMode ="QERecoil"
    );
    void ReadASCIIFile(const Char_t* aFileName = "");
  ClassDef(KQContourPointList,0);
};

#endif