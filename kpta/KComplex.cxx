//______________________________________________________________________
//
// KComplex.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Methods of this class return magnitude or phase values for the input vector in a half-complex format. They are static,
// so there is no need to create an instance of KComplex. KComplex::Get.... should be enough
//
// WARNING. When you call a method below to get some calculation, YOU OWN THE MEMORY and therefore
// must take care to deallocate the space, otherwise you will create a leak. 
//



#include "KComplex.h"
#include <cmath>
#include <iostream>
using namespace std;

//ClassImp(KComplex);


KComplex::KComplex(void)
{
}

KComplex::~KComplex(void)
{
}

double* KComplex::GetHalfComplexMagnitude(double* input, unsigned int size)
{
  
  //This function returns a pointer to an array of size = "size/2" filled with the magnitude values of the input vector "input",
  //which has to be in half-complex format
  //
  //Don't forget - you own the memory pointed to by the returned pointer!
  //
  double* magnitude = 0; 
  if(size != 0){
    magnitude = new double[size/2+1];
    *magnitude = *input;
    *(magnitude+size/2)=*(input+size/2);
    for(unsigned int i = 1;i<size/2;i++)
      *(magnitude+i) = sqrt(*(input+i) * (*(input+i)) + *(input+size-i) * (*(input+size-i))); 
  }
  else
    cerr<<"size is zero!"<<endl;
  return magnitude;   
}

double* KComplex::GetHalfComplexPhase(double* input, unsigned int size)
{
  //This function returns a pointer to an array of size = "size/2" filled with the phase values of the input vector "input",
  //which has to be in half-complex format
  //
  //Don't forget - you own the memory pointed to by the returned pointer!
  //
  double* phase = 0; 
  if(size != 0){
    phase = new double[size/2+1];
    
    // z = a + ib
    // phase calculation in the interval [-pi,+pi]
    *phase = 0;
    *(phase+size/2)=0;
    for(unsigned int i = 0; i < size/2; i++){
      if(*(input+i) != 0){
        *(phase+i) = atan(*(input+size-i)/(*(input+i))); // a>0: phase = arctan(b/a)
        if((*(input+i)<0) && (*(input+size-i)>=0))       // a<0,b>=0: phase = arctan(b/a)+pi
          *(phase+i) += PI;
        if((*(input+i)<0) && (*(input+size-i)<0))        // a<0,b<0: phase = arctan(b/a)-pi
          *(phase+i) -= PI;
      }
      else{
        if(*(input+size-i) > 0)       // a=0,b>0: phase = pi/2
          *(phase+i) = PI/2.0;
        if(*(input+size-i) < 0)       // a=0,b<0: phase = -pi/2
          *(phase+i) =  -PI/2.0;
        if(*(input+size-i) == 0)      // a=0,b=0: phase undefined => set to 0
          *(phase+i) = 0;
      }       
    }    
  }
  else
    cerr<<"size is zero!"<<endl;
  
  return phase;    
}
