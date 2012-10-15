//_____________________________________________
//
// KQDataEvaluator.h
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/18/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "Rtypes.h"
#include "KQDataReader.h"
#include "KQHistogramManager.h"

class KQDataEvaluator {
  private:
    KQDataReader* fQDataReader;
    KQHistogramManager* fQHistogramManager;
  public:
    KQDataEvaluator();
    virtual ~KQDataEvaluator();
    
    Bool_t ReadEvents(const Char_t* aSourceFile = "",
                      const Char_t* aBoloConfigFile = "",
                      const Char_t* aBoloName = "ALL",
                      const Char_t* aCategoryName = "fiducial");
                      
    Bool_t FillHistograms(Int_t aNumHistograms = 10);
    
    
  ClassDef(KQDataEvaluator,1);
};