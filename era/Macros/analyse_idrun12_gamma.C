
// Analyse Gamma run 12 - Aou 09
// Valable pour les differents macs
// Update oct09 - Update Feb 2010

string bolo="ID4";

.x params_id_run12.C
.x start_idrun12_gamma.C


//**********************
// Raw list of periods : only once per mac
UInt_t periodlength_minutes=60; // 100->60 apres reprocessing avril
BuildPeriodList(chain,qualdir,periodlength_minutes);

// Timestamp tree from Ana's correction (only once per mac)
BuildTimestamp(chain,tstfile,tstnew_dir);

//**********************
// Calibration des ionisations

BuildBoloRunList(bolo,qualdir);

//**********************
int choicesign=1; //flag polar
string* runs=FillRunList(qualdir+"listruns.txt");
Int_t* sgn_ion=FillPolarSignList(qualdir+"listruns_"+bolo+".txt");
string cutsign = CutIonSign(runs,sgn_ion,nbruns,choicesign);

string calibrated=gar2, calibrator=chal;
string amplcut=" > 0.5 "; if (choicesign>0) amplcut=" < -0.5 ";
string ratio="WienerAmpl_"+calibrated+"/WienerAmpl_"+calibrator;

chain->Draw(("WienerAmpl_"+calibrated).c_str(),cutsign.c_str());
chain->Draw(("WienerAmpl_"+calibrated+":DateSec").c_str(),cutsign.c_str());
chain->Draw(ratio.c_str(),(ratio+" > 0.1 && "+ratio+" < 3.2 && "+"WienerAmpl_"+calibrated+amplcut+"&& WienerAmpl_"+calibrator+amplcut+"&& "+cutsign).c_str());
chain->Draw(("WienerAmpl_"+calibrator+":WienerAmpl_"+calibrated).c_str(),cutsign.c_str());
// crosstalks
chain->Draw(("WienerAmpl_"+calibrator+":WienerAmpl_"+calibrated).c_str(),(cutsign+" && WienerAmpl_"+calibrated+" < 0.1").c_str());

//**********************
// Puis creation du ntp des ionisations calibrees
string* runs=FillRunList(runlist);
Int_t* sgn_ion=FillPolarSignList(runlist_bolo);
BuildIonEnergy(chain,bolo,runs,sgn_ion,nbruns,eionntpfile);


//**********************
// Analyse ldb ion et collectrodes
string channel=col1;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);

string channel=col2;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);
string channel=vet1;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);
string channel=vet2;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);
string channel=gar1;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);

string channel=gar2;
string baselinefile=qualdir+"/baseline_"+channel+".txt";
BaselineEvol(chain,channel,eionntpfile,periodfile,baselinefile,bolonum,0);

PlotIonBaselines(bolo,qualdir,col1,col2,vet1,vet2,gar1,gar2);
UpdateIonEnergy(chain,bolo,eionntpfile,qualdir);


//**********************
// Complete le ntp des ionisations calibrees
Float_t sigmacut_vet=2;
Float_t sigmacut_delta=2;
Float_t sigmacut_etot=6;
Float_t vetopercentcut=2;
Float_t deltapercentcut=5;
Float_t deltabin_col=10;
CutVetos(chain,eionntpfile,bolo,sigmacut_vet,sigmacut_delta,sigmacut_etot,deltabin_col,deltapercentcut,vetopercentcut);

//Collectrodes(eionntpfile,bolo,sigmacut_delta,deltapercentcut);

//**********************
// Heat calibration
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
Int_t timebin=3600.*12; // 12 heures standard
Float_t Ecution=50.; // au lieu de 100 (avoir la meme en gamma et fond!)

HeatIonEvol(chain,Eion,bolo,Ecution,timebin,periodfile,heationfile);

int usevoltflag=-1; // defaut=-1; Pour FID402: separe 0/1 pour polars 8V/4V
HeatVsIon(chain,bolo,Eion,heationfile,NULL,NULL,"",usevoltflag);

// Bonus : on ajoute le fond
int nbntps2=389;
TChain* chain2 = new TChain("EdwRootNtp","calib evts");
string ntpdir2="/Users/armengau/Data/Edelweiss/ID_Run12/Bckgd"+mac+"/ntps/";
for (int i=1; i<=nbntps2; i++) { std::ostringstream truc; truc<<i; chain2->AddFile((ntpdir2+"ntp_"+truc.str()+".root").c_str()); }
TFile f2(("/Users/armengau/Data/Edelweiss/ID_Run12/Bckgd"+mac+"/eion_"+bolo+".root").c_str(),"READ");
TTree* Eion2 = f2.Get("energytree");
string heationfile2="/Users/armengau/Data/Edelweiss/ID_Run12/Bckgd"+mac+"/periods/heation_"+bolo+".txt";
HeatVsIon(chain,bolo,Eion,heationfile,chain2,Eion2,heationfile2);

