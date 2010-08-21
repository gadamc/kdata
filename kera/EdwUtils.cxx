/**
   @file    EdwUtils.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Small utilities for the EdwRootAna package
*/


#include "EdwUtils.h"
#include "Mixfft.cxx" // Required for EdwRealFFT (library from Jens Joergen Nielsen)

int checkequal(string s) {
  if (s != "=") {
    cout << "Syntax error in parameter file. Syntax: variable = value" << endl; 
    exit(-1);
  }
  return 0;
}

string int2str(Int_t aNum,Int_t aWidth) {
  std::ostringstream lStream; 
  lStream.width(aWidth); 
  lStream.fill('0'); 
  lStream << aNum;
  return lStream.str();
}

string int2str(UInt_t aNum,Int_t aWidth) {
  Int_t lNum = (Int_t)aNum;
  return int2str(lNum, aWidth);
}

vector<Float_t> hist2vect(const TH1F* aHist) {
  vector<Float_t> lVect;
  for (Int_t i=0; i<aHist->GetNbinsX(); i++)
    lVect.push_back(aHist->GetBinContent(i+1)); // Warning ROOT binning convention
  return lVect;
}

TH1F* vect2hist(const vector<Float_t> & aVect, string aVectName) {
  Int_t lNbPts = aVect.size();
  TH1F* lHist = new TH1F(aVectName.c_str(),aVectName.c_str(),lNbPts,-0.5,lNbPts-0.5);
  for (Int_t i=0; i<lNbPts; i++) lHist->SetBinContent(i+1,aVect[i]); // Warning ROOT binning
  return lHist;
}

TH1F* vect2hist(const vector<Short_t> & aVect, string aVectName) {
  Int_t lNbPts = aVect.size();
  TH1F* lHist = new TH1F(aVectName.c_str(),aVectName.c_str(),lNbPts,-0.5,lNbPts-0.5);
  for (Int_t i=0; i<lNbPts; i++) lHist->SetBinContent(i+1,(Float_t)(aVect[i])); // Warning ROOT binning
  return lHist;
}

void plothisto(TH1F* aHist, string aPlotName, Bool_t aLog) {
  TCanvas* c = new TCanvas("c","canvas");
  if (aLog) c->SetLogy();
  aHist->SetStats(0);
  aHist->Draw();
  c->SaveAs(aPlotName.c_str()); // Here there is some memory leak in this root function..
  delete c;
}

void plotvector(const vector<Float_t> & aVect, string aPlotName, Bool_t aLog,
		string aVectName) {
  TH1F* lHist = vect2hist(aVect, aVectName);
  plothisto(lHist,aPlotName,aLog);
  delete lHist;
}

void plotvector(const vector<Short_t> & aVect, string aPlotName, Bool_t aLog,
		string aVectName) {
  TH1F* lHist = vect2hist(aVect, aVectName);
  plothisto(lHist,aPlotName,aLog);
  delete lHist;
}

Bool_t isheat(const string& aChannel) {
  Bool_t lIsHeat = 0;
  if (aChannel.substr(0,7) == "chaleur") lIsHeat = 1 ;
  return lIsHeat ;
}

Int_t stringnum(const vector<string> & aStrings, string myString) {
  Int_t lNum = -1;
  for (UInt_t i=0; i<aStrings.size(); i++) {
    if (aStrings[i] == myString) lNum = i;
  }
  return lNum;
}

Int_t uintnum(const vector<UInt_t> & aUInts, UInt_t myUInt) {
  Int_t lNum = -1;
  for (UInt_t i=0; i<aUInts.size(); i++) {
    if (aUInts[i] == myUInt) lNum = i;
  }
  return lNum;
}

Float_t vectmean(const vector<Float_t> & aVect) {
  UInt_t n = aVect.size();
  if (!n) cerr << "vectmean: vector contains nothing." << endl;
  Float_t sum = 0;
  for (UInt_t i=0; i<n; i++) sum+=(aVect[i]);
  return sum/(Float_t)n;
}

