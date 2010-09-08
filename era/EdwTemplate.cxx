/**
 @file    EdwTemplate.cc
 @author  Eric Armengaud, armengau@in2p3.fr
 @brief   Implementation of the EdwTemplate class. See the .h file
 */

#include "EdwTemplate.h"

ClassImp(EdwTemplate); /**< Root dictionnary */

EdwTemplate::EdwTemplate() : OffsetFFT() {
	
}

EdwTemplate::EdwTemplate(time_t aStart, time_t aEnd, vector<string> aRuns, string aChannel) : OffsetFFT() {
  fStartValidity = aStart;
  fEndValidity = aEnd;
  fChannel = aChannel;
  fBuildRuns = aRuns;
  fPulseBin = 0; fPulseAbsMax = 0; fPulseSign = 0;
}

EdwTemplate::EdwTemplate(time_t aStart, time_t aEnd, vector<string> aRuns, string aChannel, string aASCIIDir, string aPlotDir) : OffsetFFT() {
  fStartValidity = aStart;
  fEndValidity = aEnd;
  fChannel = aChannel;
  fBuildRuns = aRuns;
  fPulseBin = 0; fPulseAbsMax = 0; fPulseSign = 0;
  string lFile = aASCIIDir+"/template_"+fChannel+".txt";
  this->BuildTemplateFromASCIITrace(lFile);
  this->SetFilterParams();
  this->BuildOffsetFFTs();
  if (aPlotDir != "None") {
    this->PlotTrace(aPlotDir+"/tmplt_"+fChannel+".gif");
    this->PlotFilteredTrace(aPlotDir+"/tmplt_filt_"+fChannel+".gif");
    this->PlotExtraTrace(aPlotDir+"/tmplt_extra_"+fChannel+".gif");
  }
}

EdwTemplate::~EdwTemplate() {
}

void EdwTemplate::SetValidity(time_t aTStart, time_t aTEnd) {
	
  fStartValidity = aTStart;
  fEndValidity = aTEnd;
	
}

void EdwTemplate::Clear(const Option_t * /*opt*/) {
  fBuildRuns.clear();
  fTrace.clear();
  fEventsUsed.clear();
  fRunsOfEventsUsed.clear();
  fTraceFFT.clear();
}

void EdwTemplate::DumpInfo() const {
  cout << "--------------------------"<<endl;
  cout << "Template for "<<fChannel<<endl;
  cout << "Validity from "<<fStartValidity<<" to "<<fEndValidity<<endl;
  cout << "Build runs: ";
  for (UInt_t i=0; i<fBuildRuns.size(); i++) cout <<fBuildRuns[i]<<" ";
  cout << endl;
  cout << "Number of events used: "<<fEventsUsed.size()<<endl;
  cout << "Pulse sign: "<<fPulseSign<<" - Pulse abs max: "<<fPulseAbsMax<<endl;
  cout << "Trace size: "<<fTrace.size()<<" - Pulse bin: "<<fPulseBin<<endl;
}

void EdwTemplate::DumpTrace(string aFile) const {
  ofstream lFile(aFile.c_str(),ios::out);
  for (UInt_t i=0; i<fTrace.size(); i++)
    lFile << fTrace.at(i) << endl;
  lFile.close();
}

Bool_t EdwTemplate::IsHeat() {
  return isheat(fChannel);
}

void EdwTemplate::PlotTrace(string aPlotName) {
  Bool_t lLog = 0;
  plotvector(fTrace,aPlotName,lLog,"Template");
}

void EdwTemplate::PlotFilteredTrace(string aPlotName) {
  Bool_t lLog = 0;
  plotvector(fFilteredTrace,aPlotName,lLog,"Filtered template");
}

void EdwTemplate::PlotExtraTrace(string aPlotName) {
  Bool_t lLog = 0;
  plotvector(fExtrapolatedTrace,aPlotName,lLog,"Extrapolated template");
}

void EdwTemplate::ComputeFFT() {
  //  TH1F* lHisto = vect2hist(fTrace,"Template FFT");
  //  fTraceFFT = (TH1F*)(lHisto->FFT(NULL,"RE R2HC ES")) ; // special fft format
  //  fTraceFFT->SetName("Template FFT");
  //  delete lHisto;
  fTraceFFT.clear();
  fTraceFFT = EdwRealFFT(fTrace);
}

FloatVect EdwTemplate::TraceFFT() const {
  FloatVect v(fTraceFFT);
  return v;
}

