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
  h.SetHeader("scripts/kdata_header.html");
  h.SetProductName("KData");
  h.SetHomepage("https://edwdev-ik.fzk.de");
  h.SetSourceDir("$KDATA_ROOT/kds:$KDATA_ROOT/kera:$KDATA_ROOT/era:$KDATA_ROOT/kpta:$KDATA_ROOT/kdsScripts:$KDATA_ROOT/kqpa:$KDATA_ROOT/ksamba:$KDATA_ROOT/kdatabase"); //list all module directories like this "./kds:./kpsa:./ktemplate"
  h.SetOutputDir("$KDATA_ROOT/html");
  h.MakeAll(kTRUE);
  ofstream afile("html/ROOT.css", fstream::in | fstream::out | fstream::ate);
  afile << 
  "#kdata_header {" << endl <<
  "   margin:0px -13px;" << endl <<
  "}" << endl <<
  "#kdata_header img {" << endl <<
  "  margin: 10px;" << endl <<
  "}" << endl;
  gSystem.Exec("cp scripts/kdata_header.html html/kdata_header.html");
  gSystem.Exec("cp scripts/edelweiss-logo.jpeg html/edelweiss-logo.jpeg");
}

