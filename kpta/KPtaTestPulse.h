//______________________________________________________________________
//
// KPtaTestPulse.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KPTATESTPULSE_H__
#define __KPTATESTPULSE_H__

#include <vector>

using namespace std;

class KPtaTestPulse  { 

public:
  //Constructors
  KPtaTestPulse(void);
  virtual ~KPtaTestPulse(void);

	virtual void CreateSineWave(void);
	virtual void CreateSquareWave(void);
  virtual void CreateWhiteNoise(void);
	
	//getters
	virtual vector<double> GetPulse(void) const {return fPulse;	}
	
	virtual unsigned int GetLength(void)  const {return fPulse.size();	}
	virtual unsigned int GetPeriod(void)  const {return fPeriod;	}
	virtual unsigned int GetAmp(void)  const {return fAmp;	}

  //setters	
	virtual void SetLength(unsigned int aVal){fPulse.resize(aVal,0);	}
	virtual void SetPeriod(unsigned int aVal){fPeriod = aVal;	}
	virtual void SetAmp(unsigned int aVal){fAmp = aVal;	}

private:

	unsigned int fPeriod;
	unsigned int fAmp;
	
	vector<double> fPulse;
	
  //private methods
  void InitializeMembers(void);

};


#endif // __KPTATESTPULSE_H__
