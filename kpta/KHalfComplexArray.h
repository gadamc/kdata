/*
*  KHalfComplexArray.h
*  kData
*
*  Created by Adam Cox on 3/19/2012.
*  Copyright 2012 Karlsruhe Institute of Technology. All rights reserved.
*
*/

#ifndef __KHALFCOMPLEXARRAY_H__
#define __KHALFCOMPLEXARRAY_H__

#include <stdexcept> 
#include <iostream>
#include <vector>
#include <string.h>
//#include "Rtypes.h"

class KHalfComplexArray  {
  
public:
  KHalfComplexArray(void);
  virtual ~KHalfComplexArray(void);
  
  template <class T> double Real(const std::vector<T> &aPulse, unsigned int pos);
  template <class T> double Real(const T* aPulse, unsigned int size, unsigned int pos);  //you are responsible for making sure that pos < size of aPulse
  template <class T> double Imag(const std::vector<T> &aPulse, unsigned int pos);
  template <class T> double Imag(const T* aPulse, unsigned int size, unsigned int pos); //you are responsible for making sure that pos < size of aPulse
  
  template <class T> void SetReal(std::vector<T> &aPulse, unsigned int pos, T value);
  template <class T> void SetReal(T* aPulse, unsigned int size, unsigned int pos, T value);  //you are responsible for making sure that pos < size of aPulse
  template <class T> void SetImag(std::vector<T> &aPulse, unsigned int pos, T value);
  template <class T> void SetImag(T* aPulse, unsigned int size, unsigned int pos, T value); //you are responsible for making sure that pos < size of aPulse

  template <class T> void Multiply(std::vector<T> &result, const std::vector<T> &left, std::vector<T> &right);
  template <class T> void Multiply(T *result, const T* left, const T* right, unsigned int size);

  // template <class T> void Divide(std::vector<T> &result, const std::vector<T> &left, std::vector<T> &right);
  // template <class T> void Divide(T *result, const T* left, const T* right, unsigned int size);

  // template <class T> void Conjugate(std::vector<T> &result);
  // template <class T> void Conjugate(T *result, unsigned int size);


  private:
  
  void InitializeMembers(void);
  //ClassDef(KHalfComplexArray,1);
  
};

template <class T> double KHalfComplexArray::Real(const T* aPulse, unsigned int size, unsigned int pos)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::Real. Position is greater than size");
  
  return aPulse[pos];
}


template <class T> double KHalfComplexArray::Real(const std::vector<T> &aPulse, unsigned int pos)
{
  return Real<T>(&aPulse[0], aPulse.size(), pos);
}

template <class T> double KHalfComplexArray::Imag(const T* aPulse, unsigned int size, unsigned int pos)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::Imag. Position is greater than size/2");
  
  if(pos == 0 || pos == size / 2) return 0;
  else return aPulse[size - pos];
}


template <class T> double KHalfComplexArray::Imag(const std::vector<T> &aPulse, unsigned int pos)
{
  return Imag<T>(&aPulse[0], aPulse.size(), pos);
}

template <class T> void KHalfComplexArray::SetReal(T* aPulse, unsigned int size, unsigned int pos, T value)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::SetReal. Position is greater than size");
  
  aPulse[pos] = value;
}


template <class T> void KHalfComplexArray::SetReal(std::vector<T> &aPulse, unsigned int pos, T value)
{
  SetReal<T>(&aPulse[0], aPulse.size(), pos, value);
}

template <class T> void KHalfComplexArray::SetImag(T* aPulse, unsigned int size, unsigned int pos, T value)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::SetImag. Position is greater than size/2");
  
  if(pos == 0 || pos == size / 2) return;  //just do nothing... print a warning? no...
  else aPulse[size - pos]  = value;
}


template <class T> void KHalfComplexArray::SetImag(std::vector<T> &aPulse, unsigned int pos, T value)
{
  SetImag<T>(&aPulse[0], aPulse.size(), pos, value);
}

template <class T> void KHalfComplexArray::Multiply(std::vector<T> &result, const std::vector<T> &left, std::vector<T> &right)
{
  Multiply<T>(&result[0], &left[0], &right[0], result.size());
}

template <class T> void KHalfComplexArray::Multiply(T *result, const T* left, const T* right, unsigned int size)
{
  for(unsigned int i = 0; i < size/2 + 1; i++) {
    SetReal<T>(result, size, i, Real(left, size, i) * Real(right, size, i) - Imag(left, size, i) * Imag(right, size, i) );  
    SetImag<T>(result, size, i, Real(left, size, i) * Imag(right, size, i) + Imag(left, size, i) * Real(right, size, i) ); 
  }
}


// template <class T> void KHalfComplexArray::Divide(std::vector<T> &result, const std::vector<T> &left, std::vector<T> &right)
// {
//   Divide<T>(&result[0], &left[0], &right[0], result.size());
// }

// template <class T> void KHalfComplexArray::Divide(T *result, const T* left, const T* right, unsigned int size)
// {
//   std::vector<T> denomConjugate(size);
//   std::copy(right, right+size, denomConjugate.begin());
  
//   Conjugate<T>(denomConjugate, size);

//   for(unsigned int i = 0; i < size/2 + 1; i++) {
//     SetReal<T>(result, size, i, Real(left, size, i) * Real(right, size, i) - Imag(left, size, i) * Imag(right, size, i) );  
//     SetImag<T>(result, size, i, Real(left, size, i) * Imag(right, size, i) + Imag(left, size, i) * Real(right, size, i) ); 
//   }
// }


#endif //end KHALFCOMPLEXARRAY
