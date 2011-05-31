//
// KMergeRoot.h
// KDataStructure
//
// Created by Benjamin Schmidt on 4/18/11.
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
//

#ifndef __KMergeRoot_H__
#define __KMergeRoot_H__

#include "KDataWriter.h"
#include "KIndex.h"


class KMergeRoot : public KIndex
{

public:
	KMergeRoot(void);
	KMergeRoot(KDataReader* r,  const char* chOut="~/uspaceBen/testData/rootData/mergedData/test.root", Int_t stamp=0, Int_t lastDay=-1);
	//mergeDate should be passed in standard edw notation for examples "lc18"
	~KMergeRoot(void);
	
    Bool_t IsReady();
    Int_t Merge();
	Int_t Merge(KDataReader *r, const char* chOut="" );
    //Int_t GetStampToUnixTimeOffset(Int_t macToSync=1); //make it possible to convert a stamp into a unixTime
    //Int_t GetStampToUnixTime(Int_t stamp) {return Int_t(fBaseStamp+(stamp-fBaseStamp)*1.008+fOffset);}
    //Int_t GetStampOfLastFile(); //Check in the output directory if a file from the day before exits, read it get last stamp
	Int_t CheckStampSpans2DaysOfData(); //Day of Year to convert
    Bool_t GetSelectedEntry( Int_t stamp);
	Int_t GetFirstStampToSetfLastStamp();
    
private:
	
    Bool_t fSorted;
    KDataWriter* fW;
    KEvent* fEOut;
    Int_t fOffset;
    Int_t fDay; //Day of Year
    Int_t fLastDay; //mergeKdataRaw keeps track of the last element written to a file
    Int_t fLastStamp; //mergeKdataRaw keeps track of the last element written to a file
    Int_t fNewLastStamp;
    TString fOutPath; //standard output directory specified in the mergeKDataRaw.py and ksamba2kdata.py scripts
	TString fDate; //the merge Date
	ClassDef(KMergeRoot, 1);
};


#endif // __KMergeRoot_H__
