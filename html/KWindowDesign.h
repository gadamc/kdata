//______________________________________________________________________
//
// KWindowDesign.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#ifndef __KWINDOWDESIGN_H__
#define __KWINDOWDESIGN_H__

//#include "Rtypes.h"

class KWindowDesign {

public:
  KWindowDesign(void);
  virtual ~KWindowDesign(void);

  static double* GetTukeyWindow(unsigned int size, double alpha = 0.5, int middle = -9999, unsigned int width = 0); 
  static double* GetBlackmanWindow(unsigned int size,double alpha = 0.16, int middle = -9999, unsigned int width = 0);

private:
  //ClassDef(KWindowDesign,1);

};


#endif // __KWINDOWDESIGN_H__

