//______________________________________________________________________
//
// KErecoilQDensity.cxx
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Friday 1. July 2011
//
// A simple class that calculates the detector resolution function A, that for a
// given true event(Erecoil,Q) there is a certain measured value (Erecoil,Q)
// By applying Bayes' theorem 
//BEGIN_LATEX
// g(a_{true} | a_{exp}) = #frac{ A(a_{exp} | a_{true}) f(a_{true})}{h(a_{exp})}
//END_LATEX
// and assuming a flat prior
//BEGIN_LATEX
// f(a_{true}) = const
//END_LATEX
// this function can be interpreted  as the reverse pdf
//BEGIN_LATEX
// g(a_{true} | a_{exp}) = A(a_{exp} | a_{true})
//END_LATEX
// that there is a certain  true value of (ERecoil,Q) 
// for a specifed experimental value (EIon, EHeat)
// (Bayesian probabilty)
// The posterior
//BEGIN_LATEX
// h(a_{exp})
//END_LATEX
// is justed a fixed parameter then, which leads to normalization of g.
//
// This can be done for a single event as well as for multiple events
// Additionally marginal distributions with respect to E_recoil and Q
// are available
//
// The user can simply instantiate these functions by calling 
// TF2 aFunction("aFunctionName",
//               &KErecoilQDensity::<aMethod>,
//               anNpx,
//               Xmin,
//               Xmax,
//               anNpy,
//               Ymin,
//               Ymax
//               NumberOfParameters);


#include "KErecoilQDensity.h"

ClassImp(KErecoilQDensity);

KErecoilQDensity::KErecoilQDensity(void)
{
}

KErecoilQDensity::~KErecoilQDensity()
{
}

Double_t KErecoilQDensity::SingleEventProbDensity(Double_t *x, Double_t * par)
{
  //This function represents the propability density g(E_recoil,Q)
  // (documented in $KDATA_ROOT/doc/ERecoilQDistribution.pdf) for a single event
  //
  // These experimental values are
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias/ epsilon_gamma
  //
  // x[0]: recoil energy
  // x[1]: Q value
  
  Double_t det = par[2]*par[2]*par[3]*par[3] - par[4]*par[4]*par[4]*par[4];
  Double_t c1 = par[3]*par[3]/det;
  Double_t c2 = par[2]*par[2]/det;
  Double_t c3 = par[4]*par[4]/det;
  Double_t n = x[0]*x[1]-par[0];
  Double_t m = (1+x[1]*par[5])/(1+par[5])*x[0]-par[1];
  Double_t a = TMath::Abs(x[0])/2/TMath::Pi()/TMath::Sqrt(det)/(1+par[5]);
  Double_t result = a*TMath::Exp(-0.5*(c1*n*n + c2*m*m - 2*c3*n*m));
  return result;
  // "TMath::Exp(-0.5 *( [3]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0])^2"
 // "+ [2]^2/([3]^2*[2]^2 - [4]^4) *( (1 + y * [5])/(1 + [5]) * x "
 // "- [1])^2"
 // "- 2 * [4]^2/([3]^2*[2]^2 - [4]^4) * ( y * x - [0]) * "
 // "( (1 + y * [5])/(1 + [5]) * x - [1])))"
 // "* TMath::Abs(x)/ 2 / TMath::Pi() / TMath::Sqrt([2]^2 * [3]^2 - [4]^4) /"
 // "(1+[5])"
}

