//______________________________________________________________________
//
// KRootMeanSquare.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// GetRms calculates the square root of the mean of the square of the elements
// of the vector passed to the method. This is inclusive of the start value, but
// exclusive of the stop value. That is
//  GetRms(double *a, 4, 8) = a[4]**2 + a[5]**2 + a[6]**2 + a[7]**2
//



#include "KRootMeanSquare.h"
#include <cmath>

ClassImp(KRootMeanSquare);

KRootMeanSquare::KRootMeanSquare(void)
{

  InitializeMembers();
}

KRootMeanSquare::~KRootMeanSquare(void)
{

}

void KRootMeanSquare::InitializeMembers(void)
{
  

}
