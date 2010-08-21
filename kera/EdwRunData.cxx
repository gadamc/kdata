/**
   @file    EdwRunData.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwRunData class. See the .h file
*/

#include "EdwRunData.h"

ClassImp(EdwRunData); /**< Root dictionnary */

EdwRunData::EdwRunData() : TObject() {

}

EdwRunData::~EdwRunData() {

}

void EdwRunData::Clear() {
  fRunName = "";
  fSambaVersion = "";
  fByteOrdering = kBigEndian;
  fRunFiles.clear();
  fSambaExposure = 0;
  fSambaSecondsRead = 0;
  fSambaSecondsNotTreated = 0;
  fChannelNames.clear();
  fIonizationVoltages.clear();
  fHeatModulations.clear();
  fThresholdsTimes.clear();
  fThresholdsPos.clear();
  fThresholdsNeg.clear();
}

void EdwRunData::Dump() {
  cout << "Run: "<< fRunName << endl;
  cout << "Samba version: "<< fSambaVersion << endl;
  cout << "Byte ordering: "<< fByteOrdering << endl;
  cout << "Run files: ";
  for (UInt_t i=0; i<fRunFiles.size()-1; i++) cout << fRunFiles[i] << " - ";
  cout << fRunFiles[fRunFiles.size()-1] << endl;
  cout << "Is Gamma Calibration: " << fIsGammaCalib << endl;
  cout << "Is Neutron Calibration: " <<fIsNeutronCalib<< endl;
  cout << "Samba exposure: " << fSambaExposure << endl;
  cout << "Samba seconds read: " << fSambaSecondsRead << endl;
  cout << "Seconds not treated by Samba: " << fSambaSecondsNotTreated << endl;
  cout << "List of channels: " << endl;
  for (UInt_t i=0; i<fChannelNames.size()-1; i++) {
    cout << "  " << fChannelNames[i] << " - Vionization = " << fIonizationVoltages[i] 
	 << " - HeatModulation = " << fHeatModulations[i] << endl;
  }

}

Float_t EdwRunData::GetVoltage(string aChannel) const {

  UInt_t nbchannels = fChannelNames.size();
  for (UInt_t i=0; i<nbchannels; i++) {
    if (fChannelNames[i] == aChannel) return fIonizationVoltages[i];
  }

  return 0;
}
