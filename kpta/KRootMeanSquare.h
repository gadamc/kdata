//______________________________________________________________________
//
// KRootMeanSquare.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KROOTMEANSQUARE_H__
#define __KROOTMEANSQUARE_H__
#include <cmath>
#include <vector>
//#include "Rtypes.h"

class KRootMeanSquare  { 

public:
  //Constructors
  KRootMeanSquare(void);
  virtual ~KRootMeanSquare(void);

  template <class T> double GetStdDev(std::vector<T> &r, unsigned int start = 0,  int stop = -1);
  template <class T> double GetStdDev(const T* r, unsigned int start = 0,  unsigned int stop = 0);

  template <class T> double GetRms(std::vector<T> &r, unsigned int start = 0,  int stop = -1);
  template <class T> double GetRms(const T* r, unsigned int start = 0,  unsigned int stop = 0);
  
private:

  void InitializeMembers(void);

  
  //ClassDef(KRootMeanSquare,1);
  
};

template <class T> double KRootMeanSquare::GetStdDev(std::vector<T> &r, unsigned int start, int stop )
{
  if (stop < 0) stop = r.size();  
  return GetStdDev( &r[0], start, stop);
}

template <class T> double KRootMeanSquare::GetStdDev(const T* r, unsigned int start, unsigned int stop)
{
  if (start >= stop) return 0.0;
  
  double val2 = 0, val = 0;
  unsigned int i = start;
  for(; i < stop; i++){
    val2 += *(r+i) * *(r+i);
    val += *(r+i);
  }
  val = val/double(i-start);

  return sqrt( fabs(val*val - val2/(double)(i-start)) );

}

template <class T> double KRootMeanSquare::GetRms(std::vector<T> &r, unsigned int start, int stop )
{
  if (stop < 0) stop = r.size();
  return GetRms( &r[0], start, stop);
}

template <class T> double KRootMeanSquare::GetRms(const T* r, unsigned int start, unsigned int stop)
{
  if (start >= stop) return 0.0;
  
  double val2 = 0;
  unsigned int i = start;
  for(; i < stop; i++){
    val2 += *(r+i) * *(r+i);
  }

  return sqrt( val2/(double)(i-start));

}

#endif // __KROOTMEANSQUARE_H__
