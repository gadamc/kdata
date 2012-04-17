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

class KRootMeanSquare  { 

public:
  //Constructors
  KRootMeanSquare(void);
  virtual ~KRootMeanSquare(void);

  virtual double GetRms(std::vector<double> &r,  unsigned int start = 0,  int stop = -1){return Rms(r, start, stop);}
  virtual double GetRms(std::vector<float> &r,  unsigned int start = 0,  int stop = -1){return Rms(r, start, stop);}
  virtual double GetRms(std::vector<int> &r, unsigned int start = 0,  int stop = -1){return Rms(r, start, stop);}
  virtual double GetRms(std::vector<short> &r, unsigned int start = 0,  int stop = -1){return Rms(r, start, stop);}

  virtual double GetRms(const double* r, unsigned int start,  unsigned int stop){return Rms(r, start, stop);}
  virtual double GetRms(const float* r, unsigned int start,  unsigned int stop){return Rms(r, start, stop);}
  virtual double GetRms(const int* r, unsigned int start,  unsigned int stop){return Rms(r, start, stop);}
  virtual double GetRms(const short* r, unsigned int start,  unsigned int stop){return Rms(r, start, stop);}
  
private:

  void InitializeMembers(void);
  template <class T> double Rms(std::vector<T> &r, unsigned int start = 0,  int stop = -1);
  template <class T> double Rms(const T* r, unsigned int start = 0,  unsigned int stop = 0);
  
  
};

template <class T> double KRootMeanSquare::Rms(std::vector<T> &r, unsigned int start, int stop )
{
  if (stop < 0) stop = r.size();
  double val2 = 0, val = 0;
  int i = start;
  for(; i < stop; i++){
    val2 += r[i]*r[i];
    val += r[i];
  }
  val = val/double(i-start);

  return sqrt( fabs(val*val - val2/(double)(i-start)) );

}

template <class T> double KRootMeanSquare::Rms(const T* r, unsigned int start, unsigned int stop)
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

#endif // __KROOTMEANSQUARE_H__
