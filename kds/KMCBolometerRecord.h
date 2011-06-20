/*
 *  KMCBolometerRecord.h
 *  KDataStructure
 *
 *  Created by Alexander Wunderele on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KMCBOLOMETERRECORD_H__
#define __KMCBOLOMETERRECORD_H__

#include "KHLABolometerRecord.h"
#include <string>

using namespace std;

class KMCBolometerRecord : public KHLABolometerRecord {

public:
  //Constructors
  KMCBolometerRecord(void);
	KMCBolometerRecord(const KMCBolometerRecord &aRec);
	KMCBolometerRecord& operator=(const KMCBolometerRecord &aRec);
  virtual ~KMCBolometerRecord(void);
	virtual void Clear(Option_t *opt = "C");
	Bool_t IsSame(const KMCBolometerRecord &aRec, Bool_t bPrint = false) const;
	Bool_t operator==(const KMCBolometerRecord &aRec) const { return IsSame(aRec,false); }
	Bool_t operator!=(const KMCBolometerRecord &aRec) const { return !(*this==aRec); }

	
	Int_t GetEventID(void) const {return fEventID;}
	Int_t GetNumHits(void) const {return fNumHits;}
	Int_t GetTrackID(void) const {return fTrackID;}
	Int_t GetParentID(void) const {return fParentID;}
	Double_t GetTime(void) const {return fTime;}
	Double_t GetEnergy(void) const {return fEnergy;}
	Double_t GetRecoilEnergy(void) const {return fRecoilEnergy;}
	string GetParticleName(void) const {string str=""; if(fParticleName==0) return str; str = fParticleName; return str;}
	string GetProcess(void) const {string str=""; if(fProcess==0) return str; str = fProcess; return str;}
	string GetCreationProcess(void) const {string str=""; if(fCreationProcess==0) return str; str = fCreationProcess; return str;}
	Double_t GetXpos(void) const {return fXpos;}
	Double_t GetYpos(void) const {return fYpos;}
	Double_t GetZpos(void) const {return fZpos;}
	Double_t GetMCQvalue(void) const {return fMCQvalue;}
	
	void SetEventID(Int_t aNum) {fEventID = aNum;}
	void SetNumHits(Int_t aNum) {fNumHits = aNum;}
	void SetTrackID(Int_t aNum) {fTrackID = aNum;}
	void SetParentID(Int_t aNum) {fParentID = aNum;}
	void SetTime(Double_t aNum) {fTime = aNum;}
	void SetEnergy(Double_t aNum) {fEnergy = aNum;}
	void SetRecoilEnergy(Double_t aNum) {fRecoilEnergy = aNum;}
	void SetParticleName(const Char_t* aWord, Int_t aSize); 
	void SetProcess(const Char_t* aWord, Int_t aSize); 
	void SetCreationProcess(const Char_t* aWord, Int_t aSize); 
	void SetXpos(Double_t aNum) {fXpos = aNum;}
	void SetYpos(Double_t aNum) {fYpos = aNum;}
	void SetZpos(Double_t aNum) {fZpos = aNum;}
	void SetMCQvalue(Double_t aNum) {fMCQvalue = aNum;}

	//void NullPointers(void){fDetectorName=0; fDetectorNameSize=0;}

private:
	
	Int_t fEventID;
	Int_t fNumHits;
	Int_t fTrackID;
	Int_t fParentID;
	Double_t fTime;
	Double_t fEnergy;
	Double_t fRecoilEnergy;
	Int_t fParticleNameSize;
	Char_t *fParticleName; //[fParticleNameSize]
	Int_t fProcessSize;
	Char_t *fProcess; //[fProcessSize]
	Int_t fCreationProcessSize;
	Char_t *fCreationProcess; //[fCreationProcessSize]
	Double_t fXpos;
	Double_t fYpos;
	Double_t fZpos;
	Double_t fMCQvalue;
	
	//private methods
  void InitializeMembers(void);
	void CopyLocalMembers(const KMCBolometerRecord &aRec);
	
  ClassDef(KMCBolometerRecord,2);
};


#endif // __KMCBOLOMETERRECORD_H__

