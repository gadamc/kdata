/**
   @file    EdwQualityFlags.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwQualityFlags class. See the .h file
*/

#include "EdwQualityFlags.h"

ClassImp(EdwQualityFlags); /**< Root dictionnary */

EdwQualityFlags::EdwQualityFlags() : TObject() {
}

EdwQualityFlags::~EdwQualityFlags() {
}

EdwQualityFlags::EdwQualityFlags(time_t aStart, time_t aStop, string aChannel) {
  fStartTime = aStart ;
  fEndTime = aStop ;
  fChannel = aChannel;
  fMeanRawNoise = 0;
  fStdRawNoise = 0;
  fBaselineTimeFit = 0;
  fBaselineFreqFit = 0;
}

void EdwQualityFlags::ComputeRawNoise(TChain* aChain) {
  // This requires that the events in the chain here have a field "Noise_boloname" 
  // [with only _ no whitespace] (Float_t) and a field "Date_s" (Long_t)
  // That is the (future) standard basic root DST format (no object, just branches!)

  string lChannel = fChannel ;
  lChannel.replace(fChannel.find_first_of(" "),1,"_");
  Float_t lNoise ;
  Long_t lDate ;
  aChain->SetBranchAddress("Date_s",&lDate) ;
  aChain->SetBranchAddress(("Noise_"+lChannel).c_str(),&lNoise) ;
  Float_t sum_x=0;
  Float_t sum_x2=0;
  Int_t nbpts=0;
  for (Int_t i=0; i<aChain->GetEntries(); i++) {
    aChain->GetEntry(i);
    if (lDate >= fStartTime && lDate <= fEndTime) {
      sum_x += lNoise ;
      sum_x2 += (lNoise*lNoise) ;
      nbpts += 1 ;
    }
  }

  Float_t lMean = sum_x/(Float_t)nbpts ;
  fMeanRawNoise = lMean ;
  fStdRawNoise = sqrt((sum_x2/(Float_t)nbpts)-lMean*lMean);

}

void EdwQualityFlags::ComputeBaselines(TChain* aChain, vector<Float_t> aTemplate, const FloatVect& aTemplateFFT, const FloatVect& aNoise, string aPlotHisto, Bool_t aDoTimeOnly, Bool_t aDoFreqOnly) {
  // from the event fit of noise traces, get a
  // gaussian distribution of amplitudes: the baseline
  // is the fwhm of this gaussian (for now!!)
  // NOTE = amplitude = A.U., as for the standard time fit (or frequency..).

  vector<Float_t> lAmplsTime;
  vector<Float_t> lAmplsFreq;
  EdwEvent *lEvent=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvent);
  EdwPulse* lEdwPulse = new EdwPulse();
  for (Long64_t i=0; i<aChain->GetEntries(); i++) {
    aChain->GetEntry(i) ;
    lEdwPulse = lEvent->Pulse(fChannel);
    if (lEdwPulse != NULL) {
      FitPulse lPulse(lEdwPulse);
      lPulse.FindPeaks(FP_STRICT); // with default parameters
      if (lPulse.GetPeakBins().size() == 0) { // No pulse!!!
	if (!aDoFreqOnly) {
	  vector<Float_t> params = lPulse.GetFitAmpl(aTemplate,0,0); // no fit of t_0!!!
	  lAmplsTime.push_back(params[0]);
	}
	if (!aDoTimeOnly) {
	  //	  TH1F* lFFT = vect2hist(aTemplateFFT);
	  //	  lAmplsFreq.push_back((lPulse.GetWienerAmpl(lFFT,aNoise,1,0,"",""))[0]);
	  //	  delete lFFT;
	  lAmplsFreq.push_back((lPulse.GetWienerAmpl(aTemplateFFT,aNoise,1))[0]);
	}
      }
    }
  }

  // do histo and fit gaussian and maybe also plot
  TCanvas* c = new TCanvas("c","Baseline histograms");
  c->SetLogy();
  Float_t lTypicalNoise = 0;
  TH1F* lHistT = new TH1F();
  TF1* lFuncT = new TF1("lfunct","gauss");
  TH1F* lHistF = new TH1F();
  TF1* lFuncF = new TF1("lfuncf","gauss");
  if (!aDoFreqOnly) {
    lTypicalNoise = vectrms(lAmplsTime);
    delete lHistT;
    lHistT = new TH1F("lhistt","Timefit baseline",200,-5*lTypicalNoise,5*lTypicalNoise);
    for (UInt_t i=0; i<lAmplsTime.size(); i++) lHistT->Fill(lAmplsTime[i]);
    lFuncT->SetParameter(0,lHistT->GetBinContent(lHistT->FindBin(0)));
    lFuncT->FixParameter(1,0);
    lFuncT->SetParameter(2,lTypicalNoise);
    lHistT->Fit(lFuncT,"WQN");
    Float_t lSigma = lFuncT->GetParameter(2);
    fBaselineTimeFit = 2.355*lSigma ; // Convert to FWHM!
    if (aPlotHisto != "") {
      lHistT->Draw();
      lFuncT->Draw("same");
    }
  }
  if (!aDoTimeOnly) {
    lTypicalNoise = vectrms(lAmplsFreq);
    delete lHistF;
    lHistF = new TH1F("lhistf","Freqfit baseline",200,-5*lTypicalNoise,5*lTypicalNoise);
    for (UInt_t i=0; i<lAmplsFreq.size(); i++) lHistF->Fill(lAmplsFreq[i]);
    lFuncF->SetParameter(0,lHistF->GetBinContent(lHistF->FindBin(0)));
    lFuncF->FixParameter(1,0);
    lFuncF->SetParameter(2,lTypicalNoise);
    lHistF->Fit(lFuncF,"WQN");
    Float_t lSigma = lFuncF->GetParameter(2);
    fBaselineFreqFit = 2.355*lSigma ; // Convert to FWHM!
    if (aPlotHisto != "") {
      lHistF->SetLineColor(2);
      lFuncF->SetLineColor(2);
      if (!aDoFreqOnly) lHistF->Draw("same");
      else lHistF->Draw();
      lFuncF->Draw("same");
    }
  }
  if (aPlotHisto != "") c->SaveAs(aPlotHisto.c_str());
  delete c;
  delete lHistT;
  delete lFuncT;
  delete lHistF;
  delete lFuncF;
  delete lEvent;
  delete lEdwPulse;
}