void EdwTemplate::ExtrapolateTrace(const Float_t aExpoFitFraction, Bool_t aFitBaseline) {
	
  if (aExpoFitFraction <= 0 || aExpoFitFraction >= 1) cerr << "fit fraction not appropriate" << endl;
  UInt_t N = fTrace.size();
  UInt_t lStartBin = (UInt_t)(aExpoFitFraction*(Float_t)N);
  vector<Float_t> lCutTrace;
  for (UInt_t i=lStartBin; i<N; i++) lCutTrace.push_back(fTrace[i]);
  TH1F* lCutHist = vect2hist(lCutTrace);
	
  // Fit a fraction of the trace
  TF1* lFunc = new TF1("myfit","[0]*exp(-[1]*x)+[2]",0,N);
  if (!aFitBaseline) {
    lFunc->FixParameter(2,0);
  } else { // need to have an appropriate initialization of parameters
    lFunc->SetParameter(2,lCutTrace[lCutTrace.size()-1]);
    lFunc->SetParameter(0,lCutTrace[0]-lFunc->GetParameter(2));
    lFunc->SetParameter(1,1/lCutTrace.size());
  }
  lCutHist->Fit("myfit","QWN");
  Float_t K = lFunc->GetParameter(0);
  Float_t p1 = lFunc->GetParameter(1);
  Float_t offset = lFunc->GetParameter(2);
	
  // Compute the full "extrapolated" trace
  fExtrapolatedTrace.clear();
  fExtrapolatedTrace = fTrace;
  for (UInt_t i=N; i<2*N; i++) fExtrapolatedTrace.push_back(offset+K*exp(-p1*(i-lStartBin)));
	
  delete lFunc;
  delete lCutHist;
}

void EdwTemplate::BuildOffsetFFTs(Int_t aMinOffset, Int_t aMaxOffset, Int_t aBin) {
	
  Int_t N = (Int_t)fTrace.size();
  fTraceSize = (UInt_t)N;
  if (aMinOffset >= aMaxOffset) cerr << "BuildOffsetFFTs: Offsets in wrong order." <<endl;
  if (aMinOffset <= -N) cerr << "BuildOffsetFFTs: MinOffset is too small"<<endl;
  if (aMaxOffset >= N) cerr << "BuildOffsetFFTs: MaxOffset is too large"<<endl;
  fBinsOffsetFFT.clear();
  fTracesOffsetFFT.clear();
  if (fExtrapolatedTrace.size() == 0) this->ExtrapolateTrace(); // size 2N automatically
	
  vector<Float_t> lLongTrace = fExtrapolatedTrace; // Add zeros to the pretrigger
  lLongTrace.insert(lLongTrace.begin(),N,0); // Size 3N
	
  for (Int_t shift=aMinOffset; shift<=aMaxOffset; shift+= aBin) {
    vector<Float_t> lOffsetTrace;
    for (Int_t i=0; i<N; i++) lOffsetTrace.push_back(lLongTrace[-shift+N+i]);
    fBinsOffsetFFT.push_back(shift);
    fTracesOffsetFFT.push_back(EdwRealFFT(lOffsetTrace));
  }
	
}

void EdwTemplate::BuildOffsetFFTs() {
	
  if (this->IsHeat()) this->BuildOffsetFFTs(FFTMINOFFSET_HEAT,FFTMAXOFFSET_HEAT,FFTBINOFFSET);
  else this->BuildOffsetFFTs(FFTMINOFFSET_ION,FFTMAXOFFSET_ION,FFTBINOFFSET);
	
}

void EdwTemplate::SetFilterParams(vector<Float_t> aDirect, vector<Float_t> aInverse) {
  fDirectFilterCoefs = aDirect;
  fInverseFilterCoefs = aInverse;
  fFilteredTrace = EdwFilter(fTrace,aDirect,aInverse);
}

void EdwTemplate::SetFilterParams() {
  fDirectFilterCoefs.clear();
  fInverseFilterCoefs.clear();
  if (this->IsHeat()) {
    Float_t a[] = DEFAUT_FILTER_HEAT_DIRECT;
    Float_t b[] = DEFAUT_FILTER_HEAT_INVERSE;
    for (Int_t i=0; i<DEFAUT_FILTER_HEAT_DIRECT_SIZE; i++)
      fDirectFilterCoefs.push_back(a[i]);
    for (Int_t i=0; i<DEFAUT_FILTER_HEAT_INVERSE_SIZE; i++)
      fInverseFilterCoefs.push_back(b[i]);
  } else {
    Float_t a[] = DEFAUT_FILTER_ION_DIRECT;
    Float_t b[] = DEFAUT_FILTER_ION_INVERSE;
    for (Int_t i=0; i<DEFAUT_FILTER_ION_DIRECT_SIZE; i++)
      fDirectFilterCoefs.push_back(a[i]);
    for (Int_t i=0; i<DEFAUT_FILTER_ION_INVERSE_SIZE; i++)
      fInverseFilterCoefs.push_back(b[i]);
  }
  fFilteredTrace = EdwFilter(fTrace,fDirectFilterCoefs,fInverseFilterCoefs);
	
}

