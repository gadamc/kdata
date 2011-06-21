//______________________________________________________________________
//
// KSimpleHighPassFilter.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Monday 20. June 2011
//
//
//
// This class performs a 1st-order high pass filter operation on the input pulse in the time domain.
// Can set the characteristic RC time constant via SetRc(double value);
// The value must be in units of "bin width", that is, in units of the sampling time
// of the input pulse.
// The output pulse is also in the time domain.
// Additionally, if you want to avoid some rounding errors during the integration
// you can call SetMinRCToBinRatio and give it a value greater than the value
// you gave in SetRc. The routine will then subdivide your input pulse accordingly
// and peform the integration on the subdivided pulse. 
// This class utilizes the KSimpleLowPassFilter class to perform the integration
// and then adds back on the necessary values to get a high pass filter. 
// This could be optimized if the integration/filtering code was all writen here.
// We'll see. If the processing takes too long, then these filters should be looked at.
//

#include "KSimpleHighPassFilter.h"
#include "KSimpleLowPassFilter.h"

KSimpleHighPassFilter::KSimpleHighPassFilter(void)
{
  
  InitializeMembers();
}

KSimpleHighPassFilter::~KSimpleHighPassFilter(void)
{
  
  
}


void KSimpleHighPassFilter::InitializeMembers(void)
{
  SetRc(20);
  SetMinRCToBinRatio(10.0);
}

bool KSimpleHighPassFilter::RunProcess(void)
{
  KSimpleLowPassFilter f;
  f.SetInputPulse(GetInputPulse());
  f.SetRc(GetRc());
  f.SetMinRCToBinRatio(GetMinRCToBinRatio());
  if(!f.RunProcess())
    return false;
  
  vector<double> lpf = f.GetOutputPulse();
  
  fOutputPulse.resize(fInputPulse.size());
  
  double nextvalue, currentvalue;
  
  
  try {
    
    for(int i = 0; i < (int)fInputPulse.size(); i++){
      if(i == (int)fInputPulse.size()-1)
        nextvalue = fInputPulse[i];
      else
        nextvalue = fInputPulse[i+1];
      
      if (i > 0) {
        if(fInputPulse[i-1] == fInputPulse[i])
          currentvalue = fInputPulse[i];
        else
          currentvalue = 0.5*(fInputPulse[i] + nextvalue);
      }
      
      fOutputPulse[i] = currentvalue - lpf[i];
    }
    
  } 
  
  catch (out_of_range& e) {
		cerr << "KSimpleHighPassFilter. exception caught: " << e.what() << endl;
		cerr << "    stopping calculation";
		return false;
	}
  
 
  return true;
}