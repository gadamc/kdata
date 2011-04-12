
string* FillRunList(const string file) {
  ifstream ff(file.c_str(),ios::in);
  if (!ff) cout << "Error reading "<<file<<endl;

  string ss; int nblines=0;
  while (ff >> ss) nblines++;
  ff.close();
  cout << nblines<<" runs"<<endl;
  string* runs = new string[nblines];
  ifstream f(file.c_str(),ios::in);
  int i=0;
  while (f >> ss) {
    runs[i] = ss;
    i++;
  }
  f.close();
  return runs;
}

Int_t* FillPolarSignList(const string file) {
  ifstream ff(file.c_str(),ios::in);
  if (!ff) cout << "Error reading "<<file<<endl;

  string line; int nblines=0;
  while (getline(ff,line)) nblines++;
  ff.close();
  cout << nblines<<" runs"<<endl;
  Int_t* runsigns = new Int_t[nblines];
  ifstream f(file.c_str(),ios::in);
  Int_t thesign; string therun;
  Int_t i=0;
  while (getline(f,line)) {
    istringstream ss(line); ss >> therun; ss >> thesign;
    runsigns[i]=thesign;
    i++;
  }
  f.close();
  return runsigns;
}
