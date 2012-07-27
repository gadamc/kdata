//______________________________________________________________________
//
// KPeakDetectorProto2.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPEAKDETECTORPROTO_H__
#define __KPEAKDETECTORPROTO_H__


#include "KTrapezoidalFilter.h"
#include "KOrderFilter.h"
#include <string.h>
#include <vector>

using namespace std;

class KPeakDetectorProto { 
  
public:
  //Constructors
  KPeakDetectorProto(void);
  //for the memory-savy programmers
  //KPeakDetectorProto(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);
  
  virtual ~KPeakDetectorProto(void);
  virtual void InitializeMembers(void);
  
  struct PeakRegion {
    double maxValue;
    unsigned int maxPos;
    unsigned int from;
    unsigned int to;
  };
  
  struct FilteringProcess {
    unsigned int riseTime;
    unsigned int flatTop;
    double threshold;
    unsigned int derivative; // 0 - pulse shape, 1 - first derivative, 2 - second derivative
    unsigned int processType; // 0 - correlation, 1 - correlation coefficient
    bool multiplePeaks;
    bool adjustConstrains;
    bool normalize;    
  };

  virtual bool RunProcess(void);
  virtual void SetInputPulse(std::vector<double> aInputPulse){ fInputPulse = aInputPulse; }
  virtual void SetInputPulse(double* aInputPulse, unsigned int aSize){ fInputPulse = ConvertToVector(aInputPulse,aSize);}
  virtual void SetPolarity(int aPolarity){ fPolarity = aPolarity; }
  virtual void SetDecayTimeConstant(double aDecayTimeConstant){ fDecayTimeConstant = (aDecayTimeConstant>0) ? aDecayTimeConstant : 0; }

  virtual std::vector < std::vector<double> > CalculateDerivatives(std::vector<double> aInputPulse);
  
  virtual std::vector<double> ConvertToVector(double* aPulse, unsigned int aSize);

  virtual std::vector<double> GetCorrelationCoefficient(std::vector<double> aInputPulse, std::vector<double> aPattern, unsigned int aStart = 0, unsigned int aStop = 0);
  virtual std::vector<double> GetCorrelation(std::vector<double> aInputPulse, std::vector<double> aPattern, unsigned int aStart = 0, unsigned int aStop = 0);
   
  virtual std::vector<double> Normalize(std::vector<double> aInputPulse, unsigned int aStart = 0, unsigned int aStop = 0);
  virtual PeakRegion FindMaximum(std::vector<double> aInputPulse, double aThreshold, int aPolarity = 1, unsigned int aStart = 0, unsigned int aStop = 0);
  virtual std::vector<KPeakDetectorProto::PeakRegion> FindMultipleMaxima(std::vector<double> aInputPulse, double aThreshold, int aPolarity = 1, unsigned int aMinRegionSize = 4, unsigned int aStart = 0, unsigned int aStop = 0);
  
  virtual bool AddFilteringProcess(unsigned int aRiseTime, unsigned int aFlatTop, double aThreshold, unsigned int aDerivative = 1, unsigned int aProcessType = 0, bool aMultiplePeaks = false, bool aAdjustConstrains = true, bool aNormalize = true);
  virtual std::vector< std::vector<double> > GetRemainingPeaks(void){ return fRemainingPeaks; }
  
  // for debugging and fine tuning
//   virtual std::vector<double> GetDCorrelations(unsigned int n, unsigned int i){
//     std::vector<double> trace;
//     if(n >= dCorrelations.size())
//       return trace;
//     trace.clear();
//     for(unsigned int j = 0; j < dCorrelations[n][i].size(); j++) 
//         trace.push_back(dCorrelations[n][i][j]); 
//     return trace;
//   }
//   virtual unsigned int GetNumCorrTraces(unsigned int n){
//     if(dCorrelations.size() > n){
//       return dCorrelations[n].size();
//     }
//     else
//       return 0;
//   }
  
  // Variables for debugging and finetuning:
  std::vector< std::vector<double> > dTrapOutputs;
  std::vector< std::vector<double> > dCorrInputs;
  std::vector< std::vector<double> > dCorrelations;
  std::vector< std::vector<unsigned int> > dPeakRegions;
  //-------
  
  
protected:
   std::vector< std::vector<double> > fRemainingPeaks; 
   std::vector<KPeakDetectorProto::FilteringProcess> fProcesses;
   double fDecayTimeConstant;
   std::vector<double> fInputPulse;
   std::vector<double> fOutputPulse;
   int fPolarity; // -1 or 1
   
   
   

  
    
  
private:
  //private methods
  
  
  ClassDef(KPeakDetectorProto,1);
  
};


#endif
