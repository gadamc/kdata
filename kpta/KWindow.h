//______________________________________________________________________
//
// KWindow.h
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KWINDOW_H__
#define __KWINDOW_H__

#include "KPtaProcessor.h"
#include <string.h>

class KWindow : public KPtaProcessor { 

public:
  //Constructors
  KWindow(void);
  //for the memory-savy programmers
  KWindow(double *inPulse, unsigned int inSize, double* outPulse, unsigned int outsize);

  virtual ~KWindow(void);

  virtual bool RunProcess(void);

	virtual void SetWindow(const double *window, unsigned int windowSize);
  virtual double* GetWindow(void) {return fWindow;}
  virtual unsigned int GetWindowSize(void) {return fWindowSize;}

protected:
  double *fWindow;
  unsigned int fWindowSize;

private:
  //private methods
  void InitializeMembers(void);

 // ClassDef(KWindow,1);

};


#endif
