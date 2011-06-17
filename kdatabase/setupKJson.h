{
  gInterpreter->AddIncludePath("$KDATA_ROOT/kdatabase");
  gSystem->Load("libkdatabase");
  gSystem->Exec("cp $KDATA_ROOT/kdatabase/KJson.cxx $PWD");
  gROOT->ProcessLine(".L KJson.cxx++");
  
}