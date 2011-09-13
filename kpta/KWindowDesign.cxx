//______________________________________________________________________
//
// KWindowDesign.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Methods of this class return a set of coefficients of different window functions. They are static,
// so there is no need to create an instance of KWindow. KWindow::GetTukeyWindow should be enough
//
// WARNING. When you call a method below to create a window function, YOU OWN THE MEMORY and therefore
// must take care to deallocate the space, otherwise you will create a leak. 
//



#include "KWindowDesign.h"
#include <cmath>
#include <iostream>
using namespace std;


KWindowDesign::KWindowDesign(void)
{
}

KWindowDesign::~KWindowDesign(void)
{
}

double* KWindowDesign::GetTukeyWindow(unsigned int size, double alpha)
{
  //A Tukey window is a window that is flat in the middle with an amplitude of 1 and flanked by
  //cosine functions that smoothly change the window value from 1 to zero.
  //
  //Don't forget - you own the memory pointed to by the returned pointer!
  //
  
  double* coef = 0; 
  if(size != 0){
    coef = new double[size];
    double  temp = alpha*(size-1)/2.0;
    for(int i = 0; i < (int) temp; i++)
      *(coef+i) = 0.5 + 0.5*cos(PI*((i/(double)temp) - 1));
    for(unsigned int i = (unsigned int) temp; i < (unsigned int) (size-temp); i++)
      *(coef+i) = 1.0;
    for(unsigned int i = (unsigned int) (size-temp); i < size; i++)
      *(coef+i) = 0.5 + 0.5*cos(PI*((i/(double) temp) - 2.0/alpha - 1));
  }
  else
    cerr<<"size is zero!"<<endl;
  return coef;   
}

double* KWindowDesign::GetBlackmanWindow(unsigned int size, double alpha)
{
  //A Blackman window looks a lot like a guassian.
  //
  //Don't forget - you own the memory pointed to by the returned pointer!
  //
  double* coef = 0; 
  if(size != 0){
    coef = new double[size];
    for(int i = 0; i < size; i++)
      *(coef+i) = (1-alpha)/2.0 - 0.5*cos(2*PI*i/(double)(size-1))+0.5*alpha*cos(4*PI*i/(double)(size-1));
  }
  else
    cerr<<"size is zero!"<<endl;
  return coef;    
}
