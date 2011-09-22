//
// This is an example ROOT macro to show how to use a KAmper
//
void testSimpleKamper(const char* input, const char * out)
{
  KDataReader f(input);

  TH1D hAmpHA("hAmpHeatA", "hAmpHeatA", 10000,-5000, 5000);
  TH1D hAmpHC("hAmpHeatC", "hAmpHeatC", 10000,-5000, 5000);
  TH1D hAmpIA("hAmpIonA", "hAmpIonA", 10000,-5000, 5000);
  TH1D hAmpIB("hAmpIonB", "hAmpIonB", 10000,-5000, 5000);
  TH1D hAmpIC("hAmpIonC", "hAmpIonC", 10000,-5000, 5000);
  TH1D hAmpID("hAmpIonD", "hAmpIonD", 10000,-5000, 5000);

  KEvent *e = f.GetEvent();
  KPulseAnalysisRecord *prec = new KPulseAnalysisRecord;
  KSimpleKamper1 myKamper;
  int heatMinPeak = 250;
  int heatMaxPeak = 290;
  int ionMinPeak = 6500;
  int ionMaxPeak = 6700;
  
  for(Int_t i = 0; i < f.GetEntries(); i++){
    f.GetEntry(i);
    if (i % 500 == 0) cout << "entry " << i << endl;
    
    for(Int_t j = 0; j < e->GetNumBoloPulses(); j++){
      KRawBoloPulseRecord *p = (KRawBoloPulseRecord *)e->GetBoloPulse(j);
      if( strcmp(p->GetBolometerRecord()->GetDetectorName(), "FID802") != 0)
        continue;
      
      if(p->GetPulseLength() == 0)
        continue;
        
      myKamper.MakeKamp(p,prec);
      string name = p->GetChannelName();
      if(strcmp(p->GetChannelName(),"chaleur FID802AB") == 0){
        if(prec->GetPeakPosition() > heatMinPeak && prec->GetPeakPosition() < heatMaxPeak)
          hAmpHA.Fill(prec->GetAmp());
      }
      else if(strcmp(p->GetChannelName(),"centre FID802AB") == 0){
        if(prec->GetPeakPosition() > ionMinPeak && prec->GetPeakPosition() < ionMaxPeak)
          hAmpIA.Fill(prec->GetAmp());
      }
      else if(strcmp(p->GetChannelName(),"garde FID802AB") == 0){
        if(prec->GetPeakPosition() > ionMinPeak && prec->GetPeakPosition() < ionMaxPeak)
          hAmpIB.Fill(prec->GetAmp());
      }
      else if(strcmp(p->GetChannelName(),"chaleur FID802CD") == 0){
        if(prec->GetPeakPosition() > heatMinPeak && prec->GetPeakPosition() < heatMaxPeak)
          hAmpHC.Fill(prec->GetAmp());
      }
      else if(strcmp(p->GetChannelName(),"centre FID802CD") == 0){
        if(prec->GetPeakPosition() > ionMinPeak && prec->GetPeakPosition() < ionMaxPeak)
          hAmpIA.Fill(prec->GetAmp());
      }
      else if(strcmp(p->GetChannelName(),"garde FID802CD") == 0){
        if(prec->GetPeakPosition() > ionMinPeak && prec->GetPeakPosition() < ionMaxPeak)
          hAmpIB.Fill(prec->GetAmp());
      }
      
      
    }
  }

  
  TFile fout(out,"recreate");
  hAmpHA.Write();
  hAmpHC.Write();
  hAmpIA.Write();
  hAmpIB.Write();
  hAmpIC.Write();
  hAmpID.Write();
    
}