Float_t vectmedian(const vector<Float_t> & aVect) {
  // For this routine we prefer to use directly the implementation by ROOT.
  UInt_t n = aVect.size();
  if (!n) cerr << "vectmedian: vector contains nothing." << endl;
  Float_t* tab = new Float_t[n];
  for (UInt_t i=0; i<n; i++) tab[i]=aVect[i];
  Float_t lMed = (Float_t)TMath::Median(n,tab);
  delete tab;
  return lMed;
}

Float_t vectrms(const vector<Float_t> & aVect) {
  UInt_t n = aVect.size();
  if (!n) cerr << "vectrms: vector contains nothing." << endl;
  Float_t sum = 0, sum2 = 0, lVal = 0;
  for (UInt_t i=0; i<n; i++) {
    lVal = aVect[i];
    sum += lVal; sum2 += (lVal*lVal);
  }
  Float_t n1 = 1/(Float_t)n;
  sum *= n1;
  return TMath::Sqrt(TMath::Abs(sum2*n1 - sum*sum));
}

Float_t vectmin(const vector<Float_t> & aVect) {
  if (!aVect.size()) cerr << "vectmin: vector contains nothing!" << endl;
  Float_t lMin=aVect[0];
  for (UInt_t i=1; i<aVect.size(); i++) {
    if (aVect[i]<lMin) lMin = aVect[i];
  }
  return lMin;
}

Float_t vectmax(const vector<Float_t> & aVect) {
  if (!aVect.size()) cerr << "vectmax: vector contains nothing!" << endl;
  Float_t lMax=aVect[0];
  for (UInt_t i=1; i<aVect.size(); i++) {
    if (aVect[i]>lMax) lMax = aVect[i];
  }
  return lMax;
}

Int_t vectmin(const vector<Int_t> & aVect) {
  if (!aVect.size()) cerr << "vectmin: vector contains nothing!" << endl;
  Int_t lMin=aVect[0];
  for (UInt_t i=1; i<aVect.size(); i++) {
    if (aVect[i]<lMin) lMin = aVect[i];
  }
  return lMin;
}

Int_t vectmax(const vector<Int_t> & aVect) {
  if (!aVect.size()) cerr << "vectmax: vector contains nothing!" << endl;
  Int_t lMax=aVect[0];
  for (UInt_t i=1; i<aVect.size(); i++) {
    if (aVect[i]>lMax) lMax = aVect[i];
  }
  return lMax;
}

Int_t vectnum(const vector<Float_t> & aVect, const Float_t aFloat) {
  
	//WHAT!!! This can't work. You can't compare two floats!!! Maybe Doubles will work... 
	//but there must be a range or something!!!
	
	Int_t i=-1;
  for (UInt_t j=0; j< aVect.size(); j++) {
    if (aFloat == aVect[j]) i=(Int_t)j;
  }
  return i;
}

Int_t vectnum(const vector<string> & aVect, const string aString) {
  Int_t i=-1;
  for (UInt_t j=0; j< aVect.size(); j++) {
    if (aString == aVect[j]) i=(Int_t)j;
  }
  return i;
}


Float_t FloatVect::Integral(Int_t xmin, Int_t xmax) const {
  if (xmin < 0 || xmax < 0 || xmin >= (Int_t)fVect.size() || xmax >= (Int_t)fVect.size()) {
    cerr << "integral: out of bounds! - exiting" << endl; exit(-1);
  }
  Float_t lInt = 0;
  for (Int_t i=xmin; i<=xmax; i++) lInt+=fVect[i];
  return lInt;
}

Float_t FloatVect::Integral() const {
  Float_t lInt = 0;
  for (UInt_t i=0; i<fVect.size(); i++) lInt+=fVect[i];
  return lInt;
}

Float_t FloatVect::Min() const {
  return vectmin(fVect);
}

Float_t FloatVect::Max() const {
  return vectmax(fVect);
}

