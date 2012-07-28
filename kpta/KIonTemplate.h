/*
 *  KIonTemplate.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KIONTEMPLATE_H__
#define __KIONTEMPLATE_H__

#include "KPulseTemplate.h"

class KIonTemplate  : public KPulseTemplate { 

public:
  //Constructors
  KIonTemplate(void);
  virtual ~KIonTemplate(void);
  
  void SetParams(double p0, double p1, double p2, double p3, double p4);
  double GetParams(unsigned int i){return (i < 5) ? fParams[i] : -1;}
  virtual double Get(double time);
  
private:
  double fParams[5];  //there are five parameters in the current ion template function

  //ClassDef(KIonTemplate,1);

};


#endif // __KIONTEMPLATE_H__