void EdwTemplate::BuildBasicTemplate(TChain* aChain, Float_t aAmpl, Int_t aBin, Float_t aDeltaBin) {
  // For all events, compute simple amplitudes and the corresponding bin of maximum/minimum.
  // Select a single event with amplitude near aAmpl and max/min bin not far from the center of the
  // trace (of order aDeltaBin). This will be the basic template.
	
  fPulseSign = (aAmpl > 0) ? 1 : -1;
  fTrace.clear();
  fEventsUsed.clear();
  fRunsOfEventsUsed.clear();
  EdwEvent *lEvent=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvent);
  EdwPulse* lEdwPulse = new EdwPulse();
  vector<Float_t> lAmpls, lPulseBins;
  vector<Long64_t> lEvtNums;
	
  // First amplitude estimate
  for (Long64_t i=0; i<aChain->GetEntries(); i++) {
		//  for (ULong_t i=0; i<5000; i++) {
    aChain->GetEntry(i) ;
    lEdwPulse = lEvent->Pulse(fChannel);
    if (lEdwPulse != NULL) {
      FitPulse lPulse(lEdwPulse);
      lPulse.SetSign(fPulseSign);
      if (lPulse.DetectSinglePulse() && !(lPulse.RemoveExpoBaseline(0))) {
				lEvtNums.push_back(i);
				lAmpls.push_back(lPulse.GetSimpleAmpl());
				lPulseBins.push_back(lPulse.GetSimpleAmplBin());
      }
    }
    lEvent->Clear();
  }
  if (!lAmpls.size()) cerr << "Template build: no good trace found for "<<fChannel<<endl;
	
  Float_t lShiftAmpl, lShiftBin, lBestDiscrim;
  vector<Float_t> lParam;
  ULong_t i_best = 0;
  lBestDiscrim = 1;
  // Find the best pulse with the criterium:
  // x1 = (ampl-ampl0)/ampl0; x2 = (t-mean)/deltabin
  // x1<0.5, x2<0.5, get min (x1+x2)
  for (ULong_t i=0; i<lAmpls.size(); i++) {
    lShiftAmpl = TMath::Abs((lAmpls[i]-aAmpl)/aAmpl) ;
    lShiftBin = TMath::Abs((lPulseBins[i]-aBin)/aDeltaBin);
    if (lShiftAmpl < 0.5 && lShiftBin < 0.5 && lShiftAmpl+lShiftBin < lBestDiscrim) {
      i_best = i ;
      lBestDiscrim = lShiftAmpl+lShiftBin;
    }
  }
	
  aChain->GetEntry(lEvtNums.at(i_best));
  fEventsUsed.push_back(lEvent->Header()->Num());
  fRunsOfEventsUsed.push_back(lEvent->Header()->Run());
  lEdwPulse = lEvent->Pulse(fChannel);
  FitPulse lPulse(lEdwPulse);
  lPulse.BasicPreprocess();
  // (comme ca on sauve le template depatterne et debase..)
  fTrace = lPulse.ProcessedTrace().Vect();
  this->ComputeFFT();
  if (KVERBOSE) cout << "Basic template built." << endl;
	
}

