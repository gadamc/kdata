//_____________________________________________
//
// example.C
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This is an example to show the use of KQDistribution
// 

{
  gSystem->Load("$KDATA_ROOT/lib/libkqpa.so"); //loading library
  
 const Char_t* aSourceFile = "/kalinka/storage/edelweiss/Bolo/Run12/Eds/Final/Neutron/Kds_Run12_v3.0_skim.root"; // source file
  string aTargetDir = gSystem->pwd(); // current directory as target directory
  aTargetDir += "/Temp/"; // with subfolder "Temp"
  Int_t aNumProjections = 20; // number of projections
  Int_t anEventCategory = 2; // 0: noise 1: heat_only 2: fiducial 3: surface  4: pure_guard 5: other
  const Char_t* aDetectorName = "FID401";

  KQDistribution aQDist(aSourceFile,aTargetDir.c_str(),aNumProjections); //
  aQDist.MakeAll(anEventCategory,,aDetectorName,aNumProjections);
}