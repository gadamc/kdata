//______________________________________________________________________
//
// KLindhard.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// A wrapper for a Lindhard formula, which uses a TF1.
// The Lindhard formula for the quenching factor of a nuclear recoil
// as a function of energy is
//
// Q = A * E^(B)
//
// where E is the recoil energy of the nucleus and A, and B are constants
// that are dependent upon the material. For Germanium, these values
// are A = 0.165 and B = 0.185. However, this class allows you to 
// change those values via the SetParameterA/B methods. 
//



#include "KLindhard.h"
#include "TF1.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TMath.h"

Double_t DistToLindhardFunction(const Double_t* xx)
{
  //This function calculates the square of the distance of an arbitrary point (anERecoil,aQ)
  //to a point (anERecoilLindhard, Q(anERecoilLindhard) on the LindhardFunction a*E^b
  //with a = 0.165 and b = 0.185
  //
  //The function is minimized by 
  //KQDistribution::CalculateMinDistanceToLinhard(Double_t anERecoil,Double_t aQ)
  
  Double_t anERecoil = xx[0];
  Double_t aQ = xx[1];
  Double_t anERecoilLindhard = xx[2];
  
  KLindhard aLind;
  
  Double_t anEDiff = anERecoilLindhard - anERecoil; //difference of ERecoil values
  Double_t aQDiff = aLind.GetQValue(anERecoilLindhard) - aQ; //difference of Q values
  
  return anEDiff*anEDiff+aQDiff*aQDiff;
}

ClassImp(KLindhard);

KLindhard::KLindhard(void)
{

  fLindhardFormula = new TF1("lindhard","[0]*pow(x,[1])",0.,1000.); 
  //range between 0 and 1000 keV. 
  
  //set the default values for Germanium
  SetParameterA(0.165);
  SetParameterB(0.185); 
}

KLindhard::~KLindhard(void)
{
  if(fLindhardFormula){
    delete fLindhardFormula;
    fLindhardFormula = 0;
  }

}

Double_t KLindhard::GetParameterA(void) const
{
  return fLindhardFormula->GetParameter(0);
}

Double_t KLindhard::GetParameterB(void) const
{
  return fLindhardFormula->GetParameter(1);
}

void KLindhard::SetParameterA(Double_t aVal)
{
  fLindhardFormula->SetParameter(0, aVal);
}

void KLindhard::SetParameterB(Double_t aVal)
{
  fLindhardFormula->SetParameter(1, aVal);
}

Double_t KLindhard::GetQValue(Double_t anEnergy) const
{
  return fLindhardFormula->Eval(anEnergy);
}

Double_t KLindhard::CalculateEOfMinDistanceLindhard(Double_t anERecoil,Double_t aQ, const Char_t* aMinimizerName,const Char_t* aMethod)
{
  // This method calculates the corresponding E_Recoil for the Lindhard function
  // for the least distance between an arbitrary point (anERecoil,aQ)
  // and a point of the LindhardFunction
  //
  // possible minimizers are
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2","Combined");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugateFR");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugatePR");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS2");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "SteepestDescent");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiFit", "");
  // ROOT::Math::Factory::CreateMinimizer("GSLSimAn", "");
  //
  // GSLMultiMin SteepestDescent is used by default
  
  ROOT::Math::Minimizer *theMinimizer = ROOT::Math::Factory::CreateMinimizer(aMinimizerName,aMethod);

  theMinimizer->SetMaxFunctionCalls(1000000); //this sets the maximal number of function calls
  theMinimizer->SetMaxIterations(100000); //this sets the maximal number of iteration
  theMinimizer->SetTolerance(0.00001); //this sets the tolerance
    
  ROOT::Math::Functor f(&DistToLindhardFunction,3); 
   
  theMinimizer->SetFunction(f);
 
  // Set the fixed variables
  theMinimizer->SetFixedVariable(0,"aERecoil",anERecoil);
  theMinimizer->SetFixedVariable(1,"aQ",aQ);
   
  //set the variable to be minimized
  theMinimizer->SetVariable(2,"anERecoilLindhard",1,0.001);
 
  theMinimizer->Minimize(); 
 
  const double *xs = theMinimizer->X();
  
  return xs[2];
}

Double_t KLindhard::CalculateMinDistanceToLindhard(Double_t anERecoil,Double_t aQ,const Char_t* aMinimizerName,const Char_t* aMethod)
{
  // This method calculates the least Euclidean distance between an arbitrary point (anERecoil,aQ)
  // and a point of the Lindhard function in the ERecoil-Q-plane
  //
  // possible minimizers are
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2","Combined");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Scan");
  // ROOT::Math::Factory::CreateMinimizer("Minuit2", "Fumili");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugateFR");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "ConjugatePR");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "BFGS2");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiMin", "SteepestDescent");
  // ROOT::Math::Factory::CreateMinimizer("GSLMultiFit", "");
  // ROOT::Math::Factory::CreateMinimizer("GSLSimAn", "");
  //
  // GSLMultiMin SteepestDescent is used by default
  
  ROOT::Math::Minimizer *theMinimizer = ROOT::Math::Factory::CreateMinimizer(aMinimizerName,aMethod);

    
  theMinimizer->SetMaxFunctionCalls(1000000); //this sets the maximal number of function calls
  theMinimizer->SetMaxIterations(100000); //this sets the maximal number of iterations
  theMinimizer->SetTolerance(0.00001); //this sets the tolerance
    
  ROOT::Math::Functor f(&DistToLindhardFunction,3); 
   
 
  theMinimizer->SetFunction(f);
 
  //set the fixed variables
  theMinimizer->SetFixedVariable(0,"anERecoil",anERecoil);
  theMinimizer->SetFixedVariable(1,"aQ",aQ);
   
  //set the variable to be minimized
  theMinimizer->SetVariable(2,"anERecoilLindhard",1,0.001);
 
  theMinimizer->Minimize(); 
 
  const double *xs = theMinimizer->X();
   
  
  return TMath::Sqrt(DistToLindhardFunction(xs));
  
}

