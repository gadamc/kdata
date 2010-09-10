/*
 *  KBolometerRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KBOLOMETERRECORD_H__
#define __KBOLOMETERRECORD_H__

#include "KSubRecord.h"
#include <string>

using namespace std;

class KBolometerRecord : public KSubRecord {

public:
  //Constructors
  KBolometerRecord(void);
	KBolometerRecord(const KBolometerRecord &aRec);
	KBolometerRecord& operator=(const KBolometerRecord &aRec);
  virtual ~KBolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KBolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KBolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KBolometerRecord &aRec) const { return !(*this==aRec); }
	virtual void Compact(void);
	
	//getters
	string GetDetectorName(void) const;
	//Int_t GetListe2(void) const {return fListe2;}
	//Int_t GetListe1(void) const {return fListe1;}
	//Int_t GetVoie(void) const {return fVoie;}
	//Int_t GetHeatPositiveThreshold(void) const {return fHeatPositiveThreshold;}
	//Int_t GetHeatNegativeThreshold(void) const {return fHeatNegativeThreshold;}
	//Int_t GetTimeOfLastThresholdChange(void) const {return fTimeOfLastThresholdChange;}
	
	//setters
	void SetDetectorName(const Char_t* aWord); 
	//void SetListe2(Int_t aWord) {fListe2 = aWord;}
	//void SetListe1(Int_t aWord) {fListe1 = aWord;}
	//void SetVoie(Int_t aWord) {fVoie = aWord;}
	//void SetHeatPositiveThreshold(Int_t aWord) {fHeatPositiveThreshold = aWord;}
	//void SetHeatNegativeThreshold(Int_t aWord) {fHeatNegativeThreshold = aWord;}
	//void SetTimeOfLastThresholdChange(Int_t aWord) {fTimeOfLastThresholdChange = aWord;}

	//void NullPointers(void){fDetectorName=0; fDetectorNameSize=0;}

private:
		
	string fDetectorName; 
	
	//Int_t fHeatPositiveThreshold;   
	//Int_t fHeatNegativeThreshold;
	//Int_t fTimeOfLastThresholdChange; //in seconds as reported in the seulis files. 
	
	//Int_t fListe2;
	//Int_t fListe1;
	//Int_t fVoie; //voie = track. this is the cabling number for each bolometer
	
	//private methods
  void InitializeMembers(void);
	void CopyLocalMembers(const KBolometerRecord &aRec);
	
  ClassDef(KBolometerRecord,1);
};


#endif // __KBOLOMETERRECORD_H__

