/*
 *  KRawEvent.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/24/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWEVENT_H__
#define __KRAWEVENT_H__

#include "KEvent.h"

//class TObject;
//class KEventFactory;

const Char_t kRawEventName[] = "Raw";
const Char_t kRawClassName[] = "KRawEvent";

class KHLAEvent;

class KRawEvent : public KEvent {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	KRawEvent(void);
	KRawEvent(const KRawEvent &anEvent);
	virtual ~KRawEvent(void);
	KRawEvent& operator=(const KEvent &anEvent);
	KRawEvent& operator=(const KHLAEvent &anEvent);
	KRawEvent& operator=(const KRawEvent &anEvent);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KRawEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const KRawEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const KRawEvent &anEvent) const { return !(*this==anEvent); }
	virtual void Compact(void);
private: 
	
	void InitializeMembers(void);
	void CopyLocalMembers(const KRawEvent &anEvent);

	ClassDef(KRawEvent ,1);
};



#endif // __KRAWEVENT_H__
