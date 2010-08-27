/**
   @file    EdwUtils.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Standard includes and small utilities for the EdwRootAna package
*/

#ifndef _EDWUTILS_
#define _EDWUTILS_

// Standard library includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include "Byteswap.h"
using namespace std;

// Root includes
#include "TObject.h"
#include "TObjArray.h"
#include "TTimeStamp.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TFile.h"
#include "TArrayS.h"
#include "TBranch.h"
#include "TGraph.h"
#include "TProfile.h"
#include "TNtuple.h"

#define KVERBOSE 1 /**< may be used in routines to do some printout. */

#define ArraySize(x) (sizeof(x)/sizeof((x)[0])) /**< Size of array[] */

int checkequal(string s); /**< checks if s is "=" (for parameter file reading) */

string int2str(Int_t aNum,Int_t aWidth = 3) ; /**< Convert an integer to a string with a given length */
string int2str(UInt_t aNum,Int_t aWidth = 3) ; /**< Convert an integer to string with a given length */

vector<Float_t> hist2vect(const TH1F* aHist); /**< Conversion TH1F -> STL vector */
TH1D* vect2hist(const vector<double> & aVect, string aVectName = "Vector"); /**< Conversion STL vector -> TH1D. Warning there is a 'new' operator in this function. */
TH1F* vect2hist(const vector<Float_t> & aVect, string aVectName = "Vector"); /**< Conversion STL vector -> TH1F. Warning there is a 'new' operator in this function. */
TH1F* vect2hist(const vector<Short_t> & aVect, string aVectName = "Vector"); /**< Conversion STL vector -> TH1F. Warning there is a 'new' operator in this function. */

void plothisto(TH1F* aHist, string aPlotName, Bool_t aLog = 0) ; /**< Plot a TH1F */
void plotvector(const vector<Float_t> & aVect, string aPlotName, Bool_t aLog = 0, 
		string aVectName = "Vector") ; /**< Plot an STL vector */
void plotvector(const vector<Short_t> & aVect, string aPlotName, Bool_t aLog = 0, 
		string aVectName = "Vector") ; /**< Plot an STL vector */

Bool_t isheat(const string& aChannel); /**< Returns 1 if the string is of the form "chaleur..." */
Int_t stringnum(const vector<string> & aStrings, string myString); /**< Returns the index of a given string in an array of strings. If this string does not exist in the array, return -1 */
Int_t uintnum(const vector<UInt_t> & aUInts, UInt_t myUInt); /**< Returns the index of a given uint in an array of uints. If this string does not exist in the array, return -1 */
// TODO : faire un truc plus generic avec template!

Float_t vectmean(const vector<Float_t> & aVect); /**< Average */
Float_t vectmedian(const vector<Float_t> & aVect); /**< Median */
Float_t vectrms(const vector<Float_t> & aVect); /**< Standard deviation */
Float_t vectmin(const vector<Float_t> & aVect); /**< Minimum */
Float_t vectmax(const vector<Float_t> & aVect); /**< Maximum */
Int_t vectmin(const vector<Int_t> & aVect); /**< Minimum */
Int_t vectmax(const vector<Int_t> & aVect); /**< Maximum */

Int_t vectnum(const vector<Float_t> & aVect, const Float_t aFloat) ; /**< Index .. */
Int_t vectnum(const vector<string> & aVect, const string aString) ; /**< Index.. */
// A nettoyer avec celles du haut!

/**
   @enum    ByteOrderType
   @brief   Flag for little or big endian data format for raw Edw data

*/
enum ByteOrderType { kLittleEndian=0, kBigEndian} ;
ByteOrderType testbyteorder(); /**< Byte-ordering of the local machine */

/**
    @class  Matrix
    @brief  Simple matrix template

    Used to define objects such as Matrix<Float_t>. No algebric operator.

*/
template <typename T>
class Matrix {
 public:
  Matrix() { this->clear(); }
  ~Matrix() { this->clear(); }
  Matrix(Int_t rows, Int_t cols) {
    vector<T> v0(cols,0); // Explicit declaration seems to be important
    dArray.resize(rows,v0);
  }
  /**< Standard constructor */
  vector<T> & operator[] (Int_t i) { return dArray[i]; } /**< Can write matrix[i][j] */
  const vector<T> & operator[] (Int_t i) const { return dArray[i]; } /**< Can write matrix[i][j] */
  void resize(Int_t rows, Int_t cols) {
    vector<T> v0(cols,0);
    dArray.resize(rows,v0);
  } /**< resize */
  void push_back(vector<T> aVect) { dArray.push_back(aVect); } /**< pushback */
  UInt_t nrows() const { return dArray.size(); } /**< rows */
  void clear() { this->resize(0,0); } /**< clear */
  vector< vector<T> > Mat() {return dArray;} /**< matrix --> STL object */

