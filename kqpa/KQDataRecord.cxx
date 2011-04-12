//_____________________________________________
//
// KQDataRecord.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 2/18/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class stores information for a single bolometer event needed 
//for data evaluation with KQDataEvaluator
//
// It stores the Q value, recoil, heat and ion energy and
// channel uncertainties on the heat and ion channels for the corresponding heat
// and ion energies, which are interpolated between the uncertainties on the
// baselines and calibrations of a 133Ba source at E=356 keV
//
// The class could possibly be removed by KHLABolometerRecord, if it supplied a
// comparator function to sort data records according the recoil energy
// However, by using KHLABolometerRecord there would be storage overhead
// as not all data is needed

#include "KQDataRecord.h"

ClassImp(KQDataRecord);

Bool_t operator<(const KQDataRecord& aFirstDataRecord,const KQDataRecord& aSecondDataRecord) {
  //implemented to order DataRecords according ascending fERecoil values
  return(aFirstDataRecord.fEnergyRecoil<aSecondDataRecord.fEnergyRecoil);
}

