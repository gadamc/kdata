/*
 *  EdwRawEvent.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 3/24/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWRAWEVENT_H__
#define __EDWRAWEVENT_H__

#include "EdwEventBase.h"

//class TObject;
//class EdwEventFactory;

const Char_t kRawEventName[] = "Raw";
const Char_t kRawClassName[] = "EdwRawEvent";

class EdwHLAEvent;

class EdwRawEvent : public EdwEventBase {         
	
public:
	//Constructors and destructor. Only Derived class and the Factory
	//SHOULD access these methods. 
	//However, in order for ROOT's I/O to work properly
	//the constructor and destructor
	//have to be public. Otherwise, I would have made them protected or private
	EdwRawEvent(void);
	EdwRawEvent(const EdwRawEvent &anEvent);
	virtual ~EdwRawEvent(void);
	EdwRawEvent& operator=(const EdwEventBase &anEvent);
	EdwRawEvent& operator=(const EdwHLAEvent &anEvent);
	EdwRawEvent& operator=(const EdwRawEvent &anEvent);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const EdwRawEvent &anEvent, Bool_t bPrint = false) const;
	Bool_t operator==(const EdwRawEvent &anEvent) const { return IsSame(anEvent,false); }
	Bool_t operator!=(const EdwRawEvent &anEvent) const { return !(*this==anEvent); }
	virtual void Compact(void);
private: 
	
	void InitializeMembers(void);
	void CopyLocalMembers(const EdwRawEvent &anEvent);

	ClassDef(EdwRawEvent ,1);
};



#endif // __EDWRAWEVENT_H__
