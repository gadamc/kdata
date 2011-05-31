/*
 *  KIndex.h
 *  KDataStructure
 *
 *  Created by Benjamin Schmidt on 04/14/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
//test build eclipse

#ifndef __KIndex_H__
#define __KIndex_H__

#include "KDataReader.h"
#include "KEvent.h"

//class KDataReader;

class KIndex {

public:
  //Constructors
    //KDataReader(const Char_t* fileName);
    KIndex(KDataReader* r);
    KIndex();
     ~KIndex(void); //virtual???

    
    
	Int_t BuildIndex(KDataReader* reader=0, Bool_t boolRestarts=true); //return number of entries in index or 0 if failed
    Int_t GetIndex( Int_t i){return fIndex[i];}
    Int_t GetIndexValue(Int_t j){return fIndexValue[j];}
    Int_t GetEntries(void) {return fN;}
    Short_t* GetNumberOfAcquisitionMacs();
	
	Int_t CheckTimeRestarts(Short_t* macs);
	Int_t RemoveEntriesFromList();
    void PrintIndex(void);
  
protected:
    
	Int_t fN; //number of entries in the TChain belonging to a particular KDataReader at first, later number of selected events in the index arrays
    Int_t* fIndex; //array fN elements
    Long64_t* fIndexValue; //array fN elements
    Int_t* fEntriesToRemove; //Enable the selection of a number of elemnts out of fIndex 
    Int_t fNEntries; //Number of elements in fEntriesToRemove
    Bool_t fNeedFurtherProcessing; //if a time restart was discovered this is set to true
    
    KDataReader* fR;
    KEvent* fE;
    
    void ReadToIndex(void);
    void SortIndex(void);
    void InitializeMembers(void);
  ClassDef(KIndex,1);
};


#endif // __KIndex_H__
