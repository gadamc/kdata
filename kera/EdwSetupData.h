/**
   @file    EdwSetupData.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Setup data (detectors, geometry...), to implement
*/

#ifndef _EDWSETUPDATA_H_
#define _EDWSETUPDATA_H_

#include "EdwUtils.h"

/**
   @class   EdwSetupData
   @brief   Setup data (detectors, geometry...), to implement
*/
class EdwSetupData : public TObject {
 public:
  EdwSetupData();
  ~EdwSetupData();

  time_t fBeginTime ; /**< Begin */
  time_t fEndTime ; /**< End */
  string fListBolos[40] ; /**< Bolos */

 private:

  ClassDef(EdwSetupData,1)
};

#endif
