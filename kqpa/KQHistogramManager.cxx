//_____________________________________________
//
// KQHistogramManager.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 3/10/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

#include "KQHistogramManager.h"

ClassImp(KQHistogramManager);

KQHistogramManager::KQHistogramManager(KQDataReader* aKQDataReader)
{
  fQDataReader = aKQDataReader;
  SetConfidenceLevel(0.9);
  fNumBinsEnergyRecoil = 100;
  fNumBinsQ = 100;
  fQMin = 0;
  fQMax = 2;
  fEnergyRecoilMin = 0;
  fEnergyRecoilMax = 1000;
}

void KQHistogramManager::Fill(Int_t aNumProjections) 
{
  //This method fills  TH2D Q-ERecoil histograms with equal numbers of entries from fData
  //Each histogram is filled with the amount of entries corresponding to
  //the integer quotient of the total entries "fDataSize"  and the number of  histograms "fNumProjections".
  //If this quotient is zero, the number of histograms "fNumProjections" is set to one.
  //The last histogram contains the remaining events "fDataSize" modulo the number of entries per histogram.
  
  cout << "Filling histograms ... " << endl;
  Int_t aDataSize = fQDataReader->GetSize();
  
  Int_t entriesPerHistogram = aDataSize/aNumProjections;
  Int_t remainEvents;
  cout << "Entries per histogram: " << entriesPerHistogram << endl;
  if(entriesPerHistogram==0) {
    aNumProjections = 1;
    remainEvents = aDataSize;
  }
  else
  {
    aNumProjections = aDataSize/entriesPerHistogram + 1;
    remainEvents = aDataSize % entriesPerHistogram;
  }
  cout << "Remaining events: " << remainEvents << endl;
  
  fHistogramRecords.clear();
  
  //all but not the last histogram
  for(int k_hist = 0; k_hist<aNumProjections-1; ++k_hist) {
    cout << "Histogram: " << k_hist << endl;
    fHistogramRecords.push_back(KQHistogramRecord());
    fHistogramRecords[k_hist].SetHistogram(new TH2D(TString::Format("hist_%s_%i",fQDataReader->GetEventCategory(),k_hist).Data(),
                                                    "",
                                                    fNumBinsEnergyRecoil,
                                                    fQDataReader->GetEnergyRecoil(k_hist*entriesPerHistogram),
                                                    fQDataReader->GetEnergyRecoil(k_hist*entriesPerHistogram + entriesPerHistogram - 1),
                                                    fNumBinsQ,
                                                    fQMin,
                                                    fQMax
                                               ));
    
    cout << TString::Format("Histogram %i created",k_hist).Data() << endl;
    for(int k_entry = 0; k_entry<entriesPerHistogram; ++k_entry)
      fHistogramRecords[k_hist].GetHistogram()->Fill(fQDataReader->GetEnergyRecoil(k_hist*entriesPerHistogram+k_entry),
                                                 fQDataReader->GetQvalue(k_hist*entriesPerHistogram+k_entry)
                                            );
    fHistogramRecords[k_hist].SetEnergyRecoilMean(fHistogramRecords[k_hist].fHistogram->GetMean(1));
    fHistogramRecords[k_hist].SetEnergyRecoilError(fConfidenceInSigmas*sqrt(fHistogramRecords[k_hist].fHistogram->GetCovariance(1,1)));
    cout << TString::Format("Histogram %i filled: range of E [%lf,%lf]",
                            k_hist,
                            fHistogramRecords[k_hist].fHistogram->GetXaxis()->GetXmin(),
                            fHistogramRecords[k_hist].fHistogram->GetXaxis()->GetXmax()).Data() << endl;
  }
  
  //last histogram
  fHistogramRecords.push_back(KQHistogramRecord());
  fHistogramRecords[aNumProjections-1].SetHistogram(new TH2D(TString::Format("hist_%s_%i",fQDataReader->GetEventCategory(),
                                                                             aNumProjections-1).Data(),
                                                             "",fNumBinsEnergyRecoil,
                                                             fQDataReader->GetEnergyRecoil((aNumProjections-1)*entriesPerHistogram),
                                                             fQDataReader->GetEnergyRecoil((aNumProjections-1)*entriesPerHistogram + remainEvents -1),
                                                             fNumBinsQ,
                                                             fQMin,
                                                             fQMax
                                                        ));
                                                        
  for(int k_entry = 0; k_entry<remainEvents; ++k_entry)
    fHistogramRecords[aNumProjections-1].GetHistogram()->Fill(fQDataReader->GetEnergyRecoil((aNumProjections-1)*entriesPerHistogram+k_entry),
                                                          fQDataReader->GetQvalue((aNumProjections-1)*entriesPerHistogram+k_entry)
                                                         );
    cout << TString::Format("Histogram %i filled: range of E [%lf,%lf]",
                            aNumProjections-1,
                            fHistogramRecords[aNumProjections-1].GetHistogram()->GetXaxis()->GetXmin(),
                            fHistogramRecords[aNumProjections-1].GetHistogram()->GetXaxis()->GetXmax()).Data() << endl;
  cout << "Histograms filled" << endl;
  fHistogramRecords[aNumProjections-1].SetEnergyRecoilMean(fHistogramRecords[aNumProjections-1].GetHistogram()->GetMean(1));
  fHistogramRecords[aNumProjections-1].SetEnergyRecoilError(fConfidenceInSigmas*sqrt(fHistogramRecords[aNumProjections-1].GetHistogram()->GetCovariance(1,1)));
  
  //labels for all histograms
  for(int k = 0; k<aNumProjections; ++k) {
    fHistogramRecords[k].GetHistogram()->SetTitle(TString::Format("category: %s #splitline{E_{R} = %f..%f}{#bar{E_{R}}=%f #pm %f}",
                                                              fQDataReader->GetEventCategory(),
                                                              fHistogramRecords[k].fHistogram->GetXaxis()->GetXmin(),
                                                              fHistogramRecords[k].fHistogram->GetXaxis()->GetXmax(),
                                                              fHistogramRecords[k].GetEnergyRecoilMean(),
                                                              fHistogramRecords[k].GetEnergyRecoilError()).Data());
    fHistogramRecords[k].GetHistogram()->GetXaxis()->SetTitle("E_{Recoil}[keV]");
    fHistogramRecords[k].GetHistogram()->GetYaxis()->SetTitle("Q");
  }
  cout << "Histogram attributes set ... " << endl;  
}

