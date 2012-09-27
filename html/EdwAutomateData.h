/**
   @file    EdwAutomateData.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class encapsulating the variables stored in the Edelweiss automate monitoring: cryogenic parameters and the status of motorized source.
*/

#ifndef _EDWAUTOMATEDATA_H_
#define _EDWAUTOMATEDATA_H_

#include "EdwUtils.h"

/**
   @class   EdwAutomateData
   @brief   Class encapsulating the variables stored in the Edelweiss automate monitoring: cryogenic parameters and the status of motorized source.

   An EdwAutomateData object has one timestamp and monitoring fields. A TTree of EdwAutomateData objects is filled and stored using the EdwRawDB structure. There is typically one monitoring entry every 5 min.

*/
class EdwAutomateData : public TObject {
 public:
  EdwAutomateData();
  ~EdwAutomateData();
  virtual void Clear(); /**< Resets the variables */

  time_t fDate; /**< Timestamp (standard Unix format) */
  Float_t fTBolo; /**< Bolometer temperature */
  Bool_t fBaSource1; 
  /**< If true, the first motorized Ba source is active (gamma-ray calibration) */
  Bool_t fBaSource2; 
  /**< If true, the second motorized Ba source is active (gamma-ray calibration) */
  Bool_t fCoSource1; 
  /**< If true, the first motorized Co source is active (detector regeneration) */
  Bool_t fCoSource2; 
  /**< If true, the second motorized Co source is active (detector regeneration) */

 private:

  ClassDef(EdwAutomateData,1)
};

#endif