Double_t KErecoilQDensity::MultiEventProbDensity(Double_t* x,Double_t* par)
{
  // This function represents the propability density for multiple events, which
  // is the product of single event propabilities g_i(E_recoil,i,Q_i).
  // (The events are assumed to be uncorrelated between each other)
  // Again from a Bayesian point of view with flat prior (f(a) = const) it
  // represents the propability densitiy
  //BEGIN_LATEX
  // f(#vec{a}|#vec{x})  = #prod_{i=0}^{n-1} f_{i}(a_{i}|x_{i})
  //END_LATEX
  // to have true events a_i in certain regions of the (ERecoil,Q)-planes
  // (in total a (ERecoil,Q)^n-plane) for the corresponding events x_i.
  // The function is 2n - dimensional, where n is the number of events
 //
 // The experimental values are
 // par[0]: number of events n
 // par[6*i+1]: mean ion energy of the i-th event
 // par[6*i+2]: mean heat energy of the i-th event
 // par[6*i+3]: sigma ion of the i-th event
 // par[6*i+4]: sigma heat of the i-th event
 // par[6*i+5]: sigma ion heat of the i-th event
 // par[6*i+6]: voltage bias/ epsilon_gamma of the i-th event
 //
 // The elements of the true values a_i are
 // x[2*i]: recoil energy of the i-th event
 // x[2*i+1]: Q value of the i-th event
 
  Int_t aSize = (Int_t)par[0];
  Double_t det, c1, c2, c3, n, m, a, result, factor;
  result = 1;
  for(Int_t k = 0; k<aSize; ++k) {
     det = par[6*k+3]*par[6*k+3]*par[6*k+4]*par[6*k+4] -
      par[6*k+5]*par[6*k+5]*par[6*k+5]*par[6*k+5];
    c1 = par[6*k+4]*par[6*k+4]/det;
    c2 = par[6*k+3]*par[6*k+3]/det;
    c3 = par[6*k+5]*par[6*k+5]/det;
    n = x[2*k]*x[2*k+1]-par[6*k+1];
    m = (1+x[2*k+1]*par[6*k+6])/(1+par[6*k+6])*x[2*k]-par[6*k+2];
    a = TMath::Abs(x[2*k])/2/TMath::Pi()/TMath::Sqrt(det)/(1+par[6*k+6]);
    factor = a*TMath::Exp(-0.5*(c1*n*n + c2*m*m - 2*c3*n*m));
    result *= factor;
  }
  return result;
}

Double_t KErecoilQDensity::SingleEventTrueProbDensity(Double_t* x,
                                                      Double_t* par)
{
  // This function represents f(a|x)
  // Bayes Theorem is given by 
  //BEGIN_LATEX
  // f(a|x) = #frac{f(x|a) #dot f(a)}{f(x)}
  //END_LATEX
  // So from a Bayesian point of view with flat prior (f(a) = const)  this
  // represents the propability density f(a|x) to have a true event in a certain
  // region in the  (ERecoil,Q)-plane for given experimental data for an event
  // The peculiarity about this function is that the uncertainties sigma ion and
  // sigma heat are dynamic since the uncertainties depend on the true value a
  // The uncertainties are linearly interpolated between uncertainties on zero
  // level and calibration level (by KQUncertainty::GetChannelUncertainty())
  //
  // The parameters are
  // par[0]: calibration energy
  // par[1]: mean ion energy
  // par[2]: sigma ion on zero level
  // par[3]: sigma ion on calibration level
  // par[4]: mean heat energy
  // par[5]: sigma heat on zero level
  // par[6]: sigma heat on calibration level
  // par[7] voltage bis/ epsilon_gamma
  //
  // x[0]: recoil energy
  // x[1]: Q value
  
  Double_t anEnergyIon, anEnergyHeat;
  KNeganovLuke::GetIonAndHeatEnergy(anEnergyIon,anEnergyHeat,
                                    x[1],x[0],
                                    par[7],1);
  Double_t UncerIon = KQUncertainty::GetChannelUncertainty(anEnergyIon,
                                                           par[2],
                                                           par[3],
                                                           par[0]);
  Double_t UncerHeat = KQUncertainty::GetChannelUncertainty(anEnergyHeat,
                                                            par[2],
                                                            par[3],
                                                            par[0]);
  Double_t n = anEnergyIon-par[1];
  Double_t m = anEnergyHeat-par[4];
  Double_t a = TMath::Abs(x[0])/2/TMath::Pi()/UncerIon/UncerHeat
               /(1+par[7]);
  Double_t result = a*TMath::Exp(-0.5*(n*n/UncerIon/UncerIon + m*m/UncerHeat/UncerHeat));
  return result;
  
}

Double_t KErecoilQDensity::SingleEventMarginalDensityErecoil(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the marginal distribution g(E_recoil) for a single
  // event, which is the integral of g(E_recoil,Q) over Q from -infinity to
  // infinity
  //
  // These experimental values are
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias/ epsilon_gamma
  
  Double_t sigma, mean,result;
  sigma = TMath::Sqrt(par[3]*par[3]*(1+par[5])*(1+par[5])
                                      -2*par[4]*par[4]*par[5]*(1+par[5])
                                      +par[2]*par[2]*par[5]*par[5]);
  mean = par[1]*(1+par[5])-par[0]*par[5];
  result = 1/TMath::Sqrt(2*TMath::Pi())/sigma *
                 TMath::Exp(- (x[0] - mean)*(x[0]-mean)/2/sigma/sigma);
  return result;
}

