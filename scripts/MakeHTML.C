{
  gSystem.Load("$ROOTSYS/lib/libMinuit.so");
  gSystem.Load("$ROOTSYS/lib/libHtml.so");
  gSystem.Load("libKData.so");
  
  THtml h;
  h.SetProductName("KData");
  h.SetSourceDir("./kds"); //list all module directories like this "./kds:./kpsa:./ktemplate"
  h->MakeAll();
}