UInt_t FloatVect::MinBin() const {
  UInt_t lMinBin = 0;
  Float_t lMin = fVect[0];
  for (UInt_t i=0; i<fVect.size(); i++) {
    if (lMin > fVect[i]) {
      lMinBin = i;
      lMin = fVect[i];
    }
  }
  return lMinBin;
}

UInt_t FloatVect::MaxBin() const {
  UInt_t lMaxBin = 0 ;
  Float_t lMax = fVect[0];
  for (UInt_t i=0; i<fVect.size(); i++) {
    if (lMax < fVect[i]) {
      lMaxBin = i;
      lMax = fVect[i];
    }
  }
  return lMaxBin;
}

void FloatVect::Multiply(const FloatVect& aVect) {
  // Attention!!! il faut pas que aVect = this sinon bobo!!
  if (this->size() != aVect.size()) {
    cerr << "Multiply with vectors of different sizes!!. Exit"<< endl; exit(-1);
  }
  for (UInt_t i=0; i<this->size(); i++) (fVect[i])*=(aVect[i]);
}

void FloatVect::Divide(const FloatVect& aVect) {
  if (this->size() != aVect.size()) {
    cerr << "Multiply with vectors of different sizes!!. Exit"<< endl; exit(-1);
  }
  for (UInt_t i=0; i<this->size(); i++)
    fVect[i]/=aVect[i]; // no test on aVect != 0 pour l'instant...

}

void FloatVect::Add(const FloatVect& aVect, const Float_t c) {
  if (this->size() != aVect.size()) {
    cerr << "Multiply with vectors of different sizes!!. Exit"<< endl; exit(-1);
  }
  for (UInt_t i=0; i<this->size(); i++) fVect[i]+=(c*aVect[i]);
}

void FloatVect::Scale(const Float_t& aFactor) {
  for (UInt_t i=0; i<this->size(); i++) fVect[i]*=aFactor;
}

void FloatVect::Plot(string aPlotName, Bool_t aLog, string aVectName) {
  plotvector(fVect, aPlotName, aLog, aVectName) ;
}

FloatVect CopyVect(const vector<Float_t> & aVect) {
  FloatVect V(aVect);
  return V;
}

FloatVect CopyVect(const vector<Short_t> & aVect) {
  FloatVect V(aVect.size());
  for (UInt_t i=0; i<aVect.size(); i++) V[i]=(Float_t)aVect[i];
  return V;
}

FloatVect CopyHisto(const TH1F* aHist) {
  FloatVect V(hist2vect(aHist));
  return V;
}

ByteOrderType testbyteorder() {
  short word = 0x0001;
  char *byte = (char *) &word;
  return(byte[0] ? kLittleEndian : kBigEndian);
}

vector<Float_t> EdwRealFFT(vector<Float_t> aVect) {

  Int_t lSize = aVect.size();
  Double_t *in_re = new Double_t[lSize];
  Double_t *in_im = new Double_t[lSize];
  for (Int_t i=0; i<lSize; i++) {
    in_re[i]=(Double_t)(aVect[i]);
    in_im[i]=0;
  }
  Double_t *out_re = new Double_t[lSize];
  Double_t *out_im = new Double_t[lSize];
  fft(lSize,in_re,in_im,out_re,out_im);
  
  // Output in "Root" semi-complex format
  // r0 ... rN/2 i(N/2-1) ... i1
  vector<Float_t> lOutput;
  for (Int_t i=0; i<= lSize/2; i++) lOutput.push_back((Float_t)(out_re[i]));
  for (Int_t i=((lSize/2)-1); i>0; i--) lOutput.push_back((Float_t)(out_im[i]));
  delete[] in_re;
  delete[] in_im;
  delete[] out_re;
  delete[] out_im;

  return lOutput;
}