Double_t KErecoilQDensity::SingleEventMomentErecoil(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the function (E_recoil-E_recoil,ref)^n*g(E_recoil)
  // from which the central moments of E_recoil can be calculated
  //
  // These  values are
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias/ epsilon_gamma
  // par[6] : index of moment n
  // par[7] : reference point E_recoil,ref
  
  Double_t sigma, mean,result;
  sigma = TMath::Sqrt(par[3]*par[3]*(1+par[5])*(1+par[5])
                                      -2*par[4]*par[4]*par[5]*(1+par[5])
                                      +par[2]*par[2]*par[5]*par[5]);
  mean = par[1]*(1+par[5])-par[0]*par[5];
  result = 1/TMath::Sqrt(2*TMath::Pi())/sigma *
                 TMath::Exp(- (x[0] - mean)*(x[0]-mean)/2/sigma/sigma)
                 *TMath::Power(x[0]-par[7],par[6]);
  return result;
}

Double_t KErecoilQDensity::MultiEventMarginalDensityErecoil(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the marginal distribution g(E_recoil) for multiple
  // events, which is the product of single event marginal densities
  // g_i(E_recoil,i)
  // From a Bayesian point of view it represents the propability density to 
  // obtain  certain true values of the recoil energy E_recoil,i 
  // for arbitrary Q values Q_i
  // and the corresponding experimental data for an event
  //
  // These experimental values are
  // par[0]: number of events
   // par[6*i+1] : mean ion energy of the i-th event
  // par[6*i+2] :  mean heat energy of the i-th event
  // par[6*i+3] :  sigma ion of the i-th event
  // par[6*i+4] : sigma heat of the i-th event
  // par[6*i+5] : sigma ion heat of the i-th event.q
  // par[6*i+6] : voltage bias/ epsilon_gamma
  
  Double_t sigma,c,mean,result,n;
  c = 1/TMath::Sqrt(2*TMath::Pi());
  n = par[0];
  
  result = 1;
  for(Int_t k = 0; k<n; ++k) {
    sigma = TMath::Sqrt(par[6*k+4]*par[6*k+4]*(1+par[6*k+6])*(1+par[6*k+6])
                                      -2*par[6*k+5]*par[6*k+5]
                                      *par[6*k+6]*(1+par[6*k+6])
                                      +par[6*k+3]*par[6*k+3]
                                      *par[6*k+6]*par[6*k+6]);
    mean = par[6*k+2]*(1+par[6*k+6])-par[6*k+1]*par[6*k+6];
    result *= c/sigma *
                  TMath::Exp(- (x[0] - mean)*(x[k]-mean)/2/sigma/sigma);
  }
  return result;
}

Double_t KErecoilQDensity::SingleEventMarginalDensityQ(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the marginal distribution g(Q) for a single
  // event, which is the integral of g(E_recoil,Q) over E_recoil from -infinity
  // to infinity.
  // From a Bayesian point of view it represents the propability density to 
  // obtain a certain true value of the Q value for arbitrary E_recoil value
  // and given experimental data for an event
  //
  // These experimental values are
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias/ epsilon_gamma
  
  Double_t kE, aE, bE, cE,g11,det,c,c2,g12,g22,result;
  det = par[2]*par[2]*par[3]*par[3]-
         -par[4]*par[4]*par[4]*par[4];
  c = (1 + x[0]*par[5])/(1+par[5]);
  g11 = par[3]*par[3]/det;
  g12 = -par[4]*par[4]/det;
  g22 = par[2]*par[2]/det;
 kE = 1./2/TMath::Pi()/TMath::Sqrt(det)/(1+par[5]);
 aE = g11/2*x[0]*x[0]+g12*x[0]*c+g22/2*c*c;
 bE = g11*x[0]*par[0]+g12*x[0]*par[1]
          +(g12*par[0] + g22*par[1])*c;
 cE = - g11/2*par[0]*par[0] - g12*par[0]*par[1]-g22/2*par[1]*par[1];
 
 c2 = bE/2/TMath::Sqrt(aE);
 result =kE/aE*TMath::Exp(cE)+kE*TMath::Exp(cE+c2*c2)
               *c2/aE*TMath::Sqrt(TMath::Pi())*
               TMath::Erf(c2);
  return result;
}

