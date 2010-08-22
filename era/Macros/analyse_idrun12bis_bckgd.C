
// Analyse Bg run 12bis
// Valable pour les differents macs
// Update Feb2010

string bolo="ID404";

.x params_id_run12.C
.x start_idrun12bis_bckgd.C

//**********************
// Raw list of periods : only once per mac
UInt_t periodlength_minutes=60; // change en 60 min pour run12bis (consistent avec noise spectra)
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
//BuildIonEnergy(chain,bolo,runs,sgn_ion,nbruns,eionntpfile,1);

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

PlotIonBaselines(bolo,qualdir,col1,col2,vet1,vet2,gar1,gar2,1);
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
Int_t timebin=3600.*24.; // ?? heures standard (12 ou 24?)
Float_t Ecution=50;
HeatIonEvol(chain,Eion,bolo,Ecution,timebin,periodfile,heationfile);

BuildHeatEnergy(chain,bolo,heationfile,echalntpfile,Eion);

//**********************
string baselinefile=qualdir+"/baseline_"+chal+".txt";
BaselineEvol(chain,chal,echalntpfile,periodfile,baselinefile,bolonum,0);
PlotHeatBaseline(bolo,baselinefile,1);
UpdateHeatEnergy(chain,bolo,echalntpfile,qualdir);

//**********************
// Chi2
Chi2Distri(chain,chal,echalntpfile,cutntpfile,eionntpfile);//,cutntpfile); // apres buildcutfile..
Chi2Distri(chain,col1,eionntpfile,cutntpfile,echalntpfile); 
Chi2Distri(chain,col2,eionntpfile,cutntpfile,echalntpfile); 

// Statistique sur les lignes de base
int p=1; // 0 a 7 (ID) ou 5 (FID)
PlotDistriBaselines(bolo,p,qualdir,cutbaselines);

//**********************
// coupures
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
BuildCutFile(chain,bolo,Eion,Echal,cutntpfile,qualdir,cutbaselines);

ExposureEvol(chain,bolo,qualdir,cutbaselines);// necessaire pr construire perdiolist

//**********************
// Fabrication "DST"
TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
BuildDST(chain,bolo,Eion,Echal,Cuts,dstfile,dst_ascii,modulation);

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
TTree* Coinc = (TTree*)f4->Get("t;2");
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


ExposureEvol(chain,bolo,qualdir,bolonum);

AcceptanceFunction(qualdir,bolo);


//**********************
// Qplot and other final plots
  // Encore pas mal a upgrader...

  //Float_t vetopercentcut=2;
  //Float_t deltapercentcut=5;
  //Float_t sigmacut_vet=2.;
  //Float_t sigmacut_delta=2.;
  //Float_t sigmacut_etot=6.;
  //Float_t deltabin_col=10;

Float_t sigmacut_heat=2.5;
Float_t sigmacut_ion=2;
Float_t sigmacut_gamma=3.29; // 3.29=99.9; 3.71=99.98 (=99.99 single sided)
Float_t sigmacut_quasifid=1.0;

//CutVetos(chain,eionntpfile,bolo,sigmacut_vet,sigmacut_delta,sigmacut_etot,deltabin_col,deltapercentcut,vetopercentcut);
// FONCTION A UPGRADER....
Float_t sigmacut_delta=2.;
AcceptanceFunction(qualdir,bolo,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,sigmacut_delta);


ExposureEvol(chain,bolo,qualdir,bolonum);

// avec temps mort trigger samba:
float sambadeadtime=0.200; // in sec.
ExposureEvol(chain,bolo,qualdir,cutbaselines,sambadeadtime,bolonum);

TFile f(eionntpfile.c_str(),"READ");
TTree* Eion = f->Get("energytree");
TFile f1(echalntpfile.c_str(),"READ");
TTree* Echal = f1->Get("energytree");
TFile f2(cutntpfile.c_str(),"READ");
TTree* Cuts = f2->Get("cuttree");
TFile f3(dstfile.c_str(),"READ");
TTree* DST = f3->Get("dsttree");


Float_t sigmacut_heat=3.5;
Float_t sigmacut_ion=3.5;
Float_t sigmacut_gamma=3.71; // 3.29=99.9; 3.71=99.98 (=99.99 single sided)
Float_t sigmacut_quasifid=1.0;


Bool_t print_wimps=0;
Int_t evtclassflag=2;
Int_t voltflag=-1;
Bool_t cut_badevts=0;
Qplot(bolo,qualdir,DST,Cuts,Echal,Eion,voltflag,evtclassflag,cut_badevts,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,0,print_wimps,0);


CosmogenicLine(bolo,Eion,Cuts,Echal);
NoiseThreshold(bolo,Eion,Echal,Cuts,sigmacut_heat,sigmacut_ion);


// Delta-Sigma plot
//Float_t f_lim=0.12;
DeltaPlot(chain,bolo,Eion,Echal,Cuts,sigmacut_vet,sigmacut_ion,sigmacut_heat);

Collectrodes(eionntpfile,bolo,sigmacut_delta,deltapercentcut);







