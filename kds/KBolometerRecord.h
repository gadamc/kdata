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

#include "TObject.h"
#include "TRef.h"
#include "TRefArray.h"
#include <string>

using namespace std;

class KBolometerRecord : public TObject {

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
  
  Bool_t IsDetector(const char* name);
  
  virtual void Compact(void);
  
  //getters
  const Char_t* GetDetectorName(void) const;
  Double32_t GetMass(void) const { return fMass;}

  //setters
  void SetDetectorName(const Char_t* aWord); 
  void SetMass(Double32_t aVal) {fMass = aVal;}

protected:
  TRef fSambaRecord;  //holds a pointer to the associated Samba record
  TRefArray fPulseRecords; //holds an array of pointers to the BoloPulse sub records associated with this bolometer event

private:
    
  string fDetectorName; 
  Double32_t fMass;

  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KBolometerRecord &aRec);

  ClassDef(KBolometerRecord,5);
};


#endif // __KBOLOMETERRECORD_H__

