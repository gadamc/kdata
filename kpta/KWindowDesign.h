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


class KWindowDesign {
  
public:
	KWindowDesign(void);
	virtual ~KWindowDesign(void);
	
  static void GetTukeyWindow(double alpha, double* coef, unsigned int size); // Sets "coef" to coefficients of a Tukey window with the parameter "alpha" for "size" Bins
  
private:
	
};


#endif // __KWINDOWDESIGN_H__

