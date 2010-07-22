/*
 *  include_all_sources.C
 *  EdwDataStructure
 *
 *  Created by Adam Cox on 4/8/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

//this currently doesn't work. but its supposed to work with ALCiC
//from the root cint by calling
//root[0] .L include_all_sources.C++
//
//but, whatever. this doesn't work for me. 
//

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//add classes here

//#include "EdwSystemRecordBase.cc"
//#pragma link C++ class EdwSystemRecordBase+;
//#include "EdwSubRecordBase.cc"
//#pragma link C++ class EdwSubRecordBase+;
//#include "EdwHLASubRecordBase.cc"
//#pragma link C++ class EdwHLASubRecordBase+;
//#include "EdwHLASystemRecordBase.cc"
//#pragma link C++ class EdwHLASystemRecordBase+;

//#include "EdwSingleBoloSubRecordBase.cc"
//#pragma link C++ class EdwSingleBoloSubRecordBase+;
//#include "EdwSambaSubRecordBase.cc"
///#pragma link C++ class EdwSambaSubRecordBase+;
//#include "EdwBoloPulseSubRecordBase.cc"
//#pragma link C++ class EdwBoloPulseSubRecordBase+;
//#include "EdwMuonModuleSubRecordBase.cc"
//
#pragma link C++ class EdwMuonModuleSubRecordBase+;

#include "EdwHLASingleBoloSubRecord.cc"
#pragma link C++ class EdwHLASingleBoloSubRecord+;
#include "EdwHLASambaSubRecord.cc"
#pragma link C++ class EdwHLASambaSubRecord+;
#include "EdwHLABoloPulseSubRecord.cc"
#pragma link C++ class EdwHLABoloPulseSubRecord+;
#include "EdwHLAMuonModuleSubRecord.cc"
#pragma link C++ class EdwHLAMuonModuleSubRecord+;

#include "EdwBoloSystemRecordBase.cc"
#pragma link C++ class EdwBoloSystemRecordBase+;
#include "EdwMuonVetoSystemRecordBase.cc"
#pragma link C++ class EdwMuonVetoSystemRecordBase+;
#include "EdwHLABoloSysRecord.cc"
#pragma link C++ class EdwHLABoloSysRecord+;
#include "EdwHLAMuonVetoSysRecord.cc"
#pragma link C++ class EdwHLAMuonVetoSysRecord+;

#include "EdwEventBase.cc"
#pragma link C++ class EdwEventBase+;
#include "EdwRawEvent.cc"
#pragma link C++ class EdwRawEvent+;
#include "EdwHLAEvent.cc"
#pragma link C++ class EdwHLAEvent+;


//linkdef
//#include "EdwEvent_LinkDef.h"

//include executables. executibles can't have a "main", however.
#include "fillEvents.cc"