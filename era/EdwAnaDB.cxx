/**
   @file    EdwAnaDB.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwAnaDB class. See the .h file
*/

#include "EdwAnaDB.h"

ClassImp(EdwAnaDB); /**< Root dictionnary */

EdwAnaDB::EdwAnaDB() : EdwDB() {

  fTemplate = new EdwTemplate();
  fPreprocess = new EdwPreprocessData();
  fQuality = new EdwQualityFlags();

  fTemplateTree = new TTree("TemplateTree","TemplateTree");
  fTemplateTree->SetDirectory(fFile);
  fPreprocessTree = new TTree("PreprocessTree","PreprocessTree");
  fPreprocessTree->SetDirectory(fFile);
  fQualityTree = new TTree("QualityTree","QualityTree");
  fQualityTree->SetDirectory(fFile);

  fTemplateTree->Branch("TemplateBranch","EdwTemplate",&fTemplate);
  fTemplateBranch = fTemplateTree->GetBranch("TemplateBranch");
  fPreprocessTree->Branch("PreprocessBranch","EdwPreprocessData",&fPreprocess);
  fPreprocessBranch = fPreprocessTree->GetBranch("PreprocessBranch");
  fQualityTree->Branch("QualityBranch","EdwQualityFlags",&fQuality);
  fQualityBranch = fQualityTree->GetBranch("QualityBranch");
}

EdwAnaDB::EdwAnaDB(string aFile, string aRead) : EdwDB(aFile,aRead) {

  fTemplate = new EdwTemplate();
  fPreprocess = new EdwPreprocessData();
  fQuality = new EdwQualityFlags();

  Bool_t TmpltDone = 0;
  Bool_t PreprocDone = 0;
  Bool_t QualDone = 0;
  if (fFile->Get("TemplateTree")) {
    fTemplateTree = (TTree*)fFile->Get("TemplateTree");
  } else {
    fTemplateTree = new TTree("TemplateTree","TemplateTree");
    fTemplateTree->SetDirectory(fFile);
    fTemplateTree->Branch("TemplateBranch","EdwTemplate",&fTemplate);
    TmpltDone = 1;
  }
  if (fFile->Get("PreprocessTree")) {
    fPreprocessTree = (TTree*)fFile->Get("PreprocessTree");
  } else {
    fPreprocessTree = new TTree("PreprocessTree","PreprocessTree");
    fPreprocessTree->SetDirectory(fFile);
    fPreprocessTree->Branch("PreprocessBranch","EdwPreprocessData",&fPreprocess);
    PreprocDone = 1;
  }
  if (fFile->Get("QualityTree")) {
    fQualityTree = (TTree*)fFile->Get("QualityTree");
  } else {
    fQualityTree = new TTree("QualityTree","QualityTree");
    fQualityTree->SetDirectory(fFile);
    fQualityTree->Branch("QualityBranch","EdwQualityFlags",&fQuality);
    QualDone = 1;
  }

  fTemplateBranch = fTemplateTree->GetBranch("TemplateBranch");
  fPreprocessBranch = fPreprocessTree->GetBranch("PreprocessBranch");
  fQualityBranch = fQualityTree->GetBranch("QualityBranch");
  if (!fTemplateBranch || !fPreprocessBranch || !fQualityBranch) {
    cerr << "EdwRawDB: at least one missing branch in file. Exiting."<< endl; exit(-1);
  }
  if (!TmpltDone) fTemplateBranch->SetAddress(&fTemplate);
  if (!PreprocDone) fPreprocessBranch->SetAddress(&fPreprocess);
  if (!QualDone) fQualityBranch->SetAddress(&fQuality);
  //  if (aRead == "READ") {
  //    fTemplateBranch->SetAddress(&fTemplate);
  //    fPreprocessBranch->SetAddress(&fPreprocess);
  //    fQualityBranch->SetAddress(&fQuality);
  //  }

}

EdwAnaDB::~EdwAnaDB() {
  // Un test... a voir!! Ca peut jouer des tours.. mais ca aide bcp apparemment.
  delete fTemplate;
  delete fPreprocess;
  delete fQuality;
}

