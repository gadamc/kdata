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

class KQDataEvaluator {
  private:
    KQDataReader* fKQDataReader;
  public:
    KQDataEvaluator();
    ~KQDataEvaluator();
    
    Bool_t ReadEvents(const Char_t* aSourceFile = "",
                      const Char_t* aBoloConfigFile = "",
                      const Char_t* aBoloName = "ALL",
                      const Char_t* aCategoryName = "fiducial");
    
    
  ClassDef(KQDataEvaluator,1);
};