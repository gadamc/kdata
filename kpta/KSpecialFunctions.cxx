//_____________________________________________
//
// KSpecialFunctions.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 9/27/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// Many functions in this class are taken from Numerical Recipes in C 
//


#include "KSpecialFunctions.h"
#include <cmath>
#include <iostream>

using namespace std;

#define ITMAX 100 
#define EPS 3.0e-7 
#define FPMIN 1.0e-30


KSpecialFunctions::KSpecialFunctions(void)
{
  //standard constructor 
  
}

KSpecialFunctions::~KSpecialFunctions(void)
{
  
}

double KSpecialFunctions::ErrF(double x)
{
  //Returns the error function erf(x). 
  return x < 0.0 ? -KSpecialFunctions::IncompleteGamma(0.5,x*x) : KSpecialFunctions::IncompleteGamma(0.5,x*x);  
}

double KSpecialFunctions::IncompleteGamma(double a, double x)
{
  //Returns the incomplete gamma function P(a,x).
  
  double gamser,gammcf,gln;
  if (x < 0.0 || a <= 0.0) {
    cout << "Invalid arguments in routine gammp" << endl;
    return -1;
  }
  
  if (x < (a+1.0)) { 
    KSpecialFunctions::Gser(&gamser,a,x,&gln); 
    return gamser;
  } 
  else { 
    KSpecialFunctions::Gcf(&gammcf,a,x,&gln); 
    return 1.0-gammcf;
  }
  
}

double KSpecialFunctions::GammaLn(double xx)
{
  //Returns the value ln[Γ(xx)] for x > 0.
  
  double x,y,tmp,ser;
  static double cof[6]={76.18009172947146,-86.50532032941677,
  24.01409824083091,-1.231739572450155,
  0.1208650973866179e-2,-0.5395239384953e-5}; 
  int j;
  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp); 
  ser=1.000000000190015;
  for (j=0;j<=5;j++) 
    ser += cof[j]/++y; 
    
  return -tmp+log(2.5066282746310005*ser/x);
  
}

void KSpecialFunctions::Gser(double *gamser, double a, double x, double* gln)
{
  //Returns the incomplete gamma function P (a, x) evaluated by its series representation as gamser. Also returns lnΓ(a) as gln
  
  int n;
  double sum,del,ap;
  *gln=KSpecialFunctions::GammaLn(a); 
  if (x <= 0.0) {
    if (x < 0.0) 
      cout << "x less than 0 in routine KSpecialFunctions::Gser" << endl;
    *gamser=0.0;
    return;
  } 
  else { 
    ap=a;
    del=sum=1.0/a;
    for (n=1;n<=ITMAX;n++) {
      ++ap;
      del *= x/ap;
      sum += del;
      if (fabs(del) < fabs(sum)*EPS) {
        *gamser=sum*exp(-x+a*log(x)-(*gln));
        return; 
      }
    }
    cout << "a too large, ITMAX too small in routine KSpecialFunctions::Gser" << endl; 
    return;
    }
}

void KSpecialFunctions::Gcf(double *gammcf, double a, double x, double *gln)
{
  int i;
  double an,b,c,d,del,h;
  *gln=KSpecialFunctions::GammaLn(a);
  b=x+1.0-a;
  c=1.0/FPMIN;
  d=1.0/b;
  h=d; 
  for (i = 1; i <= ITMAX; i++){
    an = -i*(i-a);
    b += 2.0;
    d = an*d + b;
    if (fabs(d) < FPMIN) d=FPMIN;
    c=b+an/c;
    if (fabs(c) < FPMIN) c=FPMIN;
    d=1.0/d;
    del=d*c;
    h *= del;
    if (fabs(del-1.0) < EPS) break;
  }
 
  if (i > ITMAX) cout << "a too large, ITMAX too small in KSpecialFunctions::Gcf" << endl;
  
  *gammcf=exp(-x+a*log(x)-(*gln))*h; //Put factors in front.
}