void EdwAnaDB::FillFromDBs(vector<string> aDBs) {

  for (UInt_t idb=0; idb<aDBs.size(); idb++) {
    EdwAnaDB* lDB = new EdwAnaDB(aDBs[idb],"READ");
    for (UInt_t j=0; j<lDB->fTemplateBranch->GetEntries(); j++) {
      lDB->fTemplateBranch->GetEntry(j);
      EdwTemplate lTemplate = *(lDB->fTemplate);
      fTemplate = &lTemplate;
      fFile->cd(); fTemplateBranch->Fill();
      fFile->cd(); fTemplateTree->Write("",kOverwrite);
    }
    for (UInt_t j=0; j<lDB->fPreprocessBranch->GetEntries(); j++) {
      lDB->fPreprocessBranch->GetEntry(j);
      EdwPreprocessData lPreprocess = *(lDB->fPreprocess);
      fPreprocess = &lPreprocess;
      fFile->cd(); fPreprocessBranch->Fill();
      fFile->cd(); fPreprocessTree->Write("",kOverwrite);
    }
    for (UInt_t j=0; j<lDB->fQualityBranch->GetEntries(); j++) {
      lDB->fQualityBranch->GetEntry(j);
      EdwQualityFlags lQuality = *(lDB->fQuality);
      fQuality = &lQuality;
      fFile->cd(); fQualityBranch->Fill();
      fFile->cd(); fQualityTree->Write("",kOverwrite);
    }
    delete lDB;
  }

}

void EdwAnaDB::FillFromDBsAndRuns(vector<string> aDBs, vector<string> aRunList) {

  for (UInt_t idb=0; idb<aDBs.size(); idb++) {
    EdwAnaDB* lDB = new EdwAnaDB(aDBs[idb],"READ");
    for (UInt_t j=0; j<lDB->fTemplateBranch->GetEntries(); j++) {
      lDB->fTemplateBranch->GetEntry(j);
      EdwTemplate lTemplate = *(lDB->fTemplate);
      fTemplate = &lTemplate;
      fFile->cd(); fTemplateBranch->Fill();
      fFile->cd(); fTemplateTree->Write("",kOverwrite);
    }
    for (UInt_t j=0; j<lDB->fPreprocessBranch->GetEntries(); j++) {
      lDB->fPreprocessBranch->GetEntry(j);
      EdwPreprocessData lPreprocess = *(lDB->fPreprocess);
      bool ok=0;
      if (lPreprocess.Run() == "") ok=1;
      for (UInt_t irun=0; irun<aRunList.size(); irun++) {
	if (aRunList[irun] == lPreprocess.Run()) ok=1;
      }
      if (ok) {
	fPreprocess = &lPreprocess;
	fFile->cd(); fPreprocessBranch->Fill();
	fFile->cd(); fPreprocessTree->Write("",kOverwrite);
      }
    }
    for (UInt_t j=0; j<lDB->fQualityBranch->GetEntries(); j++) {
      lDB->fQualityBranch->GetEntry(j);
      EdwQualityFlags lQuality = *(lDB->fQuality);
      fQuality = &lQuality;
      fFile->cd(); fQualityBranch->Fill();
      fFile->cd(); fQualityTree->Write("",kOverwrite);
    }
    delete lDB;
  }

}

void EdwAnaDB::FillFromASCIITemplates(vector<string> aChannels, string aASCIIDir, string aPlotDir) {
  vector<string> RunList;
  RunList.push_back("dummyrun");
  for (UInt_t i=0; i<aChannels.size(); i++) {
    EdwTemplate lTmplt(0,time(NULL),RunList,aChannels[i],aASCIIDir,aPlotDir);
    this->FillTemplate(lTmplt);
    cout << "Filled DB from template for "<<aChannels[i]<<endl;
  }
}

void EdwAnaDB::FillTemplate(EdwTemplate aTemplate) {
  fTemplate = &aTemplate;
  fFile->cd(); fTemplateBranch->Fill();
  fFile->cd(); fTemplateTree->Write("",kOverwrite);
}

void EdwAnaDB::FillTemplates(vector<EdwTemplate> aTemplates) {
  for (unsigned int i=0; i<aTemplates.size(); i++) {
    fTemplate = &(aTemplates.at(i));
    fFile->cd(); fTemplateBranch->Fill();
  }
  fFile->cd(); fTemplateTree->Write("",kOverwrite);
}

void EdwAnaDB::FillPreprocess(EdwPreprocessData aPreprocess) {
  fPreprocess = &aPreprocess;
  fFile->cd(); fPreprocessBranch->Fill();
  fFile->cd(); fPreprocessTree->Write("",kOverwrite);
}

void EdwAnaDB::FillPreprocess(vector<EdwPreprocessData> aPreprocess) {
  for (UInt_t i=0; i<aPreprocess.size(); i++) {
    fPreprocess = &(aPreprocess[i]);
    fFile->cd(); fPreprocessBranch->Fill();
  }
  fFile->cd(); fPreprocessTree->Write("",kOverwrite);
}

