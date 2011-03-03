//_____________________________________________
//
// KQDataEvaluator.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/18/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KQDataEvaluator.h"

ClassImp(KQDataEvaluator);

KQDataEvaluator::KQDataEvaluator() 
{
  //constructor
}

KQDataEvaluator::~KQDataEvaluator() 
{
  if(fKQDataReader) {
    delete fKQDataReader;
    fKQDataReader = 0;
  }
}

Bool_t KQDataEvaluator::ReadEvents(const Char_t* aSourceFile,
                                   const Char_t* aBoloConfigFile,
                                   const Char_t* aBoloName,
                                   const Char_t* aCategoryName)
{
  if(!fKQDataReader)
    fKQDataReader = new KQDataReader(aSourceFile,aBoloConfigFile,
                                     aBoloName,aCategoryName);
  return(fKQDataReader->ReadEvents(aSourceFile,aBoloConfigFile,aBoloName,aCategoryName));
}
