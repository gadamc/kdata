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

void OffsetFFT::ComputeKernels(FloatVect aNoise) {

  if (aNoise.size() == 0) {
    //    cerr << "ComputeKernels: empty noise. Continuing with previous kernel, but this is dangerous.." << endl;
    //    exit(-1);
    // Changement (rustine) pour eviter pbl...
    cerr << "ComputeKernels: empty noise. Using a flat noise spectrum as a rustine..."<<endl;
    aNoise.resize((fKernels.at(0)).size(),1);
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
    Float_t dede = lDenom.Integral();
    if (dede == 0) dede=1; // cas particulier trace tmplt offset = 00 ==> mettre 1 et ca doit passer
    fDenoms.push_back(dede);
    lK.Scale(((Float_t)1)/fDenoms[i]);
    fKernels[i] = lK.Vect();
  }
}

vector<Float_t> OffsetFFT::GetNonIntegerOffsetFFT(Float_t aOffset) const {
  if ((UInt_t)(TMath::Abs(aOffset)) >= fTraceSize-1 ) {
    cerr << "Too large offset "<<aOffset<<" in GetNonIntegerOffsetFFT. Exiting." << endl;
    exit(-1);
  }

  // The old way: it generated a bias (some kind of aliasing...)
  // pbl fextrapolatedtrace avec le windowing qu'il faut aussi...
  /*  vector<Float_t> lTTrace = EdwInvFFT(this->GetOffsetFFT(0));
  vector<Float_t> lOffsetTrace;
  for (UInt_t i=0; i<fTraceSize; i++) {
    Float_t pt = (Float_t)i - aOffset;
    Int_t i0 = (Int_t)TMath::Floor(pt);
    if (i0 < 0) lOffsetTrace.push_back(0);
    else {
      if (i0+1 >= fTraceSize) {
	Float_t template_interpol = fExtrapolatedTrace[i0];
	template_interpol += (fExtrapolatedTrace[i0+1]-template_interpol)*(pt-i0);
	lOffsetTrace.push_back(template_interpol);
      } else {
	Float_t template_interpol = lTTrace[i0];
	template_interpol += (lTTrace[i0+1]-template_interpol)*(pt-i0);
	lOffsetTrace.push_back(template_interpol);
      }
    }
  }
  return EdwRealFFT(lOffsetTrace);
  */

  // Analytic way : but generates some numerical effect 
  // (oscillations in the offseted template after FFT)
  /*
     Int_t lIntOffset = (Int_t)TMath::Floor(aOffset);
  Float_t lNonIntOffset = aOffset - (Float_t)lIntOffset;
  if (lNonIntOffset > 0.5) { // recollement... mouais!
    lIntOffset +=1;
    lNonIntOffset -=1;
  }
  vector<Float_t> lIntOffsetFFT = this->GetOffsetFFT(lIntOffset); 
  vector<Float_t> lNonIntOffsetFFT = lIntOffsetFFT;

  if (lNonIntOffset != 0 ) {
    UInt_t n = lIntOffsetFFT.size();
    for (UInt_t i=0; i<= (UInt_t)(n/2); i++) {
      Float_t lAngle = -2*TMath::Pi()*i*lNonIntOffset/(Float_t)n ; // signe 'moins' empirique! 
      Float_t cosang = cos(lAngle);
      Float_t sinang = sin(lAngle);
      lNonIntOffsetFFT[i] = lIntOffsetFFT[i]*cosang - lIntOffsetFFT[n-i]*sinang;
      if (i > 0 && 2*i != n)
      	lNonIntOffsetFFT[n-i] = lIntOffsetFFT[n-i]*cosang + lIntOffsetFFT[i]*sinang;
    }
  }
  */

  // Methode retenue : interpolation lineaire des offsetfft entiers
  Int_t lIntOffset = (Int_t)TMath::Floor(aOffset);
  Float_t alpha = aOffset - (Float_t)lIntOffset;
  // Calcul des offset min et max tabules.
  Int_t lbinmin = 0, lbinmax=0; // choses qu'on calcule ici mais devraient etre faites
  // 1 seule fois... (on veut juste pas changer la classe maintenant)
  for (UInt_t i=0; i<fBinsOffsetFFT.size(); i++) {
    if (fBinsOffsetFFT[i] < lbinmin) lbinmin=fBinsOffsetFFT[i];
    if (fBinsOffsetFFT[i] > lbinmax) lbinmax=fBinsOffsetFFT[i];
  }

  vector<Float_t> lNonIntOffsetFFT;
  // Securite:
  if (lIntOffset <= lbinmin) lNonIntOffsetFFT = this->GetOffsetFFT(lbinmin);
  else if (lIntOffset > lbinmax-1) lNonIntOffsetFFT = this->GetOffsetFFT(lbinmax);
  else {
    vector<Float_t> lIntOffset_inf = this->GetOffsetFFT(lIntOffset); 
    vector<Float_t> lIntOffset_sup = this->GetOffsetFFT(lIntOffset+1); 
    for (UInt_t i=0; i<fTraceSize; i++)
      lNonIntOffsetFFT.push_back((1-alpha)*lIntOffset_inf[i]+alpha*lIntOffset_sup[i]);
  }

  return lNonIntOffsetFFT;
}
