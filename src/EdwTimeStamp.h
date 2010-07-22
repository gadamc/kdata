/*
 *  EdwTimeStamp.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#ifndef __EDWTIMESTAMP_H__
#define __EDWTIMESTAMP_H__

#include "TTimeStamp.h"

class EdwTimeStamp : public TTimeStamp {

public:
	EdwTimeStamp(void);
	virtual ~EdwTimeStamp(void);
	
private:
	
	ClassDef(EdwTimeStamp, 1);
};


#endif // __EDWTIMESTAMP_H__
