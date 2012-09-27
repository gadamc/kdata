/*
 *  KPulseTemplate.h
 *  KDataStructure
 *
 *  Created by Michael Unrau on 8/30/11.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KPULSETEMPLATE_H__
#define __KPULSETEMPLATE_H__

//#include "Rtypes.h"


class KPulseTemplate  { 

public:
  //Constructors
  KPulseTemplate(void);
  virtual ~KPulseTemplate(void);
  
  virtual double Get(double time) = 0;
private:
  //ClassDef(KPulseTemplate,1);

};


#endif // __KPULSETEMPLATE_H__
