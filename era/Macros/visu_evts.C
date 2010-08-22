
.L PlotEvent.C

string libdir="/afs/in2p3.fr/home/a/armengau/EdwRootAna/lib/";
string basedir="/sps/edelweis/EdwRootAna/ID_Run12/BckgdS1/";
bool plotfit=1;
bool remove_pattern=1;
string bolo="ID5";
string sambarun="jf27a013";
int sambanum=364;
//long chainnum=923248;
long chainnum=-1;

PlotEvent(sambarun,sambanum,bolo,basedir,libdir,plotfit,chainnum,remove_pattern);
