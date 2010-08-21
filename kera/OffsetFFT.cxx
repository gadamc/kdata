/**
   @file    OffsetFFT.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the OffsetFFT class. See the .h file
*/

#include "OffsetFFT.h"

ClassImp(OffsetFFT); /**< Root dictionnary */

OffsetFFT::OffsetFFT() : TObject() {
  fTStartKernel = 0;
  fTEndKernel = 0;
  fTraceSize = 0;
}

OffsetFFT::~OffsetFFT() {
}

vector<Float_t> OffsetFFT::GetOffsetFFT(Int_t aOffset) const {
  vector<Float_t> lOffsetFFT;
  for (UInt_t i=0; i<fBinsOffsetFFT.size(); i++) {
    if (fBinsOffsetFFT[i] == aOffset) lOffsetFFT = fTracesOffsetFFT[i];
  }
  if (lOffsetFFT.size() == 0) cerr<<"No good offset fft found for offset="<<aOffset<< endl;

  return lOffsetFFT;
}

vector<Float_t> OffsetFFT::GetKernel(Int_t aOffset) const {
  for (UInt_t i=0; i<fBinsOffsetFFT.size(); i++) {
    if (fBinsOffsetFFT[i] == aOffset) return fKernels[i]; // ose.. a voir! (la fct est const)
  }
  cerr << "No good kernel found for offset "<<aOffset<<endl;
  vector<Float_t> lV;
  return lV;
}

Float_t OffsetFFT::GetDenom(Int_t aOffset) const {
  Float_t lDenoms = 0;
  for (UInt_t i=0; i<fBinsOffsetFFT.size(); i++) {
    if (fBinsOffsetFFT[i] == aOffset) lDenoms = fDenoms[i];
  }
  if (lDenoms == 0) cerr << "No good denom found for offset "<<aOffset<<endl;
  return lDenoms;
}

void OffsetFFT::ComputeKernels(const FloatVect& aNoise) {

  if (aNoise.size() == 0) {
    cerr << "ComputeKernels: empty noise. Continuing with previous kernel, but this is dangerous.." << endl;
    //    exit(-1);
  }

  if (fKernels.size() != 0 && fKernels.size() != fBinsOffsetFFT.size()) {
    cerr << "ComputeKernel: pbl with kernel size: "<<fKernels.size()<<" exiting"<<endl; exit(-1);
  }
  if (fDenoms.size() != 0 && fDenoms.size() != fBinsOffsetFFT.size()) {
    cerr << "ComputeKernel: pbl with denom size: "<<fDenoms.size()<<" exiting"<<endl; exit(-1);
  }
  if (fKernels.size() == 0) {
    fKernels.resize(fBinsOffsetFFT.size());
  }
  fDenoms.clear();
  for (UInt_t i=0; i<fKernels.size(); i++) {
    fKernels[i] = fTracesOffsetFFT[i] ;
    FloatVect lK(fKernels[i]);
    FloatVect lDenom(fKernels[i]);
    lDenom.Multiply(lDenom);
    lK.Divide(aNoise);
    lDenom.Divide(aNoise);
    fDenoms.push_back(lDenom.Integral());
    lK.Scale(((Float_t)1)/fDenoms[i]);
    fKernels[i] = lK.Vect();

  }
  
}

vector<Float_t> OffsetFFT::GetNonIntegerOffsetFFT(Float_t aOffset) const {
  if ((UInt_t)(TMath::Abs(aOffset)) >= fTraceSize-1 ) {
    cerr << "Too large offset "<<aOffset<<" in GetNonIntegerOffsetFFT. Exiting." << endl;
    exit(-1);
  }
  //  if (fExtrapolatedTrace.size() == 0) this->ExtrapolateTrace(); // size 2N automatically

  // faire l'interpolation!!
  //  for (Int_t i=0; i<N; i++) lOffsetTrace.push_back(lLongTrace[-aOffset+N+i]);

  vector<Float_t> lOffsetTrace;
  for (UInt_t i=0; i<fTraceSize; i++) {
    Float_t pt = (Float_t)i - aOffset;
    Int_t i0 = (Int_t)TMath::Floor(pt);
    if (i0 < 0) lOffsetTrace.push_back(0);
    else {
      Float_t template_interpol = fExtrapolatedTrace[i0];
      template_interpol += (fExtrapolatedTrace[i0+1]-template_interpol)*(pt-i0);
      lOffsetTrace.push_back(template_interpol);
    }
  }
  
  return EdwRealFFT(lOffsetTrace);
}
