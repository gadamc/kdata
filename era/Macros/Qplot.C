
int Qplot(string bolo, string qualdir, TTree* DSTRed, Int_t voltflag=-1, Int_t evtclassflag=2, Bool_t cut_badevts=0, Bool_t dobinnedplot=0, Bool_t print_wimps=0, Bool_t dohisto_nr=0, string outfile="") {

  Float_t epsilon=3;
  Float_t a=0.16, b=0.18; // NR band parameters
  Float_t k1=gSigmaCut_Gamma/2.355; // gamma band limit
  Float_t k2=gNRWidth/2.355; // wimp band limit
  Float_t c=0.035;
  Float_t sigmacut_heat=gSigmaCut_Heat;
  Float_t sigmacut_ion=gSigmaCut_Efid;
  Float_t sigmacut_gamma=gSigmaCut_Gamma;

  Float_t ldbcut_efid=gLdbCut_Efid;
  Float_t ldbcut_eheat=gLdbCut_Eheat; // figes ici pour l'instant
  Float_t ldbcut_ecol=gLdbCut_Ecol; // en plus de efid.. faut pas avoir une collectrice (trop) mal en point.
  Float_t ldbcut_gar=gLdbCut_Egar;
  Float_t ldbcut_evet=gLdbCut_Evet;

  Int_t nb = DSTRed->GetEntries();
  
  Int_t Vflag, EventFlag;
  Float_t Erec,Q;
  Float_t ldbheat=0, eheat=0;
  Float_t ldbfid=0, ldbcol1=0, ldbcol2=0, ldbvet1=0, ldbvet2=0;
  Float_t ldbgar1=0,ldbgar2=0,ldbgar=0;
  Int_t cutchi2=1;
  Bool_t isbolocoin=0;

  DSTRed->SetBranchAddress("IsBoloCoinc",&isbolocoin);
  DSTRed->SetBranchAddress("Vflag",&Vflag);
  DSTRed->SetBranchAddress("Erec",&Erec);
  DSTRed->SetBranchAddress("Q",&Q);
  DSTRed->SetBranchAddress("EventFlag",&EventFlag);
  DSTRed->SetBranchAddress("Eheat",&eheat);
  DSTRed->SetBranchAddress("LdbHeat",&ldbheat);
  DSTRed->SetBranchAddress("LdbFid",&ldbfid);
  DSTRed->SetBranchAddress("LdbCol1",&ldbcol1);
  DSTRed->SetBranchAddress("LdbCol2",&ldbcol2);
  DSTRed->SetBranchAddress("LdbVet1",&ldbvet1);
  DSTRed->SetBranchAddress("LdbVet2",&ldbvet2);
  DSTRed->SetBranchAddress("Chi2Flag",&cutchi2);
  if (!IsFID(bolo)) {
    DSTRed->SetBranchAddress("LdbGar1",&ldbgar1);
    DSTRed->SetBranchAddress("LdbGar2",&ldbgar2);
  }

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

  ofstream out; Bool_t print_ascii=0;
  if (outfile != "") {
    out.open(outfile.c_str(),ios::out);
    print_ascii=1;
  }

  Int_t compteur=0;
  Float_t ldbh_prev=0, ldbi_prev=0; Int_t vflag_prev=-1;
  Float_t nbpts_ldbs=0, ldbh_mean=0, ldbi_mean=0;
  for (Int_t i=0; i<nb; i++) {

    DSTRed->GetEntry(i);
    // A) Calcul de l'evt: est-ce qu'on le garde dans le Qplot?
    Bool_t docut=0;
    // Categorie d'evt
    if (evtclassflag != -1 && evtclassflag != EventFlag) docut=1;
    // Quality cut
    if (!IsFID(bolo)) {
      if (ldbgar1 < 0.01) ldbgar=ldbgar2;
      else if (ldbgar2 < 0.01) ldbgar=ldbgar1;
      else ldbgar=ldbgar1*ldbgar2/sqrt(ldbgar1*ldbgar1+ldbgar2*ldbgar2) ; // "ldb garde"
      if ( ldbgar > ldbcut_gar || ldbgar < 0.01) docut=1;
    }
    if (ldbheat > ldbcut_eheat || ldbfid > ldbcut_efid || !cutchi2) docut=1;
    if ( ldbcol1 > ldbcut_ecol || ldbcol2 > ldbcut_ecol ) { 
      // do the cut depending on the vetos!
      if (ldbvet1 > ldbcut_evet || ldbvet2 > ldbcut_evet || ldbvet1 < 0.01 || ldbvet2 < 0.01) 
	docut=1;
    }
    if (ldbheat < 0.01 || ldbcol1 < 0.01 || ldbcol2 < 0.01) docut=1;
    // Polar
    if (voltflag != -1) {
      if (voltflag != Vflag) docut=1;
    } else {
      if (Vflag == -1) docut=1;
    }
    // Coincidences
    if (isbolocoin == 1) docut=1;
    // Evt retire a la main
    if (badevts.size() != 0) {
      for (int p=0; p<badevts.size(); p++) {
	if (i == badevts[p]) docut=1;
      }
    }
    // Seuil chaleur et ionisation
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
    if (ldbheat != ldbh_prev || ldbfid != ldbi_prev) {
      qdown_gamma->SetParameter(2,ldbfid);
      qdown_gamma->SetParameter(3,ldbheat);
      qup_nr->SetParameter(4,ldbheat);
      qup_nr->SetParameter(5,ldbfid);
      qdown_nr->SetParameter(4,ldbheat);
      qdown_nr->SetParameter(5,ldbfid);
      ldbh_prev=ldbheat; ldbi_prev=ldbfid;
      if (!docut) { // ce truc est un peu foireux. mieux vadra calculer
	// la moyenne associee par ailleurs (en meme temps que le 
	// "monitoring" des ldbs..)
	ldbh_mean += ldbheat; ldbi_mean += ldbfid; nbpts_ldbs+=1;
      }
    }
    if (Vflag != vflag_prev && Vflag != -1) {
      heat_thresh->SetParameter(0,epsilon/gVolts[Vflag]);
      qdown_gamma->SetParameter(1,gVolts[Vflag]/epsilon);
      qup_nr->SetParameter(3,gVolts[Vflag]/epsilon);
      qdown_nr->SetParameter(3,gVolts[Vflag]/epsilon);
    }
    // 2) Calcul categorie d'evt
    IsNR[i]=0;
    if (ArrayQ[i] != 0) {
      if (ArrayQ[i] < qup_nr->Eval(ArrayErec[i]) && ArrayQ[i] > qdown_nr->Eval(ArrayErec[i]))
	IsNR[i]=1;
      if (ArrayQ[i] > qdown_gamma->Eval(ArrayErec[i])) IsNR[i]=0;
      if (print_ascii) out << ArrayErec[i]<<" "<<ArrayQ[i]<<" "<<IsNR[i]<<endl;
    }

    // SELECTION A LA MAIN
    if (ArrayQ[i] > 0.5 && ArrayQ[i]<1.5 && ArrayErec[i]>4 && ArrayErec[i]<1000) compteur++;
    if (fmod((double)i,(double)100000) == 0) cout << i << endl;
  }
  cout << "total evts in hand-selected region="<<compteur<<endl;

  if (print_wimps) {
    char therun[9]; Int_t evtnum=0;
    DSTRed->SetBranchAddress("RunName",&therun);
    DSTRed->SetBranchAddress("EventNum",&evtnum);
    cout <<"** Leaking events in hand-selected nucl band:"<<endl;
    for (Int_t i=0; i<nb; i++) {
      // SELECTION A LA MAIN
      if ((ArrayQ[i]<1.8 && ArrayQ[i]>1.3 && ArrayErec[i]>70 && ArrayErec[i]<400) ||
	  //	  (ArrayQ[i]<0.7 && ArrayQ[i]>0.05 && ArrayErec[i]>30 && ArrayErec[i]<400) ||
	  (ArrayQ[i]<0.7 && ArrayQ[i]>0.05 && ArrayErec[i]>17)) {
	DSTRed->GetEntry(i);
	cout<<"Evt "<<i<<" (Run "<<therun<<" samba evt "<<evtnum<<")"<<endl;
	cout << " -- Q="<<ArrayQ[i]<<" - Erec="<<ArrayErec[i]<<endl;
      }
    }
    cout << "**"<<endl;
  }

  if (print_ascii) out.close();

  ldbh_mean/=nbpts_ldbs;
  ldbi_mean/=nbpts_ldbs;

  ion_thresh->SetParameter(0,ldbi_mean*sigmacut_ion);
  heat_thresh->SetParameter(1,ldbh_mean*sigmacut_heat);
  qdown_gamma->SetParameter(2,ldbi_mean);
  qdown_gamma->SetParameter(3,ldbh_mean);
  qup_nr->SetParameter(4,ldbh_mean);
  qup_nr->SetParameter(5,ldbi_mean);
  qdown_nr->SetParameter(4,ldbh_mean);
  qdown_nr->SetParameter(5,ldbi_mean);

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
    Float_t* QF = new Float_t[nb];
    for (int i=0; i<nb; i++) {
      if (IsNR[i] == 1) {
	QF[i]=ArrayQ[i];
	histowimps->Fill(ArrayErec[i]);
      } else QF[i]=-1;
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

  ion_thresh->Draw("same");
  heat_thresh->Draw("same");
  qmean_gamma->Draw("same");
  qdown_gamma->Draw("same");
  qmean_nr->Draw("same");
  qup_nr->Draw("same");
  qdown_nr->Draw("same");

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

  delete[] ArrayErec; delete[] ArrayQ; delete[] IsNR;
  return 0;
}
