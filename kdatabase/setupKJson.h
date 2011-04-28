{
  gInterpreter->AddIncludePath("$KDATA_ROOT/include");
  gSystem->Load("libkdatabase");
  gROOT->ProcessLine(".L $KDATA_ROOT/include/KJson.h+");
  
}