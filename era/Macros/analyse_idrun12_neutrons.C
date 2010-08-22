
// Analyse neutrons run 12 - Sep 09
// Update mars 2010 puis mai2010

string bolo="FID402";

.x params_id_run12.C
.x start_idrun12_neutrons.C

//**********************
// Raw list of periods : only once per mac
UInt_t periodlength_minutes=60; // 100->60
BuildPeriodList(chain,qualdir,periodlength_minutes);

// Timestamp tree from Ana's correction (only once per mac)
BuildTimestamp(chain,tstfile,tstnew_dir);

//**********************
BuildBoloRunList(bolo,qualdir);

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

//**********************
// Heat calibration
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
Int_t timebin=3600.*12; // 12 heures standard
Float_t Ecution=50.;
Float_t Emaxion=400; // necessaire en calib neutron

HeatIonEvol(chain,Eion,bolo,Ecution,timebin,periodfile,heationfile,1,Emaxion);

BuildHeatEnergy(chain,bolo,heationfile,echalntpfile,Eion); // besoin Eion pour polarflag


//**********************
string baselinefile=qualdir+"/baseline_"+chal+".txt";
BaselineEvol(chain,chal,echalntpfile,periodfile,baselinefile,bolonum,0);
//PlotHeatBaseline(bolo,baselinefile);
UpdateHeatEnergy(chain,bolo,echalntpfile,qualdir);

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
// Chi2
Chi2Distri(chain,chal,echalntpfile,cutntpfile);
Chi2Distri(chain,col1,eionntpfile,""); 

Chi2Evol(chain,col1,eionntpfile,0);

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





//*********************
// OLD A NETTOYER...
//**********************

Float_t vetopercentcut=2;
Float_t deltapercentcut=5;
Float_t sigmacut_vet=2.;
Float_t sigmacut_delta=2.;
Float_t sigmacut_etot=6.;
Float_t sigmacut_heat=3.5;
Float_t sigmacut_ion=3.5;
Float_t sigmacut_gamma=3.29;
Float_t sigmacut_quasifid=1.0; // 2.0
Float_t deltabin_col=10;

CutVetos(chain,eionntpfile,bolo,sigmacut_vet,sigmacut_delta,sigmacut_etot,deltabin_col,deltapercentcut,vetopercentcut);

TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");

ExposureEvol(chain,bolo,qualdir,cutbaselines);

AcceptanceFunction(qualdir,bolo,volt,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,sigmacut_delta,kappa);

Float_t doqualcut=1; // flag coupures periodes et chi2 =0 pour pas couper
Float_t dovetocut=1; // 
Bool_t print_wimps=0, dohist=0, dohist_nr=1;
Qplot(chain,bolo,qualdir,Eion,Echal,Cuts,volt,kappa,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,doqualcut,dovetocut,dohist,print_wimps,dohist_nr);
