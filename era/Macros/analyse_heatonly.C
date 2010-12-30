
//**********************************
// Part 1
//**********************************
// Etude Gc2
// Jul 2010

string bolo="Gc2";

.x params_id_run12.C
.x start_idrun12bis_neutrons.C

// Determination de Gchal a partir du 356keV
// stabilite au cours du temps. .~ 10%

cout << chain->GetEntries() << endl;
BuildHeatEnergy(chain,bolo,"",echalntpfile,NULL);

string baselinefile=qualdir+"/baseline_"+chal+".txt";
BaselineEvol(chain,chal,echalntpfile,periodfile,baselinefile,bolonum,0);
PlotHeatBaseline(bolo,baselinefile);
UpdateHeatEnergy(chain,bolo,echalntpfile,qualdir);

// Etude de Gc2..

TChain* chain = new TChain("energytree","EdwHeatEnergy");
chain->AddFile("~/Data/Edelweiss/eheat_Gc2_a.root");
chain->AddFile("~/Data/Edelweiss/eheat_Gc2_b.root");

chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<1000");
chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.5+0.0007*Eheat*Eheat)","same");

// cut: LdbHeat<0.45 && Eheat > -100 && Eheat<500 && Chi2Heat < 1000
// chi2 evts "propres":
// && Chi2Heat < 1.5+0.0007*Eheat*Eheat

// on essaye de reduire au mieux avec le chi2 le plus fin possible a
// basse energie. cut a 1.1+0.0007*E^2 est bien (pas de perte
// d'efficacite, et bonne rejection jusqu'a 3 keV)
// en coupant a 1, on garde ~3/4 stat sur le 10keV
// et on reste plat jusqu'a presque 2 keV...!
// en coupant a 0.85.. on voit qu'on perd de la stat en fonction de l'energie

chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.1+0.0007*Eheat*Eheat)","same");
chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.07+0.0007*Eheat*Eheat)","same");
chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1+0.0007*Eheat*Eheat)","same");   
chain->Draw("Eheat","LdbHeat < 0.45 && Eheat > -5 && Eheat < 30 && Chi2Heat<(0.95+0.0007*Eheat*Eheat)","same");

// Etude RiseTime

chain->Draw("RiseTime:Chi2Heat","LdbHeat < 0.45 && Eheat > 5 && Eheat < 30 && Chi2Heat<15 && RiseTime> 0.3 && RiseTime < 20");
chain->Draw("RiseTime:Chi2Heat","LdbHeat < 0.45 && Eheat > 3 && Eheat < 5 && Chi2Heat<15 && RiseTime> 0.3 && RiseTime < 20","same");
chain->Draw("RiseTime:Chi2Heat","LdbHeat < 0.45 && Eheat > 2 && Eheat < 3 && Chi2Heat<15 && RiseTime> 0.3 && RiseTime < 20","same");
chain->Draw("RiseTime:Chi2Heat","LdbHeat < 0.45 && Eheat > 9 && Eheat < 12 && Chi2Heat<15 && RiseTime> 0.3 && RiseTime < 20","same");


// on voit tout de suite les 2 populations !
// (mais va falloir faire gaffe aux pertes dues a la coupure RiseTime)
// sans faire de truc trop complique: cut 5<risetime<9


// Et maintenant le fwhm
chain->Draw("PulseWidth:Chi2Heat","LdbHeat < 0.45 && Eheat > 9 && Eheat < 12 && Chi2Heat<10 && PulseWidth > 1 && PulseWidth < 70");
chain->Draw("PulseWidth:Chi2Heat","LdbHeat < 0.45 && Eheat > 4 && Eheat < 8 && Chi2Heat<10 && PulseWidth > 1 && PulseWidth < 70","same");
chain->Draw("PulseWidth:Chi2Heat","LdbHeat < 0.45 && Eheat > 2 && Eheat < 4 && Chi2Heat<10 && PulseWidth > 1 && PulseWidth < 70","same");
// La encore c'est extraordinaire
// cut a la louche ultrasimple 24<fwhm<55