 private:
  vector< vector<T> > dArray; /**< encapsulated STL object */
};

// Float vectors (better than TH1F* !!)
/**
   @class  FloatVect
   @brief  vector of floats, with algebric operations

   I prefer to use this rather than the complicated TH1F. 
   It is a vector<Float_t> with basic algebric operations, and the possibility to plot the vector.

*/
class FloatVect {
 public:
  FloatVect() {}
  FloatVect(const vector<Float_t> & aVect) { fVect = aVect; } /**< Constructor */
  ~FloatVect() {}
  void Set(const vector<Float_t> & aVect) { fVect = aVect; } /**< Copy */
  void clear() { fVect.clear(); } /**< Clear */
  FloatVect(Int_t nb, const Float_t& val=0) { fVect.resize(nb,val); } /**< Initialize */
  Float_t & operator[] (Int_t i) { return fVect[i]; } /**< Can use vect[j] notation */
  const Float_t & operator[] (Int_t i) const { return fVect[i]; } /**< Can use vect[j] notation */
  void resize(Int_t nb, const Float_t& val=0) { fVect.resize(nb,val); } /**< Resize */
  void push_back(Float_t val) { fVect.push_back(val); } /**< pushback */
  vector<Float_t> Vect() const { return fVect; } /**< FloatVect--> STL */

  Float_t Integral(Int_t xmin, Int_t xmax) const ; /**< Integral in a given range */
  Float_t Integral() const ; /**< Integral of the whole vector */
  Float_t Min() const ; /**< Minimum value */
  Float_t Max() const ; /**< Maximum value */
  UInt_t MinBin() const ; /**< Bin of the minimum value */
  UInt_t MaxBin() const ; /**< Bin of the maximum value */
  void Multiply(const FloatVect& aVect) ; /**< Scalar product of two vectors */
  void Divide(const FloatVect& aVect) ; /**< Division */
  void Add(const FloatVect& aVect, const Float_t c = 1) ; /**< this += c*aVect */
  void Scale(const Float_t& aFactor) ; /**< this *= factor */
  void Scale(const Int_t& aFactor) { this->Scale((Float_t)aFactor); } /**< this *= factor */
  UInt_t size() const { return fVect.size(); } /**< Vector size */
  void Plot(string aPlotName, Bool_t aLog = 0, string aVectName = "Vector") ; /**< Root plot */

 private:
  vector<Float_t> fVect; /**< encapsulated STL vector */
};

FloatVect CopyVect(const vector<Float_t> & aVect) ; /**< Convert STL vector -> FloatVect object */
FloatVect CopyVect(const vector<Short_t> & aVect) ; /**< Convert STL vector -> FloatVect object */
FloatVect CopyHisto(const TH1F* aHist) ; /**< Convert TH1F -> FloatVect object */

vector<Float_t> EdwRealFFT(vector<Float_t> aVect) ; /**< FFT driver */
vector<Float_t> EdwFFTPower(vector<Float_t> aFFT) ; /**< Power of a given fft */
vector<Float_t> EdwPowerHalf(vector<Float_t> aPower) ; /**< Convert power N -> N/2 points */
FloatVect EdwRealFFT(FloatVect aVect); /**< FFT */
vector<Float_t> EdwInvFFT(vector<Float_t> aFFT); /**< Inverse FFT */
FloatVect EdwInvFFT(FloatVect aVect); /**< Inverse FFT */

vector<Float_t> EdwFilter(vector<Float_t> aData, vector<Float_t> aDirect, vector<Float_t> aInverse);
/**< Filtering:
   With aData=x; aDirect=(a_p)p=0...j ; aInverse=(b_p)p=0...k-1:
   y_n = sum_1^k b_(p-1) y_(n-p) + sum_0^j a_p x_(n-p)   for n>=sup(j,k)
*/

#endif
