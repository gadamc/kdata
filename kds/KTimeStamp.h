/*
 *  KTimeStamp.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __KTIMESTAMP_H__
#define __KTIMESTAMP_H__

#include "TTimeStamp.h"

class KTimeStamp : public TTimeStamp {

public:
	KTimeStamp(void);
	virtual ~KTimeStamp(void);
	
private:
	
	ClassDef(KTimeStamp, 1);
};


#endif // __KTIMESTAMP_H__
