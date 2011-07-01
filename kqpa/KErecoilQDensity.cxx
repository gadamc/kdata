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
//

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
   // par[0] : mean ion energy
  // par[1] :  mean heat energy
  // par[2] :  sigma ion
  // par[3] : sigma heat
  // par[4] : sigma ion heat
  // par[5] : voltage bias
  // par[6] : epsilon
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
  Int_t aSize = (Int_t)par[0];
  Double_t det, c1, c2, c3, n, m, a, result, factor;
  
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


