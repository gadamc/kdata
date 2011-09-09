//______________________________________________________________________
//
// KWindowDesign.cxx
// Author: Michael Unrau
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// Methods of this class return a set of coefficients of different window functions. They are static,
// so there is no need to create an instance of KWindow. KWindow::GetTukeyWindow should be enough



#include "KWindowDesign.h"
#include <cmath>
#include <iostream>
using namespace std;


KWindowDesign::KWindowDesign(void)
{
}

KWindowDesign::~KWindowDesign(void)
{
}

void KWindowDesign::GetTukeyWindow(double alpha, double* coef, unsigned int size){
  if(size != 0){
		double  temp = alpha*(size-1)/2.0;
		for(int i = 0; i < (int) temp; i++)
			*(coef+i) = 0.5 + 0.5*cos(3.14159265358979*((i/(double)temp) - 1));
		for(unsigned int i = (unsigned int) temp; i < (unsigned int) (size-temp); i++)
			*(coef+i) = 1.0;
		for(unsigned int i = (unsigned int) (size-temp); i < size; i++)
			*(coef+i) = 0.5 + 0.5*cos(3.14159265358979*((i/(double) temp) - 2.0/alpha - 1));		
	}
	else
		cerr<<"size is zero!"<<endl;
}


