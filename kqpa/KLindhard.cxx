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
  //to a point (anERecoilLindhard, Q(anERecoilLindhard) on the Lindhard function a*E^b
  //with a = 0.165 and b = 0.185
  //
  //The function is minimized by 
  //KQDistribution::CalculateMinDistanceToLinhard(Double_t anERecoil,Double_t aQ)
  
  Double_t anEnergyRecoil = xx[0];
  Double_t aQ = xx[1];
  Double_t aScaleEnergyRecoil = xx[2];
  Double_t aScaleQ = xx[3];
  Double_t anEnergyRecoilLindhard = xx[4];
  
  KLindhard aLind;
  
  Double_t anEDiff = anEnergyRecoilLindhard - anEnergyRecoil; //difference of ERecoil values
  Double_t aQDiff = aLind.GetQValue(anEnergyRecoilLindhard) - aQ; //difference of Q values
  
  return anEDiff*anEDiff/aScaleEnergyRecoil / aScaleEnergyRecoil +aQDiff*aQDiff / aScaleQ / aScaleQ;
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

Double_t KLindhard::GetQMeanValue(Double_t anEnergy,
                              Double_t anEnergyUncertainty) const
{
  // This method calculates the Q mean value for binned data with mean "anEnergy"
  // and uncertainty "anEnergyUncertainty" in 2nd order approximation
  //
  // BEGIN_LATEX
  // < Q > = a < E > ^{b} + b (b-1) a < E > ^{b-2} #sigma_{E}^{2}
  // END_LATEX
  return fLindhardFormula->Eval(anEnergy) + fLindhardFormula->Derivative2(anEnergy)*anEnergyUncertainty;
}


Double_t KLindhard::GetEOfMinDistanceLindhard(Double_t anEnergyRecoil,Double_t aQ,
                                              Double_t aScaleEnergyRecoil,
                                              Double_t aScaleQ,
                                              const Char_t* aMinimizerName,
                                              const Char_t* aMethod,
                                              Int_t aMaxNumFunctionCalls,
                                              Int_t aMaxNumIterations,
                                              Double_t aTolerance)
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

  theMinimizer->SetMaxFunctionCalls(aMaxNumFunctionCalls); //this sets the maximal number of function calls
  theMinimizer->SetMaxIterations(aMaxNumIterations); //this sets the maximal number of iteration
  theMinimizer->SetTolerance(aTolerance); //this sets the tolerance
    
  ROOT::Math::Functor f(&DistToLindhardFunction,5); 
   
  theMinimizer->SetFunction(f);
 
  // Set the fixed variables
  theMinimizer->SetFixedVariable(0,"anEnergyRecoil",anEnergyRecoil);
  theMinimizer->SetFixedVariable(1,"aQ",aQ);
  theMinimizer->SetFixedVariable(2,"aScaleEnergyRecoil",aScaleEnergyRecoil);
  theMinimizer->SetFixedVariable(3,"aScaleQ",aScaleQ);
   
  //set the variable to be minimized
  theMinimizer->SetVariable(4,"anERecoilLindhard",1,0.001);
 
  theMinimizer->Minimize(); 
 
  const double *xs = theMinimizer->X();
  
  return xs[2];
}

Double_t KLindhard::GetMinDistanceToLindhard(Double_t anEnergyRecoil,Double_t aQ,
                                             Double_t aScaleEnergyRecoil,
                                             Double_t aScaleQ,
                                             const Char_t* aMinimizerName,
                                             const Char_t* aMethod,
                                             Int_t aMaxNumFunctionCalls,
                                             Int_t aMaxNumIterations,
                                             Double_t aTolerance)
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

    
  theMinimizer->SetMaxFunctionCalls(aMaxNumFunctionCalls); //this sets the maximal number of function calls
  theMinimizer->SetMaxIterations(aMaxNumIterations); //this sets the maximal number of iterations
  theMinimizer->SetTolerance(aTolerance); //this sets the tolerance
    
  ROOT::Math::Functor f(&DistToLindhardFunction,5); 
   
 
  theMinimizer->SetFunction(f);
 
  //set the fixed variables
  theMinimizer->SetFixedVariable(0,"anEnergyRecoil",anEnergyRecoil);
  theMinimizer->SetFixedVariable(1,"aQ",aQ);
  theMinimizer->SetFixedVariable(2,"aScaleEnergyRecoil",aScaleEnergyRecoil);
  theMinimizer->SetFixedVariable(3,"aScaleQ",aScaleQ);
   
   
  //set the variable to be minimized
  theMinimizer->SetVariable(4,"anERecoilLindhard",1,0.001);
 
  theMinimizer->Minimize(); 
 
  const double *xs = theMinimizer->X();
   
  
  return TMath::Sqrt(DistToLindhardFunction(xs));
  
}

Double_t KLindhard::GetArcLength(Double_t anEnergyRecoil,
                                 Double_t aScaleFactor,
                                 Double_t aRefEnergyRecoil)  
{
  // This method calculates the arc length s(E_Recoil) on the Lindhard function
  // for a specified recoil energy with respect to the origin, where s(0)= 0
  // The scale factor is the recoil energy in keV which corresponds to one
  // unit length on the Q axis
  
  TF1 a("a","0.165");
  TF1 b("b","0.185");
  TF1 c("c",TString::Format("%lf",aScaleFactor).Data());
  TF1 aFunction("f","TMath::Sqrt(a^2*b^2 * x^(2*b-2) + 1/c^2)");
  return aFunction.Integral(aRefEnergyRecoil,anEnergyRecoil);
}

