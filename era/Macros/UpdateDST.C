
int UpdateDST(string bolo, string dstfile, string eionntpfile) {
  
  Float_t epsilon=3;
  
  string simplentp=(dstfile.substr(0,dstfile.length()-5))+"_simple.root";
  system(("cp "+simplentp+" "+dstfile).c_str());
  
  TFile f(dstfile.c_str(),"UPDATE");
  TTree* dst = (TTree*)f.Get("dsttree");
  Int_t nbevts=dst->GetEntries();  
  
  Float_t Erec=0, Eheat=0, Q=0, Eion=0, Eluke=0;
  Int_t ionflags[6];
  Int_t EventFlag=0, Vflag_dst=0;
  dst->Branch("Erec",&Erec,"Erec/F");
  dst->Branch("Q",&Q,"Q/F");
  dst->Branch("Vflag",&Vflag_dst,"Vflag/I");
  dst->SetBranchAddress("IonFlags",ionflags);
  dst->SetBranchAddress("Eheat",&Eheat);
  dst->SetBranchAddress("Eion",&Eion);
  dst->SetBranchAddress("EventFlag",&EventFlag);
  
  TFile f1(eionntpfile.c_str(),"READ");
  TTree* EionTree = (TTree*)f1.Get("energytree");
  Float_t Ecol1=0, Ecol2=0, Evet1=0, Evet2=0, Egar1=0, Egar2=0;
  Int_t Vflag=1;
  if (EionTree->GetEntries() != nbevts) cout << "Error nbevts" << endl;
  EionTree->SetBranchAddress("PolarFlag",&Vflag);
  EionTree->SetBranchAddress("Ecol1",&Ecol1); 
  if (!IsID(bolo)) {
    EionTree->SetBranchAddress("Egar1",&Egar1); 
  } else {
    EionTree->SetBranchAddress("Ecol2",&Ecol2);
    EionTree->SetBranchAddress("Evet1",&Evet1); 
    EionTree->SetBranchAddress("Evet2",&Evet2);
    if (bolo.substr(0,2) == "ID") { // ID vs FID 
      EionTree->SetBranchAddress("Egar1",&Egar1); 
      EionTree->SetBranchAddress("Egar2",&Egar2);
    }
  }
  
  for (Int_t i=0; i<nbevts; i++) {
    dst->GetEntry(i);
    EionTree->GetEntry(i);
    Vflag_dst=Vflag;
    if (EventFlag <= 1 || Vflag < 0 ) Eluke=0;
    else if (EventFlag == 2) { // fiducial event : optimise!
      Eluke = gVolts[Vflag]*Eion/epsilon;
    } else { // pour l'instant c'est comme ca: pas optimise!
      Eluke = fabs(gVref_Col1[Vflag]*Ecol1)*ionflags[0]+
      fabs(gVref_Col2[Vflag]*Ecol2)*ionflags[1]+
      fabs(gVref_Vet1[Vflag]*Evet1)*ionflags[2]+
      fabs(gVref_Vet2[Vflag]*Evet2)*ionflags[3]+
      fabs(gVref_Gar1[Vflag]*Egar1)*ionflags[4]+
      fabs(gVref_Gar2[Vflag]*Egar2)*ionflags[5];
      Eluke /= epsilon;
    }
    if (EventFlag == 0 || Vflag < 0) {
      Erec=0; Q=0;
    } else {
      Erec = Eheat*(1+gVolts[Vflag]/epsilon) - Eluke;
      if (Erec != 0) Q = Eion/Erec;
      else Q=0;
    }
    
    dst->GetBranch("Erec")->Fill();
    dst->GetBranch("Q")->Fill();
    dst->GetBranch("Vflag")->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i <<"/"<<nbevts<< endl;}
  }
  Int_t optionecriture=TObject::kOverwrite; // root
  f.cd(); dst->Write(0,optionecriture);
  f.Close();
  
  return 0;
}
