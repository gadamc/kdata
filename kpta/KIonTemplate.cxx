//_____________________________________________
//
// KIonTemplate.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 9/27/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//  The currently defined ion pulse template
//
//  p1 *(1 - p3) *(1 + erf( (time - p0)/p4)) for time <= p0
//  p1 *(1 + erf((time - p0)/p4)) * (exp(- (time - p0)/p2) - p3) for time > p0
//
//


#include "KIonTemplate.h"
#include "KSpecialFunctions.h"
#include <cmath>

//ClassImp(KIonTemplate);

KIonTemplate::KIonTemplate(void)
{
  //standard constructor 
  SetParams(0,0,0,0,0);  //what are the default set of values?
}

KIonTemplate::~KIonTemplate(void)
{

}

void KIonTemplate::SetParams(double p0, double p1, double p2, double p3, double p4)
{
  fParams[0] = p0;
  fParams[1] = p1;
  fParams[2] = p2;
  fParams[3] = p3;
  fParams[4] = p4;
}


double KIonTemplate::Get(double time)
{
  //returns
  //  p1 *(1 - p3) *(1 + erf( (time - p0)/p4)) for time <= p0
  //  p1 *(1 + erf((time - p0)/p4)) * (exp(- (time - p0)/p2) - p3) for time > p0
  //
  if(time <= *fParams)
    return *(fParams+1) * (1 - *(fParams+3)) * (1 + KSpecialFunctions::ErrF( (time - *fParams)/ *(fParams+4) ) );
  else 
    return *(fParams+1) * (1 + KSpecialFunctions::ErrF( (time - *fParams)/ *(fParams+4) ) ) * (exp(- (time - *fParams)/ *(fParams+2) ) - *(fParams+3));
}


