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
  if(pos > aPulse.size()/2)
    throw std::out_of_range("KHalfComplexArray::Real. Position is greater than size/2");
    
  return aPulse.at(pos);
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
  if(pos > aPulse.size()/2)
    throw std::out_of_range("KHalfComplexArray::Imag. Position is greater than size/2");
    
  if(pos == 0 || pos == aPulse.size() / 2) return 0;
   else return aPulse[aPulse.size() - pos];
}


#endif //end KHALFCOMPLEXARRAY
