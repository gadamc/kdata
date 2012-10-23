/*
 *  KHLAMuonModuleRecord.h
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/25/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#ifndef __KHLAMUONMODULERECORD_H__
#define __KHLAMUONMODULERECORD_H__

#include "KMuonModuleRecord.h"

class KHLAMuonModuleRecord : public KMuonModuleRecord
{
  
public:
  //Constructors
  KHLAMuonModuleRecord(void);
  KHLAMuonModuleRecord(const KHLAMuonModuleRecord &aRec);
  KHLAMuonModuleRecord& operator=(const KHLAMuonModuleRecord &aRec);
  virtual ~KHLAMuonModuleRecord(void);
  virtual void Clear(Option_t *anOpt);
  Bool_t IsSame(const KHLAMuonModuleRecord &aRec, Bool_t bPrint = false) const;
  Bool_t operator==(const KHLAMuonModuleRecord &aRec) const { return IsSame(aRec,false); }
  Bool_t operator!=(const KHLAMuonModuleRecord &aRec) const { return !(*this==aRec); }
  virtual void Compact(void);
  
  
private:
    
  //private methods
  void InitializeMembers(void);
  void CopyLocalMembers(const KHLAMuonModuleRecord &aRec);
  ClassDef(KHLAMuonModuleRecord,2);
};


#endif // __KHLAMUONMODULERECORD_H__
