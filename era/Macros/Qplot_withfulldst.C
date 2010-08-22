
int Qplot(string bolo, string qualdir, TTree* DST, TTree* Cuts, TTree* Eheat, TTree* Eion, Int_t voltflag=-1, Int_t evtclassflag=2, Bool_t cut_badevts=0, Float_t sigmacut_heat, Float_t sigmacut_ion, Float_t sigmacut_gamma, Float_t sigmacut_quasifid, Bool_t dobinnedplot=0, Bool_t print_wimps=0, Bool_t dohisto_nr=0) {

  Float_t epsilon=3;
  Float_t a=0.16, b=0.18; // NR band parameters
  Float_t k1=sigmacut_gamma/2.355; // gamma band limit
  Float_t k2=1.645/2.355; // wimp band limit
  Float_t k3=sigmacut_quasifid/2.355; // quasifid band limit
  Float_t c=0.035;

  Int_t nb = DST->GetEntries();
  if (nb != Cuts->GetEntries() || nb != Eheat->GetEntries() ||
      nb != Eion->GetEntries()) cout << "pbl taille trees" <<endl;
  
  Int_t Vflag, EventFlag;
  Float_t Erec,Q;
  DST->SetBranchAddress("Vflag",&Vflag);
  DST->SetBranchAddress("Erec",&Erec);
  DST->SetBranchAddress("Q",&Q);
  DST->SetBranchAddress("EventFlag",&EventFlag);

  Float_t ldbheat=0, eheat=0;
  Float_t ldbfid=0; Float_t esidecut=0;
  Eheat->SetBranchAddress("Eheat",&eheat);
  Eheat->SetBranchAddress("LdbHeat",&ldbheat);
  Eion->SetBranchAddress("LdbFid",&ldbfid);
  Eion->SetBranchAddress("EsideCut",&esidecut);
  Bool_t cutbases=1,cutrun=1; Int_t cutchi2=1;
  Cuts->SetBranchAddress("CutBases",&cutbases);
  Cuts->SetBranchAddress("CutRun",&cutrun);
  //  Cuts->SetBranchAddress("CutChi2",&cutchi2);
  DST->SetBranchAddress("Chi2Flag",&cutchi2); // plus complet normalement (evts non fiduciels)

  TF1* ion_thresh = new TF1("it","[0]/x",0.1,500);
  ion_thresh->SetParameter(0,1); // Seuil ion = ldb*sigmacut_ion
  TF1* heat_thresh = new TF1("ht","-[0]+(1+[0])*[1]/x",0.1,500);
  heat_thresh->SetParameter(0,1); // param = epsilon/Volt
  heat_thresh->SetParameter(1,1); // Seuil chal = ldb*sigmacut_heat

  TF1* qmean_gamma = new TF1("qg","1",0.1,500);
  TF1* qdown_gamma = new TF1("qd_g","1-[0]*(1+[1])*sqrt([2]*[2]+[3]*[3]+x*x*([4]+[5]))/x",0,500);
  qdown_gamma->SetParameter(0,k1);
  qdown_gamma->SetParameter(1,1); // param = volt/epsilon
  qdown_gamma->SetParameter(2,1); // LDB ion
  qdown_gamma->SetParameter(3,1); // LDB chal
  qdown_gamma->SetParameter(4,gKappaIon);
  qdown_gamma->SetParameter(5,gKappaHeat);

  TF1* qmean_nr = new TF1("qnr","[0]*pow(x,[1])",0,500);
  qmean_nr->SetParameter(0,a);
  qmean_nr->SetParameter(1,b);
  TF1* qup_nr = new TF1("qu_rn","[0]*pow(x,[1])+[2]*sqrt((1+[3])*(1+[3])*([4]*[4]+[7]*x*x)*[0]*pow(x,[1])*[0]*pow(x,[1])+(1+[3]*[0]*pow(x,[1]))*(1+[3]*[0]*pow(x,[1]))*([5]*[5]+[8]*x*x)+pow([6]*x,2))/x",0,500);
  qup_nr->SetParameter(0,a);
  qup_nr->SetParameter(1,b);
  qup_nr->SetParameter(2,k2);
  qup_nr->SetParameter(3,1); // param = volt/epsilon
  qup_nr->SetParameter(4,1); // LDB heat
  qup_nr->SetParameter(5,1); // LDB ion
  qup_nr->SetParameter(6,c);
  qup_nr->SetParameter(7,gKappaHeat);
  qup_nr->SetParameter(8,gKappaIon);
  TF1* qdown_nr = new TF1("qd_rn","[0]*pow(x,[1])-[2]*sqrt((1+[3])*(1+[3])*([4]*[4]+[7]*x*x)*[0]*pow(x,[1])*[0]*pow(x,[1])+(1+[3]*[0]*pow(x,[1]))*(1+[3]*[0]*pow(x,[1]))*([5]*[5]+[8]*x*x)+pow([6]*x,2))/x",0,500);
  qdown_nr->SetParameter(0,a);
  qdown_nr->SetParameter(1,b);
  qdown_nr->SetParameter(2,k2);
  qdown_nr->SetParameter(3,1); // param = volt/epsilon
  qdown_nr->SetParameter(4,1); // LDB heat
  qdown_nr->SetParameter(5,1); // LDB ion
  qdown_nr->SetParameter(6,c);
  qdown_nr->SetParameter(7,gKappaHeat);
  qdown_nr->SetParameter(8,gKappaIon);

  // quasi-fiducial evts with a given Eside
  TF1* qmean_qf = new TF1("qfid","(x-[0])/(x+[1]*[0])",0,500); 
  TF1* qdown_qf = new TF1("qd_fid","(x-[0])/(x+[1]*[0])-[2]*(1+[1])*sqrt((x-[0])*(x-[0])*([3]*[3]+[5]*x*x)+([4]*[4]+[6]*x*x)*x*x)/pow(x,2)",0,500);
  bool doquasifid=0;
  if (sigmacut_quasifid > 0) {
    doquasifid=1;
    qmean_qf->SetParameter(0,2); // Eside
    qmean_qf->SetParameter(1,1); // param = volt/epsilon
    qdown_qf->SetParameter(0,2); // Eside
    qdown_qf->SetParameter(1,1); // param = volt/epsilon
    qdown_qf->SetParameter(2,k3);
    qdown_qf->SetParameter(3,1); // LDB heat
    qdown_qf->SetParameter(4,1); // LDB ion
    qdown_qf->SetParameter(5,gKappaHeat);
    qdown_qf->SetParameter(6,gKappaIon);
  }

  vector<Int_t> badevts;
  if (cut_badevts) {
    int numero;
    ifstream badbuf((qualdir+"/badevts_"+bolo+".txt").c_str(),ios::in);
    if (badbuf) {
      while (badbuf >> numero) badevts.push_back(numero);
      cout << badevts.size() << " events rejected by hand..." << endl;
    }
  }

  Float_t* ArrayErec = new Float_t[nb];
  Float_t* ArrayQ = new Float_t[nb];
  Bool_t* IsNR = new Bool_t[nb];
  Bool_t* IsGamma = new Bool_t[nb];
  Bool_t* IsQuasiFid = new Bool_t[nb];

  Int_t compteur=0;
  Float_t ldbh_prev=0, ldbi_prev=0, esidecut_prev=0; Int_t vflag_prev=-1;
  Float_t nbpts_ldbs=0, ldbh_mean=0, ldbi_mean=0, esidecut_mean=0;
  for (Int_t i=0; i<nb; i++) {

    Eion->GetEntry(i); Eheat->GetEntry(i);
    Cuts->GetEntry(i); DST->GetEntry(i);

    // A) Calcul de l'evt: est-ce qu'on le garde dans le Qplot?
    Bool_t docut=0;
    // Categorie d'evt
    if (evtclassflag != -1 && evtclassflag != EventFlag) docut=1;
    // Quality cut
    if (!cutrun || !cutbases || !cutchi2) docut=1;
    // Polar
    if (voltflag != -1) {
      if (voltflag != Vflag) docut=1;
    } else {
      if (Vflag == -1) docut=1;
    }
    // Evt retire a la main
    if (badevts.size() != 0) {
      for (int p=0; p<badevts.size(); p++) {
	if (i == badevts[p]) docut=1;
      }
    }
    // Seuil chaleur et ionisation (?)(redondant avec classification evts...)
    if (eheat < ldbheat*sigmacut_heat) docut=1;
    if (EventFlag < 2) docut=1; // pas de Qplot des chaleurs seule!!!
    // Mais ca va donc bcp dependre du cut fait dans le calcul EventFlag.
    if (docut) {
      ArrayErec[i]=0; ArrayQ[i]=0;
    } else {
      ArrayErec[i]=Erec; ArrayQ[i]=Q;
    }

    // B) Calcul des seuils/categories.
    // 1) D'abord mise a jour des valeurs des params de coupure si necessaire seulement
    if (ldbheat != ldbh_prev || ldbfid != ldbi_prev || esidecut != esidecut_prev) {
      qdown_gamma->SetParameter(2,ldbfid);
      qdown_gamma->SetParameter(3,ldbheat);
      qup_nr->SetParameter(4,ldbheat);
      qup_nr->SetParameter(5,ldbfid);
      qdown_nr->SetParameter(4,ldbheat);
      qdown_nr->SetParameter(5,ldbfid);
      if (doquasifid) {
	qdown_qf->SetParameter(0,esidecut);
	qdown_qf->SetParameter(3,ldbheat);
	qdown_qf->SetParameter(4,ldbfid);
      }
      ldbh_prev=ldbheat; ldbi_prev=ldbfid; esidecut_prev=esidecut;
      if (!docut) { // ce truc est un peu foireux. mieux vadra calculer
	// la moyenne associee par ailleurs (en meme temps que le 
	// "monitoring" des ldbs..)
	ldbh_mean += ldbheat; ldbi_mean += ldbfid; nbpts_ldbs+=1;
	esidecut_mean += esidecut;
      }
    }
    if (Vflag != vflag_prev && Vflag != -1) {
      heat_thresh->SetParameter(0,epsilon/gVolts[Vflag]);
      qdown_gamma->SetParameter(1,gVolts[Vflag]/epsilon);
      qup_nr->SetParameter(3,gVolts[Vflag]/epsilon);
      qdown_nr->SetParameter(3,gVolts[Vflag]/epsilon);
      if (doquasifid) {
	qmean_qf->SetParameter(1,gVolts[Vflag]/epsilon);
	qdown_qf->SetParameter(1,gVolts[Vflag]/epsilon);
      }
    }
    // 2) Calcul categorie d'evt
    IsNR[i]=0;
    IsGamma[i]=0;
    IsQuasiFid[i]=0;
    if (ArrayQ[i] != 0) {
      if (ArrayQ[i] < qup_nr->Eval(ArrayErec[i]) && ArrayQ[i] > qdown_nr->Eval(ArrayErec[i]))
	IsNR[i]=1;
      if (ArrayQ[i] > qdown_gamma->Eval(ArrayErec[i])) IsGamma[i]=1;
      if (doquasifid && ArrayQ[i] > qdown_qf->Eval(ArrayErec[i])) IsQuasiFid[i]=1;
    }

    // SELECTION A LA MAIN
    if (ArrayQ[i] > 0.5 && ArrayQ[i]<1.5 && ArrayErec[i]>10 && ArrayErec[i]<1000) compteur++;
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }
  cout << "total evts in hand-selected region="<<compteur<<endl;

  if (print_wimps) {
    char therun[9]; Int_t evtnum=0;
    DST->SetBranchAddress("RunName",&therun);
    DST->SetBranchAddress("EventNum",&evtnum);
    cout <<"** Leaking events in hand-selected nucl band:"<<endl;
    for (Int_t i=0; i<nb; i++) {
      // SELECTION A LA MAIN
      if ((ArrayQ[i]<0.5 && ArrayQ[i]>0.1 && ArrayErec[i]>100 && ArrayErec[i]<400) ||
	  (ArrayQ[i]<0.45 && ArrayQ[i]>0.05 && ArrayErec[i]>10 && ArrayErec[i]<100)) {
	DST->GetEntry(i);
	cout<<"Evt "<<i<<" (Run "<<therun<<" samba evt "<<evtnum<<")"<<endl;
	cout << " -- Q="<<ArrayQ[i]<<" - Erec="<<ArrayErec[i]<<endl;
      }
    }
    cout << "**"<<endl;
  }

  ldbh_mean/=nbpts_ldbs;
  ldbi_mean/=nbpts_ldbs;
  esidecut_mean/=nbpts_ldbs;

  ion_thresh->SetParameter(0,ldbi_mean*sigmacut_ion);
  heat_thresh->SetParameter(1,ldbh_mean*sigmacut_heat);
  qdown_gamma->SetParameter(2,ldbi_mean);
  qdown_gamma->SetParameter(3,ldbh_mean);
  qup_nr->SetParameter(4,ldbh_mean);
  qup_nr->SetParameter(5,ldbi_mean);
  qdown_nr->SetParameter(4,ldbh_mean);
  qdown_nr->SetParameter(5,ldbi_mean);
  if (doquasifid) {
    qmean_qf->SetParameter(0,esidecut_mean);
    qdown_qf->SetParameter(0,esidecut_mean);
    qdown_qf->SetParameter(3,ldbh_mean);
    qdown_qf->SetParameter(4,ldbi_mean);
  }

  TH1F* histowimps = new TH1F("NR spectrum","NR spectrum",100,0,200);

  if (dobinnedplot) {
    TCanvas* c2 = new TCanvas("qhist","qhist");
    TH2F* h2 = new TH2F("h2","Binned Qplot",100,0.001,400,50,0,2);
    for (Int_t i=0; i<nb; i++) h2->Fill(ArrayErec[i],ArrayQ[i]);
    gStyle->SetPalette(1);
    h2->Draw("COLZ"); // a ameliorer mais c'est l'esprit
  } else {
    TCanvas* c1 = new TCanvas("qplot","qplot");
    TGraph* gr = new TGraph(nb,ArrayErec,ArrayQ);
    gr->SetTitle(bolo.c_str());  
    gr->GetXaxis()->SetRangeUser(0,400);
    gr->GetYaxis()->SetRangeUser(0,2);
    gr->GetXaxis()->SetTitle("Recoil energy [keV]");
    gr->GetYaxis()->SetTitle("Ionization yield");
    gr->SetMarkerStyle(7);
    gr->Draw("AP");
    Float_t* QG = new Float_t[nb];
    Float_t* QF = new Float_t[nb];
    for (int i=0; i<nb; i++) {
      if (IsGamma[i] == 0 && IsNR[i] == 1) QG[i]=ArrayQ[i];
      else QG[i]=-1;
      if (IsQuasiFid[i] == 0 && IsNR[i] == 1) {
	QF[i]=ArrayQ[i];
	histowimps->Fill(ArrayErec[i]);
      } else QF[i]=-1;
    }
    if (doquasifid) {
      TGraph* gr2 = new TGraph(nb,ArrayErec,QG); // "Points marrons"
      gr2->SetMarkerStyle(8); gr2->SetMarkerColor(28);
      //    if (cutvetoflag == 0) gr2->SetMarkerSize(0.5);
      gr2->Draw("sameP");
    }
    TGraph* gr3 = new TGraph(nb,ArrayErec,QF); // "Points rouges"
    gr3->SetMarkerStyle(8); gr3->SetMarkerColor(2);
    //    if (cutvetoflag == 0) gr3->SetMarkerSize(0.5);
    gr3->Draw("sameP");
  }
  
  ion_thresh->SetLineWidth(2); ion_thresh->SetLineColor(8);
  heat_thresh->SetLineWidth(2); heat_thresh->SetLineColor(2);
  qmean_gamma->SetLineWidth(2); qmean_gamma->SetLineColor(4);
  qdown_gamma->SetLineWidth(2); qdown_gamma->SetLineColor(4); qdown_gamma->SetLineStyle(2);
  qmean_nr->SetLineWidth(2); qmean_nr->SetLineColor(2);
  qup_nr->SetLineWidth(2); qup_nr->SetLineColor(2); qup_nr->SetLineStyle(2);
  qdown_nr->SetLineWidth(2); qdown_nr->SetLineColor(2); qdown_nr->SetLineStyle(2);
  if (doquasifid) {
    qmean_qf->SetLineWidth(2); qmean_qf->SetLineColor(28);
    qdown_qf->SetLineWidth(2); qdown_qf->SetLineColor(28); qdown_qf->SetLineStyle(2);
  }

  ion_thresh->Draw("same");
  heat_thresh->Draw("same");
  qmean_gamma->Draw("same");
  qdown_gamma->Draw("same");
  qmean_nr->Draw("same");
  qup_nr->Draw("same");
  qdown_nr->Draw("same");
  if (doquasifid) {
    qmean_qf->Draw("same");
    qdown_qf->Draw("same");
  }

  if (dohisto_nr) {
    TCanvas* nrs = new TCanvas("nrs","nrs");
    histowimps->Draw();
    ifstream buf((qualdir+"/acceptance_"+bolo+".txt").c_str(),ios::in);
    vector<Float_t> er, accept; Float_t aa,bb;
    while (!buf.eof()) {
      buf >> aa >> bb; er.push_back(aa); accept.push_back(bb);
    }
    buf.close();
    Int_t p=0;
    while (accept[p] < 0.98) p++;    
    Int_t nbpts_accept = er.size();
    TF1* fitexpo = new TF1("fitexpo","[0]*exp(-[1]*x)",er[p],er[nbpts_accept-1]);
    fitexpo->SetLineStyle(2);
    histowimps->Fit("fitexpo","MR","same");
    Float_t* xx = new Float_t[nbpts_accept];
    Float_t* yy = new Float_t[nbpts_accept];
    fitexpo->SetRange(0,200);
    for (int k=0; k<nbpts_accept; k++) {
      xx[k]=er[k];
      yy[k]=accept[k]*fitexpo->Eval(er[k]);
    }
    TGraph* gracc = new TGraph(nbpts_accept,xx,yy);
    gracc->SetLineColor(2);
    //    TCanvas* toto  =new TCanvas("tt","TT");
    //    gracc->Draw("same");
    gracc->Draw("sameL");
  } else delete histowimps;

  /*  TF1* ionseuil=  new TF1("ionseuil","[0]/x",0.001,400);
  ionseuil->SetParameter(0,Ei_seuil);
  ionseuil->SetLineColor(4);
  ionseuil->Draw("same");
  TF1* heatseuil = new TF1("heatseuil","-[1]+(1+[1])*[0]/x",0.001,400);
  heatseuil->SetParameter(0,Ec_seuil);
  heatseuil->SetParameter(1,epsilon/voltage);
  heatseuil->SetLineColor(2);
  heatseuil->Draw("same"); */

  /*
  if (dumpfile != "") {
    // A mixer avec les DSTs...
    ofstream buf2(dumpfile.c_str(),ios::out);
    Int_t nbpts_dump=0;
    ULong64_t date=0;
    chain->SetBranchStatus("*",0);
    chain->SetBranchAddress("DateSec",&date);
    chain->SetBranchStatus("DateSec",1);
    for (Int_t i=0; i<nb; i++) {
      if (Q[i] != 0) {
	chain->GetEntry(i);
	buf2 << ArrayErec[i]<<" "<<ArrayQ[i]<<" "<<IsNR[i]<<" "<<IsGamma[i]
	     <<" "<<date<<" "<<1<<endl;
	// Last flag = 1 : used to be able to put it to zero by hand in case of coincidence..
	// (for latter..)
	nbpts_dump++;
      }
    }
    buf2.close();
    cout << "Ascii file filled, "<<nbpts_dump<<" evts stored."<<endl;
    chain->SetBranchStatus("*",1);
  }
  */

  delete[] ArrayErec; delete[] ArrayQ; delete[] IsNR; delete[] IsQuasiFid; delete[] IsGamma;
  return 0;
}
