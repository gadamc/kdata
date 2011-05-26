//_____________________________________________
//
// KQContourPointList.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/26/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include "KQContourPointList.h" 

ClassImp(KQContourPointList);

KQContourPointList::KQContourPointList(const Char_t* aFileName,
                                       const Char_t* aMode)
{
  fFileName = aFileName;
  fMode = aMode;
}

void KQContourPointList::ReadASCIIFile(const Char_t* aFileName)
{
  if(strcmp(aFileName,""))
    fFileName = aFileName;
  
  if(strcmp(fFileName.c_str(),""))
  {
    ifstream is(aFileName);
    Double_t anEnergyIon, anEnergyHeat;
    Double_t aQvalue, anEnergyRecoil, aSigmaIon, aSigmaHeat;
    if(fMode=="QERecoil")
      while(!is.eof())
      {
        is >> aQvalue >> anEnergyRecoil >> aSigmaIon >> aSigmaHeat;
        fPoints.push_back(new KQContourPoint(aQvalue, anEnergyRecoil,
                                            aSigmaIon, aSigmaHeat));
      }
      /* //has to be implemented
      else
    if(fMode=="EIonEHeat")
      while(!is.eof())
      {
        is >> anEnergyIon >> anEnergyHeat >> aSigmaIon, aSigmaHeat;
        fPoints.push_back(new KQContourPoint(
      }
      */
  }
  else
  {
    cout << "void KQContourPointList::ReadASCIIFile(): no ASCII file specified"
    << endl;
    return;
  }
}
