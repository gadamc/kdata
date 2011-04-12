
// Etude Gc2
// Jul 2010

string bolo="Gc2";

.x params_id_run12.C
.x start_idrun12bis_bckgd.C

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
chain->AddFile("~/ID_Run12/BckgdS4/Data/eheat_Gc2.root");
chain->AddFile("~/ID_Run12Bis/BckgdS4/Data/eheat_Gc2.root");

chain->Draw("Eheat","LdbHeat < 0.5 && Eheat > -10 && Eheat < 5000");

