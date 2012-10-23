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
class KAmpBoloPulseRecord;
class KAmpBolometerRecord;

class KPulsePolarityCalculator  { 

public:
  //Constructors
  KPulsePolarityCalculator(void);
  virtual ~KPulsePolarityCalculator(void);

  //this is written separately for the amp and raw level records because the base class
  //for those records need to be rewritten. 
  
  static int GetExpectedPolarity(KRawBoloPulseRecord *p, KRawBolometerRecord* bolo = 0);
  static int GetExpectedPolarity(KAmpBoloPulseRecord *p, KAmpBolometerRecord* bolo = 0);
  //setters
private:

  //private methods
  static int GetExpectedPolarityFromFID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  static int GetExpectedPolarityFromID(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  static int GetPolarityStandardCalculator(KRawBolometerRecord* bolo, KRawBoloPulseRecord *p);
  
  static int GetExpectedPolarityFromFID(KAmpBolometerRecord* bolo, KAmpBoloPulseRecord *p);
  static int GetExpectedPolarityFromID(KAmpBolometerRecord* bolo, KAmpBoloPulseRecord *p);
  static int GetPolarityStandardCalculator(KAmpBolometerRecord* bolo, KAmpBoloPulseRecord *p);
  
};


#endif // __KPULSEPOLARITYCALCULATOR_H__
