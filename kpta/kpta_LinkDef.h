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
#pragma link C++ function KConvolution::SetResponse<double>;
#pragma link C++ function KConvolution::SetResponse<float>;
#pragma link C++ function KConvolution::SetResponse<int>;
#pragma link C++ function KConvolution::SetResponse<short>;
#pragma link C++ function KConvolution::SetResponse(double*, unsigned int, bool );
#pragma link C++ function KConvolution::SetResponse(float*, unsigned int, bool );
#pragma link C++ function KConvolution::SetResponse(int*, unsigned int, bool );
#pragma link C++ function KConvolution::SetResponse(short*, unsigned int, bool );
#pragma link C++ function KCorrelation::SetResponse<double>;
#pragma link C++ function KCorrelation::SetResponse<float>;
#pragma link C++ function KCorrelation::SetResponse<int>;
#pragma link C++ function KCorrelation::SetResponse<short>;
#pragma link C++ function KCorrelation::SetResponse(double*, unsigned int, bool );
#pragma link C++ function KCorrelation::SetResponse(float*, unsigned int, bool );
#pragma link C++ function KCorrelation::SetResponse(int*, unsigned int, bool );
#pragma link C++ function KCorrelation::SetResponse(short*, unsigned int, bool );

#pragma link C++ class KEraPeakFinder+;
#pragma link C++ class KOrderThresholdFinder+;
#pragma link C++ class KFIRFilter+;
#pragma link C++ class KHalfComplexArray+;
#pragma link C++ function KHalfComplexArray::Real<double>;
#pragma link C++ function KHalfComplexArray::Real<float>;
#pragma link C++ function KHalfComplexArray::Real<int>;
#pragma link C++ function KHalfComplexArray::Real<short>;
#pragma link C++ function KHalfComplexArray::Real(double*, unsigned int, unsigned int);
#pragma link C++ function KHalfComplexArray::Real(float*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::Real(int*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::Real(short*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::Imag<double>;
#pragma link C++ function KHalfComplexArray::Imag<float>;
#pragma link C++ function KHalfComplexArray::Imag<int>;
#pragma link C++ function KHalfComplexArray::Imag<short>;
#pragma link C++ function KHalfComplexArray::Imag(double*, unsigned int, unsigned int);
#pragma link C++ function KHalfComplexArray::Imag(float*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::Imag(int*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::Imag(short*, unsigned int, unsigned int );
#pragma link C++ function KHalfComplexArray::SetReal<double>;
#pragma link C++ function KHalfComplexArray::SetReal<float>;
#pragma link C++ function KHalfComplexArray::SetReal<int>;
#pragma link C++ function KHalfComplexArray::SetReal<short>;
#pragma link C++ function KHalfComplexArray::SetReal(double*, unsigned int, unsigned int, double);
#pragma link C++ function KHalfComplexArray::SetReal(float*, unsigned int, unsigned int, float );
#pragma link C++ function KHalfComplexArray::SetReal(int*, unsigned int, unsigned int, int );
#pragma link C++ function KHalfComplexArray::SetReal(short*, unsigned int, unsigned int, short );
#pragma link C++ function KHalfComplexArray::SetImag<double>;
#pragma link C++ function KHalfComplexArray::SetImag<float>;
#pragma link C++ function KHalfComplexArray::SetImag<int>;
#pragma link C++ function KHalfComplexArray::SetImag<short>;
#pragma link C++ function KHalfComplexArray::SetImag(double*, unsigned int, unsigned int, double);
#pragma link C++ function KHalfComplexArray::SetImag(float*, unsigned int, unsigned int, float );
#pragma link C++ function KHalfComplexArray::SetImag(int*, unsigned int, unsigned int, int );
#pragma link C++ function KHalfComplexArray::SetImag(short*, unsigned int, unsigned int, short );
#pragma link C++ function KHalfComplexArray::Multiply(double*, double*, double*, unsigned int);
#pragma link C++ function KHalfComplexArray::Multiply(float*, float*, float*, unsigned int);
#pragma link C++ function KHalfComplexArray::Multiply(short*, short*, short*, unsigned int);
#pragma link C++ function KHalfComplexArray::Multiply(int*, int*, int*, unsigned int);
#pragma link C++ function KHalfComplexArray::Multiply<double>;
#pragma link C++ function KHalfComplexArray::Multiply<float>;
#pragma link C++ function KHalfComplexArray::Multiply<int>;
#pragma link C++ function KHalfComplexArray::Multiply<short>;
// #pragma link C++ function KHalfComplexArray::Divide(double*, double*, double*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Divide(float*, float*, float*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Divide(short*, short*, short*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Divide(int*, int*, int*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Divide<double>;
// #pragma link C++ function KHalfComplexArray::Divide<float>;
// #pragma link C++ function KHalfComplexArray::Divide<int>;
// #pragma link C++ function KHalfComplexArray::Divide<short>;
// #pragma link C++ function KHalfComplexArray::Conjugate(double*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Conjugate(float*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Conjugate(short*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Conjugate(int*, unsigned int);
// #pragma link C++ function KHalfComplexArray::Conjugate<double>;
// #pragma link C++ function KHalfComplexArray::Conjugate<float>;
// #pragma link C++ function KHalfComplexArray::Conjugate<int>;
// #pragma link C++ function KHalfComplexArray::Conjugate<short>;

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
#pragma link C++ function KPatternRemoval::SetPattern<double>;
#pragma link C++ function KPatternRemoval::SetPattern<float>;
#pragma link C++ function KPatternRemoval::SetPattern<int>;
#pragma link C++ function KPatternRemoval::SetPattern<short>;

#pragma link C++ class KPeakDetectorProto+;
#pragma link C++ class KPeakFinder+;
#pragma link C++ class KPtaTestPulse+;
#pragma link C++ class KPulseAnalysisChain+;
#pragma link C++ class KPulseShifter+;
#pragma link C++ class KPulseTemplate+;
#pragma link C++ class KRealToHalfComplexDFT+;
#pragma link C++ class KRingBuffer+;

#pragma link C++ class KRootMeanSquare+;
#pragma link C++ function KRootMeanSquare::GetRms<double>;
#pragma link C++ function KRootMeanSquare::GetRms<float>;
#pragma link C++ function KRootMeanSquare::GetRms<int>;
#pragma link C++ function KRootMeanSquare::GetRms<short>;
#pragma link C++ function KRootMeanSquare::GetRms(double*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetRms(float*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetRms(int*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetRms(short*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetStdDev<double>;
#pragma link C++ function KRootMeanSquare::GetStdDev<float>;
#pragma link C++ function KRootMeanSquare::GetStdDev<int>;
#pragma link C++ function KRootMeanSquare::GetStdDev<short>;
#pragma link C++ function KRootMeanSquare::GetStdDev(double*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetStdDev(float*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetStdDev(int*, unsigned int, unsigned int );
#pragma link C++ function KRootMeanSquare::GetStdDev(short*, unsigned int, unsigned int );

#pragma link C++ class KSpecialFunctions+;
#pragma link C++ class KTrapezoidalFilter+;
#pragma link C++ class KTrapezoidalPeakDetector+;
#pragma link C++ class KWindow+;
#pragma link C++ class KWindowDesign+;




#endif 
