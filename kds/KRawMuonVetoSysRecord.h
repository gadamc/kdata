/*
 *  KRawMuonVetoSysRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWMUONVETOSYSRECORD_H__
#define __KRAWMUONVETOSYSRECORD_H__

#include "KMuonVetoSystemRecord.h"

class KRawMuonVetoSysRecord : public KMuonVetoSystemRecord
{ 

public:
  //Constructors
  KRawMuonVetoSysRecord(void);
  KRawMuonVetoSysRecord(const KRawMuonVetoSysRecord &aSysRec);
  KRawMuonVetoSysRecord& operator=(const KRawMuonVetoSysRecord &aSysRec);
  virtual void Clear(Option_t *option = "C"); 
  virtual ~KRawMuonVetoSysRecord(void);
  Bool_t IsSame(const KRawMuonVetoSysRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KRawMuonVetoSysRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KRawMuonVetoSysRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);
  
private:

  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KRawMuonVetoSysRecord &aSysRec);
  ClassDef(KRawMuonVetoSysRecord,1);
};


#endif // __KRAWMUONVETOSYSRECORD_H__