EdwTemplate* EdwAnaDB::GetTemplate(time_t aTime, string aChannel) {

  EdwTemplate* lTemplate = NULL;
  vector<unsigned int> indices;
  for (unsigned int i=0; i<fTemplateBranch->GetEntries(); i++) {
    fTemplateBranch->GetEntry(i);
    if (fTemplate->StartValidity() <= aTime &&
	fTemplate->EndValidity() >= aTime &&
	fTemplate->Channel() == aChannel) indices.push_back(i);
    fTemplate->Clear();
  }
  if (indices.size() == 0) {
    cerr << "no good template found!" << endl;
    return NULL;
    //    exit(2);
  } else if (indices.size() >= 2) {
    cerr << "several good templates found!" << endl;
    return NULL;
  } else {
    fTemplateBranch->GetEntry(indices.at(0));
    lTemplate = fTemplate;
  }

  return lTemplate;
}

EdwTemplate EdwAnaDB::GetTemplate(string aChannel) const {

  EdwTemplate lTemplate;
  vector<UInt_t> indices;

  fTemplateTree->SetBranchStatus("*",0);
  fTemplateTree->SetBranchStatus("fChannel",1);
  for (UInt_t i=0; i<fTemplateBranch->GetEntries(); i++) {
    fTemplateBranch->GetEntry(i);
    if (fTemplate->Channel() == aChannel) indices.push_back(i);
    fTemplate->Clear();
  }
  fTemplateTree->SetBranchStatus("*",1);

  if (indices.size() == 1) {
    fTemplateBranch->GetEntry(indices[0]);
    lTemplate = *fTemplate;
  } else {
    cerr << "no good template found, exiting: "<<indices.size()<< endl; exit(-1);
  }

  return lTemplate;
}

EdwPreprocessData* EdwAnaDB::GetPreprocessPtr(time_t aTime, string aChannel) {

  EdwPreprocessData* lPreprocess = NULL;
  vector<unsigned int> indices;
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    if (fPreprocess->StartTime() <= aTime &&
	fPreprocess->EndTime() >= aTime &&
	fPreprocess->Channel() == aChannel) indices.push_back(i);
    fPreprocess->Clear();
  }
  if (indices.size() == 0) {
    cerr << "no good preprocessdata found!" << endl;
    return NULL;
    //    exit(2);
  } else if (indices.size() >= 2) {
    cerr << "several good preprocessdata found!" << endl;
    return NULL;
  } else {
    fPreprocessBranch->GetEntry(indices.at(0));
    lPreprocess = fPreprocess;
  }

  return lPreprocess;
}

EdwPreprocessData EdwAnaDB::GetPreprocess(time_t aTime, string aChannel) {

  EdwPreprocessData lPreprocess;
  vector<unsigned int> indices;

  fPreprocessTree->SetBranchStatus("*",0);
  fPreprocessTree->SetBranchStatus("fChannel",1);
  fPreprocessTree->SetBranchStatus("fStartTime",1);
  fPreprocessTree->SetBranchStatus("fEndTime",1);
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    if (fPreprocess->StartTime() <= aTime &&
	fPreprocess->EndTime() >= aTime &&
	fPreprocess->Channel() == aChannel) indices.push_back(i);
    fPreprocess->Clear();
  }

  if (indices.size() == 0) {
    cerr << "No good preprocessdata found for "<<aChannel<<" at "<<aTime<<endl;
    Int_t i_best = -1;
    Float_t delta_t = 1e10;
    for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
      fPreprocessBranch->GetEntry(i);
      if (fPreprocess->Channel() == aChannel) {
	Float_t ta = TMath::Abs((Float_t)aTime - (Float_t)fPreprocess->StartTime());
	Float_t tb = TMath::Abs((Float_t)aTime - (Float_t)fPreprocess->EndTime());
	if (ta < delta_t) {
	  delta_t = ta; i_best = (Int_t)i;
	}
	if (tb < delta_t) {
	  delta_t = tb; i_best = (Int_t)i;
	}
      }
    }
    if (i_best == -1) cerr << "Warning: no preprocess at all for "<<aChannel<<endl;
    else indices.push_back((UInt_t)i_best);
  } else if (indices.size() >= 2) {
    cerr << "Several good preprocessdata found for "<<aChannel<<" at "<<aTime<<endl;
  }

  fPreprocessTree->SetBranchStatus("*",1);
  if (indices.size() != 0) {
    fPreprocessBranch->GetEntry(indices.at(0));
    lPreprocess = *fPreprocess;
  }
  
  return lPreprocess;
}

