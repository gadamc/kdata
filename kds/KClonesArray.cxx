//______________________________________________________________________
//
// KClonesArray.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2011 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Created Wednesday 23. February 2011.
// Special thanks to Mike Marino for "assistance" with this class. See his 
// post on the ROOT user forums for the discussion and his example,
// which was blatantly stolen.
//
//

#include "KClonesArray.h"
#include "TClass.h"

ClassImp(KClonesArray)
//______________________________________________________________________________
TObject *&KClonesArray::operator[](Int_t idx)
{
  // Overload of TClonesArray::operator[], zeros memory allocated so
  // that calls like:
  //
  // temp = myClonesArray[i];
  // temp->TestBit(TObject::kNotDeleted)
  // 
  // will behave correctly. 
  Bool_t mustZero = false;
  if (idx >= 0 && fClass && (idx >= fSize || !fKeep->UncheckedAt(idx))) {
    mustZero = true;
  }
  TObject *&temp = TClonesArray::operator[](idx);
  if ( temp && mustZero ) memset(temp, 0, fClass->Size());
  return temp;
}

//______________________________________________________________________________
TObject *KClonesArray::GetNewOrCleanedObject(Int_t idx) 
{
  // Get a new or cleaned object from the array.  Tests to see if the
  // destructor has been called on the object.  If so, or if the object
  // has never been constructed the class constructor is called using
  // New().  If not, return a pointer to the correct memory location.
  // This explicitly to deal with TObject classes that allocate memory
  // which will be reset (but not deallocated) in their Clear()
  // functions.  
  
  TObject* temp = (*this)[idx];
  if ( temp && temp->TestBit(TObject::kNotDeleted) ) return temp;
  return (fClass) ? static_cast<TObject*>(fClass->New(temp)) : 0;
}
//______________________________________________________________________________
void KClonesArray::Clear(Option_t *option)
{
  //
  // Properly reset the UUID, reference bits and reset the UniqueID of each
  // object.
  //
  if (option && option[0] == 'C') {
    Int_t n = GetEntriesFast();
    for (Int_t i = 0; i < n; i++) {
      TObject *obj = UncheckedAt(i);
      if (obj) {
        obj->ResetBit( kHasUUID ); 
        obj->ResetBit( kIsReferenced ); 
        obj->SetUniqueID( 0 ); 
      }
    }
  }
  TClonesArray::Clear(option);
}
//______________________________________________________________________________
void KClonesArray::RemoveAndCompress(TObject *obj)
{
  // Remove an object, compress to remove spaces in the array if necessary.
  // The compression will only be called if obj is not the last element in the
  // array.
  
  Int_t index = IndexOf(obj);
  if (index < LowerBound()) return;
  
  bool mustCompress = true;
  // check if it's the last guy;
  if (index == GetAbsLast()) mustCompress = false;
  RemoveAt(index);
  if (mustCompress) Compress();
}