void EdwTemplate::BuildFitTemplate(TChain* aChain, Float_t aAmpl, Int_t aBin, Float_t aDeltaBin, Float_t aChi2PerDofCut, string aPlotPulses) {
	
  // First basic template if no other exists
  fPulseSign = (aAmpl > 0) ? 1 : -1;
  if (!fTrace.size()) this->BuildBasicTemplate(aChain,aAmpl,aBin,aDeltaBin);
	
  vector<Float_t> lBasicTmplt = fTrace;
  fTrace.clear();
  fEventsUsed.clear();
  fRunsOfEventsUsed.clear();
  EdwEvent *lEvent=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvent);
  EdwPulse* lEdwPulse = new EdwPulse();
  vector<Float_t> lShiftBins;
  vector<Long64_t> lEvtNums;
	
  // Cut events with "correct" raw amplitude / time shift / nb of pulses
  // Time fit of these events to basic template and get Chi2, ampl, t0.
  for (Long64_t i=0; i<aChain->GetEntries(); i++) {
    //  for (ULong_t i=0; i<5000; i++) {
    aChain->GetEntry(i) ;
    lEdwPulse = lEvent->Pulse(fChannel);
    if (lEdwPulse != NULL) {
      FitPulse lPulse(lEdwPulse);
      lPulse.SetSign(fPulseSign);
      if (lPulse.DetectSinglePulse() && !(lPulse.RemoveExpoBaseline(0))) {
				Float_t theAmpl = lPulse.GetSimpleAmpl();
				Float_t theBin = lPulse.GetSimpleAmplBin();
				Float_t lShiftAmpl = TMath::Abs((theAmpl-aAmpl)/aAmpl) ;
				Float_t lShiftBin = TMath::Abs((theBin-aBin)/aDeltaBin);
				if (lShiftAmpl < 0.2 && lShiftBin < 0.2) { // 20% tolerance
					// The pulse is selected to be fit.
					vector<Float_t> params = lPulse.GetFitAmpl(lBasicTmplt,1,1);	  
					Float_t lChi2PerDof = (aChi2PerDofCut == 0) ? -1 : params[2]/(lPulse.TraceSize());
					if (lChi2PerDof <= aChi2PerDofCut && TMath::Abs(params[0]-1) <= 0.2 &&
							TMath::Abs((params[1]-aBin)/aDeltaBin) <= 0.5) { 
						// 10% tolerance on the amplitude
						// and ask |time-t0|<0.5*delta_t
						//	    if (KVERBOSE) cout << "BuildFitTemplate: event "<<i<<" selected."<<endl;
						lEvtNums.push_back(i);
						lShiftBins.push_back(params[1]);
					}
				}
      }
    }
    lEvent->Clear();
    if (KVERBOSE && i%10000 == 0) cout << "BuildFitTemplate: event "<< i << endl;
  }
	
  // Time shift = use median delay as the reference, not the delay from the basic template!!!
  UInt_t NbEvts = lEvtNums.size();
  cout << NbEvts << " events used to build the template." << endl;
  if (NbEvts == 0) {
    cerr<< "Exiting." << endl; exit(-1);
  }
  Float_t lMed = vectmedian(lShiftBins);
  for (UInt_t i=0; i<NbEvts; i++) lShiftBins[i]-=lMed;
	
  // Now use selected events to build the template.
  //  TH1F* lTmplt = new TH1F();
  FloatVect lTmplt(0);
	
  for (UInt_t i=0; i<NbEvts; i++) {
    aChain->GetEntry(lEvtNums[i]);
    fEventsUsed.push_back(lEvent->Header()->Num());
    fRunsOfEventsUsed.push_back(lEvent->Header()->Run());
    FitPulse lPulse(lEvent->Pulse(fChannel));
    lPulse.BasicPreprocess();
    lPulse.TranslateTrace(-(Int_t)lShiftBins[i]);
    if (aPlotPulses != "") lPulse.PlotTrace(aPlotPulses+"_"+int2str(i)+".gif"); // convention nom plot
    if (i == 0) {
      //      delete lTmplt;
      //      lTmplt = new TH1F(*(lPulse.ProcessedTrace()));
      lTmplt = lPulse.ProcessedTrace();
    }
    else lTmplt.Add(lPulse.ProcessedTrace()); // no weighting by amplitude, that's what we want.
    lEvent->Clear();
  }
  lTmplt.Scale(1/(Float_t)NbEvts);  
	
  //  fTrace = hist2vect(lTmplt);
  //  delete lTmplt;
  fTrace = lTmplt.Vect();
  this->ComputeFFT();
  this->ExtrapolateTrace(); // expofitfraction a customizer..
  this->ComputePulseBin();
	
  // todo =  cut on baseline noise too!
}

void EdwTemplate::BuildTemplateFromASCIITrace(string aFile) {
	
  // Load the trace
  vector<Float_t> lLoadTrace; string line;
  ifstream lFile(aFile.c_str(),ios::in);
  if (!lFile) {
    cerr << "No ascii trace file found: " << aFile << endl; exit(-1);
  }
  while (getline(lFile,line)) {
    istringstream ss(line);
    Float_t f; ss >> f; lLoadTrace.push_back(f);
  }
  lFile.close();
	
  // Build the template from that trace
  Float_t lMin = vectmin(lLoadTrace);
  Float_t lMax = vectmax(lLoadTrace);
  fPulseSign = (TMath::Abs(lMin) > lMax) ? -1 : 1;
  fTrace = lLoadTrace;
  fEventsUsed.clear();
  fRunsOfEventsUsed.clear();
  this->ComputeFFT();
  this->ExtrapolateTrace();
  this->ComputePulseBin();
	
}


void EdwTemplate::ComputePulseBin() {
  fPulseBin = 0;
  fPulseAbsMax =0 ;
  fPulseSign = 0;
  for (UInt_t i=0; i<fTrace.size(); i++) {
    if (TMath::Abs(fTrace[i]) >= fPulseAbsMax) {
      fPulseBin = i; 
      fPulseAbsMax = TMath::Abs(fTrace[i]);
      if (fTrace[i] >= 0) fPulseSign = 1;
      else fPulseSign = -1;
    }
  }
}