// Etude plus fine des variables discriminantes
chain->Draw("PulseWidth:RiseTime","LdbHeat < 0.35 && Eheat > 1 && Eheat < 2 && Chi2Heat<100 && PulseWidth > 1 && PulseWidth < 70 && RiseTime>0.3 && RiseTime<20");
chain->Draw("PulseWidth:RiseTime","LdbHeat < 0.35 && Eheat > 1 && Eheat < 2 && Chi2Heat<(1.1+0.0007*Eheat*Eheat) && PulseWidth > 1 && PulseWidth < 70 && RiseTime>0.3 && RiseTime<20","same");

// Et dans l'autre sens
chain->Draw("log10(Chi2Heat):Eheat","Eheat>-3 && Eheat<30 && Chi2Heat<100 && LdbHeat<0.35");
TF1* ff=new TF1("ff","log10(1.1+0.0007*x*x)",0,30);
ff->Draw("same");

chain->Draw("log10(Chi2Heat):Eheat","Eheat>-3 && Eheat<30 && LdbHeat<0.35 && RiseTime > 5 && RiseTime<9","same");
chain->Draw("log10(Chi2Heat):Eheat","Eheat>-3 && Eheat<30 && LdbHeat<0.35 && RiseTime > 5 && RiseTime<9 && PulseWidth>24 && PulseWidth<55","same");
chain->Draw("log10(Chi2Heat):Eheat","Eheat>-3 && Eheat<30 && LdbHeat<0.35 && (RiseTime > 5 || PulseWidth>24)","same");

// Autre facon: pulse shape variable vs energy : RiseTime
chain->Draw("RiseTime:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && Chi2Heat < 100");
chain->Draw("RiseTime:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && Chi2Heat<(1.15+0.0007*Eheat*Eheat)","same");
chain->Draw("RiseTime:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && PulseWidth > 24 && Chi2Heat<100","same");
// La on peut deviner l'efficacite de la coupure RiseTime en fonction de l'energie!!
TCanvas* cc = new TCanvas("cc","cc"); cc->Divide(2,2);
cc->cd(1); chain->Draw("RiseTime","LdbHeat<0.35 && Eheat > 8 && Eheat <12 && PulseWidth>24 && Chi2Heat<100 && RiseTime > 0 && RiseTime < 30");
cc->cd(2); chain->Draw("RiseTime","LdbHeat<0.35 && Eheat > 4 && Eheat <8 && PulseWidth>24 && Chi2Heat<100 && RiseTime > 0 && RiseTime < 30");
cc->cd(3); chain->Draw("RiseTime","LdbHeat<0.35 && Eheat > 2 && Eheat <4 && PulseWidth>24 && Chi2Heat<100 && RiseTime > 0 && RiseTime < 30");
cc->cd(4); chain->Draw("RiseTime","LdbHeat<0.35 && Eheat > 1 && Eheat <2 && PulseWidth>24 && Chi2Heat<100 && RiseTime > 0 && RiseTime < 30");

// Autre facon: pulse shape variable vs energy : PulseWidth
chain->Draw("PulseWidth:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && Chi2Heat < 100");
chain->Draw("PulseWidth:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && Chi2Heat<(1.15+0.0007*Eheat*Eheat)","same");
chain->Draw("PulseWidth:Eheat","LdbHeat<0.35 && Eheat>-3 && Eheat<50 && RiseTime > 5 && Chi2Heat<100","same");
// On fait pas l'efficacite de la coupure en fonction de E car on voit que "RiseTime>5" n'est pas
// genial pour bien nettoyer a basse E..

// Combinaison Rise+Width... par bin d'energie
chain->Draw("PulseWidth:RiseTime","LdbHeat<0.35 && Eheat > 1.5 && Eheat<2 && Chi2Heat<100");
chain->Draw("PulseWidth:RiseTime","LdbHeat<0.35 && Eheat > 1.5 && Eheat<2 && Chi2Heat<(1.15+0.0007*Eheat*Eheat)","same");
// 8 - 12 nickel; 4-8 idem; 2-4 : on voit que ca va encore.
// 1-2 : c'est tendu du string - meme 1.5-2...



