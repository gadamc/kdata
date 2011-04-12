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
  if(fQHistogramManager) {
    delete(fQHistogramManager);
    fQHistogramManager = 0;
  }
  if(fQDataReader) {
    delete fQDataReader;
    fQDataReader = 0;
  }
}

Bool_t KQDataEvaluator::ReadEvents(const Char_t* aSourceFile,
                                   const Char_t* aBoloConfigFile,
                                   const Char_t* aBoloName,
                                   const Char_t* aCategoryName)
{
  if(!fQDataReader)
    fQDataReader = new KQDataReader(aSourceFile,aBoloConfigFile,
                                     aBoloName,aCategoryName);
  return(fQDataReader->ReadEvents(aSourceFile,aBoloConfigFile,aBoloName,aCategoryName));
}

Bool_t KQDataEvaluator::FillHistograms(Int_t aNumHistograms)
{
  if(!fQDataReader) {
    cout << "KQDataEvaluator::FillHistograms(): first data has to be read!!" << endl;
    cout << "  call KQDataEvaluator::ReadEvents() " << endl;
    return false;
  }
  if(!fQHistogramManager)
    fQHistogramManager = new KQHistogramManager(fQDataReader);
    else
    fQHistogramManager->SetQDataReader(fQDataReader);
  fQHistogramManager->Fill(aNumHistograms);
  
}
