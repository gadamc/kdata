{
  gSystem->Load("../lib/libEdwDS.so");
  TFile f("new.root","recreate");
  TTree *t = new TTree("EdwT","EDS Tree");
  EdwHLAEvent *e = new EdwHLAEvent();
  TBranch *branch = t->Branch("EdwEventBranch","EdwHLAEvent",&e,16000, 99);
  t->SetAutoSave(100000000); // autosave when 0.1 Gbyte written
  t->SetCacheSize(10000000);    
  branch->SetAutoDelete(kFALSE);
  for(Int_t i = 0; i < 10000000; i++){
    e->Clear("C");
    //e->AddMuonModule();
    //e->AddBoloPulse();
    //e->AddSamba();
    //e->AddBolo();
    e->SetRunNumber(i);
    t->Fill();

  }

}
