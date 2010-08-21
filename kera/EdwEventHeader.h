/**
   @file    EdwEventHeader.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Simple class containing raw event header information. An EdwEventHeader is stored into each EdwEvent.
*/

#ifndef _EDWEVENTHEADER_H_
#define _EDWEVENTHEADER_H_
#include "EdwUtils.h"

/**
   @class   EdwEventHeader
   @brief   Simple class containing raw event header information. An EdwEventHeader is stored into each EdwEvent.

   The class isn't complete yet, only some vital informations are extracted.

*/
class EdwEventHeader : public TObject {
 public:
  EdwEventHeader() ;
  ~EdwEventHeader() {}
  string Run() const { return fRun; }
  /**< Name of the Samba run in which the event was recorded */
  UInt_t Num() const { return fNum; }
  /**< Samba event number within the run. This is not the same as the entry number of the event when it is read from a TChain */
  time_t DateSec() const { return fDateSec; } 
  /**< Samba seconds */
  Int_t DateMuSec() const { return fDateMuSec; }
  /**< Samba microseconds */
  Float_t SambaDelay() const { return fSambaDelay; }
  /**< Delay (in seconds) with respect to the previous event, as recorded by Samba */
  UInt_t TriggerBit(Int_t num) const ;
  /**< Returns the trigger bit 1 or 2 */

  void SetNum(unsigned long num) { fNum=num; }
  /**< Set Samba event number */
  void SetDateSec(time_t sec) { fDateSec = sec; }
  /**< Set event time */
  void SetDateMuSec(Int_t musec) { fDateMuSec = musec; }
  /**< Set event time */
  void SetRun(string run) { fRun=run; }
  /**< Set Samba run */
  void SetSambaDelay(Float_t aDelay) { fSambaDelay = aDelay; }
  /**< Set Samba delay */
  void SetTriggerBit(UInt_t aNum, UInt_t aBit) ;
  /**< Set the trigger bit num. 1 or 2 */
  void Clear() ; /**< Clear the header */

 private:
  string fRun ; /**< Run name */
  UInt_t fNum ; /**< Samba event num */
  time_t fDateSec ; /**< Event unix time */
  Int_t fDateMuSec; /**< Microsec time */
  Float_t fSambaDelay ; /**< Delay wrt previous event */
  UInt_t fTriggerBit1; /**< First part of bit trigger encoding */
  UInt_t fTriggerBit2; /**< Second part of bit trigger encoding */

  ClassDef(EdwEventHeader,1)
};
 
#endif