void KQHistogramManager::CalculateTheoErrors()
{
  //This method calculates the theoretical central Q values and band widths for 
  // the gamma and neutron recoil bands for all histograms.
  // At the moment, this is done the following way:
  // The central Q value for gamma recoil band is always 1.
  // The central Q value for the neutron recoil is calculated by the 
  // Lindhard formula Q(E_Recoil) = 0.165* E_Recoil^0.185.
  // For the band uncertainties, first the heat and ion channel  uncertainties
  // on baseline and calibration level E=365.0 keV
  // for the corresponding bolometer delivering the data read by KQDataReader
  // are read from an ASCII file with tabled values via KBoloConfig.
  // Then via KQUncertainty the heat and ion channel uncertainties for all
  // histograms are calculated in order to determine the gamma and neutron
  // band widths
  
  fBoloConfig = new KBoloConfig(fQDataReader->GetBoloName(),
                                fQDataReader->GetBoloConfigFile());
  
  
  for(UInt_t k = 0; k<fHistogramRecords.size(); ++k)
  {
    fHistogramRecords[k].SetQGammaTheo(1);
    fHistogramRecords[k].SetQNeutronTheo(KQUncertainty::GetQMeanValue(fHistogramRecords[k].GetEnergyRecoilMean(),fHistogramRecords[k].GetEnergyRecoilError()));
    
    Double_t aHeatUncertainty = KQUncertainty::GetChannelUncertainty(fHistogramRecords[k].GetEnergyRecoilMean(),
                                                                     fBoloConfig->GetSigmaHeatZero(),
                                                                     fBoloConfig->GetSigmaHeatCalibration(),
                                                                     fBoloConfig->GetEnergyCalibration());
    
    Double_t anIonUncertainty =  KQUncertainty::GetChannelUncertainty(fHistogramRecords[k].GetEnergyRecoilMean(),
                                                                      fBoloConfig->GetSigmaIonZero(),
                                                                      fBoloConfig->GetSigmaIonCalibration(),
                                                                      fBoloConfig->GetEnergyCalibration());
    
    Double_t aVoltageBias = fBoloConfig->GetChannelVoltage();
    
    fHistogramRecords[k].SetQGammaTheoError(fConfidenceInSigmas* KQUncertainty::GetElecRecoilWidth(fHistogramRecords[k].GetEnergyRecoilMean(),
                                                                              anIonUncertainty,
                                                                              aHeatUncertainty,
                                                                              aVoltageBias));
                                                                              
    fHistogramRecords[k].SetQNeutronTheoError(fConfidenceInSigmas*KQUncertainty::GetNuclRecWidth(fHistogramRecords[k].GetEnergyRecoilMean(),
                                                                              anIonUncertainty,
                                                                              aHeatUncertainty,
                                                                              aVoltageBias));
    
  }
}

void KQHistogramManager::PrintErrors(ostream& os)
{
  // This method prints the empirical and theoretical values for the gamma and
  // nuclear recoil band widths for all histograms with the previously specified
  // confidence level
  
  CalculateTheoErrors();
  os << setw(5) << "#" << setw(20) << "energy recoil " << setw(20) <<
  "Q neutron" << setw(25) << "Q neutron (theoretical)" << endl;
  for(UInt_t k = 0; k<fHistogramRecords.size(); ++k)
    os << setw(5) << " " << setw(20) << fHistogramRecords[k].GetEnergyRecoilMean()
    << " +/- " << fHistogramRecords[k].GetEnergyRecoilError()
    << setw(20) << fHistogramRecords[k].GetQNeutron() << " +/- "
    << fHistogramRecords[k].GetQNeutronError() << setw(25)
    << fHistogramRecords[k].GetQNeutronTheo() << " +/- "
    << fHistogramRecords[k].GetQNeutronTheoError() << endl;
  
  os << setw(5) << "#" << setw(20) << "energy recoil " << setw(20) <<
  "Q gamma " << setw(25) << "Q gamma (theoretical)" << endl;
  for(UInt_t k = 0; k<fHistogramRecords.size(); ++k)
    os << setw(5) << " " << setw(20) << fHistogramRecords[k].GetEnergyRecoilMean()
    << " +/- " << fHistogramRecords[k].GetEnergyRecoilError()
    << setw(20) << fHistogramRecords[k].GetQGamma() << " +/- "
    << fHistogramRecords[k].GetQGammaError() << setw(25)
    << fHistogramRecords[k].GetQGammaTheo() << " +/- "
    << fHistogramRecords[k].GetQGammaTheoError() << endl;
}







void KQHistogramManager::SetConfidenceLevel(Double_t aQuantil)
{
  //This method sets the confidence level according to the specified quantil
  fConfidenceInSigmas = sqrt(2)*TMath::ErfInverse(aQuantil);
}

Double_t KQHistogramManager::GetConfidenceLevel()
{
  //This method returns the current confidence level
  return TMath::Erf(fConfidenceInSigmas/TMath::Sqrt(2));
}