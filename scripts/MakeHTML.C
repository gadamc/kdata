{
  gSystem.Load("$ROOTSYS/lib/libMinuit.so");
  gSystem.Load("$ROOTSYS/lib/libHtml.so");
  gSystem.Load("$KDATA_ROOT/lib/libkds.so");
  gSystem.Load("$KDATA_ROOT/lib/libkera.so");
  gSystem.Load("$KDATA_ROOT/lib/libEra.so");
  gSystem.Load("$KDATA_ROOT/lib/libkpta.so");
  gSystem.Load("$KDATA_ROOT/lib/libkqpa.so");
  gSystem.Load("$KDATA_ROOT/lib/libksamba.so");
  gSystem.Load("$KDATA_ROOT/lib/libkdatabase.so");

  THtml h;
  h.SetProductName("KData");
  h.SetSourceDir("$KDATA_ROOT/kds:$KDATA_ROOT/kera:$KDATA_ROOT/era:$KDATA_ROOT/kpta:$KDATA_ROOT/kdsScripts:$KDATA_ROOT/kqpa:$KDATA_ROOT/ksamba:$KDATA_ROOT/kdatabase"); //list all module directories like this "./kds:./kpsa:./ktemplate"
  h.SetOutputDir("$KDATA_ROOT/html");
  h.MakeAll(kTRUE);
}