Double_t KErecoilQDensity::SingleEventMomentQ(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the function  (Q-Q_ref)^n*g(Q)
  // from which the n-th central moment of Q can be calculated
  //
  // The values are
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias/ epsilon_gamma
  // par[6]: index of the moment n
  // par[7]: reference point Q_ref
  
  Double_t kE, aE, bE, cE,g11,det,c,c2,g12,g22,result;
  det = par[2]*par[2]*par[3]*par[3]-
         -par[4]*par[4]*par[4]*par[4];
  c = (1 + x[0]*par[5])/(1+par[5]);
  g11 = par[3]*par[3]/det;
  g12 = -par[4]*par[4]/det;
  g22 = par[2]*par[2]/det;
 kE = 1./2/TMath::Pi()/TMath::Sqrt(det)/(1+par[5]);
 aE = g11/2*x[0]*x[0]+g12*x[0]*c+g22/2*c*c;
 bE = g11*x[0]*par[0]+g12*x[0]*par[1]
          +(g12*par[0] + g22*par[1])*c;
 cE = - g11/2*par[0]*par[0] - g12*par[0]*par[1]-g22/2*par[1]*par[1];
 
 c2 = bE/2/TMath::Sqrt(aE);
 result =kE/aE*TMath::Exp(cE)+kE*TMath::Exp(cE+c2*c2)
               *c2/aE*TMath::Sqrt(TMath::Pi())*
               TMath::Erf(c2)*TMath::Power(x[0]-par[7],par[6]);
  return result;
}

Double_t KErecoilQDensity::MultiEventMarginalDensityQ(Double_t *x,
                                                             Double_t* par)
{
  // This function represents the marginal distribution g(Q) for multiple
  // events, which is the product of single event marginal densities
  // g_i(
  // From a Bayesian point of view it represents the propability density to 
  // obtain a certain true value of the Q value for arbitrary Q value
  // and given experimental data for an event
  //
  // These experimental values are
  // par[0]: number of events n
  // par[6*k+1] : mean ion energy of the k-th event
  // par[6*k+2] :  mean heat energy of the k-th event
  // par[6*k+3] :  sigma ion of the k-th event
  // par[6*k+4] : sigma heat of the k-th event
  // par[6*k+5] : sigma ion heat of the k-th event
  // par[6*k+6] : voltage bias/ epsilon_gamma
  
  Double_t kE, aE, bE, cE,g11,det,c,c2,g12,g22,n,result;
  n = par[0];
  
  result = 1;
  for(Int_t k = 0; k<n; ++k) {
    det = par[6*k+3]*par[6*k+3]*par[6*k+4]*par[6*k+4]-
          -par[6*k+5]*par[6*k+5]*par[6*k+5]*par[6*k+5];
    c = (1 + x[k]*par[6*k+6])/(1+par[6*k+6]);
    g11 = par[6*k+4]*par[6*k+4]/det;
    g12 = -par[6*k+5]*par[6*k+5]/det;
    g22 = par[6*k+3]*par[6*k+3]/det;
  kE = 1/2/TMath::Pi()/TMath::Sqrt(det)/(1+par[6*k+6]);
  aE = g11/2*x[k]*x[k]+g12*x[k]*c+g22/2*c*c;
  bE = g11*x[k]*par[6*k+1]+g12*x[k]*par[6*k+2]
            +(g12*par[6*k+1] + g22*par[6*k+2])*c;
  cE = - g11/2*par[6*k+1]*par[6*k+1] -
  g12*par[6*k+1]*par[6*k+2]-g22/2*par[6*k+2]*par[6*k+2];
  
  c2 = bE/2/TMath::Sqrt(aE);
  result *= kE/aE*TMath::Exp(cE)+kE*TMath::Exp(cE+c2*c2)
                *c2/aE*TMath::Sqrt(TMath::Pi())*
                TMath::Erf(c2);
  }
  return result;
}

