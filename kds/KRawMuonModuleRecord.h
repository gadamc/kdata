/*
 *  KRawMuonModuleRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KRAWMUONMODULERECORD_H__
#define __KRAWMUONMODULERECORD_H__

#include "KMuonModuleRecord.h"

class KRawMuonModuleRecord : public KMuonModuleRecord
{
  
public:
  //Constructors
  KRawMuonModuleRecord(void);
  KRawMuonModuleRecord(const KRawMuonModuleRecord &aRec);
  KRawMuonModuleRecord& operator=(const KRawMuonModuleRecord &aRec);
  virtual ~KRawMuonModuleRecord(void);
  virtual void Clear(Option_t *anOpt);
  Bool_t IsSame(const KRawMuonModuleRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KRawMuonModuleRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KRawMuonModuleRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);
  
  
private:
    
  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KRawMuonModuleRecord &aRec);
  ClassDef(KRawMuonModuleRecord,2);
};


#endif // __KRAWMUONMODULERECORD_H__
