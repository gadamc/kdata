//_____________________________________________
//
// KPulsePolarityCalculator.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@kit.edu> on 11/19/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KPULSEPOLARITYCALCULATOR_H__
#define __KPULSEPOLARITYCALCULATOR_H__

class KRawBoloPulseRecord;
class KRawBolometerRecord;

class KPulsePolarityCalculator  { 

public:
  //Constructors
  KPulsePolarityCalculator(void);
  virtual ~KPulsePolarityCalculator(void);

  static int GetExpectedPolarity(KRawBoloPulseRecord *p, KRawBolometerRecord* bolo = 0);
	
	//setters
private:

	//private methods
  static int GetExpectedPolarityFromFID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  static int GetExpectedPolarityFromID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  static int GetPolarityStandardCalculator(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  
  
};


#endif // __KPULSEPOLARITYCALCULATOR_H__
