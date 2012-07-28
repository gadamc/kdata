//______________________________________________________________________
//
// KPeakDetectorProto.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPeakDetectorProto.h"
#include <cmath>
#include <iostream>

using namespace std;

//ClassImp(KPeakDetectorProto);

KPeakDetectorProto::KPeakDetectorProto(void)
{
  InitializeMembers();
}



KPeakDetectorProto::~KPeakDetectorProto(void)
{
  
}


void KPeakDetectorProto::InitializeMembers(void)
{
//   fPeaks.clear();
   fProcesses.clear();
   fDecayTimeConstant = 0.0;
   fPolarity = 1;
}

std::vector<double> KPeakDetectorProto::GetCorrelationCoefficient(std::vector<double> aInputPulse, std::vector<double> aPattern, unsigned int aStart, unsigned int aStop)
{
  // Returns a correlation coefficient of the input pulse with the pattern
  unsigned int stop = (aStop == 0) ? (aInputPulse.size()-aPattern.size()) : aStop;
  if( stop == aStop)
    if(stop > (aInputPulse.size()-aPattern.size()))
      stop = aInputPulse.size()-aPattern.size();
//   if((stop - aStart) < aPattern.size()){
//     cout << "Input pulse range is smaller than the pattern to correlate!"<<endl;
//     std::vector<double> Correlation (aInputPulse.size(),0.0);
//     return Correlation;
//   }
  
  std::vector<double> Correlation(stop-aStart,0.0);
  
  double mean = 0;
  double mean_patt = 0;
  double sqsumPattern = 0;
  std::vector<double> sqsumInput(stop-aStart,0.0);
  
  for(unsigned int i = 0; i < aPattern.size(); i++)
    mean_patt += aPattern[i];
  mean_patt /= aPattern.size();
  for(unsigned int i = 0; i < aPattern.size(); i++)
    sqsumPattern += (aPattern[i] - mean_patt) * (aPattern[i] - mean_patt);
  
  for(unsigned int n = 0; n < stop-aStart; n++){
    mean = 0;
    for(unsigned int i = 0; i < aPattern.size(); i++)
      mean += aInputPulse[ n + i ];
    mean /= aPattern.size();
    for(unsigned int i = 0; i < aPattern.size(); i++){
      Correlation[n] += (aInputPulse[ aStart + n + i ] - mean) * (aPattern[i] - mean_patt);
      sqsumInput[n] += (aInputPulse[ aStart + n + i ] - mean) * (aInputPulse[ aStart + n + i ] - mean);      
    }    
  }
  
  for(unsigned int n = 0; n < stop-aStart; n++)
    Correlation[n] /= sqrt(sqsumInput[n]*sqsumPattern);
  
  return Correlation;  
}

std::vector<double> KPeakDetectorProto::GetCorrelation(std::vector<double> aInputPulse, std::vector<double> aPattern, unsigned int aStart, unsigned int aStop)
{
  // Returns a correlation of the input pulse with the pattern
  unsigned int stop = (aStop == 0) ? (aInputPulse.size()-aPattern.size()) : aStop;
  if( stop == aStop)
    if(stop > (aInputPulse.size()-aPattern.size()))
      stop = aInputPulse.size()-aPattern.size();
  //cout << "GetCorrelation: start at " << aStart <<", stop at " << stop << endl;
  
  
  
//   if((stop - aStart) < aPattern.size()){
//     cout << "Input pulse range is smaller than the pattern to correlate!"<<endl;
//     std::vector<double> Correlation (aInputPulse.size(),0.0);
//     return Correlation;
//   }
  std::vector<double> Correlation (stop-aStart,0.0);
  
 

  for(unsigned int n = 0; n < stop-aStart; n++)
    for(unsigned int i = 0; i < aPattern.size(); i++)
      Correlation[n] += (aInputPulse[ aStart + n + i ]) * (aPattern[i]);  
    
  return Correlation;  
}

std::vector<double> KPeakDetectorProto::ConvertToVector(double* aPulse, unsigned int aSize)
{
  // Stores a trace at the address aPulse with the size aSize into a std::vector<double>
  std::vector<double> Pulse;
  for(unsigned int i = 0; i < aSize; i++)
    Pulse.push_back(*(aPulse + i));
  return Pulse;
}