EdwQualityFlags* EdwAnaDB::GetQuality(time_t aTime, string aChannel) {

  EdwQualityFlags* lQuality = NULL;
  vector<unsigned int> indices;
  for (UInt_t i=0; i<fQualityBranch->GetEntries(); i++) {
    fQualityBranch->GetEntry(i);
    if (fQuality->StartTime() <= aTime &&
	fQuality->EndTime() >= aTime &&
	fQuality->Channel() == aChannel) indices.push_back(i);
  }
  if (indices.size() == 0) {
    cerr << "no good qualityflags found!" << endl;
    //    exit(2);
  } else if (indices.size() >= 2) {
    cerr << "several good qualityflags found!" << endl;
  } else {
    fQualityBranch->GetEntry(indices.at(0));
    lQuality = fQuality;
  }

  return lQuality;
}

void EdwAnaDB::BasicFillPreprocessData(TChain* aChain, vector<string> aChannels, vector<Int_t> aBins, vector<Int_t> aPatterns, time_t aStart, time_t aStop, string aPlotSpectrum) {

  UInt_t NbChannels = aChannels.size();
  if (aBins.size() != NbChannels && aBins.size() != 0) cerr << "Basicfillpreprocess: mismatch bin vector size" << endl;
  if (aPatterns.size() != NbChannels && aPatterns.size() != 0) cerr << "Basicfillpreprocess: mismatch pattern vector size" << endl;
  for (UInt_t i=0; i<NbChannels; i++) {
    fPreprocess->Clear();
    string lChannel = aChannels[i];
    fPreprocess->SetStartTime(aStart);
    fPreprocess->SetEndTime(aStop);
    fPreprocess->SetChannel(lChannel);

    // Default setting for channels to correlate
    vector<string> lCorrelChannels;
    Bool_t lIsHeat = isheat(lChannel);
    for (UInt_t j=0; j<NbChannels; j++) {
      if (isheat(aChannels[j]) == lIsHeat && aChannels[j] != lChannel)
	lCorrelChannels.push_back(lChannel);
    }
    fPreprocess->SetCorrelChannels(lCorrelChannels);
    // Noise spectrum computation
    // First get pulse bin for correct baseline preprocessing, depending on the channel
    if (aBins.size() == 0) fPreprocess->SetPulseBin((this->GetTemplate(lChannel)).PulseBin());
    // Pattern also. Only one pattern as of now.
    else fPreprocess->SetPulseBin(aBins[i]);
    vector<Int_t> patt(1,0);
    if (aPatterns.size() != 0) patt[0]=aPatterns[i];
    fPreprocess->SetNbPtsPatterns(patt);
    //    Int_t lBin = (this->GetTemplate(lChannel)).PulseBin();
    //    fPreprocess->SetPulseBin(lBin);
    fPreprocess->ComputeNoiseSpectrum(aChain);
    if (aPlotSpectrum != "") fPreprocess->PlotSpectrum(aPlotSpectrum+"_"+lChannel+".gif");
    //    fPreprocess->SetFrequencyWeights(this->ComputeFrequencyWeights(lChannel));
    // a gerer plus tard...

    fFile->cd(); fPreprocessBranch->Fill();
    fFile->cd(); fPreprocessTree->Write("",kOverwrite);
  }

}

vector<Float_t> EdwAnaDB::ComputeFrequencyWeights(string aChannel) const {
  // The computation is made using the *current* fPreprocess.
  if (fPreprocess->Channel() != aChannel) cerr << "computefreqweight big pbl!" << endl;
  EdwTemplate lTemplate = this->GetTemplate(aChannel);
  vector<Float_t> lNoisePower = EdwPowerHalf((fPreprocess->NoiseSpectrum()).Vect());
  vector<Float_t> lTemplatePower = EdwFFTPower(lTemplate.VectTraceFFT());
  FloatVect den(lNoisePower); FloatVect num(lTemplatePower);
  num.Divide(den);
  return num.Vect();
}

