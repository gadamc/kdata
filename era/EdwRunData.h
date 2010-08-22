/**
   @file    EdwRunData.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Object, associated to each Samba run, encapsulating basic run parameters
*/

#ifndef _EDWRUNDATA_H_
#define _EDWRUNDATA_H_

#include "EdwUtils.h"

/**
   @class   EdwRunData
   @brief   Object, associated to each Samba run, encapsulating basic run parameters

   This object is to be inserted in the EdwRawDB database. It contains for each run various parameters declared by Samba: channels with their DAQ settings (heat modulation, voltages), trigger thresholds (fixed or adaptative).

*/
class EdwRunData : public TObject {
 public:
  EdwRunData();
  ~EdwRunData();
  void Clear(); /**< Clear */
  void Dump(); /**< Print most of the informations to stdout */

  string fRunName ; /**< Samba run name */
  string fSambaVersion ; /**< Version of Samba used during the acquisition of that run */
  ByteOrderType fByteOrdering ; /**< Little or big endian format for raw data */
  vector<string> fRunFiles ; /**< List of raw data files ("partitions") */

  Bool_t fIsGammaCalib; /**< Not maintained */
  Bool_t fIsNeutronCalib; /**< Not maintained */

  Float_t fSambaExposure ; /**< Exposure in kg.d as indicated at the end of Samba log */
  ULong_t fSambaSecondsRead ; /**< Number of seconds treated by Samba, as indicated at the end of the log */
  ULong_t fSambaSecondsNotTreated ; /**< Dead time, as indicated at the end of the log */

  vector<string> fChannelNames ; /**< Channels recorded */
  vector<Float_t> fIonizationVoltages ; /**< Taken from the log. Set to 0 for heat channels */
  vector<Float_t> fHeatModulations ; /**< Taken from the log. Set to 0 for ionization channels */

  vector<time_t> fThresholdsTimes ; /**< Case of adaptative trigger */
  vector< vector<Float_t> > fThresholdsPos ; /**< Case of adaptative trigger */
  vector< vector<Float_t> > fThresholdsNeg ; /**< Case of adaptative trigger */

  Float_t GetVoltage(string aChannel) const ; /**< Voltages */

 private:

  ClassDef(EdwRunData,2)
};

#endif