std::vector < std::vector<double> > KPeakDetectorProto::CalculateDerivatives(std::vector<double> aInputPulse)
{
  // Calculates the first and the second derivatives of the input trace
  // Returns a vector, which first element is a trace with the first derivative and the second element
  // is a trace with the second derivative
  std::vector < std::vector<double> > Output;
  KOrderFilter ord1;
  KOrderFilter ord2;
  ord1.SetOrder(1);
  ord1.SetInitOutputValue(0.0);
  ord2.SetOrder(1);
  ord2.SetInitOutputValue(0.0);
  
  ord1.SetInputPulse(aInputPulse);
  ord1.RunProcess();
  ord2.SetInputPulse(ord1.GetOutputPulse(), ord1.GetOutputPulseSize());
  ord2.RunProcess();
  
  Output.push_back(ConvertToVector(ord1.GetOutputPulse(), ord1.GetOutputPulseSize()));
  Output.push_back(ConvertToVector(ord2.GetOutputPulse(), ord2.GetOutputPulseSize()));
  
  return Output;  
}

std::vector<double> KPeakDetectorProto::Normalize(std::vector<double> aInputPulse, unsigned int aStart, unsigned int aStop)
{
  // Normalizes the trace by its root mean square
  double rms = 0.0;
  unsigned int stop = (aStop == 0) ? aInputPulse.size() : aStop;
  for(unsigned int i = aStart; i < stop; i++)
    rms += aInputPulse[i] * aInputPulse[i];
  rms = sqrt(rms/double(aInputPulse.size()));
  
  std::vector<double> Output;
  for(unsigned int i = aStart; i < stop; i++)
    Output.push_back(aInputPulse[i]/double(rms));
  
  return Output;  
}

KPeakDetectorProto::PeakRegion KPeakDetectorProto::FindMaximum(std::vector<double> aInputPulse, double aThreshold, int aPolarity, unsigned int aStart, unsigned int aStop)
{ 
  // Returns the peak region above the threshold which contains the maximal value of the trace
  unsigned int stop = (aStop == 0) ? aInputPulse.size() : aStop;
  bool inRegion = false;
  if( !((aPolarity == 1) || (aPolarity == -1)) )
    cout << "Polarity should be either +1 or -1, which is not the case!" << endl;
  if( aThreshold < 0 )
    aThreshold *= -1;
  
  KPeakDetectorProto::PeakRegion peak; 
  peak.maxValue = 0.;
  peak.maxPos = 0;
  peak.from = 0;
  peak.to = 0;
  
  unsigned int from = 0;
  double actualVal = 0.;
  
  for(unsigned int i = aStart; i < stop; i++){
    actualVal = aInputPulse[i]*aPolarity;
    if( actualVal > aThreshold ){
      if(!inRegion){
        from = i;
        inRegion = true;
      }
      if( actualVal > peak.maxValue ){
        peak.maxValue = actualVal;
        peak.maxPos = i;
        peak.from = from;
      }
    }
    else{
      if(inRegion){
        inRegion = false;
        if( peak.from == from )
          peak.to = i;
      }        
    }
  }
  if(inRegion && peak.from == from)
    peak.to = stop-1;
  //cout << "maximal peak "<< peak.maxValue <<" at " << peak.maxPos << " between "<< peak.from << " and " << peak.to << endl;
  return peak;  
}