void EdwAnaDB::PlotWienerSpectra(string aChannel, string aPlotName) const {

  EdwTemplate lTemplate = this->GetTemplate(aChannel);
  vector<Float_t> lTemplatePower = EdwFFTPower(lTemplate.VectTraceFFT());
  for (UInt_t k=0; k<lTemplatePower.size(); k++) lTemplatePower[k]=sqrt(lTemplatePower[k]);
  TH1F* lTemplHist = vect2hist(lTemplatePower,"Pulse shape");
  lTemplHist->Scale(1./lTemplHist->GetMaximum());
  lTemplHist->SetLineColor(4);
  lTemplHist->SetStats(0);
  lTemplHist->GetXaxis()->SetRangeUser(1.5,lTemplatePower.size());

  Int_t num_noise = 0;
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    if (fPreprocess->Channel() == aChannel) {
      num_noise++;
      vector<Float_t> lNoisePower = EdwPowerHalf((fPreprocess->NoiseSpectrum()).Vect());
      FloatVect den(lNoisePower); FloatVect num(lTemplatePower);
      num.Divide(den);
      vector<Float_t> lOptFilter = num.Vect();

      // Plot the spectra
      TCanvas* c = new TCanvas("c","canvas");
      c->SetFillColor(0);
      TH1F* lNoiseHist = vect2hist(lNoisePower,"");
      lNoiseHist->Scale(1./lNoiseHist->GetMaximum());
      lNoiseHist->SetLineColor(1);
      lNoiseHist->SetStats(0);
      TH1F* lFilterHist = vect2hist(lOptFilter,"Optimal filter");
      lFilterHist->Scale(1./lFilterHist->GetMaximum());
      lFilterHist->SetLineColor(2);
      lFilterHist->SetStats(0);
      c->SetLogy(); c->SetLogx();
      lNoiseHist->GetXaxis()->SetRangeUser(1.5,lTemplatePower.size());
      lFilterHist->GetXaxis()->SetRangeUser(1.5,lTemplatePower.size());
      lNoiseHist->GetYaxis()->SetRangeUser(0.0001,2);
      lNoiseHist->GetXaxis()->SetTitle("Normalized frequency");
      lNoiseHist->Draw();
      lTemplHist->Draw("same");
      lFilterHist->Draw("same");
      TLegend* leg = new TLegend(0.12,0.12,0.3,0.3);
      leg->SetFillColor(0);
      leg->SetHeader(aChannel.c_str());
      leg->AddEntry(lNoiseHist,"Noise spectrum");
      leg->AddEntry(lTemplHist,"Pulse signal");
      leg->AddEntry(lFilterHist,"Optimal filter");
      leg->Draw();
      c->SaveAs((aPlotName+"_"+int2str(num_noise)+".gif").c_str());
      delete c;
      delete lNoiseHist; delete lFilterHist;
    }
  }
  delete lTemplHist;
}

void EdwAnaDB::PlotNoiseSpectra(string aChannel, string aPlotName) const {

  vector<time_t> ti, tf;
  vector<UInt_t> test_nbpts, goodentries;

  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    if (fPreprocess->Channel() == aChannel) {
      ti.push_back(fPreprocess->StartTime());
      tf.push_back(fPreprocess->EndTime());
      test_nbpts.push_back((fPreprocess->NoiseSpectrum()).size());
      goodentries.push_back(i);
    }
  }
  if (test_nbpts.size() == 0) {
    cerr << "No spectra for channel " << aChannel << endl;
    return;
  }
  UInt_t nbpts_spec = 1+(test_nbpts[0])/2; // size of EdwPowerHalf(noisespectra)
  /*
  for (UInt_t i=1; i<test_nbpts.size(); i++) {
    if (test_nbpts[i] != nbpts_spec) {
      cerr << "Pbl in nb of pts for different noise spectra of channel "<< aChannel<<endl;
      exit(-1);
    }
  }
  */
  vector<time_t> times;
  vector<Bool_t> spectrum_in_bin;
  times.push_back(ti[0]);
  for (UInt_t i=0; i<ti.size()-1; i++) {
    times.push_back(tf[i]);
    spectrum_in_bin.push_back(1);
    if (tf[i] != ti[i+1]) {
      times.push_back(ti[i+1]);
      spectrum_in_bin.push_back(0);
    }
  }
  times.push_back(tf[tf.size()-1]);
  spectrum_in_bin.push_back(1);

  Int_t nbinsx=times.size()-1;
  Double_t* xbins = new Double_t[times.size()];
  for (UInt_t i=0; i<times.size(); i++) xbins[i]=(Double_t)(times[i]);
  Int_t nbinsy = (Int_t)nbpts_spec, ylow=0, yup=nbpts_spec;
  TH2F* hist = new TH2F(("spec"+aChannel).c_str(),(aChannel+" Noise").c_str(),nbinsx,xbins,nbinsy,ylow,yup);
  Int_t i_entry=0;
  for (Int_t p=0; p<nbinsx; p++) {
    if (spectrum_in_bin[p] == 0) {
      for (Int_t q=0; q<nbinsy; q++) hist->SetBinContent(p+1,q+1,TMath::Log10(0));
    } else {
      if ((1+(test_nbpts[i_entry])/2) == nbpts_spec) { // else it may be empty spectrum
	fPreprocessBranch->GetEntry(goodentries.at(i_entry));
	vector<Float_t> sp = EdwPowerHalf(fPreprocess->NoiseSpectrumVector());
	for (Int_t q=0; q<nbinsy; q++) {
	  hist->SetBinContent(p+1,q+1,TMath::Log10(sp[q]));
	}
	sp.clear();
      } else {
	for (Int_t q=0; q<nbinsy; q++) hist->SetBinContent(p+1,q+1,TMath::Log10(0));
      }
      i_entry++;
    }
  }

  TCanvas* c = new TCanvas("c","canvas",1500,500);
  c->SetFillColor(0);
  c->SetLogy(1);
  gStyle->SetPalette(1);
  gStyle->SetTimeOffset(0);
  hist->GetXaxis()->SetTitle("Time");
  hist->SetStats(0);
  hist->GetXaxis()->SetTimeDisplay(1);
  hist->GetYaxis()->SetTitle("Frequency (arb. unit)");
  hist->Draw("COLZ");
  c->SaveAs(aPlotName.c_str());
  delete c;
  delete hist;
  delete[] xbins;
  times.clear(); spectrum_in_bin.clear();
  ti.clear(); tf.clear();
  test_nbpts.clear(); goodentries.clear();
}


