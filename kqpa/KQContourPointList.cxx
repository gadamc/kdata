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
                                       const Char_t* aMode,
                                       Double_t anEnergyRecoilMin,
                                       Double_t anEnergyRecoilMax,
                                       Double_t aQvalueMin,
                                       Double_t aQvalueMax
                                      )
: fMode(aMode), fFileName(aFileName), fEnergyRecoilMin(anEnergyRecoilMin),
fEnergyRecoilMax(anEnergyRecoilMax), fQvalueMin(aQvalueMin),
fQvalueMax(aQvalueMax)
{
  fEmptyFrame = new TF2("emptyframe",
                        "0*x*y",
                        fEnergyRecoilMin,
                        fEnergyRecoilMax,
                        fQvalueMin,
                        fQvalueMax);
  fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fEmptyFrame->GetYaxis()->SetTitle("Q");
}

KQContourPointList::~KQContourPointList()
{
  if(fEmptyFrame)
    delete fEmptyFrame;
}

void KQContourPointList::UpdateEmptyFrame()
{
  if(fEmptyFrame) {
    fEmptyFrame->SetRange(fEnergyRecoilMin,
                          fQvalueMin,
                          fEnergyRecoilMax,
                          fQvalueMax);
    fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
    fEmptyFrame->GetYaxis()->SetTitle("Q");
  }
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

void KQContourPointList::Draw(Option_t* anOption)
{
  fEmptyFrame->SetRange(fEnergyRecoilMin,
                        fQvalueMin,
                        fEnergyRecoilMax,
                        fQvalueMax);
  fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
  fEmptyFrame->GetYaxis()->SetTitle("Q");
  if(!fPoints.size()) {
    cout << "void KQContourPointList::Draw(Option_t* anOption): no points to"
    " draw" << endl;
    return;
  }
  fEmptyFrame->Draw(anOption);
  for(UInt_t k = 0; k < fPoints.size(); ++k) {
    fPoints[k]->Draw("same");
  }
}

void KQContourPointList::AddPoint(Double_t aQvalue,
              Double_t anEnergyRecoil,
              Double_t aSigmaIon,
              Double_t aSigmaHeat)
{
  fPoints.push_back(new KQContourPoint(aQvalue,
                                       anEnergyRecoil,
                                       aSigmaIon,
                                       aSigmaHeat));
}

void KQContourPointList::ClearPoints()
{
  fPoints.clear();
}

void KQContourPointList::RemovePoint(UInt_t anIndex)
{
  if(anIndex>=fPoints.size()) {
    cout << "void KQContourPointList::RemovePoint(): index out of range, must"
    << " be smaller than " << fPoints.size() << endl;
    return;
  }
  fPoints.erase(fPoints.begin()+anIndex);
}

void KQContourPointList::ShowPoints()
{
  for(UInt_t k = 0; k< fPoints.size(); ++k) {
    cout << setw(4) << "Q: " << setw(15) <<  fPoints[k]->GetQvalue() 
    << setw(10) << "E_recoil: " << setw(15) << 
    fPoints[k]->GetEnergyRecoil() << setw(10) << "sigma_ion:" << setw(15) << 
    fPoints[k]->GetSigmaEnergyIon() << setw(10) << "sigma_heat:" << setw(15) <<
    fPoints[k]->GetSigmaEnergyHeat() << endl;
  }
}
