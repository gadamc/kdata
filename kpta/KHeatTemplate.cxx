//_____________________________________________
//
// KHeatTemplate.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 9/27/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// if(time >= p[0])  
// return p[1] * (1 - exp(- (time - p[0])/p[2] )) * (  exp(- (time - p[0])/p[3] )  
//                             + p[4]* exp( -(time - p[0])/p[5] )          
//                             + p[6]* exp( -(time - p[0])/p[7] )
// else return 0.0
//


#include "KHeatTemplate.h"
#include "KSpecialFunctions.h"
#include <cmath>

KHeatTemplate::KHeatTemplate(void)
{
  //standard constructor 
}

KHeatTemplate::~KHeatTemplate(void)
{

}

void KHeatTemplate::SetParams(double p0, double p1, double p2, double p3, double p4,
                              double p5, double p6, double p7)
{
  fParams[0] = p0;
  fParams[1] = p1;
  fParams[2] = p2;
  fParams[3] = p3;
  fParams[4] = p4;
  fParams[5] = p5;
  fParams[6] = p6;
  fParams[7] = p7;
}


double KHeatTemplate::Get(double time)
{
  // if(time >= p[0])  
  // return p[1] * (1 - exp(- (time - p[0])/p[2] )) * (  exp(- (time - p[0])/p[3] )  
  //                             + p[4]* exp( -(time - p[0])/p[5] )          
  //                             + p[6]* exp( -(time - p[0])/p[7] )
  //   

  if(time >= *fParams)
    return *(fParams+1) * (1 - exp( (*fParams - time)/ *(fParams+2) )) * (  exp( (*fParams - time)/ *(fParams+3) )  
                              + *(fParams+4) * exp( (*fParams - time)/ *(fParams+5)  )          
                              + *(fParams+6) * exp( (*fParams - time)/ *(fParams+7)  ) );
  
  else return 0.0;
}


