
#include <cmath>
#include "UpdateEnergy.C"

int AcceptanceFunction(string qualdir, string bolo, float sigmacut_fid=2, Float_t sigmacut_delta=2) {

  // sigmacut_heat/ion = seuils bruits (var. globale)
  // gamma = confusion gammas (var. globale)
  // sigmacut_delta/fid : on a un 'seuil' en Eside = sigmacut_delta*sigma(E2-E1) puis
  //    sigmacut_fid = confusion de ces evts a Eside UN PEU OBSOLETE...

  // STILL TO DO = TRIGGER ONLINE!

  Float_t epsilon=3.;
  Float_t a=0.16, b=0.18; // NR band parameters
  Float_t c=0.035; // param en + pour sigmaQnr
  Float_t kappa_ion=gKappaIon, kappa_chal=gKappaHeat; // increase of resolution at high E
  Float_t k1=gSigmaCut_Gamma/2.355; // gamma band limit
  //  Float_t k2=1.645/2.355; // wimp band limit
  Float_t k3=sigmacut_fid/2.355; // quasi fid limit

  Int_t nbins=500;
  Float_t binsperkev=10;
  Float_t* array_accept = new Float_t[nbins];
  for (int i=0; i<nbins; i++) array_accept[i]=0;
  Float_t* array_accept_heat = new Float_t[nbins];
  for (int i=0; i<nbins; i++) array_accept_heat[i]=0;
  Float_t* array_accept_ion = new Float_t[nbins];
  for (int i=0; i<nbins; i++) array_accept_ion[i]=0;
  Float_t* array_accept_gammarejec = new Float_t[nbins];
  for (int i=0; i<nbins; i++) array_accept_gammarejec[i]=0;
  Float_t* array_accept_fidcor = new Float_t[nbins];
  for (int i=0; i<nbins; i++) array_accept_fidcor[i]=0;
  //Float_t* array_accept_vol = new Float_t[nbins];
  //  for (int i=0; i<nbins; i++) array_accept_vol[i]=0;


  // The "non-erf" way:
  /*
  TF1* Eheat = new TF1("eheat","x*(1+[0]*[1]*pow(x,[2]))/(1+[0])",0,500);
  Eheat->SetParameter(0,voltage/epsilon);
  Eheat->SetParameter(1,a);
  Eheat->SetParameter(2,b);

  //  TF1* MagicPointFunc = new TF1("magicpoint","x*(1-[1]*pow(x,[2])-[7]*(1+[0]))-[3]*(1+[0])*sqrt([4]*[4]+[5]*[5])-[6]*sqrt(pow((1+[0])*[1]*pow(x,[2])*[4],2)+pow((1+[0]*[1]*pow(x,[2]))*[5],2))",0.1,300); // prend le point haut
  TF1* MagicPointFunc = new TF1("magicpoint","x*(1-[1]*pow(x,[2]))-[3]*(1+[0])*sqrt([4]*[4]+[5]*[5]+x*x*([6]+[7]))",0.1,300); // Modifie : prend le milieu de la bande NR pour calculer le "seuil" d'acceptance
  MagicPointFunc->SetParameter(0,voltage/epsilon);
  MagicPointFunc->SetParameter(1,a);
  MagicPointFunc->SetParameter(2,b);
  MagicPointFunc->SetParameter(3,k1);
  MagicPointFunc->SetParameter(4,1); // ldb_chal
  MagicPointFunc->SetParameter(5,1); // ldb_ion
  //  MagicPointFunc->SetParameter(6,k2);
  MagicPointFunc->SetParameter(6,kappa_ion);
  MagicPointFunc->SetParameter(7,kappa_chal);

  TF1* FidPointFunc = new TF1("fidpoint","(x-[0])/(x+[0]*[1])-[7]*pow(x,[8])-[2]*(1+[1])*sqrt((x-[0])*(x-[0])*([3]*[3]+x*x*[5])+x*x*([4]*[4]+x*x*[6]))/(x*x)",0.1,300);
  FidPointFunc->SetParameter(0,2); // Eside_thr
  FidPointFunc->SetParameter(1,voltage/epsilon);
  FidPointFunc->SetParameter(2,k3);
  FidPointFunc->SetParameter(3,1); // ldb_chal
  FidPointFunc->SetParameter(4,1); // ldb_ion
  FidPointFunc->SetParameter(5,kappa_chal);
  FidPointFunc->SetParameter(6,kappa_ion);
  FidPointFunc->SetParameter(7,a);
  FidPointFunc->SetParameter(8,b);
  */

  // The "erf" way:
  int vflag=0;
  TF1* QlimHeat = new TF1("qlimheat","((1+[0])*[1]/x)-[0]",0.1,300);
  QlimHeat->SetParameter(0,epsilon/gVolts[vflag]);
  QlimHeat->SetParameter(1,1); // seuil chaleur=ldb*sigmacut_heat
  TF1* QlimIon = new TF1("qlimion","[0]/x",0.1,300);
  QlimIon->SetParameter(0,1); // seuil ion = ldb*sigmacut_ion
  TF1* QlimGamma = new TF1("qlimgamma","1-[0]*(1+[1])*sqrt([2]*[2]+[3]*[3]+x*x*([4]+[5]))/x",0.1,300);
  QlimGamma->SetParameter(0,k1);
  QlimGamma->SetParameter(1,gVolts[vflag]/epsilon);
  QlimGamma->SetParameter(2,1); // ldb_chal
  QlimGamma->SetParameter(3,1); // ldb_ion
  QlimGamma->SetParameter(4,kappa_ion);
  QlimGamma->SetParameter(5,kappa_chal);
  TF1* QlimQf = new TF1("qlimqf","(x-[0])/(x+[0]*[1])-[2]*(1+[1])*sqrt((x-[0])*(x-[0])*([3]*[3]+x*x*[5])+x*x*([4]*[4]+x*x*[6]))/(x*x)",0.1,300);
  QlimQf->SetParameter(0,2); // Eside_thr
  QlimQf->SetParameter(1,gVolts[vflag]/epsilon);
  QlimQf->SetParameter(2,k3);
  QlimQf->SetParameter(3,1); // ldb_chal
  QlimQf->SetParameter(4,1); // ldb_ion
  QlimQf->SetParameter(5,kappa_chal);
  QlimQf->SetParameter(6,kappa_ion);
  TF1* Qnr = new TF1("qnr","[0]*pow(x,[1])",0.1,300);
  Qnr->SetParameter(0,a);
  Qnr->SetParameter(1,b);
  TF1* SigmaQnr = new TF1("sigmaqnr","sqrt((1+[0])*(1+[0])*([1]*[1]+[2]*x*x)*[3]*pow(x,[4])*[3]*pow(x,[4])+(1+[0]*[3]*pow(x,[4]))*(1+[0]*[3]*pow(x,[4]))*([5]*[5]+[6]*x*x)+[7]*[7]*x*x)/(x*2.355)",0.1,300);
  SigmaQnr->SetParameter(0,gVolts[vflag]/epsilon);
  SigmaQnr->SetParameter(1,1); // ldb_chal
  SigmaQnr->SetParameter(2,kappa_chal);
  SigmaQnr->SetParameter(3,a);
  SigmaQnr->SetParameter(4,b);
  SigmaQnr->SetParameter(5,1); // ldb_ion
  SigmaQnr->SetParameter(6,kappa_ion);
  SigmaQnr->SetParameter(7,c);

  string periodlist=qualdir+"/period_list_"+bolo+".txt" ;
  ULong_t ti,tf; Int_t isok;
  vector<Float_t> ldb_heat=GetLDB(ChannelName(bolo,"heat"),qualdir);
  vector<Float_t> ldb_col1=GetLDB(ChannelName(bolo,"col1"),qualdir);
  vector<Float_t> ldb_col2=GetLDB(ChannelName(bolo,"col2"),qualdir);

  ifstream buf(periodlist.c_str(),ios::in);
  if (!buf) { cout << "No period list!" <<endl; return 0; }
  ifstream buf2((qualdir+"/period_list_withruns.txt").c_str(),ios::in);
  ifstream buf3((qualdir+"/runlist_"+bolo+".txt").c_str(),ios::in);
  string current_run;
  buf3 >> current_run >> vflag;
  int p=0; string therun; ULong_t ti2,tf2;
  while (buf >> ti >> tf >> isok) {
    buf2 >> ti2 >> tf2 >> therun;
    if (ti2 != ti || tf2 != tf) cout << "pbl period files!"<<endl;
    if (isok) {
      Double_t length_day=Double_t(tf-ti)/86400.;
      while (current_run != therun) buf3 >> current_run >> vflag;

      // The "non-erf" way:
      /*
      // Seuil bruit chal
      Float_t Erec_thresh = Eheat->GetX(ldb_heat[p]*sigmacut_heat);
      Int_t binrec_thresh = ceil(Erec_thresh*binsperkev);
      for (int k=binrec_thresh; k<nbins; k++) array_accept_heat[k] += length_day;

      // Seuil bruit ion fid
      if (ldb_col1[p] == 0 || ldb_col2[p] == 0 ) cout << p << endl;
      Float_t ldb_deltae = sqrt(ldb_col1[p]*ldb_col1[p]+ldb_col2[p]*ldb_col2[p]);
      Float_t ldb_ion=ldb_col1[p]*ldb_col2[p]/ldb_deltae;
      Float_t Efid_thresh = sigmacut_ion*ldb_ion;
      Float_t Erec_thresh_ion = pow(Efid_thresh/a,1./(1.+b));
      binrec_thresh = ceil(Erec_thresh_ion*binsperkev);
      for (int k=binrec_thresh; k<nbins; k++) array_accept_ion[k] += length_day;

      // Seuil confusion gammas ("point magique")
      MagicPointFunc->SetParameter(4,ldb_heat[p]);
      MagicPointFunc->SetParameter(5,ldb_ion);
      Float_t Magic_thresh = MagicPointFunc->GetX(0);
      binrec_thresh = ceil(Magic_thresh*binsperkev);
      for (int k=binrec_thresh; k<nbins; k++) array_accept_gammarejec[k] += length_day;

      // Seuil rejection evts quasi-fiduciels.
      //      Float_t rejec=1.7; // "k" arbitraire.... 90% bruit
      //Float_t Evol_thresh = rejec*sqrt(ldb_col1[p]*ldb_col1[p]+ldb_col2[p]*ldb_col2[p])/(2.355*2*f_fid);
      //Float_t Erec_thresh_vol = pow(Evol_thresh/a,1./(1.+b));
      //      binrec_thresh = ceil(Erec_thresh_vol*binsperkev);
      //      for (int k=binrec_thresh; k<nbins; k++) array_accept_vol[k] += length_day;

      // Seuil confusion evts gamma a Eion sous-estimee d'une fraction f_fid
      FidPointFunc->SetParameter(3,ldb_heat[p]);
      FidPointFunc->SetParameter(4,ldb_ion);
      Float_t Eside_thr = ldb_deltae*sigmacut_delta;
      FidPointFunc->SetParameter(0,Eside_thr);
      Float_t Fid_thresh = FidPointFunc->GetX(0);
      binrec_thresh = ceil(Fid_thresh*binsperkev);
      for (int k=binrec_thresh; k<nbins; k++) array_accept_fidcor[k] += length_day;
      
      if (Erec_thresh_ion > Erec_thresh) Erec_thresh=Erec_thresh_ion;
      if (Magic_thresh > Erec_thresh) Erec_thresh=Magic_thresh;
      if (Fid_thresh > Erec_thresh) Erec_thresh=Fid_thresh;
      binrec_thresh = ceil(Erec_thresh*binsperkev);
      for (int k=binrec_thresh; k<nbins; k++) array_accept[k] += length_day;
*/      

      // The "erf" way
      QlimHeat->SetParameter(0,epsilon/gVolts[vflag]);
      QlimGamma->SetParameter(1,gVolts[vflag]/epsilon);
      QlimQf->SetParameter(1,gVolts[vflag]/epsilon);
      SigmaQnr->SetParameter(0,gVolts[vflag]/epsilon);

      QlimHeat->SetParameter(1,ldb_heat[p]*gSigmaCut_Heat);
      if (ldb_col1[p] == 0 || ldb_col2[p] == 0 ) cout << p << endl;
      Float_t ldb_deltae = sqrt(ldb_col1[p]*ldb_col1[p]+ldb_col2[p]*ldb_col2[p]);
      Float_t ldb_ion=ldb_col1[p]*ldb_col2[p]/ldb_deltae;
      QlimIon->SetParameter(0,gSigmaCut_Efid*ldb_ion);
      QlimGamma->SetParameter(2,ldb_heat[p]);
      QlimGamma->SetParameter(3,ldb_ion);
      QlimQf->SetParameter(0,ldb_deltae*sigmacut_delta);
      QlimQf->SetParameter(3,ldb_heat[p]);
      QlimQf->SetParameter(4,ldb_ion);
      SigmaQnr->SetParameter(1,ldb_heat[p]);
      SigmaQnr->SetParameter(5,ldb_ion);
      for (int k=1; k<nbins; k++) {
	Float_t Erec=k/Float_t(binsperkev);
	Float_t qlh=QlimHeat->Eval(Erec);
	Float_t qli=QlimIon->Eval(Erec);
	Float_t qlg=QlimGamma->Eval(Erec);
	Float_t qlqf=QlimQf->Eval(Erec);
	Float_t qnr=Qnr->Eval(Erec);
	Float_t sqnr=SigmaQnr->Eval(Erec);
	array_accept_heat[k] += (length_day*0.5*(1-TMath::Erf((qlh-qnr)/(sqrt(2.)*sqnr))));
	array_accept_ion[k] += (length_day*0.5*(1-TMath::Erf((qli-qnr)/(sqrt(2.)*sqnr))));
	array_accept_gammarejec[k] += (length_day*0.5*(1+TMath::Erf((qlg-qnr)/(sqrt(2.)*sqnr))));
	array_accept_fidcor[k] += (length_day*0.5*(1+TMath::Erf((qlqf-qnr)/(sqrt(2.)*sqnr))));
	Float_t qinf=qli;
	if (qlh > qli) qinf=qlh;
	Float_t qsup=qlg;
	//	if (qlqf < qlg) qsup=qlqf; // on fait plus de quasifid!
	Float_t qnr_inf=qnr-gNRWidth*sqnr; // 90% en dur..
	Float_t qnr_sup=qnr+gNRWidth*sqnr;
	if (qnr_inf > qinf) qinf=qnr_inf;
	if (qnr_sup < qsup) qsup=qnr_sup;
	if (qsup > qinf) array_accept[k] += (length_day*0.5*(TMath::Erf((qsup-qnr)/(sqrt(2.)*sqnr))-TMath::Erf((qinf-qnr)/(sqrt(2.)*sqnr)))); // else zero!
      }
    }
    p++;
  }
  buf.close();
  buf2.close(); buf3.close();
  Float_t norm=array_accept[nbins-1];
  for (int k=0; k<nbins; k++) array_accept[k] /= norm;
  norm=array_accept_ion[nbins-1];
  for (int k=0; k<nbins; k++) array_accept_ion[k] /= norm;
  norm=array_accept_heat[nbins-1];
  for (int k=0; k<nbins; k++) array_accept_heat[k] /= norm;
  norm=array_accept_gammarejec[nbins-1];
  for (int k=0; k<nbins; k++) array_accept_gammarejec[k] /= norm;
  norm=array_accept_fidcor[nbins-1];
  for (int k=0; k<nbins; k++) array_accept_fidcor[k] /= norm;
  //  norm=array_accept_vol[nbins-1];
  //  for (int k=0; k<nbins; k++) array_accept_vol[k] /= norm;


  TCanvas* cc=  new TCanvas("accept","accept");
  Float_t* xx = new Float_t[nbins];
  for (int k=0; k<nbins; k++) xx[k]=k/binsperkev;
  TGraph* gr = new TGraph(nbins,xx,array_accept);
  TGraph* grh = new TGraph(nbins,xx,array_accept_heat);
  TGraph* gri = new TGraph(nbins,xx,array_accept_ion);
  //  TGraph* griv = new TGraph(nbins,xx,array_accept_vol);
  TGraph* grm = new TGraph(nbins,xx,array_accept_gammarejec);
  //  TGraph* grc = new TGraph(nbins,xx,array_accept_fidcor);
  gr->SetTitle(("Acceptance - "+bolo).c_str());
  gr->GetXaxis()->SetTitle("Er [keV]");
  gr->GetYaxis()->SetTitle("Relative acceptance");
  gr->SetLineStyle(5);
  gr->Draw("AL");
  grh->SetLineColor(2); gri->SetLineColor(8);
  //  griv->SetLineColor(4); griv->SetLineStyle(7);
  grm->SetLineColor(4);
  //  grc->SetLineColor(28);
  //  grc->Draw("sameL"); //griv->Draw("sameL");
  grh->Draw("sameL"); gri->Draw("sameL");
  grm->Draw("sameL");
  gr->SetLineWidth(2);
  gr->Draw("sameL");
  TLegend* leg = new TLegend(0.5,0.15,0.85,0.25);
  leg->SetFillColor(0);
  leg->AddEntry(grh,"Offline heat threshold","L");
  leg->AddEntry(gri,"Offline ionization threshold","L");
  leg->AddEntry(grm,"Gamma confusion threshold","L");
  // leg->AddEntry(griv,"Fiducial confusion threshold","L");
  //  leg->AddEntry(grc,"Gamma confusion for quasi-fiducial evts","L");
  leg->AddEntry(gr,"Resulting acceptance","L");
  leg->Draw();

  // Save final acceptance function into an ascii file
  ofstream of((qualdir+"/acceptance_"+bolo+".txt").c_str(),ios::out);
  for (int p=0; p<nbins; p++) of << xx[p]<<" "<<array_accept[p]<<endl;
  of.close();

  return 0;
}
