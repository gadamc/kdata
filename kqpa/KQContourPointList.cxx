//_____________________________________________
//
// KQContourPointList.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 5/26/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is used to draw multiple KQContourPoints in the same frame
// The user can manually add events by calling
//   void KQContourPointList::AddPoint(Double_t aQvalue,
//             Double_t anEnergyRecoil,
//             Double_t aSigmaIon,
//             Double_t aSigmaHeat)
//
//  or he can read ASCII files by calling
//    void KQContourPointList::ReadASCIIFile(const Char_t* aFileName,
//                                       const Char_t aMode)
//
// Then he can draw the events by calling
//        void Draw(Option_t* anOption = "");
//  or print the events by calling
//   void KQContourPointList::ShowPoints()

#include "KQContourPointList.h" 

ClassImp(KQContourPointList);

KQContourPointList::KQContourPointList(const Char_t* aMode,
                                       const Char_t* aFileName,
                                       Double_t anEnergyRecoilMin,
                                       Double_t anEnergyRecoilMax,
                                       Double_t aQvalueMin,
                                       Double_t aQvalueMax
                                      )
: fMode(aMode), fFileName(aFileName), fEnergyRecoilMin(anEnergyRecoilMin),
fEnergyRecoilMax(anEnergyRecoilMax), fQvalueMin(aQvalueMin),
fQvalueMax(aQvalueMax)
{
  // The constructor generates an empty frame with the specified range
  
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
  // This method updates the empty frame if the ranges change
  if(fEmptyFrame) {
    fEmptyFrame->SetRange(fEnergyRecoilMin,
                          fQvalueMin,
                          fEnergyRecoilMax,
                          fQvalueMax);
    fEmptyFrame->GetXaxis()->SetTitle("E_{Recoil} [keV]");
    fEmptyFrame->GetYaxis()->SetTitle("Q");
  }
}

void KQContourPointList::ReadASCIIFile(const Char_t* aFileName,
                                       const Char_t* aMode)
{
  // This method reads an ASCII file and builds a list of KQContourPoints
  // Depending form the specified mode a different format of the lines is
  // expected:
 //
 // aMode = "QErecoil":    < Q > <E_recoil> <sigma_ion> <sigma_heat>
 // aMode = "IonHeat": <E_ion> <E_heat> <sigma_ion> <sigma_heat>
 
  if(strcmp(aFileName,""))
    fFileName = aFileName;
  if(strcmp(aMode,""))
    fMode = aMode;
  
  if(strcmp(fFileName.c_str(),""))
  {
    ifstream is(aFileName);
    Double_t anEnergyIon, anEnergyHeat;
    Double_t aQvalue, anEnergyRecoil, aSigmaIon, aSigmaHeat;
    char aCString[100];
    TString aString;
    TObjArray* theTokens;
    TObjString* anElement;
    Int_t aLineCounter = 0;
    
    if(fMode=="QErecoil")
      while(!is.eof())
      {
        //is >> aQvalue >> anEnergyRecoil >> aSigmaIon >> aSigmaHeat;
        ++aLineCounter;
        is.getline(aCString,100);
        aString = aCString;
        theTokens = aString.Tokenize(" ");
        
        //empty lines
        if(!theTokens->GetEntries())
          continue;
        
        //comment lines 
        anElement = (TObjString*)theTokens->At(0);
        if(anElement->GetString()=="#") 
          continue;
        
        if(theTokens->GetEntries()!=4) {
          cout << "KQContourPointList::ReadASCIIFile(): invalid format in line "
          << aLineCounter <<", 4 Double_t values <Q> <E_recoil> <sigma_ion> "
          << "<sigma_heat> expected " << endl;
          continue;
        }
        
        anElement = (TObjString*)theTokens->At(0);
        aQvalue = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(1);
        anEnergyRecoil = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(2);
        aSigmaIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(3);
        aSigmaHeat = anElement->GetString().Atof();
          
        fPoints.push_back(new KQContourPoint(aQvalue, anEnergyRecoil,
                                             "QErecoil",
                                            aSigmaIon, aSigmaHeat));
        cout << "... event " << fPoints.size() << " processed" << endl;
      }
      else
    if(fMode=="IonHeat")
      while(!is.eof())
      {
        //is >> aQvalue >> anEnergyRecoil >> aSigmaIon >> aSigmaHeat;
        ++aLineCounter;
        is.getline(aCString,100);
        aString = aCString;
        theTokens = aString.Tokenize(" ");
        
        //empty lines
        if(!theTokens->GetEntries())
          continue;
        
        //comment lines 
        anElement = (TObjString*)theTokens->At(0);
        if(anElement->GetString()=="#") 
          continue;
        
        if(theTokens->GetEntries()!=4) {
          cout << "KQContourPointList::ReadASCIIFile(): invalid format in line "
          << aLineCounter <<", 4 Double_t values <E_ion> <E_heat> <sigma_ion>"
          << " <sigma_heat> expected " << endl;
          continue;
        }
        
        anElement = (TObjString*)theTokens->At(0);
        anEnergyIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(1);
        anEnergyHeat = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(2);
        aSigmaIon = anElement->GetString().Atof();
        anElement = (TObjString*)theTokens->At(3);
        aSigmaHeat = anElement->GetString().Atof();
          
        fPoints.push_back(new KQContourPoint(anEnergyIon, anEnergyHeat,
                                             "IonHeat",aSigmaIon, aSigmaHeat));
        cout << "... event " << fPoints.size() << " processed" << endl;
      }
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
  //This method draws all events in the list (marker + contour line)
  //in the empty frame
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

void KQContourPointList::AddPoint(Double_t aQvalueOrEnergyIon,
              Double_t anEnergyRecoilOrEnergyHeat,
              Double_t aSigmaIon,
              Double_t aSigmaHeat)
{
  //This method adds a point to the list of events
  fPoints.push_back(new KQContourPoint(aQvalueOrEnergyIon,
                                       anEnergyRecoilOrEnergyHeat,
                                       fMode.c_str(),
                                       aSigmaIon,
                                       aSigmaHeat));
}

void KQContourPointList::ClearPoints()
{
  // This method clears the events
  fPoints.clear();
}

void KQContourPointList::RemovePoint(UInt_t anIndex)
{
  // This method removes a single event from the list
  if(anIndex>=fPoints.size()) {
    cout << "void KQContourPointList::RemovePoint(): index out of range, must"
    << " be smaller than " << fPoints.size() << endl;
    return;
  }
  fPoints.erase(fPoints.begin()+anIndex);
}

void KQContourPointList::ShowPoints()
{
  //This methods prints the parameters of all events in the list
  for(UInt_t k = 0; k< fPoints.size(); ++k) {
    cout << setw(4) << "Q: " << setw(15) <<  fPoints[k]->GetQvalue() 
    << setw(10) << "E_recoil: " << setw(15) << 
    fPoints[k]->GetEnergyRecoil() << setw(10) << "sigma_ion:" << setw(15) << 
    fPoints[k]->GetSigmaEnergyIon() << setw(10) << "sigma_heat:" << setw(15) <<
    fPoints[k]->GetSigmaEnergyHeat() << endl;
  }
}