std::vector<KPeakDetectorProto::PeakRegion> KPeakDetectorProto::FindMultipleMaxima(std::vector<double> aInputPulse, double aThreshold, int aPolarity, unsigned int aMinRegionSize, unsigned int aStart, unsigned int aStop)
{
  // Returns a vector of peak regions above the threshold value and wider than the aMinRegionSize
  unsigned int stop = (aStop == 0) ? aInputPulse.size() : aStop;
  bool inRegion = false;
  if( !((aPolarity == 1) || (aPolarity == -1)) )
    cout << "Polarity should be either +1 or -1, which is not the case!" << endl;
  if( aThreshold < 0 )
    aThreshold *= -1;
  
  vector<KPeakDetectorProto::PeakRegion> peaks;
  PeakRegion peak; 
  peak.maxValue = 0.;
  peak.maxPos = 0;
  peak.from = 0;
  peak.to = 0;
  
  unsigned int from = 0;
  double actualVal = 0.;
  for(unsigned int i = aStart; i < stop; i++){
    actualVal = aInputPulse[i]*aPolarity;
    if( actualVal > aThreshold ){
      if(!inRegion){
        from = i;
        inRegion = true;
      }
      if( actualVal > peak.maxValue ){
        peak.maxValue = actualVal;
        peak.maxPos = i;
        peak.from = from;
      }
    }
    else{
      if(inRegion){
        inRegion = false;
        if( peak.from == from )
          peak.to = i;
        if( peak.to - peak.from >= aMinRegionSize)
          peaks.push_back(peak);
        peak.maxValue = 0.;
        from = 0;
      }
    }
  }
  if(inRegion && peak.from == from){
    peak.to = stop-1;
    peaks.push_back(peak);
  }
 /* cout << peaks.size() << " peaks found:"<<endl;
  for(unsigned int i = 0; i < peaks.size(); i++)
    cout << "peak value " << peaks[i].maxValue << " at " << peaks[i].maxPos << " between " << peaks[i].from << " and " << peaks[i].to << endl;*/
  return peaks;  
}

bool KPeakDetectorProto::AddFilteringProcess(unsigned int aRiseTime, unsigned int aFlatTop, double aThreshold, unsigned int aDerivative, unsigned int aProcessType, bool aMultiplePeaks, bool aAdjustConstrains, bool aNormalize)
{
  // This function adds a filtering process into the processing chain. The first process in the chain is allowed to
  // return multiple peaks. All following processes are working on the peak regions returned by the first
  // process. The processes can adjust the upper and lower limits of the peak region (and the maximum position) or
  // just work as a filter which either rejects a peak region or passes it to the next process without adjusting
  // the peak region constrains.
  //  Parameters:
  //    aRiseTime   - rise time of the applied trapezoidal filter
  //    aFlatTop    - flat top width of the applied trapezoidal filter 
  //    aThreshold  - a threshold value which a peak has to reach to be accepted
  //    aDerivative - pulse to which the correlation will be applied:
  //                      0:  output of the trapezoidal filter
  //                      1:  first derivative of the trapezoidal filter 
  //                      2:  second derivative of the trapezoidal filter 
  //    aProcessType  - type of correlation function:
  //                      0:  correlation
  //                      1:  correlation coefficient
  //    aMultiplePeaks  - "true" for search of multiple peaks in one trace (only allowed for the first process in the chain
  //                      "false" for search of the global maximum
  //    aAdjustConstrains - Constrains adjustment / peak region filtering mode:
  //                      "true": if the peak region passes the process, when its upper/lower limit and the maximum value
  //                              will be adjusted before passing to the next process
  //                      "false": the peak region will be either accepted or rejected, in the case of acceptance
  //                              the peak region properties will passed unchanged to the next process
  //    aNormalize  - if "true" then pulse will be normalized by its root mean square before applying the correlation
  
  FilteringProcess process;
  process.riseTime = aRiseTime;
  process.flatTop = aFlatTop;
  process.threshold = (aThreshold > 0) ? aThreshold : -1 * aThreshold;
  if(aDerivative <= 2)
    process.derivative = aDerivative;
  else{
    cout << "KPeakDetectorProto::AddFilteringProcess: Invalid value of the derivative parameter (only 0,1,2 allowed!)"<<endl;
    return false;
  }
  if(aProcessType <= 1)
    process.processType = aProcessType;
  else{
    cout << "KPeakDetectorProto::AddFilteringProcess: Invalid value of the processType parameter (only 0 and 1 allowed!)"<<endl;
    return false;
  }
  if(aMultiplePeaks && !fProcesses.empty()){
    cout << "KPeakDetectorProto::AddFilteringProcess: Multiple peaks search only supported for the first process in chain!" << endl;
    return false;
  }
  process.multiplePeaks = aMultiplePeaks;
  process.adjustConstrains = aAdjustConstrains;
  process.normalize = aNormalize;
  
  fProcesses.push_back(process);
  return true;
}