vector<Float_t> EdwFFTPower(vector<Float_t> aFFT) {
  // From the FFT vector (r0 .. i1) returns the corresponding power vector,
  // eg. Ak=rk^2+ik^2 for k=0..N/2 (with i0=iN/2=0)

  UInt_t N = aFFT.size();
  vector<Float_t> power(1+(N/2),0);
  power[0]=pow(aFFT[0],2);
  power[N/2]=pow(aFFT[N/2],2);
  for (UInt_t k=1; k<N/2; k++) power[k] = pow(aFFT[k],2) + pow(aFFT[N-k],2);

  return power;
}

vector<Float_t> EdwPowerHalf(vector<Float_t> aPower) {
  // calcule le power de taille N/2 pour un "power" de taille N
  // (pour le spectre de bruit)
  UInt_t N = aPower.size();
  vector<Float_t> powerhalf(1+(N/2),0);
  powerhalf[0]=aPower[0];
  powerhalf[N/2]=aPower[N/2];
  for (UInt_t k=1; k<N/2; k++) powerhalf[k] = aPower[k] + aPower[N-k];

  return powerhalf;
}

vector<Float_t> EdwInvFFT(vector<Float_t> aFFT) {
  // uses the formula: iFFT(x) = 1/N * conj(FFT(conj(x)))

  // And play with the semi-complex representation of fourier transform:
  // Here of course aFFT is a real fft, format r0 ... rN/2 i(N/2-1) ... i1
  Int_t lSize = aFFT.size();
  if (lSize == 0) {
    cerr << "EdwInvFFT: input vector has zero size. Exiting" << endl;
    exit(-1);
  }
  Double_t *in_re = new Double_t[lSize];
  Double_t *in_im = new Double_t[lSize];
  in_re[0]=(Double_t)(aFFT[0]);
  for (Int_t i=1; i<=lSize/2; i++) {
    in_re[i]=(Double_t)(aFFT[i]);
    in_re[lSize-i]=in_re[i];
  }
  in_im[0]=0;
  in_im[lSize-1]=0;
    for (Int_t i=1; i<lSize/2; i++) {
    in_im[i]=-(Double_t)(aFFT[lSize-i]);
    in_im[lSize-i]=-in_im[i];
  }
  Double_t *out_re = new Double_t[lSize];
  Double_t *out_im = new Double_t[lSize];
  fft(lSize,in_re,in_im,out_re,out_im);

  // Output = only the real part. The imaginary one should be null!
  vector<Float_t> lOutput;
  for (Int_t i=0; i<lSize; i++) lOutput.push_back(out_re[i]/lSize);
  delete[] in_re;
  delete[] in_im;
  delete[] out_re;
  delete[] out_im;

  return lOutput;
}

FloatVect EdwRealFFT(FloatVect aVect) {
  FloatVect lVect;
  lVect.Set(EdwRealFFT(aVect.Vect()));
  return lVect;
}

FloatVect EdwInvFFT(FloatVect aVect) {
  FloatVect lVect;
  lVect.Set(EdwInvFFT(aVect.Vect()));
  return lVect;
}

vector<Float_t> EdwFilter(vector<Float_t> aData, vector<Float_t> aDirect, vector<Float_t> aInverse) {
  Int_t n = aData.size();
  Int_t j = aDirect.size()-1;
  Int_t k = aInverse.size();
  Int_t m = k; if (j > k) m = j;
  vector<Float_t> y(n,0);
  // First steps:
  for (Int_t i = 0; i<m; i++) {
    for (Int_t p=1; p<=k; p++) { if (i >= p) y[i]+=(aInverse[p-1]*y[i-p]);}
    for (Int_t p=0; p<=j; p++) { if (i >= p) y[i]+=(aDirect[p]*aData[i-p]);}
  }
  // Other steps: no need to have the "ifs"
  for (Int_t i = m; i<n; i++) {
    for (Int_t p=1; p<=k; p++) y[i]+=(aInverse[p-1]*y[i-p]);
    for (Int_t p=0; p<=j; p++) y[i]+=(aDirect[p]*aData[i-p]);
  }

  return y;
}
