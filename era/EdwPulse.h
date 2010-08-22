/**
   @file    EdwPulse.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class encapsulating the raw pulse information from Samba, as stored in the EdwEvent object.
*/

#ifndef _EDWPULSE_H_
#define _EDWPULSE_H_

#include "Pulse.h"

/**
   @class   EdwPulse
   @brief   Class encapsulating the raw pulse information from Samba, as stored in the EdwEvent object.

   This class contains raw pulse information (as stored in the Pulse object from which it inherits), with specific informations recorded by Samba, such as the online amplitude and baseline.

*/

class EdwPulse : public Pulse {
 public:
  EdwPulse() ;
  ~EdwPulse() ;
  Float_t SambaAmpl() const { return fSambaAmpl; } 
  /**< Online pulse amplitude (filtered ADUs) */
  Float_t SambaBase() const { return fSambaBase; }
  /**< Online baseline level (filtered ADUs) */
  Float_t SambaNoise() const { return fSambaNoise; }
  /**< Online baseline fluctuations (filtered ADUs) */
  
  void SetSambaAmpl(Float_t aAmpl) { fSambaAmpl = aAmpl; }  /**< Online pulse amplitude (filtered ADUs) */
  void SetSambaBase(Float_t aBase) { fSambaBase = aBase; }  /**< Online baseline level (filtered ADUs) */
  void SetSambaNoise(Float_t aNoise) { fSambaNoise = aNoise; }  /**< Online baseline fluctuations (filtered ADUs) */
  
 private:
  Float_t fSambaAmpl;  /**< Online pulse amplitude (filtered ADUs) */
  Float_t fSambaBase; /**< Online baseline level (filtered ADUs) */
  Float_t fSambaNoise;  /**< Online baseline fluctuations (filtered ADUs) */


  ClassDef(EdwPulse,1)
};

#endif
