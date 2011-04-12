
vector<Float_t> GetLDB(string channel, string qualdir, Int_t nblines=0) {
  
  string file=qualdir+"/baseline_"+channel+".txt";
  ifstream buf(file.c_str(),ios::in);
  ULong_t ti, tf; Float_t base; vector<Float_t> ldb;
  while (buf >> ti >> tf >> base) ldb.push_back(base);
  buf.close();
  if (nblines !=0 && ldb.size() != nblines) {
    cout << "Wrong number of ldbs for "<<channel<<endl;
    cout << nblines<<" "<<ldb.size()<<endl;
    exit(0);
  }
  
  return ldb;
}

int UpdateIonEnergy(TChain* chain, string bolo, string eionntp, string qualdir) {
  
  ULong64_t date=0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("DateSec",&date);
  chain->SetBranchStatus("DateSec",1);
  Int_t nbevts=chain->GetEntries();
  
  vector<ULong_t> ti,tf; ULong_t a,b;
  string file=qualdir+"/period_list.txt";
  ifstream buf(file.c_str(),ios::in);
  while (buf >> a >> b) {
    ti.push_back(a); tf.push_back(b);
  }
  buf.close();
  int nbperiods=ti.size();
  
  TFile f(eionntp.c_str(),"UPDATE");
  TTree* Eion = (TTree*)f.Get("energytree");
  if (Eion->GetEntries() != nbevts) cout << "Pbl tree size" << endl;
  vector<Float_t> v_ldbcol1, v_ldbcol2, v_ldbvet1, v_ldbvet2, v_ldbgar1, v_ldbgar2;
  Float_t ldbcol1, ldbcol2, ldbvet1, ldbvet2, ldbgar1, ldbgar2;
  Int_t boloflag=0;
  if (bolo.substr(0,2) == "ID") {
    v_ldbcol1=GetLDB(ChannelName(bolo,"col1"),qualdir,nbperiods);
    Eion->Branch("LdbCol1",&ldbcol1,"LdbCol1/F"); ldbcol1=v_ldbcol1[0];
    v_ldbcol2=GetLDB(ChannelName(bolo,"col2"),qualdir,nbperiods);
    Eion->Branch("LdbCol2",&ldbcol2,"LdbCol2/F"); ldbcol2=v_ldbcol2[0];
    v_ldbvet1=GetLDB(ChannelName(bolo,"vet1"),qualdir,nbperiods);
    Eion->Branch("LdbVet1",&ldbvet1,"LdbVet1/F"); ldbvet1=v_ldbvet1[0];
    v_ldbvet2=GetLDB(ChannelName(bolo,"vet2"),qualdir,nbperiods);
    Eion->Branch("LdbVet2",&ldbvet2,"LdbVet2/F"); ldbvet2=v_ldbvet2[0];
    v_ldbgar1=GetLDB(ChannelName(bolo,"gar1"),qualdir,nbperiods);
    Eion->Branch("LdbGar1",&ldbgar1,"LdbGar1/F"); ldbgar1=v_ldbgar1[0];
    v_ldbgar2=GetLDB(ChannelName(bolo,"gar2"),qualdir,nbperiods);
    Eion->Branch("LdbGar2",&ldbgar2,"LdbGar2/F"); ldbgar2=v_ldbgar2[0];
  } else if (bolo.substr(0,3) == "FID") { // FID
    boloflag=1;
    v_ldbcol1=GetLDB(ChannelName(bolo,"col1"),qualdir,nbperiods);
    Eion->Branch("LdbCol1",&ldbcol1,"LdbCol1/F"); ldbcol1=v_ldbcol1[0];
    v_ldbcol2=GetLDB(ChannelName(bolo,"col2"),qualdir,nbperiods);
    Eion->Branch("LdbCol2",&ldbcol2,"LdbCol2/F"); ldbcol2=v_ldbcol2[0];
    v_ldbvet1=GetLDB(ChannelName(bolo,"vet1"),qualdir,nbperiods);
    Eion->Branch("LdbVet1",&ldbvet1,"LdbVet1/F"); ldbvet1=v_ldbvet1[0];
    v_ldbvet2=GetLDB(ChannelName(bolo,"vet2"),qualdir,nbperiods);
    Eion->Branch("LdbVet2",&ldbvet2,"LdbVet2/F"); ldbvet2=v_ldbvet2[0];
  } else { // Classic GeNTD
    boloflag=2;
    v_ldbcol1=GetLDB(ChannelName(bolo,"col1"),qualdir,nbperiods);
    Eion->Branch("LdbCol1",&ldbcol1,"LdbCol1/F"); ldbcol1=v_ldbcol1[0];
    v_ldbgar1=GetLDB(ChannelName(bolo,"gar1"),qualdir,nbperiods);
    Eion->Branch("LdbGar1",&ldbgar1,"LdbGar1/F"); ldbgar1=v_ldbgar1[0];
  }
  
  ULong_t ti_loc=ti[0], tf_loc=tf[0];
  Int_t p=0;
  for (Int_t i=0; i<nbevts; i++) {
    chain->GetEntry(i); Eion->GetEntry(i);
    if (date > tf_loc || date < ti_loc) { // met a jour les ldbs
      if (date < ti_loc) p--;
      if (date > tf_loc) p++;
      ti_loc=ti[p];
      tf_loc=tf[p];
      if (date > tf_loc || date < ti_loc) 
        cout << "Event timing error.. "<<date<<" "<<ti_loc<<" "<<tf_loc<<endl;
      ldbcol1=v_ldbcol1[p];
      if (boloflag == 2) {
        ldbgar1=v_ldbgar1[p];
      } else {
        ldbcol2=v_ldbcol2[p];
        ldbvet1=v_ldbvet1[p];
        ldbvet2=v_ldbvet2[p];
        if (boloflag == 0) {
          ldbgar1=v_ldbgar1[p];
          ldbgar2=v_ldbgar2[p];
        }
      }
    }
    
    Eion->GetBranch("LdbCol1")->Fill();
    if (boloflag == 2) {
      Eion->GetBranch("LdbGar1")->Fill();
    } else {
      Eion->GetBranch("LdbCol2")->Fill();
      Eion->GetBranch("LdbVet1")->Fill();
      Eion->GetBranch("LdbVet2")->Fill();
      if (boloflag == 0) {
        Eion->GetBranch("LdbGar1")->Fill();
        Eion->GetBranch("LdbGar2")->Fill();
      }
    }
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  //  Int_t optionecriture=TObject::kWriteDelete; // root
  Int_t optionecriture=TObject::kOverwrite; // root
  f.cd(); Eion->Write(0,optionecriture);
  f.Close();
  chain->SetBranchStatus("*",0);
  
  string simplentp=(eionntp.substr(0,eionntp.length()-5))+"_simple.root";
  system(("cp "+eionntp+" "+simplentp).c_str());
  return 0;
}


int UpdateHeatEnergy(TChain* chain, string bolo, string eheatntp, string qualdir) {
  
  ULong64_t date=0;
  chain->SetBranchStatus("*",0);
  chain->SetBranchAddress("DateSec",&date);
  chain->SetBranchStatus("DateSec",1);
  Int_t nbevts=chain->GetEntries();
  
  vector<ULong_t> ti,tf; ULong_t a,b;
  string file=qualdir+"/period_list.txt";
  ifstream buf(file.c_str(),ios::in);
  while (buf >> a >> b) {
    ti.push_back(a); tf.push_back(b);
  }
  buf.close();
  int nbperiods=ti.size();
  
  TFile f(eheatntp.c_str(),"UPDATE");
  TTree* Eheat = (TTree*)f.Get("energytree");
  if (Eheat->GetEntries() != nbevts) cout << "Pbl tree size" << endl;
  
  vector<Float_t> v_ldbheat=GetLDB(ChannelName(bolo,"heat"),qualdir,nbperiods);
  Float_t ldbheat=v_ldbheat[0];
  Eheat->Branch("LdbHeat",&ldbheat,"LdbHeat/F");
  
  ULong_t ti_loc=ti[0], tf_loc=tf[0];
  Int_t p=0;
  for (Int_t i=0; i<nbevts; i++) {
    chain->GetEntry(i); Eheat->GetEntry(i);
    if (date > tf_loc || date < ti_loc) { // met a jour les ldbs
      if (date < ti_loc) p--;
      if (date > tf_loc) p++;
      ti_loc=ti[p];
      tf_loc=tf[p];
      if (date > tf_loc || date < ti_loc) 
        cout << "Event timing error.. "<<date<<" "<<ti_loc<<" "<<tf_loc<<endl;
      ldbheat=v_ldbheat[p];
    }
    
    Eheat->GetBranch("LdbHeat")->Fill();
    if (fmod((double)i,(double)100000) == 0) {cout << i << endl;}
  }
  
  //  Int_t optionecriture=TObject::kWriteDelete; // root
  Int_t optionecriture=TObject::kOverwrite;
  f.cd(); Eheat->Write(0,optionecriture);
  f.Close();
  chain->SetBranchStatus("*",0);
  return 0;
}