/*

void EdwAnaDB::BuildPreprocessCorrels(TChain* aChain, time_t aStart, time_t aStop, string aPlotProfiles) {

  // Pas operationnel!

  // first initialize the profiles and parameters to compute correl. coefs.
  vector<TProfile*> lProfiles;
  vector<ULong_t> lNb;
  vector<Float_t> lSumX, lSumY, lSumXY, lSumXX, lSumYY;
  vector<string> lChannels;
  vector< vector<string> > lCorrelChannels;
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    if (fPreprocess->StartTime() == aStart && fPreprocess->EndTime() == aStop) {
      // ose??? ca doit passer.. - condition pour calculer le truc!
      string theChannel = fPreprocess->Channel();
      lChannels.push_back(theChannel);
      lCorrelChannels.push_back(fPreprocess->CorrelChannels());
      lNb.push_back(0);
      lSumX.push_back(0);      
      lSumY.push_back(0);
      lSumXX.push_back(0);      
      lSumYY.push_back(0);
      lSumXY.push_back(0);
      lProfiles.push_back(new TProfile(("profile_"+theChannel).c_str(),("Correlation "+theChannel).c_str(),100,-10,10)); // parametres a fixer, tunner... hem. - also the option blank for fit
    }
  }
   
  // Parse the events
  EdwEvent* lEvt=new EdwEvent();
  EdwPulse* lEdwPulse = new EdwPulse();
  vector<string> theEvtChannels;
  vector< vector<Float_t> > theEvtPoints;
  aChain->SetBranchAddress("Event",&lEvt);
  for (UInt_t ievt=0; ievt<aChain->GetEntries(); ievt++) {
    aChain->GetEntry(ievt);
    time_t lTime = lEvt->Header()->DateSec();
    if (lTime >= aStart && lTime <= aStop) {
      theEvtChannels.clear();
      theEvtPoints.clear(); // ok ou mem leak???
      // 1) Get the data
      for (UInt_t j=0; j<lEvt->NbPulses(); j++) {
	lEdwPulse = lEvt->Pulse(j);
	FitPulse lPulse(lEdwPulse);
	lPulse.FindPeaks(FP_STRICT); // standard parameters here
	if (lPulse.GetPeakBins().size() == 0) {
	  theEvtChannels.push_back(lPulse.Channel());
	  theEvtPoints.push_back(hist2vect(lPulse.ProcessedTrace()));
	}
      }
      // 2) Fill the correlation objects
      for (UInt_t j=0; j<lChannels.size(); j++) {
	Int_t lPt = stringnum(theEvtChannels,lChannels[j]);
	if (lPt != -1) { // the channel is here
	  vector<Int_t> lChannelNumsToCorrel; // in the event
	  for (UInt_t k=0; k<(lCorrelChannels[j]).size(); k++) {
	    Int_t llPt = stringnum(theEvtChannels,lCorrelChannels[j][k]);
	    if (llPt != -1) lChannelNumsToCorrel.push_back(llPt);
	  }
	  if (lChannelNumsToCorrel.size() >= 3) { // choix arbitraire a fixer...
	    // La on a choisi d'utiliser cet evt pour la correlation de la voie j
	    // a) Calcul trace moyenne a correler
	    vector<Float_t> lXPts = theEvtPoints.at(lPt);
	    vector<Float_t> lYPts(lXPts.size(),0);
	    for (UInt_t k=0; k<lYPts.size(); k++) {
	      for (UInt_t l=0; lChannelNumsToCorrel.size(); l++)
		lYPts[k]+=(theEvtPoints[lChannelNumsToCorrel[l]])[k];
	      lYPts[k]/=lChannelNumsToCorrel.size();
	    }
	    // b) Remplissage objets
	    for (UInt_t k=0; k<lXPts.size(); k++) {
	      (lProfiles[j])->Fill(lXPts[k],lYPts[k]);
	      (lNb[j])+=1;
	      (lSumX[j])+=lXPts[k];
	      (lSumY[j])+=lYPts[k];
	      (lSumXX[j])+=(lXPts[k]*lXPts[k]);
	      (lSumXY[j])+=(lXPts[k]*lYPts[k]);
	      (lSumYY[j])+=(lYPts[k]*lYPts[k]);
	    }
	  }
	}
      }
    }
    lEvt->Clear();
  }
  delete lEdwPulse;
  delete lEvt;

  // Last step = use the objects to compute correlations and fill the db.
  TF1* lFitFunc = new TF1("fitfunc","++x",-100000,100000);
  for (UInt_t i=0; i<lChannels.size(); i++) {
    (lProfiles[i])->Fit(lFitFunc,"WQNC");
    if (aPlotProfiles != "") {
      TCanvas* c = new TCanvas("c",("Correlation for "+lChannels[i]).c_str());
      (lProfiles[i])->Draw();
      c->SaveAs((aPlotProfiles+"_"+lChannels[i]+".gif").c_str()); // pas parfait la...
      delete c;
    }
    Float_t lCorrelSlope = lFitFunc->GetParameter(0);
    Float_t lCorrelFactor = (lSumXY[i]/lNb[i]-(lSumX[i]*lSumY[i])/(lNb[i]*lNb[i]))/
      (sqrt(lSumXX[i]/lNb[i]-lSumX[i]*lSumX[i]/(lNb[i]*lNb[i]))*
       sqrt(lSumYY[i]/lNb[i]-lSumY[i]*lSumY[i]/(lNb[i]*lNb[i])));
    for (UInt_t j=0; j<fPreprocessBranch->GetEntries(); j++) {
      fPreprocessBranch->GetEntry(j);
      if (fPreprocess->StartTime() == aStart && fPreprocess->EndTime() == aStop
	  && fPreprocess->Channel() == lChannels[i]) {
	fPreprocess->SetCorrelFactor(lCorrelFactor);
	fPreprocess->SetCorrelSlope(lCorrelSlope);
      }
    }
  }
  fPreprocessTree->Write("",kOverwrite); // est-ce que ca va ca??? on sait pas...

  delete lFitFunc;
  for (UInt_t i=0; i<lProfiles.size(); i++) delete lProfiles[i];
}

*/

