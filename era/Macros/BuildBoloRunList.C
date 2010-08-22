
#include "ChannelName.C"

Float_t GetVoltage(string channel, string rawdbfile, string run) {
  // Recupere le champ VrelIonization de la rawdb pour un run et une voie donnees.

  Float_t volt=0;
  if (channel == "") return 0;
  string ch_id=channel;
  channel.replace(channel.find_first_of("_"),1," "); // dans la DB c'est avec des espaces
  ifstream file(rawdbfile.c_str(),ios::in);  
  string line;
  //  bool runfound=0;
  //  while (getline(file,line)) {
  system(("grep -b 'Run: "+run+"' "+rawdbfile+" | awk -F : '{print $1}' > tmptoto_"+ch_id+".txt").c_str());
  size_t pos;
  ifstream toto(("tmptoto_"+ch_id+".txt").c_str(),ios::in);
  toto >> pos; toto.close(); system(("rm -f tmptoto_"+ch_id+".txt").c_str());
  file.seekg(pos);
  getline(file,line);
  vector<Float_t> allvolts; Float_t thevolt;
  string bolo=BoloName(channel," ");
  string col1=ChannelName(bolo,"col1"," "); // option "no underscore"
  string col2=ChannelName(bolo,"col2"," ");
  string vet1=ChannelName(bolo,"vet1"," ");
  string vet2=ChannelName(bolo,"vet2"," ");
  string gar1=ChannelName(bolo,"gar1"," ");
  string gar2=ChannelName(bolo,"gar2"," ");
  // Now we use only the field Vionization of the rawdb
  // (because VrelIon is temporarily bugged)
  // and reconstruct the relative voltage..
  while (getline(file,line)) {
    size_t pos = line.find("Vioniz");
    if (pos != string::npos) {
      if (line.find(bolo) != string::npos) {
	string thevoie=line.substr(2,line.find("- ")-3);
	if (thevoie == channel) {
	  //      if (line.find(channel) != string::npos) {
	  //	size_t pos = line.find("Vionization");
	  size_t pos1 = line.find("=",pos);
	  size_t pos2 = line.find("- ",pos1);
	  string ll = line.substr(pos1+1,pos2-pos1-2);
	  istringstream ss(ll); ss >> volt;
	  // RUSTINE A 2 BALLES...
	  if (bolo == "GGA4") volt=-7;
	}
	//    if (line.find(bolo) != string::npos && line.find("chaleur")==string::npos) {
	// Ca ca fout la merde pour eg. ID4/ID404
	if (thevoie == col1 || thevoie == col2 || thevoie==vet1 ||
	    thevoie==vet2 || thevoie==gar1 || thevoie==gar2) {
	  //	size_t pos = line.find("Vionization");
	  size_t pos1 = line.find("=",pos);
	  size_t pos2 = line.find("- ",pos1);
	  string ll = line.substr(pos1+1,pos2-pos1-2);
	  istringstream ss(ll); ss >> thevolt; 
	  if (bolo == "GGA4") thevolt=-7;
	  allvolts.push_back(thevolt);
	}
      }
    }
    if (line.find("------------") != string::npos) break;
  }
  file.close();
  if (IsID(bolo)) {
    Float_t meanvolt=0;
    for (int p=0; p<allvolts.size();p++) meanvolt+=allvolts[p];
    meanvolt/=allvolts.size();
    volt-=meanvolt;
  }

  return volt;
}


Int_t BuildBoloRunList(string bolo, string perioddir) {
  
  // Build ASCII raw db
  string rootana=ERAPATH+"/bin/edwrootana.exe";
  string tmpparamfile=perioddir+"/tmpparams_"+bolo+".txt";
  ofstream tmppars(tmpparamfile.c_str(),ios::out);
  tmppars << "gProcessingStep = DisplayRawDB" << endl;
  tmppars << "gBaseDir = "+perioddir<<endl;
  tmppars << "gRawDBFile = "+perioddir+"/rawdb.root" <<endl;
  tmppars.close();
  string asciidb = perioddir+"/rawdb_dump.txt";
  system((rootana+" "+tmpparamfile+" > "+asciidb).c_str());
  system(("rm -f "+tmpparamfile).c_str());

  string rawrunlist=perioddir+"/runlist.txt";
  string bolorunlist=perioddir+"/runlist_"+bolo+".txt";
  ifstream rawrunstr(rawrunlist.c_str(),ios::in);
  string therun;
  while (rawrunstr >> therun) {
    string line; bool runfound=0;
    ifstream bolorunstr_i(bolorunlist.c_str(),ios::in);
    if (bolorunstr_i.is_open()) {
      while (getline(bolorunstr_i,line)) {
	if (line.find(therun) != string::npos) runfound=1;
      }
      bolorunstr_i.close();
    } else system(("touch "+bolorunlist).c_str());
    if (!runfound) { // Add the run to the file
      Float_t v_col1=GetVoltage(ChannelName(bolo,"col1"),asciidb,therun);
      Float_t v_col2=GetVoltage(ChannelName(bolo,"col2"),asciidb,therun);
      Float_t v_vet1=GetVoltage(ChannelName(bolo,"vet1"),asciidb,therun);
      Float_t v_vet2=GetVoltage(ChannelName(bolo,"vet2"),asciidb,therun);
      Float_t v_gar1=GetVoltage(ChannelName(bolo,"gar1"),asciidb,therun);
      Float_t v_gar2=GetVoltage(ChannelName(bolo,"gar2"),asciidb,therun);
      Int_t voltflag=-1;
      for (int p=0; p<gNbVoltages; p++) {
	// Pour les gardes en FID normalement il teste 0=0...
	Float_t eps=0.0001;
	if (fabs(v_col1-gVref_Col1[p])<eps && fabs(v_col2-gVref_Col2[p])<eps 
	    && fabs(v_vet1-gVref_Vet1[p])<eps && fabs(v_vet2-gVref_Vet2[p])<eps 
	    && fabs(v_gar1-gVref_Gar1[p])<eps && fabs(v_gar2-gVref_Gar2[p])<eps)
	  voltflag=p;
      }
      if (voltflag == -1) {
	cout << "** Voltage pbl for "<<therun<<endl;
	cout << v_col1<<" "<< v_col2<<endl;
	cout << v_vet1<<" "<< v_vet2<<endl;
	cout << v_gar1<<" "<< v_gar2<<endl;
      }
      ofstream bolorunstr_o(bolorunlist.c_str(),ios::out | ios::app);
      if (bolorunstr_o.is_open()) {
	bolorunstr_o << therun <<" "<<voltflag<<endl;
	bolorunstr_o.close();
      } else cout <<"jj"<<endl;
    }
  }

  return 0;
}
