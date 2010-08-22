/**
   @file    SambaToRoot.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Interface class to read Samba run data files and fill Root trees of EdwEvents.
*/

#ifndef _EDWRAWDATA_H_
#define _EDWRAWDATA_H_

#include "EdwUtils.h"
#include "EdwEvent.h"

#define DEFAULT_BUFSIZE 32000 /**< Buffer size of the event branch in the root tree */

/**
   @class   SambaToRoot
   @brief   Interface class to read Samba run data files and fill Root trees of EdwEvents.

*/
class SambaToRoot : public TObject {
 public:
  SambaToRoot() ;
  SambaToRoot(ByteOrderType aByteOrder, string aEdwFile, string aRootFile) ; /**< Standard constructor */
  ~SambaToRoot() ;
  string EdwFileName() const { return fEdwFileName; } /**< Name of raw data file */
  string RootFileName() const { return fRootFileName; } /**< Name of root file to be created */
  string EdwRunName() const { return fEdwRunName; } /**< Samba run */
  void SetByteOrdering(ByteOrderType aOrder) { fByteOrdering = aOrder; } /**< Byte ordering of the raw data file */
  void SetEdwFileName(string aName) { fEdwFileName=aName; } /**< Name of raw data file */
  void SetRootFileName(string aName) { fRootFileName=aName; } /**< Name of root file to be created */
  void SetEdwRunName(string aRunName) { fEdwRunName = aRunName; } /**< Samba run */

  Int_t FillRootFile(Int_t aBufSize=DEFAULT_BUFSIZE) ; /**< Main routine. returns 0 if filling was ok */

 private:
  ByteOrderType fByteOrdering ; /**< byte ordering by samba */
  string fEdwFileName ; /**< samba file */
  string fEdwRunName ; /**< samba run */
  ifstream fEdwFile ; /**< samba file */
  string fRootFileName ; /**< root file */
  TFile* fRootFile ; /**< root file */

  ClassDef(SambaToRoot,1)
};

#endif
