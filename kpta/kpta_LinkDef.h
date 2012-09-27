#include <vector>
#include "KPtaProcessor.h"
//using namespace std;

#ifdef __CINT__ 
#pragma link off all globals; 
#pragma link off all classes; 
#pragma link off all functions; 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedef;

// #pragma link C++ class std::vector<double>;
// #pragma link C++ class std::vector<float>;
// #pragma link C++ class std::vector<int>;
// #pragma link C++ class std::vector<short>;
#pragma link C++ class KPtaProcessor+;
//#pragma link C++ function <double>KPtaProcessor::SetInputPulse(std::vector<double>&);
//#pragma link C++ function <float>KPtaProcessor::SetInputPulse(std::vector<float>& );
//#pragma link C++ function <int>KPtaProcessor::SetInputPulse(std::vector<int>& );
//#pragma link C++ function <short>KPtaProcessor::SetInputPulse(std::vector<short>& );
#pragma link C++ function KPtaProcessor::SetInputPulse<double>;
#pragma link C++ function KPtaProcessor::SetInputPulse<float>;
#pragma link C++ function KPtaProcessor::SetInputPulse<int>;
#pragma link C++ function KPtaProcessor::SetInputPulse<short>;
#pragma link C++ function KPtaProcessor::SetInputPulse(double*, unsigned int );
#pragma link C++ function KPtaProcessor::SetInputPulse(float*, unsigned int );
#pragma link C++ function KPtaProcessor::SetInputPulse(int*, unsigned int );
#pragma link C++ function KPtaProcessor::SetInputPulse(short*, unsigned int );

#pragma link C++ class KBaselineRemoval+;
#pragma link C++ class KComplex+;
#pragma link C++ class KConvolution+;
#pragma link C++ class KCorrelation+;
#pragma link C++ class KEraPeakFinder+;
#pragma link C++ class KFIRFilter+;
#pragma link C++ class KHalfComplexArray+;
#pragma link C++ class KHalfComplexPower+;
#pragma link C++ class KHalfComplexToRealDFT+;
#pragma link C++ class KHeatTemplate+;
#pragma link C++ class KIIRFilter+;
#pragma link C++ class KIIRFirstOrder+;
#pragma link C++ class KIIRFourthOrder+;
#pragma link C++ class KIIRSecondOrder+;
#pragma link C++ class KIIRThirdOrder+;
#pragma link C++ class KIonTemplate+;
#pragma link C++ class KLinearRemoval+;
#pragma link C++ class KMultiEraPeakFinder+;
#pragma link C++ class KNoiseWhitening+;
#pragma link C++ class KNullProcessor+;

#pragma link C++ class KOptimalFilter+;
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum<double>;
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum<float>;
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum<int>;
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum<short>;
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum(double*, unsigned int );
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum(float*, unsigned int );
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum(int*, unsigned int );
#pragma link C++ function KOptimalFilter::SetNoiseSpectrum(short*, unsigned int );
#pragma link C++ function KOptimalFilter::SetTemplateDFT<double>;
#pragma link C++ function KOptimalFilter::SetTemplateDFT<float>;
#pragma link C++ function KOptimalFilter::SetTemplateDFT<int>;
#pragma link C++ function KOptimalFilter::SetTemplateDFT<short>;
#pragma link C++ function KOptimalFilter::SetTemplateDFT(double*, unsigned int );
#pragma link C++ function KOptimalFilter::SetTemplateDFT(float*, unsigned int );
#pragma link C++ function KOptimalFilter::SetTemplateDFT(int*, unsigned int );
#pragma link C++ function KOptimalFilter::SetTemplateDFT(short*, unsigned int );

#pragma link C++ class KOrderFilter+;
#pragma link C++ class KPatternRemoval+;
#pragma link C++ class KPeakDetectorProto+;
#pragma link C++ class KPeakFinder+;
#pragma link C++ class KPtaTestPulse+;
#pragma link C++ class KPulseAnalysisChain+;
#pragma link C++ class KPulseShifter+;
#pragma link C++ class KPulseTemplate+;
#pragma link C++ class KRealToHalfComplexDFT+;
#pragma link C++ class KRingBuffer+;
#pragma link C++ class KRootMeanSquare+;
#pragma link C++ class KSimpleHighPassFilter+;
#pragma link C++ class KSimpleLowPassFilter+;
#pragma link C++ class KSpecialFunctions+;
#pragma link C++ class KTrapezoidalFilter+;
#pragma link C++ class KTrapezoidalPeakDetector+;
#pragma link C++ class KWindow+;
#pragma link C++ class KWindowDesign+;




#endif 
