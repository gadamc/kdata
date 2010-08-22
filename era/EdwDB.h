/**
   @file    EdwDB.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Basis class (but non-virtual) encapsulating simple databasis needed for the analysis.
*/

#ifndef _EDWDB_H_
#define _EDWDB_H_

#include "EdwUtils.h"

/**
   @class   EdwDB
   @brief   Basis class (but non-virtual) encapsulating simple databasis needed for the analysis.

   This class simply contains the file and tree into which different database branches may be stored. Used for the EdwRawDB and EdwAnaDB classes.

*/
class EdwDB : public TObject {
 public:
  EdwDB() ;
  ~EdwDB() ;
  EdwDB(string aFile, string aRead = "READ") ;
  /**< Opens the file and tree, either in "READ" mode or "RECREATE" mode */
  string FileName() const { return fFileName; } /**< Full name of the DB file */
  void CloseFile(); /**< Close fFile */
  Bool_t IsWritable() const { return fFile->IsWritable(); } /**< Returns 1 if fFile is writable (ROOT function) */

 protected:
  string fFileName ; /**< Name of the DB file */
  TFile* fFile ; /**< TFile object containing all the db */

  ClassDef(EdwDB,1)
};

#endif
