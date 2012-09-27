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
  
  //this weird pattern of coding is so that Real/Imag can be overloaded in derived classes.
  //and because using template functions in ROOT's CINT isn't trivial.
  //If you know a better way, please simplify this horrible looking code!!!
  virtual double Real(std::vector<double> &aPulse, unsigned int pos){return GetRealValue(aPulse, pos);}
  virtual double Real(std::vector<float> &aPulse, unsigned int pos){return GetRealValue(aPulse, pos);}
  virtual double Real(std::vector<int> &aPulse, unsigned int pos){return GetRealValue(aPulse, pos);}
  virtual double Real(std::vector<short> &aPulse, unsigned int pos){return GetRealValue(aPulse, pos);}
  
  virtual double Real(const double* aPulse, unsigned int size, unsigned int pos){return GetRealValue(aPulse, size, pos);}
  virtual double Real(const float* aPulse, unsigned int size, unsigned int pos){return GetRealValue(aPulse, size, pos);}
  virtual double Real(const int* aPulse, unsigned int size, unsigned int pos){return GetRealValue(aPulse, size, pos);}
  virtual double Real(const short* aPulse, unsigned int size, unsigned int pos){return GetRealValue(aPulse, size, pos);}
  
  virtual double Imag(std::vector<double> &aPulse, unsigned int pos){return GetImagValue(aPulse, pos);}
  virtual double Imag(std::vector<float> &aPulse, unsigned int pos){return GetImagValue(aPulse, pos);}
  virtual double Imag(std::vector<int> &aPulse, unsigned int pos){return GetImagValue(aPulse, pos);}
  virtual double Imag(std::vector<short> &aPulse, unsigned int pos){return GetImagValue(aPulse, pos);}
  
  virtual double Imag(const double* aPulse, unsigned int size, unsigned int pos){return GetImagValue(aPulse, size, pos);}
  virtual double Imag(const float* aPulse, unsigned int size, unsigned int pos){return GetImagValue(aPulse, size, pos);}
  virtual double Imag(const int* aPulse, unsigned int size, unsigned int pos){return GetImagValue(aPulse, size, pos);}
  virtual double Imag(const short* aPulse, unsigned int size, unsigned int pos){return GetImagValue(aPulse, size, pos);}
  
  
protected:
  template <class T> double GetRealValue(const std::vector<T> &aPulse, unsigned int pos);
  template <class T> double GetRealValue(const T* aPulse, unsigned int size, unsigned int pos);  //you are responsible for making sure that pos < size of aPulse
  template <class T> double GetImagValue(const std::vector<T> &aPulse, unsigned int pos);
  template <class T> double GetImagValue(const T* aPulse, unsigned int size, unsigned int pos); //you are responsible for making sure that pos < size of aPulse
  
private:
  
  void InitializeMembers(void);
  //ClassDef(KHalfComplexArray,1);
  
};

template <class T> double KHalfComplexArray::GetRealValue(const T* aPulse, unsigned int size, unsigned int pos)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::GetRealValue. Position is greater than size");
  
  return aPulse[pos];
}


template <class T> double KHalfComplexArray::GetRealValue(const std::vector<T> &aPulse, unsigned int pos)
{
  if(pos > aPulse.size()/2)
    throw std::out_of_range("KHalfComplexArray::GetRealValue. Position is greater than size/2");
    
  return aPulse.at(pos);
}

template <class T> double KHalfComplexArray::GetImagValue(const T* aPulse, unsigned int size, unsigned int pos)
{
  if(pos > size/2)
    throw std::out_of_range("KHalfComplexArray::GetRealValue. Position is greater than size/2");
  
  if(pos == 0 || pos == size / 2) return 0;
  else return aPulse[size - pos];
}


template <class T> double KHalfComplexArray::GetImagValue(const std::vector<T> &aPulse, unsigned int pos)
{
  if(pos > aPulse.size()/2)
    throw std::out_of_range("KHalfComplexArray::GetRealValue. Position is greater than size/2");
    
  if(pos == 0 || pos == aPulse.size() / 2) return 0;
   else return aPulse[aPulse.size() - pos];
}


#endif //end KHALFCOMPLEXARRAY