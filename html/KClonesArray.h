/*
 *  KClonesArray.h
 *
 *  Created by Adam Cox on 2/23/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *  Special thanks to Mike Marino for "help" with this class.
 */

#ifndef __KCLONESARRAY_H__
#define __KCLONESARRAY_H__

#include "TClonesArray.h"

class KClonesArray : public TClonesArray {
  
  // An initial wrapper of TClonesArray, in case 
  // extra functionality is required.
public:
  KClonesArray() : TClonesArray() {}
  
  KClonesArray( const KClonesArray& array ) : TClonesArray( array ) {}
  
  KClonesArray( const char* classname, Int_t size = 1000, Bool_t call_dtor = false ) : 
  TClonesArray( classname, size, call_dtor ) {}
  
  KClonesArray( const TClass* aClass, Int_t size = 1000, Bool_t call_dtor = false ) : 
  TClonesArray( aClass, size, call_dtor ) {}
  
  virtual TObject *&operator[](Int_t idx);
  virtual TObject *operator[](Int_t idx) const { return
    TClonesArray::operator[](idx); }
  virtual TObject *GetNewOrCleanedObject(Int_t idx);
  virtual void Clear(Option_t *opt);
  virtual void RemoveAndCompress(TObject *obj);
  
  ClassDef( KClonesArray, 2 )
};

#endif /* __KCLONESARRAY_H__ */
