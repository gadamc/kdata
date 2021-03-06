//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Aug 25 13:58:22 2011 by ROOT version 5.28/00
// from TTree data/random experimental events generated byscattering true events uniformly distributedin (E_{ion},E_{heat}) (flat prior)
// found on file: coverage_ID401.root
//////////////////////////////////////////////////////////

#ifndef CoverageSelector_h
#define CoverageSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include "KQContourPoint.h"
#include <iostream>
using namespace std;

class CoverageSelector : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   KQContourPoint* fPoint;
   Double_t fInCounter;
   Double_t fOutCounter;
   

   // Declaration of leaf types
   Double_t        EnergyIonTrue;
   Double_t        EnergyHeatTrue;
   Double_t        EnergyRecoilTrue;
   Double_t        QvalueTrue;
   Double_t        EnergyIonExp;
   Double_t        EnergyHeatExp;
   Double_t        EnergyRecoilExp;
   Double_t        QvalueExp;
   

   // List of branches
   TBranch        *b_EnergyIonTrue;   //!
   TBranch        *b_EnergyHeatTrue;   //!
   TBranch        *b_EnergyRecoilTrue;   //!
   TBranch        *b_QvalueTrue;   //!
   TBranch        *b_EnergyIonExp;   //!
   TBranch        *b_EnergyHeatExp;   //!
   TBranch        *b_EnergyRecoilExp;   //!
   TBranch        *b_QvalueExp;   //!

   CoverageSelector(TTree * /*tree*/ =0) { }
   virtual ~CoverageSelector() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   virtual void    MakeContourPoint(Double_t aEnergyRecoil,
                                    Double_t aQvalue,
                                    Double_t UncerIon,
                                    Double_t UncerHeat);
   virtual KQContourPoint* GetPoint();

   ClassDef(CoverageSelector,0);
};

#endif

#ifdef CoverageSelector_cxx
void CoverageSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EnergyIonTrue", &EnergyIonTrue, &b_EnergyIonTrue);
   fChain->SetBranchAddress("EnergyHeatTrue", &EnergyHeatTrue, &b_EnergyHeatTrue);
   fChain->SetBranchAddress("EnergyRecoilTrue", &EnergyRecoilTrue, &b_EnergyRecoilTrue);
   fChain->SetBranchAddress("QvalueTrue", &QvalueTrue, &b_QvalueTrue);
   fChain->SetBranchAddress("EnergyIonExp", &EnergyIonExp, &b_EnergyIonExp);
   fChain->SetBranchAddress("EnergyHeatExp", &EnergyHeatExp, &b_EnergyHeatExp);
   fChain->SetBranchAddress("EnergyRecoilExp", &EnergyRecoilExp, &b_EnergyRecoilExp);
   fChain->SetBranchAddress("QvalueExp", &QvalueExp, &b_QvalueExp);
}

Bool_t CoverageSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef CoverageSelector_cxx