bool KPeakDetectorProto::RunProcess(void)
{
  // Main process, which applies the filter processing chain and stores peak regions which passed through
  // all processes in fRemainingRegions (first element - maximal value, second element - lower limit of
  // the corresponding peak region, third element - upper limit of the corresponding peak region.
  if(fInputPulse.empty()){
    cout << "KPeakDetectorProto::RunProcess: Input pulse is empty!" << endl;
    return false;
  }
  if(fProcesses.empty()){
    cout << "KPeakDetectorProto::RunProcess: No filtering processes added to the chain!" << endl;
    return false;
  }
  if(fDecayTimeConstant == 0.){
    cout << "KPeakDetectorProto::RunProcess: Decay time constant is not set!" << endl;
    return false;
  }
  
  vector<KPeakDetectorProto::PeakRegion> peaks,temp_peaks;
  PeakRegion peak; 
  peak.maxValue = 0.;
  peak.maxPos = 0;
  peak.from = 0;
  peak.to = fInputPulse.size();
  peaks.push_back(peak);
  KTrapezoidalFilter trapfilter;
  vector<double> trapOutput, corrInput, pattern, corrResult;
  std::vector < std::vector<double> > derivatives;
  unsigned int temp_from,temp_to;
  double minRegionSize = 4;
  
  //fine tuning and debugging
  dCorrelations.resize(fProcesses.size());
  dPeakRegions.resize(fProcesses.size());
  dTrapOutputs.resize(fProcesses.size());
  dCorrInputs.resize(fProcesses.size());
  //----------
  
  
  for(unsigned int proc = 0; proc < fProcesses.size(); proc++){
    //cout << " Starting filtering process: " << proc << endl;
    trapfilter.SetInputPulse(fInputPulse);
    trapfilter.SetParams(fDecayTimeConstant, fProcesses[proc].riseTime, fProcesses[proc].flatTop);
    if(!trapfilter.RunProcess()){
      cout << "KPeakDetectorProto::RunProcess: Trapezoidal Filter failed! Number = " << proc << endl;
      return false;
    }
    trapOutput = ConvertToVector(trapfilter.GetOutputPulse(), trapfilter.GetOutputPulseSize());
    
    // fine tuning and debugging
    dTrapOutputs[proc] = trapOutput;
    //---
    
    // Setting up the pulse/pulse derivative which should be correlated
      if(fProcesses[proc].derivative == 0)
        corrInput = trapOutput;
      else{
        derivatives = CalculateDerivatives(trapOutput);
        corrInput = derivatives[(fProcesses[proc].derivative == 1) ? 0 : 1 ];
      }
      
      // fine tuning and debugging
      dCorrInputs[proc] = corrInput;
      //-----
      
    // Setting up the correlation pattern
    switch(fProcesses[proc].derivative){
      // pulse itself
      case 0:{
        pattern.clear();
        for(unsigned int i = 0; i < fProcesses[proc].riseTime; i++)
          pattern.push_back(i/double(fProcesses[proc].riseTime));
        for(unsigned int i = 0; i < fProcesses[proc].flatTop; i++)
          pattern.push_back(1.0);
        for(unsigned int i = 0; i < fProcesses[proc].riseTime; i++)
          pattern.push_back(1.0 - i/double(fProcesses[proc].riseTime));
        minRegionSize = 4;
        break;
      }
      // first derivative
      case 1:{
        pattern.clear();
        for(unsigned int i = 0; i < fProcesses[proc].riseTime; i++)
          pattern.push_back(1.0);
        for(unsigned int i = 0; i < fProcesses[proc].flatTop; i++)
          pattern.push_back(0.0);
        for(unsigned int i = 0; i < fProcesses[proc].riseTime; i++)
          pattern.push_back(-1.0);
        minRegionSize = 4;
        break;
      }
      // second derivative
      case 2:{
        pattern.clear();
        pattern.assign(2*fProcesses[proc].riseTime + fProcesses[proc].flatTop+3, 0.0);
        pattern[1] = 1.0;
        if(fProcesses[proc].flatTop > 0){
          pattern[fProcesses[proc].riseTime + 1] = -1.0;
          pattern[fProcesses[proc].riseTime + fProcesses[proc].flatTop + 1] = -1.0;
        }
        else
          pattern[fProcesses[proc].riseTime + 1] = -2.0;
        pattern[2*fProcesses[proc].riseTime + fProcesses[proc].flatTop + 1] = 1.0;
        minRegionSize = 0;
        break;
      }
    }
    
    temp_peaks.clear();
    dCorrelations[proc].assign(fInputPulse.size(),0.0);
    for(unsigned int n = 0; n < peaks.size(); n++){
      if((peaks[n].from - peaks[n].to) < 3*pattern.size()){
        temp_from = ((peaks[n].maxPos - 1.5*pattern.size()) > 0) ? peaks[n].maxPos - 1.5*pattern.size() : 0;
        temp_to = peaks[n].maxPos + 1.5*pattern.size();
      }
      else{
        temp_from = peaks[n].from;
        temp_to = peaks[n].to;
      };
      
      if(fProcesses[proc].processType == 0)
        corrResult = GetCorrelation(corrInput, pattern, peaks[n].from, peaks[n].to);
      if(fProcesses[proc].processType == 1)
        corrResult = GetCorrelationCoefficient(corrInput, pattern, peaks[n].from, peaks[n].to);
      
      if(fProcesses[proc].normalize)
        corrResult = Normalize(corrResult);
      
      // fine tuning and debugging
      for(unsigned int i = 0; i < corrResult.size(); i++)
        dCorrelations[proc][i+peaks[n].from] = corrResult[i];
      //------
      
      // This will only work if you search for multiple peaks in one peak region only!
      if(fProcesses[proc].multiplePeaks){
        temp_peaks = FindMultipleMaxima(corrResult, fProcesses[proc].threshold, fPolarity, minRegionSize);
        //cout << " Multiple peaks search processed. " << temp_peaks.size() << " peaks found." << endl;
      }
      else{
        peak = FindMaximum(corrResult, fProcesses[proc].threshold, fPolarity);
        if(peak.maxValue > 0){
          if(fProcesses[proc].adjustConstrains){
            peak.maxPos += peaks[n].from;
            peak.from += peaks[n].from;
            peak.to += peaks[n].from;
            temp_peaks.push_back(peak);
            //cout << "Updating peak" << peaks[n].maxPos << ", new constrains: max at " << peak.maxPos << " in [" << peak.from << "," << peak.to << "]" << endl;
          }
          else{
            temp_peaks.push_back(peaks[n]);
            //cout << " Peak at " << peaks[n].maxPos << " is still remaining " << endl;
          }
        }
        //else
          //cout << " Peak at " << peaks[n].maxPos << " disappears " << endl;
      }
    }
    peaks = temp_peaks;
    dPeakRegions[proc].clear();
    for(unsigned int i = 0; i < peaks.size();i++){
      dPeakRegions[proc].push_back(peaks[i].maxPos);
      dPeakRegions[proc].push_back(peaks[i].from);
      dPeakRegions[proc].push_back(peaks[i].to);
    }
    //cout << "Number of remaining peaks after processing: " << peaks.size() << endl;
  }
  
  fRemainingPeaks.clear();
  fRemainingPeaks.resize(peaks.size());
  
  for(unsigned int i = 0; i < peaks.size(); i++){
    fRemainingPeaks[i].push_back(double(peaks[i].maxPos));
    fRemainingPeaks[i].push_back(double(peaks[i].from));
    fRemainingPeaks[i].push_back(double(peaks[i].to));
    //cout << "KPeakDetectorProto:Peak at " << fRemainingPeaks[i][0] << " in [" << fRemainingPeaks[i][1] << "," << fRemainingPeaks[i][2] << "]" << endl;
  }
  
  
  return true;
}