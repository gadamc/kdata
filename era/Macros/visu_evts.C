
.L PlotEvent.C

string libdir="/afs/in2p3.fr/home/a/armengau/EdwRootAna/lib/";
string basedir="/sps/edelweis/EdwRootAna/ID_Run12Bis/GammaS1/";
string bolo="ID403";
string sambarun="kd06a029";
int sambanum=29628;
//long chainnum=61826;
long chainnum=-1;
bool plotfit=1;
bool remove_pattern=1;
PlotEvent(sambarun,sambanum,bolo,basedir,libdir,plotfit,chainnum,remove_pattern);
