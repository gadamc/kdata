
gSystem->Load("~/EdwRootAna/lib/EraLib.so")

TFile f("~/SPSEdw/EdwRootAna/ID_Run12/BckgdS2/rootdbs/anadb_70.root","READ");

TTree* t = (TTree*)f.Get("PreprocessTree");
TBranch* br = t->GetBranch("PreprocessBranch");

EdwPreprocessData* data = new EdwPreprocessData();
br->SetAddress(&data);

br->GetEntry(5);

vector<Float_t> spec = data->NoiseSpectrumVector();

time_t ti=data->StartTime();
time_t tf=data->StartTime();
// see standard c library ctime.h to convert times
string channel=data->Channel();

data->DumpInfo();

// spectre de bruit: nb de pts = nb de points des traces
// obtenu a partir de N traces (de longueur M)
// 0<i<M-1 : spectre[i] = (1/N) Sum_{j} ([FFT(trace_{j})][i])^2
// * le vrai "spectre de bruit" a M/2 points
//   et est obtenu par bruit[i] ~ spectre[i]+spectre[M-i]:



vector<Float_t> EdwPowerHalf(vector<Float_t> aPower) {
  // calcule le power de taille N/2 pour un "power" de taille N
  // (pour le spectre de bruit)
  UInt_t N = aPower.size();
  vector<Float_t> powerhalf(1+(N/2),0);
  powerhalf[0]=aPower[0];
  powerhalf[N/2]=aPower[N/2];
  for (UInt_t k=1; k<N/2; k++) powerhalf[k] = aPower[k] + aPower[N-k];

  return powerhalf;
}