BuildHeatEnergy(chain,bolo,heationfile,echalntpfile,Eion); // besoin Eion pour polarflag


//**********************
string baselinefile=qualdir+"/baseline_"+chal+".txt";
BaselineEvol(chain,chal,echalntpfile,periodfile,baselinefile,bolonum,0);
PlotHeatBaseline(bolo,baselinefile);
UpdateHeatEnergy(chain,bolo,echalntpfile,qualdir);

//**********************
// Chi2
Chi2Distri(chain,chal,echalntpfile,"");
Chi2Distri(chain,col1,eionntpfile,""); // possibilite de voir la distri apres BuildCutFile
Chi2Distri(chain,col2,eionntpfile,"")//,cutntpfile); // possibilite de voir la distri apres BuildCutFile

// Largeur a 356keV
Float_t cutldbion=2;
Float_t cutldbheat=3.5;
GetSigma356keV(bolo,basedir,cutldbion,cutldbheat);

//**********************
// coupures
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
BuildCutFile(chain,bolo,Eion,Echal,cutntpfile,qualdir,cutbaselines);


//**********************
// Fabrication "DST"
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
BuildDST(chain,bolo,Eion,Echal,Cuts,dstfile,dst_ascii,modulation);


//**********************
// Rajout variables qplot
UpdateDST(bolo,dstfile,eionntpfile);

//**********************
// Fabrication DST reduit

TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
TFile f3(dstfile.c_str(),"READ");
TTree* DST = f3->Get("dsttree");
TFile f4(coinfile.c_str(),"READ");
TTree* Coinc = (TTree*)f4->Get("t"); // t;2
if (!Coinc) cout << "ah" << endl;
TFile f5(tstfile.c_str(),"READ");
TTree* Tst = f5->Get("tst_new");

BuildDSTRed(chain,bolo,Eion,Echal,Cuts,DST,Coinc,Tst,dstredfile);

//**********************
// Qplot : new version!

TFile f(dstredfile.c_str(),"READ");
TTree* DSTRed = f->Get("dstred");
Bool_t print_wimps=1;
Int_t evtclassflag=2;
Int_t voltflag=-1;
Bool_t cut_badevts=1;
gSigmaCut_Efid=2; gSigmaCut_Heat=3.5; gSigmaCut_Gamma=3.71;
gLdbCut_Ecol=4; // infini pour ID405 sinon 4.
gLdbCut_Evet=1.5; // 1.5...va avec Ecol
string qplotfile=basedir+"/qplot_"+bolo+".txt";

Qplot(bolo,qualdir,DSTRed,voltflag,evtclassflag,cut_badevts,0,print_wimps,0,qplotfile);

// bonus sept 2010
SynchroFidCut(bolo,DSTRed);


//**********************
// Qplot
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
TFile f3(dstfile.c_str(),"READ");
TTree* DST = f3->Get("dsttree");


Float_t sigmacut_heat=3.5;
Float_t sigmacut_ion=3.;
Float_t sigmacut_gamma=3.29;
Float_t sigmacut_quasifid=1.0;

Bool_t print_wimps=0;
Int_t evtclassflag=2;
Int_t voltflag=-1;
Bool_t cut_badevts=0;
Qplot(bolo,qualdir,DST,Cuts,Echal,Eion,voltflag,evtclassflag,cut_badevts,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,0,print_wimps,0);

//***********************
// Study of luke effect for surface events
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
TFile f3(dstfile.c_str(),"READ");
TTree* DST = f3->Get("dsttree");
Eion->AddFriend(Echal); Eion->AddFriend(DST);

TF1* lukefunc = new TF1("luke","(1+[0]/3.)*x",0,1000);
lukefunc->SetParameter(0,8);
lukefunc->SetLineColor(2);
lukefunc->SetLineWidth(1);
TF1* lukefuncth = new TF1("luketh","(1+[0]/3.)*x",0,1000);
lukefuncth->SetParameter(0,5.5);
lukefuncth->SetLineColor(4);
lukefuncth->SetLineWidth(1);

TCut cutvolt="dsttree.Vflag == 0";
TCut cutevt="dsttree.EventFlag == 3 && Ecol2 < 2";
Eion->Draw("dsttree.Eheat*(1.+8./3.):dsttree.Eion",cutvolt&&cutevt);
TGraph* gr=(TGraph*)gROOT->FindObject("Graph"); gr->Fit(lukefunc,"Q");
lukefunc->Draw("same"); lukefuncth->Draw("same");
cout << lukefunc->GetParameter(0)<<" "<<lukefunc->GetParError(0)<<endl;
