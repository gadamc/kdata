
// Mise en commun des fcts d'acceptance, qplots etc des differents bolos

// S1: FID401, ID403, ID5
// S2: FID402, ID401, ID3
// S3: ID6, ID404, ID4
// S4: ID402, ID405, ID2

string basedir="/Users/armengau/Data/Edelweiss/ID_Run12/";
vector<string> bolos, macs; vector<float> efficiencies;

// Select here the list of bolometers
bolos.push_back("ID403"); macs.push_back("S1"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID5"); macs.push_back("S1"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID401"); macs.push_back("S2"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID3"); macs.push_back("S2"); efficiencies.push_back(1);//0.5);
bolos.push_back("ID6"); macs.push_back("S3"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID404"); macs.push_back("S3"); efficiencies.push_back(1);//0.95);
//bolos.push_back("ID4"); macs.push_back("S3");
bolos.push_back("ID402"); macs.push_back("S4"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID405"); macs.push_back("S4"); efficiencies.push_back(1);//0.95);
bolos.push_back("ID2"); macs.push_back("S4"); efficiencies.push_back(1);//0.95);

// Recup aussi les data du run 10 - attention a la syntaxe a la con.. 
//(indexation par polar et pas par mac)
string basedir2="/Users/armengau/Data/Edelweiss/ID_Run10/Bckgd4V";
string basedir3="/Users/armengau/Data/Edelweiss/ID_Run10/Bckgd8V";

AcceptGlobal(bolos,macs,efficiencies,basedir,basedir2,basedir3);

bool display_coinc=1;
float erec_min=20;
QplotGlobal(bolos,macs,basedir,basedir2,basedir3,display_coinc,erec_min);

BckgdEvolGlobal(bolos,macs,basedir);


// Calib gamma..
vector<string> qplotfiles;
qplotfiles.push_back("ID_Run12/GammaS1/Data/qplot_ID403.txt");
qplotfiles.push_back("ID_Run12/GammaS1/Data/qplot_ID5.txt");
qplotfiles.push_back("ID_Run12/GammaS2/Data/qplot_ID3.txt");
qplotfiles.push_back("ID_Run12/GammaS2/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run12/GammaS3/Data/qplot_ID4.txt");
qplotfiles.push_back("ID_Run12/GammaS3/Data/qplot_ID404.txt");
qplotfiles.push_back("ID_Run12/GammaS3/Data/qplot_ID6.txt");
qplotfiles.push_back("ID_Run12/GammaS4/Data/qplot_ID402.txt");
qplotfiles.push_back("ID_Run12/GammaS4/Data/qplot_ID405.txt");
qplotfiles.push_back("ID_Run12/GammaS4/Data/qplot_ID2.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS1/Data/qplot_ID403.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS1/Data/qplot_ID5.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS2/Data/qplot_ID3.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS2/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS3/Data/qplot_ID4.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS3/Data/qplot_ID404.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS3/Data/qplot_ID6.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS4/Data/qplot_ID2.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS4/Data/qplot_ID402.txt");
qplotfiles.push_back("ID_Run12Bis/GammaS4/Data/qplot_ID405.txt");
qplotfiles.push_back("ID_Run10/Gamma/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run10/Gamma/Data/qplot_ID3.txt");

QplotGlobal(qplotfiles);

// Fond : nouveau
vector<string> qplotfiles;
qplotfiles.push_back("ID_Run12/BckgdS1/Data/qplot_ID403.txt");
qplotfiles.push_back("ID_Run12/BckgdS1/Data/qplot_ID5.txt");
qplotfiles.push_back("ID_Run12/BckgdS2/Data/qplot_ID3.txt");
qplotfiles.push_back("ID_Run12/BckgdS2/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run12/BckgdS3/Data/qplot_ID4.txt");
qplotfiles.push_back("ID_Run12/BckgdS3/Data/qplot_ID404.txt");
qplotfiles.push_back("ID_Run12/BckgdS3/Data/qplot_ID6.txt");
qplotfiles.push_back("ID_Run12/BckgdS4/Data/qplot_ID402.txt");
qplotfiles.push_back("ID_Run12/BckgdS4/Data/qplot_ID2.txt");
qplotfiles.push_back("ID_Run12/BckgdS4/Data/qplot_ID405.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS1/Data/qplot_ID403.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS1/Data/qplot_ID5.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS2/Data/qplot_ID3.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS2/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS3/Data/qplot_ID4.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS3/Data/qplot_ID404.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS3/Data/qplot_ID6.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS4/Data/qplot_ID2.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS4/Data/qplot_ID402.txt");
qplotfiles.push_back("ID_Run12Bis/BckgdS4/Data/qplot_ID405.txt");
qplotfiles.push_back("ID_Run10/Bckgd/Data/qplot_ID401.txt");
qplotfiles.push_back("ID_Run10/Bckgd/Data/qplot_ID3.txt");
QplotGlobal(qplotfiles);

vector<string> expofiles;
expofiles.push_back("ID_Run12/BckgdS1/Data/periods/period_list_ID403.txt");
expofiles.push_back("ID_Run12/BckgdS1/Data/periods/period_list_ID5.txt");
expofiles.push_back("ID_Run12/BckgdS2/Data/periods/period_list_ID3.txt");
expofiles.push_back("ID_Run12/BckgdS2/Data/periods/period_list_ID401.txt");
expofiles.push_back("ID_Run12/BckgdS3/Data/periods/period_list_ID4.txt");
expofiles.push_back("ID_Run12/BckgdS3/Data/periods/period_list_ID404.txt");
expofiles.push_back("ID_Run12/BckgdS3/Data/periods/period_list_ID6.txt");
expofiles.push_back("ID_Run12/BckgdS4/Data/periods/period_list_ID402.txt");
expofiles.push_back("ID_Run12/BckgdS4/Data/periods/period_list_ID2.txt");
expofiles.push_back("ID_Run12/BckgdS4/Data/periods/period_list_ID405.txt");
expofiles.push_back("ID_Run12Bis/BckgdS1/Data/periods/period_list_ID403.txt");
expofiles.push_back("ID_Run12Bis/BckgdS1/Data/periods/period_list_ID5.txt");
expofiles.push_back("ID_Run12Bis/BckgdS2/Data/periods/period_list_ID3.txt");
expofiles.push_back("ID_Run12Bis/BckgdS2/Data/periods/period_list_ID401.txt");
expofiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/period_list_ID4.txt");
expofiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/period_list_ID404.txt");
expofiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/period_list_ID6.txt");
expofiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/period_list_ID2.txt");
expofiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/period_list_ID402.txt");
expofiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/period_list_ID405.txt");
expofiles.push_back("ID_Run10/Bckgd/Data/periods/period_list_ID401.txt");
expofiles.push_back("ID_Run10/Bckgd/Data/periods/period_list_ID3.txt");

vector<string> acceptfiles;
acceptfiles.push_back("ID_Run12/BckgdS1/Data/periods/acceptance_ID403.txt");
acceptfiles.push_back("ID_Run12/BckgdS1/Data/periods/acceptance_ID5.txt");
acceptfiles.push_back("ID_Run12/BckgdS2/Data/periods/acceptance_ID3.txt");
acceptfiles.push_back("ID_Run12/BckgdS2/Data/periods/acceptance_ID401.txt");
acceptfiles.push_back("ID_Run12/BckgdS3/Data/periods/acceptance_ID4.txt");
acceptfiles.push_back("ID_Run12/BckgdS3/Data/periods/acceptance_ID404.txt");
acceptfiles.push_back("ID_Run12/BckgdS3/Data/periods/acceptance_ID6.txt");
acceptfiles.push_back("ID_Run12/BckgdS4/Data/periods/acceptance_ID402.txt");
acceptfiles.push_back("ID_Run12/BckgdS4/Data/periods/acceptance_ID2.txt");
acceptfiles.push_back("ID_Run12/BckgdS4/Data/periods/acceptance_ID405.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS1/Data/periods/acceptance_ID403.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS1/Data/periods/acceptance_ID5.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS2/Data/periods/acceptance_ID3.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS2/Data/periods/acceptance_ID401.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/acceptance_ID4.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/acceptance_ID404.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS3/Data/periods/acceptance_ID6.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/acceptance_ID2.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/acceptance_ID402.txt");
acceptfiles.push_back("ID_Run12Bis/BckgdS4/Data/periods/acceptance_ID405.txt");
acceptfiles.push_back("ID_Run10/Bckgd/Data/periods/acceptance_ID401.txt");
acceptfiles.push_back("ID_Run10/Bckgd/Data/periods/acceptance_ID3.txt");

AcceptGlobal(expofiles,acceptfiles);