// Spectre "final"
chain->Draw("Eheat","LdbHeat < 0.35 && LdbHeat>0.1 && Eheat > -2 && Eheat < 12 && Chi2Heat<1000");
chain->Draw("Eheat","LdbHeat < 0.35 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.1+0.0007*Eheat*Eheat)","same");
chain->Draw("Eheat","LdbHeat < 0.35 && Eheat > -5 && Eheat < 30 && RiseTime > 5 && RiseTime<9","same");
chain->Draw("Eheat","LdbHeat < 0.35 && Eheat > -5 && Eheat < 30 && PulseWidth > 24 && PulseWidth<55","same");

chain->Draw("Eheat","LdbHeat < 0.35  && LdbHeat>0.1 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.15+0.0007*Eheat*Eheat)","same");
chain->Draw("Eheat","LdbHeat < 0.35  && LdbHeat>0.1 && Eheat > -5 && Eheat < 30 && Chi2Heat<(1.15+0.0007*Eheat*Eheat) && RiseTime > 5 && RiseTime<15 && PulseWidth>24 && PulseWidth<60","same");
// MAGNIFICO!!!! ESPECTACULAR!!!!

//*****************
// Part 2
//**************

// ID6

TChain* chain = new TChain("energytree","EdwHeatEnergy");
chain->AddFile("~/ID_Run12/BckgdS3/Data/eheat_ID6.root");
chain->AddFile("~/ID_Run12Bis/BckgdS3/Data/eheat_ID6.root");

chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<1000");
chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat)","same");

chain->Draw("PulseWidth:RiseTime","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > 4 && Eheat < 12 && Chi2Heat<100 && PulseWidth > 1 && PulseWidth < 200 && RiseTime>0.3 && RiseTime<50");
chain->Draw("PulseWidth:RiseTime","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > 4 && Eheat < 12 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat) && PulseWidth > 1 && PulseWidth < 200 && RiseTime>0.3 && RiseTime<50","same");

chain->Draw("RiseTime:Eheat","LdbHeat<0.75 && Eheat>-3 && Eheat<50 && Chi2Heat < 100");
chain->Draw("RiseTime:Eheat","LdbHeat<0.75 && Eheat>-3 && Eheat<50 && Chi2Heat < (1.3+8.e-4*Eheat*Eheat)","same");


// rajoutons l'ion et voyons toutes coupures
TChain* chion = new TChain("dsttree","EdwDST");
chion->AddFile("~/ID_Run12/BckgdS3/Data/dst_ID6.root");
chion->AddFile("~/ID_Run12Bis/BckgdS3/Data/dst_ID6.root");
chain->AddFriend(chion);

chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<1000");
chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat)","same");
chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat) && EventFlag >=2","same");
chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat) && RiseTime>1.5 && PulseWidth<50 && PulseWidth>-1+3.5*RiseTime","same");
chain->Draw("Eheat","LdbHeat < 0.75 && LdbHeat>0.1 && Eheat > -5 && Eheat < 20 && Chi2Heat<(1.3+8.e-4*Eheat*Eheat) && EventFlag >=1","same");


// Un test: pulse shape parameter en fonction de la position de l'electrode!
// On voit une difference!!
chain->Draw("PulseWidth","LdbHeat<0.75 && LdbHeat>0.1 && EventFlag==2 && Eion > 50 && Eion<500");
chain->Draw("PulseWidth","LdbHeat<0.75 && LdbHeat>0.1 && EventFlag==3 && Eion > 50 && Eion<500","same");
chain->Draw("PulseWidth","LdbHeat<0.75 && LdbHeat>0.1 && IonFlags[5]==1 && Eion > 50 && Eion<500","same");