void EdwAnaDB::BasicFillQuality(TChain* aChain, vector<string> aChannels, time_t aStart, time_t aStop) {

  UInt_t NbChannels = aChannels.size();
  for (UInt_t i=0; i<NbChannels; i++) {
    string lChannel = aChannels[i];
    EdwQualityFlags* lQuality = new EdwQualityFlags(aStart,aStop,lChannel);
    time_t lTime = (time_t)((aStart+aStop)/2.); // somewhat arbitrary...
    EdwTemplate* lTemplate = this->GetTemplate(lTime,lChannel);
    EdwPreprocessData* lPreprocess = this->GetPreprocessPtr(lTime,lChannel);
    if (lTemplate == NULL || lPreprocess == NULL) {
      cerr << "No good template or preprocess found in basicfillquality for "<<lChannel
	   <<" - Baselines will not be computed for this channel."<<endl;
      continue;
    }
    lQuality->ComputeBaselines(aChain,lTemplate->Trace(),lTemplate->VectTraceFFT(),lPreprocess->NoiseSpectrum());
    fQuality = lQuality;
    fQualityBranch->Fill();
    delete lQuality; // bof bof
  }
  fQualityTree->Write("",kOverwrite);  

}

void EdwAnaDB::DumpFrequencyWeights(string aFileName, UInt_t aFreqStart, UInt_t aFreqStop) {
  // PAR DEFAUT: SEULEMENT LES CHALEURS !!!!
  vector<string> lChannelList;
  for (UInt_t k=0; k<fTemplateBranch->GetEntries(); k++) {
    fTemplateBranch->GetEntry(k);
    if (fTemplate->IsHeat()) lChannelList.push_back(fTemplate->Channel());
  }
  this->DumpFrequencyWeights(aFileName,lChannelList,aFreqStart,aFreqStop);
}

