/*
 *  EdwEventCollection.h
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 5/26/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __EDWEVENTCOLLECTION_H__
#define __EDWEVENTCOLLECTION_H__

#include "TObjArray.h"
#include "TCollection.h"

class EdwEventBase;

class EdwEventCollection : public TObjArray { 

public:
  //Constructors
  EdwEventCollection(Int_t s = TCollection::kInitCapacity, Int_t lowerBound = 0);
  virtual ~EdwEventCollection(void);
  virtual void Clear(Option_t *opt = "C");

	void AddEvent(const EdwEventBase *anEvent);
	//void AddInitialEvent(const EdwEventBase *anEvent);
	EdwEventBase* GetEvent(Int_t i){return (EdwEventBase *)At(i);}
	EdwEventBase* GetEntry(Int_t i){return GetEvent(i);}
	//EdwEventBase* GetEvent(Int_t i);
	//Int_t GetNumEvents(void);
	
		
private:
	
	//private methods
  void InitializeMembers(void);
	void Add(TObject */*obj*/){	}  //remove this method from use
	
  ClassDef(EdwEventCollection,1);
};


#endif // __EDWEVENTCOLLECTION_H__
