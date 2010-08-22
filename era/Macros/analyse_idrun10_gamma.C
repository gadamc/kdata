
// Analyse Gamma run 10
// ID201, ID401, ID3

string bolo="ID401";

.x params_id_run10.C
.x start_idrun10_gamma.C

//**********************
// Raw list of periods : only once per mac
UInt_t periodlength_minutes=60;
BuildPeriodList(chain,qualdir,periodlength_minutes);

//**********************
// Calibration des ionisations
BuildBoloRunList(bolo,qualdir);

//********************
int choicesign=3;
string* runs=FillRunList(runlist);
Int_t* sgn_ion=FillPolarSignList(runlist_bolo);
string cutsign = CutIonSign(runs,sgn_ion,nbruns,choicesign);

string calibrated=gar1, calibrator=chal;
string amplcut=" <-0.5 "; //if (choicesign>0) amplcut=" < -0.5 ";
//string ctcut=" < 0.1"; if (choicesign>0) ctcut=" > -0.1";
string ratio="WienerAmpl_"+calibrated+"/WienerAmpl_"+calibrator;

chain->Draw(("WienerAmpl_"+calibrated).c_str(),cutsign.c_str());
chain->Draw(("WienerAmpl_"+calibrated+":DateSec").c_str(),cutsign.c_str());
chain->Draw(ratio.c_str(),(ratio+" > 0.1 && "+ratio+" < 3.2 && "+"WienerAmpl_"+calibrated+amplcut+"&& WienerAmpl_"+calibrator+amplcut+"&& "+cutsign).c_str());
chain->Draw(("WienerAmpl_"+calibrator+":WienerAmpl_"+calibrated).c_str(),cutsign.c_str());
// crosstalks
chain->Draw(("WienerAmpl_"+calibrator+":WienerAmpl_"+calibrated).c_str(),(cutsign+" && WienerAmpl_"+calibrated+">-0.5").c_str());
chain->Draw(("WienerAmpl_"+calibrator+":WienerAmpl_"+calibrated+"_Sync_"+calibrator).c_str(),(cutsign+" && WienerAmpl_"+calibrated+"_Sync_"+calibrator+">-0.5").c_str());


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
Float_t Ecution=50.;

HeatIonEvol(chain,Eion,bolo,Ecution,timebin,periodfile,heationfile);

int usevoltflag=-1; // defaut=-1 (use all data)
HeatVsIon(chain,bolo,Eion,heationfile,NULL,NULL,"",usevoltflag);

BuildHeatEnergy(chain,bolo,heationfile,echalntpfile,Eion);

//**********************
string baselinefile=qualdir+"/baseline_"+chal+".txt";
BaselineEvol(chain,chal,echalntpfile,periodfile,baselinefile,bolonum,0);
PlotHeatBaseline(bolo,baselinefile);
UpdateHeatEnergy(chain,bolo,echalntpfile,qualdir);

//**********************
// Chi2
Chi2Distri(chain,chal,echalntpfile,"");
Chi2Distri(chain,col1,eionntpfile,""); // possibilite de voir la distri apres BuildCutFile
Chi2Distri(chain,col2,eionntpfile,"");//,cutntpfile); // possibilite de voir la distri apres BuildCutFile
// DO OTHER CHANNELS!


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
TTree* Coinc = NULL; // run 10...
TTree* Tst = NULL;

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
Float_t sigmacut_ion=3.5;
Float_t sigmacut_gamma=3.71;
Float_t sigmacut_quasifid=1.0;

Bool_t print_wimps=1;
Int_t evtclassflag=2;
Int_t voltflag=-1;
Bool_t cut_badevts=0;
Qplot(bolo,qualdir,DST,Cuts,Echal,Eion,voltflag,evtclassflag,cut_badevts,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,0,print_wimps,0);

// OLD:
Float_t doqualcut=1; // flag coupures periodes et chi2 =0 pour pas couper
Float_t dovetocut=1;
Bool_t print_wimps=1, dohist=0;
string outfile="";
Qplot(chain,bolo,qualdir,Eion,Echal,Cuts,volt,kappa,sigmacut_heat,sigmacut_ion,sigmacut_gamma,sigmacut_quasifid,doqualcut,dovetocut,dohist,print_wimps,0,-1,outfile);