void EdwAnaDB::DumpFrequencyWeights(string aFileName, vector<string> aChannelList, UInt_t aFreqStart, UInt_t aFreqStop) {

  if (aFileName == "None") cerr << "Dumpfreqw error: output file is none!" << endl;
  ofstream lFile(aFileName.c_str(),ios::out);
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    Int_t j = stringnum(aChannelList,fPreprocess->Channel());
    if (j != -1) {
      EdwTemplate lTemplate = this->GetTemplate(aChannelList[j]);
      vector<Float_t> lNoisePower = EdwPowerHalf((fPreprocess->NoiseSpectrum()).Vect());
      vector<Float_t> lTemplatePower = EdwFFTPower(lTemplate.VectTraceFFT());
      if (lNoisePower.size() != lTemplatePower.size()) {
	cerr << "Mismatching sizes between noise and template for "<<aChannelList[j]<<endl; exit(-1);
      }
      if (aFreqStart >= aFreqStop || aFreqStop >= lNoisePower.size()) {
	cerr << "Freq range incorrect in dumpfreqweight. exit."<<endl; exit(-1);
      }
      FloatVect lWeights;
      for (UInt_t p=aFreqStart; p<aFreqStop; p++)
	lWeights.push_back(lTemplatePower[p]/lNoisePower[p]);
      lFile << aChannelList[j] <<" ";
      lFile << fPreprocess->StartTime()<<" "<<fPreprocess->EndTime()<<" " ;
      lFile << lWeights.Integral() << endl;
    }
  }
  lFile.close();
}

/*
  TFile* lFile = new TFile(aFileName.c_str(),"RECREATE","FreqWeights");
  TTree* lTree = new TTree("FreqWeightTree","Frequency weights");
  char lChannel[20] ; // on espere que ca suffira
  ULong64_t lTStart, lTStop;
  Int_t lFrequency;
  Float_t lWeight, lNoiseFreq, lTemplateFreq;
  lTree->Branch("Channel",&lChannel,"Channel/C");
  lTree->Branch("TStart",&lTStart,"TStart/l");
  lTree->Branch("TStop",&lTStop,"TStop/l");
  lTree->Branch("Frequency",&lFrequency,"Frequency/I");
  lTree->Branch("Noise",&lNoiseFreq,"Noise/F");
  lTree->Branch("Template",&lTemplateFreq,"Template/F");
  lTree->Branch("Weight",&lWeight,"Weight/F");
  for (UInt_t j=0; j<aChannelList.size(); j++) {
    string mychannel = aChannelList[j];
    mychannel.replace(mychannel.find_first_of(" "),1,"_");
    strcpy(lChannel,mychannel.c_str());
    for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
      fPreprocessBranch->GetEntry(i);
      if (fPreprocess->Channel() == aChannelList[j]) {
	EdwTemplate lTemplate = this->GetTemplate(aChannelList[j]);
	lTStart = (ULong64_t)(fPreprocess->StartTime());
	lTStop = (ULong64_t)(fPreprocess->EndTime());
	//	vector<Float_t> lNoisePower = EdwFFTPower((fPreprocess->NoiseSpectrum()).Vect());
	vector<Float_t> lNoisePower = EdwPowerHalf((fPreprocess->NoiseSpectrum()).Vect());
	vector<Float_t> lTemplatePower = EdwFFTPower(lTemplate.VectTraceFFT());
	if (lNoisePower.size() != lTemplatePower.size()) {
	  cerr << "Mismatching sizes between noise and template for "<<aChannelList[j]<<endl;
	  return;
	}
	for (UInt_t p=0; p<lNoisePower.size(); p++) {
	  lFrequency = p;
	  lTemplateFreq = lTemplatePower[p];
	  lNoiseFreq = lNoisePower[p];
	  lWeight = lTemplateFreq/lNoiseFreq;
	  lFile->cd(); lTree->Fill();
	}
      }
    }
  }
  lFile->cd(); lTree->Write("",kOverwrite);


*/

void EdwAnaDB::DumpTemplatesInfo(string aASCIITemplateDir) const {
  cout << "-- EdwAnaDB: "<<fTemplateBranch->GetEntries()<<" templates stored."<<endl;
  for (UInt_t i=0; i<fTemplateBranch->GetEntries(); i++) {
    fTemplateBranch->GetEntry(i);
    fTemplate->DumpInfo();
    if (aASCIITemplateDir != "None") {
      string thefile = aASCIITemplateDir+"/template_"+fTemplate->Channel()+".txt";
      fTemplate->DumpTrace(thefile);
      cout << "-- Template file " << thefile << " created." << endl;
    }
  }
}

void EdwAnaDB::DumpPreprocessInfo() const {
  cout << "-- EdwAnaDB: "<<fPreprocessBranch->GetEntries()<<" preprocesses stored."<<endl;
  for (UInt_t i=0; i<fPreprocessBranch->GetEntries(); i++) {
    fPreprocessBranch->GetEntry(i);
    fPreprocess->DumpInfo();
  }
}