Double_t KErecoilQDensity::MultiEventCummulativeProbDensity(Double_t* x,Double_t* par)
{
  // This function represents a cummulative distribution h(E_recoil,Q) for 
  // multiple events, which represents the propability that there exists at least one true event a
  // in a certain small region dOmega
  //
  //BEGIN_LATEX
  // P(#exists a #in d#Omega) = h(E_{recoil},Q) d#Omega = 1 - #prod_{i=1}^{n} (1 - g_{i}(E_{recoil},Q) d#Omega)
  // = 1 - ( 1  - #sum_{i=1}^{n} g_{i}(E_{recoil},Q) d#Omega  + O(d#Omega^{2})}
  // = #sum_{i=1}^{n} g_{i}(E_{recoil},Q) d#Omega
  //END_LATEX
  // 
  // Thus we obtain
  //
  //BEGIN_LATEX
  // h(E_{recoil},Q) = #sum_{i=1}^{n} g_{i}(E_{recoil},Q)
  //END_LATEX
  //
  // This distribution is unconvenient to integrate over large regions
  //
  //BEGIN_LATEX
  // d#Omega >> #frac{1}{g_i(E_{recoil},Q)}
  //END_LATEX
  //
  //where the function values of the distributions g_i(E_recoil,Q) have strong variations.
  //
  // These experimental values are
  // par[0]: number of events n
  // par[6*k+1] : mean ion energy of the k-th event
  // par[6*k+2] :  mean heat energy of the k-th event
  // par[6*k+3] :  sigma ion of the k-th event
  // par[6*k+4] : sigma heat of the k-th event
  // par[6*k+5] : sigma ion heat of the k-th event
  // par[6*k+6] : voltage bias/ epsilon_gamma
  
    Int_t aSize = (Int_t)par[0];
  Double_t det, c1, c2, c3, n, m, a, result, summand;
  result = 0;
  for(Int_t k = 0; k<aSize; ++k) {
     det = par[6*k+3]*par[6*k+3]*par[6*k+4]*par[6*k+4] -
      par[6*k+5]*par[6*k+5]*par[6*k+5]*par[6*k+5];
    c1 = par[6*k+4]*par[6*k+4]/det;
    c2 = par[6*k+3]*par[6*k+3]/det;
    c3 = par[6*k+5]*par[6*k+5]/det;
    n = x[0]*x[1]-par[6*k+1];
    m = (1+x[1]*par[6*k+6])/(1+par[6*k+6])*x[0]-par[6*k+2];
    a = TMath::Abs(x[0])/2/TMath::Pi()/TMath::Sqrt(det)/(1+par[6*k+6]);
    summand = a*TMath::Exp(-0.5*(c1*n*n + c2*m*m - 2*c3*n*m));
    result += summand;
  }
  return result;
  
}

Double_t KErecoilQDensity::MultiEventProbabilityOfAtLeastOneEvent(Double_t* x,Double_t* par)
{
  // This function represents the propability that there exists at least one true event
  //  in an arbitrary area Omega of rectangular shape in the E_recoil-Q plane
  // It is given by
  //BEGIN_LATEX
  // P(#exists a #in Omega) = h(E_{recoil},Q) d#Omega = 1 - #prod_{i=1}^{n} (1 - #int_{#Omega} g_{i}(E_{recoil},Q) d#Omega)
  //END_LATEX
  
  // The parameters are
  //
  // par[0] : number of events n
  // par[1]: integration tolerance
  // par[2] : lower boundary of the recoil energy for integration
  // par[3]: higher boundary of the recoil energy for integration
  // par[4]: lower boundary of the Q value for integration
  // par[5]: higher boundary of the Q value for integration
  // par[6*k+6]: ion energy of the k-th event
  // par[6*k+7]: heat energy of the k-th event
  // par[6*k+8]: ion energy uncertainty of the k-th event
  // par[6*k+9]: heat energy uncertainty of the k-th event
  // par[6*k+10]: ion-heat energy correlation of the k-th event
  // par[6*k+11]: voltage bias over epsilon_gamma of the k-th event
  
  Double_t p[6];
  Double_t aSize = par[0];
  Double_t aTolerance = par[1];
  Double_t anEnergyRecoilMin = par[2];
  Double_t anEnergyRecoilMax = par[3];
  Double_t aQvalueMin = par[4];
  Double_t aQvalueMax = par[5];
  Double_t product = 1;
  
  // create the function and wrap it
  TF2 f("f",
        &KErecoilQDensity::SingleEventProbDensity,
        anEnergyRecoilMin,
        anEnergyRecoilMax,
        aQvalueMin,
        aQvalueMax,
        6);
  ROOT::Math::WrappedMultiTF1 wf1(f);
  
  // create the integrator
  //ROOT::Math::AdaptiveIntegratorMultiDim ig;
  ROOT::Math::GSLMCIntegrator ig(ROOT::Math::IntegrationMultiDim::kVEGAS);
  
  //Set parameters of the integration
  ig.SetFunction(wf1);
  ig.SetRelTolerance(aTolerance);
  
  Double_t xmin[] = { anEnergyRecoilMin, aQvalueMin };
  Double_t xmax[] = { anEnergyRecoilMax, aQvalueMax };
  
  
  for(Int_t k = 0; k<aSize; ++k) {
    p[0] = par[6*k+6];
    p[1] = par[6*k+7];
    p[2] = par[6*k+8];
    p[3] = par[6*k+9];
    p[4] = par[6*k+10];
    p[5] = par[6*k+11];
    
    wf1.SetParameters(p);
    product *= 1 - ig.Integral(xmin,xmax);
  }
  
  return 1 - product;
  
}










