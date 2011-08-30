/*
 *  KHeatTemplate.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHEATTEMPLATE_H__
#define __KHEATTEMPLATE_H__

#include "KPulseTemplate.h"

class KHeatTemplate : public KPulseTemplate { 

public:
  //Constructors
  KHeatTemplate(void);
  virtual ~KHeatTemplate(void);
  
  void SetParams(double p0, double p1, double p2, double p3, double p4,
                  double p5, double p6, double p7);
  double GetParams(unsigned int i){return (i < 8) ? fParams[i] : -1;}
  virtual double Get(double time);
  
private:
  double fParams[8];  //there are five parameters in the current ion template function


};


#endif // __KHEATTEMPLATE_H__
