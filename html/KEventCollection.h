/*
 *  KEventCollection.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 5/26/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KEVENTCOLLECTION_H__
#define __KEVENTCOLLECTION_H__

#include "TObjArray.h"
#include "TCollection.h"

class KEvent;

class KEventCollection : public TObjArray { 

public:
  //Constructors
  KEventCollection(Int_t s = TCollection::kInitCapacity, Int_t lowerBound = 0);
  virtual ~KEventCollection(void);
  virtual void Clear(Option_t *opt = "C");

	void AddEvent(const KEvent *anEvent);
	//void AddInitialEvent(const KEvent *anEvent);
	KEvent* GetEvent(Int_t i){return (KEvent *)At(i);}
	KEvent* GetEntry(Int_t i){return GetEvent(i);}
	//KEvent* GetEvent(Int_t i);
	//Int_t GetNumEvents(void);
	
		
private:
	
	//private methods
  void InitializeMembers(void);
	void Add(TObject */*obj*/){	}  //remove this method from use
	
  ClassDef(KEventCollection,2);
};


#endif // __KEVENTCOLLECTION_H__
