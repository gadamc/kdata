/**
   @file    EdwNtuple.cc
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Implementation of the EdwNtuple class. See the .h file
*/

#include "EdwNtuple.h"

ClassImp(EdwNtuple); /**< Root dictionnary */

EdwNtuple::EdwNtuple() : TObject() {

  fFileName = "edwntp.root";
  fFile = new TFile(fFileName.c_str(),"RECREATE");
  fTree = new TTree(fFileName.c_str(),"Edelweiss Data Tree");
  fTree->SetDirectory(fFile);
  strcpy(fRun,"aa00a000");
  fEvtBufferSize = EVTBUFFERSIZE ;
  fHeatIonSynchroFile = "None";
  fWienerSyncAmpl = new FloatVect[NBMAX_AMPL];
  fWienerSyncChi2 = new FloatVect[NBMAX_AMPL];
  fAmplFlag = AMPL_RAW;
  fShapeFlag = SHAPE_NO;
}

EdwNtuple::~EdwNtuple() {
  delete [] fWienerSyncAmpl;
  delete [] fWienerSyncChi2;
  if (fFile->IsZombie()) cerr << "EdwNtuple destructor: fFile is zombie." << endl;
  fFile->Close();
  delete fFile ;
}

EdwNtuple::EdwNtuple(string aFileName, string aTitle) : TObject() {

  fFileName = aFileName;
  fFile = new TFile(fFileName.c_str(),"RECREATE");
  fAmplFlag = AMPL_RAW; // default value as for now!
  fShapeFlag = SHAPE_NO;
  fPrintAmplErr = DEFAULT_PRINTAMPLERR;
  fTree = new TTree(aTitle.c_str(),"Edelweiss Data Tree");
  fTree->SetDirectory(fFile);
  strcpy(fRun,"aa00a000");
  fEvtBufferSize = EVTBUFFERSIZE ;
  fHeatIonSynchroFile = "None";
  fWienerSyncAmpl = new FloatVect[NBMAX_AMPL];
  fWienerSyncChi2 = new FloatVect[NBMAX_AMPL];

}

void EdwNtuple::SetHeatIonSynchro(string aFile) {
  fHeatIonSynchroFile = aFile;
  // Read the file and check inconsistencies
  ifstream lFile(aFile.c_str(),ios::in);
  if (!lFile) {
    cout << "Error opening synchro file " << aFile << endl; exit(-1);
  }
  string line;
  while (getline(lFile,line)) {
    string::size_type pos_a = line.find('"',0);
    string::size_type pos_b = line.find('"',pos_a+1);
    if (pos_a == string::npos || pos_b == string::npos) { 
      cerr << "error synchro file 1" << endl; exit(-1);
    }
    string field = line.substr(pos_a+1,pos_b-pos_a-1);
    if (field.substr(0,7) != "chaleur" &&
	field.substr(0,6) != "centre" && field.substr(0,5) != "garde") { // now it may be an ionization!
      cerr << "error synchro file 2" << endl; exit(-1);
    }
    pos_a = line.find(':',pos_b+1);
    pos_a = line.find('"',pos_a+1);
    if (pos_a == string::npos) {
      cerr << "error synchro file 3" << endl; exit(-1);
    }
    while (pos_a != string::npos) {
      pos_b = line.find('"',pos_a+1);
      if (pos_b == string::npos) {
	cerr << "error synchro file 4" << endl; exit(-1);
      }
      field = line.substr(pos_a+1,pos_b-pos_a-1);
      if (field.substr(0,6) != "centre" && field.substr(0,5) != "garde") {
	cerr << "error synchro file 5" << endl; exit(-1);
      }
      pos_a = line.find('"',pos_b+1);
    }
  }
  lFile.close();
}

vector<Float_t> EdwNtuple::GetSyncParams(string aHeatChannel) const {
  if (!isheat(aHeatChannel)) cerr << "Warning input channel in GetSyncParams:"<< aHeatChannel <<endl;
  ifstream lFile(fHeatIonSynchroFile.c_str(),ios::in);
  string line; 
  vector<Float_t> params(2,0);
  while (getline(lFile,line)) {
    string::size_type pos_a = line.find('"',0);
    string::size_type pos_b = line.find('"',pos_a+1);
    if (line.substr(pos_a+1,pos_b-pos_a-1) == aHeatChannel) {
      pos_a = line.find(':',pos_b+1);
      istringstream ss(line); ss.seekg(pos_a+1);
      ss >> params[0]; ss >> params[1];
    }
  }
  //  if (params[0] == 0 && params[1] == 0) cerr << "warning: empty params returned"<<endl;
  lFile.close();

  return params;
}

vector<string> EdwNtuple::GetSyncIonChannels(string aHeatChannel) const {
  ifstream lFile(fHeatIonSynchroFile.c_str(),ios::in);
  string line; 
  vector<string> IonChannels;
  while (getline(lFile,line)) {
    string::size_type pos_a = line.find('"',0);
    string::size_type pos_b = line.find('"',pos_a+1);
    if (line.substr(pos_a+1,pos_b-pos_a-1) == aHeatChannel) {
      pos_a = line.find(':',pos_b+1);
      pos_a = line.find('"',pos_a+1);
      while (pos_a != string::npos) {
	pos_b = line.find('"',pos_a+1);
	IonChannels.push_back(line.substr(pos_a+1,pos_b-pos_a-1));
	pos_a = line.find('"',pos_b+1);
      }
    }
  }
  lFile.close();

  return IonChannels;
}

string EdwNtuple::GetSyncHeatChannel(string aIonChannel) const {
  ifstream lFile(fHeatIonSynchroFile.c_str(),ios::in);
  string line; 
  string HeatChannel = "";
  while (getline(lFile,line)) {
    string::size_type pos_a = line.find('"',0);
    string::size_type pos_b = line.find('"',pos_a+1);
    string lheat = line.substr(pos_a+1,pos_b-pos_a-1);
    if (isheat(lheat)) { // Necessary to reject lines with ion-ion synchro
      pos_a = line.find(':',pos_b+1);
      pos_a = line.find('"',pos_a+1);
      while (pos_a != string::npos) {
	pos_b = line.find('"',pos_a+1);
	if (line.substr(pos_a+1,pos_b-pos_a-1) == aIonChannel)
	  HeatChannel = lheat;
	pos_a = line.find('"',pos_b+1);
      }
    }
  }
  if (HeatChannel == "") cerr <<"Warning no heat channel corresponding found"<<endl;
  lFile.close();

  return HeatChannel;
}

void EdwNtuple::PrepareNtp() {
  // Subroutine used in main ntp filling drivers
  if (fChannels.size() == 0) {
    cerr<<"Prepare Ntp: no channels! Exiting." <<endl; exit(-1);
  }
  UInt_t NbChannels = fChannels.size();
  vector<string> lNtpChannels(fChannels);
  for (UInt_t i=0; i<NbChannels; i++) (lNtpChannels[i]).replace((lNtpChannels[i]).find_first_of(" "),1,"_");

  fRawBaseline.resize(NbChannels,0);
  fRawNoise.resize(NbChannels,0);
  fNbPeaks.resize(NbChannels,0);
  fRawAmpl.resize(NbChannels,0);
  fRawBin.resize(NbChannels,0);
  fFitAmpl.resize(NbChannels,0);
  fFitZeroAmpl.resize(NbChannels,0);
  fFitBin.resize(NbChannels,0);
  fFitChi2.resize(NbChannels,0);
  fFitAmplErr.resize(NbChannels,0);
  fFitFilterAmpl.resize(NbChannels,0);
  fFitFilterZeroAmpl.resize(NbChannels,0);
  fFitFilterBin.resize(NbChannels,0);
  fFitFilterChi2.resize(NbChannels,0);
  fFitFilterAmplErr.resize(NbChannels,0);
  fWienerAmpl.resize(NbChannels,0);
  fWienerZeroAmpl.resize(NbChannels,0);
  fWienerBin.resize(NbChannels,0);
  fWienerChi2.resize(NbChannels,0);
  fWienerAmplErr.resize(NbChannels,0);
  fRiseTime.resize(NbChannels,0);
  fFallTime.resize(NbChannels,0);
  fFWHM.resize(NbChannels,0);
  for (UInt_t k=0; k<NBMAX_AMPL; k++) {
    (fWienerSyncAmpl[k]).resize(NbChannels,0);
    (fWienerSyncChi2[k]).resize(NbChannels,0);
  }
  fTree->Branch("EventNum",&fEvtNum,"EventNum/i");
  fTree->Branch("RunName",&fRun,"RunName/C");
  fTree->Branch("DateSec",&fDateSec,"DateSec/l");
  fTree->Branch("DateMuSec",&fDateMuSec,"DateMuSec/I");
  fTree->Branch("TriggerBit1",&fTriggerBit1,"TriggerBit1/I");
  fTree->Branch("TriggerBit2",&fTriggerBit2,"TriggerBit2/I");
  for (UInt_t i=0; i<NbChannels; i++) {
    fTree->Branch(("RawBase_"+lNtpChannels.at(i)).c_str(),&(fRawBaseline[i]),("RawBase_"+lNtpChannels[i]+"/F").c_str());
    fTree->Branch(("RawNoise_"+lNtpChannels.at(i)).c_str(),&(fRawNoise[i]),("RawNoise_"+lNtpChannels[i]+"/F").c_str());
    fTree->Branch(("NbPeaks_"+lNtpChannels.at(i)).c_str(),&(fNbPeaks[i]),("NbPeaks_"+lNtpChannels[i]+"/I").c_str());
    fTree->Branch(("RawAmpl_"+lNtpChannels.at(i)).c_str(),&(fRawAmpl[i]),("RawAmpl_"+lNtpChannels[i]+"/F").c_str());
    fTree->Branch(("RawBin_"+lNtpChannels.at(i)).c_str(),&(fRawBin[i]),("RawBin_"+lNtpChannels[i]+"/F").c_str());
    if (fAmplFlag == AMPL_FIT || fAmplFlag == AMPL_ALL) {
      fTree->Branch(("FitAmpl_"+lNtpChannels.at(i)).c_str(),&(fFitAmpl[i]),("FitAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitZeroAmpl_"+lNtpChannels.at(i)).c_str(),&(fFitZeroAmpl[i]),("FitZeroAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitBin_"+lNtpChannels.at(i)).c_str(),&(fFitBin[i]),("FitBin_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitChi2_"+lNtpChannels.at(i)).c_str(),&(fFitChi2[i]),("FitChi2_"+lNtpChannels[i]+"/F").c_str());
      if (fPrintAmplErr) fTree->Branch(("FitAmplErr_"+lNtpChannels.at(i)).c_str(),&(fFitAmplErr[i]),("FitAmplErr_"+lNtpChannels[i]+"/F").c_str());
    } 
    if (fAmplFlag == AMPL_FITFILT || fAmplFlag == AMPL_ALL) {
      fTree->Branch(("FitFilterAmpl_"+lNtpChannels.at(i)).c_str(),&(fFitFilterAmpl[i]),("FitFilterAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitFilterZeroAmpl_"+lNtpChannels.at(i)).c_str(),&(fFitFilterZeroAmpl[i]),("FitFilterZeroAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitFilterBin_"+lNtpChannels.at(i)).c_str(),&(fFitFilterBin[i]),("FitFilterBin_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("FitFilterChi2_"+lNtpChannels.at(i)).c_str(),&(fFitFilterChi2[i]),("FitFilterChi2_"+lNtpChannels[i]+"/F").c_str());
      if (fPrintAmplErr) fTree->Branch(("FitFilterAmplErr_"+lNtpChannels.at(i)).c_str(),&(fFitFilterAmplErr[i]),("FitFilterAmplErr_"+lNtpChannels[i]+"/F").c_str());
    } 
    if (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL) {
      fTree->Branch(("WienerAmpl_"+lNtpChannels.at(i)).c_str(),&(fWienerAmpl[i]),("WienerAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("WienerZeroAmpl_"+lNtpChannels.at(i)).c_str(),&(fWienerZeroAmpl[i]),("WienerZeroAmpl_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("WienerBin_"+lNtpChannels.at(i)).c_str(),&(fWienerBin[i]),("WienerBin_"+lNtpChannels[i]+"/F").c_str());
      fTree->Branch(("WienerChi2_"+lNtpChannels.at(i)).c_str(),&(fWienerChi2[i]),("WienerChi2_"+lNtpChannels[i]+"/F").c_str());
      if (fPrintAmplErr) fTree->Branch(("WienerAmplErr_"+lNtpChannels.at(i)).c_str(),&(fWienerAmplErr[i]),("WienerAmplErr_"+lNtpChannels[i]+"/F").c_str());
    }
    if (isheat(fChannels[i])) { // on ne sauvegarde les pulse shape params que pr voies chaleur
      if (fShapeFlag == SHAPE_RISE || fShapeFlag == SHAPE_ALL) fTree->Branch(("RiseTime_"+lNtpChannels.at(i)).c_str(),&(fRiseTime[i]),("RiseTime_"+lNtpChannels[i]+"/F").c_str());
      if (fShapeFlag == SHAPE_FALL || fShapeFlag == SHAPE_ALL) fTree->Branch(("FallTime_"+lNtpChannels.at(i)).c_str(),&(fFallTime[i]),("FallTime_"+lNtpChannels[i]+"/F").c_str());
      if (fShapeFlag == SHAPE_FWHM || fShapeFlag == SHAPE_ALL) fTree->Branch(("FWHM_"+lNtpChannels.at(i)).c_str(),&(fFWHM[i]),("FWHM_"+lNtpChannels[i]+"/F").c_str());
    }
    if (fHeatIonSynchroFile != "None") { // more wiener ampls and chi2
      //      if (isheat(fChannels[i])) // ok pour chaleur<-ion ou ion<-ion
      vector<string> lIons = this->GetSyncIonChannels(fChannels[i]);
      for (UInt_t k=0; k<lIons.size(); k++) {
	string theion=lIons[k];
	theion.replace(theion.find_first_of(" "),1,"_");
	fTree->Branch(("WienerAmpl_"+lNtpChannels.at(i)+"_Sync_"+theion).c_str(),&((fWienerSyncAmpl[k])[i]),("WienerAmpl_"+lNtpChannels[i]+"_Sync_"+theion+"/F").c_str());
	fTree->Branch(("WienerChi2_"+lNtpChannels.at(i)+"_Sync_"+theion).c_str(),&((fWienerSyncChi2[k])[i]),("WienerChi2_"+lNtpChannels[i]+"_Sync_"+theion+"/F").c_str());
      }
    }
  }

}

void EdwNtuple::FillEvtFromBuffer(UInt_t ievt, EdwNtpBuffer& aBuffer) {
  if (ievt >= aBuffer.BufferSize) {
    cerr <<"FillEvtFromBuffer error. Exiting." << endl; exit(-1);
  }
  fDateSec=aBuffer.DateSec[ievt];
  fDateMuSec=aBuffer.DateMuSec[ievt];
  fTriggerBit1=aBuffer.TriggerBit1[ievt];
  fTriggerBit2=aBuffer.TriggerBit2[ievt];
  fEvtNum=aBuffer.EvtNum[ievt];
  strcpy(fRun,(aBuffer.Run[ievt]).c_str()) ;
  fRawBaseline=aBuffer.RawBaseline[ievt];
  fRawNoise=aBuffer.RawNoise[ievt];
  fNbPeaks=aBuffer.NbPeaks[ievt];
  fRawAmpl=aBuffer.RawAmpl[ievt];
  fRawBin=aBuffer.RawBin[ievt];
  fFitAmpl=aBuffer.FitAmpl[ievt];
  fFitZeroAmpl=aBuffer.FitZeroAmpl[ievt];
  fFitBin=aBuffer.FitBin[ievt];
  fFitChi2=aBuffer.FitChi2[ievt];
  fFitAmplErr=aBuffer.FitAmplErr[ievt];
  fFitFilterAmpl=aBuffer.FitFilterAmpl[ievt];
  fFitFilterZeroAmpl=aBuffer.FitFilterZeroAmpl[ievt];
  fFitFilterBin=aBuffer.FitFilterBin[ievt];
  fFitFilterChi2=aBuffer.FitFilterChi2[ievt];
  fFitFilterAmplErr=aBuffer.FitFilterAmplErr[ievt];
  fWienerAmpl=aBuffer.WienerAmpl[ievt];
  fWienerZeroAmpl=aBuffer.WienerZeroAmpl[ievt];
  fWienerBin=aBuffer.WienerBin[ievt];
  fWienerChi2=aBuffer.WienerChi2[ievt];
  fWienerAmplErr=aBuffer.WienerAmplErr[ievt];
  fRiseTime=aBuffer.RiseTime[ievt];
  fFallTime=aBuffer.FallTime[ievt];
  fFWHM=aBuffer.FWHM[ievt];
  for (Int_t i=0; i<NBMAX_AMPL; i++) {
    fWienerSyncAmpl[i]=((aBuffer.WienerSyncAmpl)[i])[ievt];
    fWienerSyncChi2[i]=((aBuffer.WienerSyncChi2)[i])[ievt];
  }
}

PulseVariables EdwNtuple::ComputeSinglePulse(EdwPulse* aEdwPulse, EdwTemplate& aTmplt, EdwPreprocessData& aPreprocess, Short_t aSign, Int_t aPattern, Int_t aOffsetScanMin, Int_t aOffsetScanMax, const vector<Float_t> *aFixTimes) 
{
  FitPulse lPulse(aEdwPulse);
  PulseVariables lData = {0};
	lData = ComputeSinglePulse(lPulse, aTmplt, aPreprocess, aSign, aPattern, aOffsetScanMin, aOffsetScanMax, aFixTimes);

	return lData;
}

PulseVariables EdwNtuple::ComputeSinglePulse(FitPulse& lPulse, EdwTemplate& aTmplt, EdwPreprocessData& aPreprocess, Short_t aSign, Int_t aPattern, Int_t aOffsetScanMin, Int_t aOffsetScanMax, const vector<Float_t> *aFixTimes)
{
  
  PulseVariables lData = {0};
  for (UInt_t k=0; k<NBMAX_AMPL; k++) {
    (lData.WienerSyncAmpl)[k]=0;
    (lData.WienerSyncChi2)[k]=0;
  } // to be sure.. (avoid NAN values..?)
  
  // First basic processing 
  if (aSign != 0) lPulse.SetSign(aSign);
  if (aPattern != 0) lPulse.SetPatternLength((UShort_t)aPattern);
  lPulse.SetBaseFromPeakBin(aTmplt.PulseBin());
  // (si pas de vrai template c'est pas grave, le bin=0 donc ca fait rien...)
  lPulse.BasicPreprocess();
  lPulse.FindPeaks(FP_STRICT); // to debatte
                               // Fit operations, using the preprocessing database
  
  if (aFixTimes == NULL) {
    lData.RawBaseline=lPulse.MeanBase();
    lData.RawNoise=lPulse.ComputeNoise();
    lData.NbPeaks=lPulse.GetPeakBins().size();
    lData.RawAmpl=lPulse.GetSimpleAmpl();
    lData.RawBin=lPulse.GetSimpleAmplBin();
    if (lPulse.IsHeat()) { // Calcule seuleemnt pour chaleur! (gain cpu)
      if (fShapeFlag == SHAPE_RISE || fShapeFlag == SHAPE_ALL) 
        lData.RiseTime=lPulse.GetRiseTime();
      if (fShapeFlag == SHAPE_FALL || fShapeFlag == SHAPE_ALL) 
        lData.FallTime=lPulse.GetFallTime();
      if (fShapeFlag == SHAPE_FWHM || fShapeFlag == SHAPE_ALL) 
        lData.FWHM=lPulse.GetFWHM();
    }
    if (fAmplFlag == AMPL_FIT || fAmplFlag == AMPL_ALL) {
      vector<Float_t> lTimeFitParams;
      if (aOffsetScanMin != NOCENTER) {
        //      Float_t lOffsetMin = aCenterScanBin-SCANRANGE_IONBIN_FROM_HEAT ;
        //      Float_t lOffsetMax = aCenterScanBin+SCANRANGE_IONBIN_FROM_HEAT ;
        lTimeFitParams = lPulse.GetFitAmpl(aTmplt.Trace(),1,1,aOffsetScanMin,aOffsetScanMax);
      } else {
        lTimeFitParams = lPulse.GetFitAmpl(aTmplt.Trace());
      }
      lData.FitAmpl=lTimeFitParams.at(0);
      lData.FitBin=lTimeFitParams.at(1);
      lData.FitChi2=lTimeFitParams.at(2);
      lData.FitAmplErr=lTimeFitParams.at(3);
      lData.FitZeroAmpl = (lPulse.GetFitAmpl(aTmplt.Trace(),0))[0];
    }
    if (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL) {
      if (aPreprocess.NoiseSpectrum().size() != 0) {
        Bool_t lFast =1;
        vector<Float_t> lWienerFitParams;
        if (aOffsetScanMin != NOCENTER) {
          //	Float_t lOffsetMin = aCenterScanBin-SCANRANGE_IONBIN_FROM_HEAT ;
          //	Float_t lOffsetMax = aCenterScanBin+SCANRANGE_IONBIN_FROM_HEAT ;
          lWienerFitParams = lPulse.GetFitWiener(&aTmplt,aPreprocess.NoiseSpectrum(),lFast,1,aOffsetScanMin,aOffsetScanMax);
        } else lWienerFitParams = lPulse.GetFitWiener(&aTmplt,aPreprocess.NoiseSpectrum(),lFast); // Note: now with 0.2 bin accuracy for heat par default.
        lData.WienerAmpl=lWienerFitParams.at(0);
        lData.WienerBin=lWienerFitParams.at(1);
        lData.WienerChi2=lWienerFitParams.at(2);
        lData.WienerAmplErr=lWienerFitParams.at(3);
        lData.WienerZeroAmpl=(lPulse.GetWienerAmpl(aTmplt.TraceFFT(),aPreprocess.NoiseSpectrum(),1))[0];
      } // sinon eh ben amplitude nulle pour le fit, facile a couper!
    }
  }
  
  // Add amplitudes at fixed times: 1)for heat channels with ion timings
  // 2) for ion channels (in that case it is not exactly fixed times...)
  // Pour l'instant on est cochon on fait que wiener. et il faut noise spectrum!
  if (aFixTimes != NULL && aPreprocess.NoiseSpectrum().size() != 0) {
    vector<Float_t> lFixTimes = *aFixTimes;
    if (lFixTimes.size() > NBMAX_AMPL) {
      cerr <<"Oh la la trop d'ampls a t fixe a calculer.. exit" << endl; exit(-1);
    }
    if (lPulse.IsHeat()) { // case 1)
      for (UInt_t k=0; k<lFixTimes.size(); k++) {
        if (lFixTimes[k] != 0) { // sinon c'est que le bin ion est zero donc rien a caler..
          Bool_t DoFFT=1; Bool_t DoChi2=1; // now we must do fft here!
          Float_t lOffset = lFixTimes[k]-aTmplt.PulseBin(); // !! convention bin/time...!!
                                                            // NB: si lOffset est trop au bord, ca va se mettre juste au bord du range
                                                            // pour lequel des fft sont calculees
          FloatVect lTmpltFFT = aTmplt.GetNonIntegerOffsetFFT(lOffset);
          vector<Float_t> lParams = lPulse.GetWienerAmpl(lTmpltFFT,aPreprocess.NoiseSpectrum(),DoFFT,DoChi2);
          if (finite(lParams.at(0)) == 0) {lParams[0]=0; lParams[2]=0;} // a cause de valeurs parfois NaN, sans qu'on sache pourquoi..
          (lData.WienerSyncAmpl)[k]=lParams.at(0);
          (lData.WienerSyncChi2)[k]=lParams.at(2);
        } else {
          (lData.WienerSyncAmpl)[k]=0;
          (lData.WienerSyncChi2)[k]=0;
        }
      }
    } else { // case 2) ion->ion
      for (UInt_t k=0; k<lFixTimes.size(); k++) {
        if (lFixTimes[k] != 0) { // sinon c'est que le bin ion est zero
          Bool_t lNoFixSign=1; Bool_t lFast=1;
          Float_t lOffset = lFixTimes[k]-aTmplt.PulseBin(); // !! convention bin/time...!!
          vector<Float_t> lParams = lPulse.GetFitWiener(&aTmplt,aPreprocess.NoiseSpectrum(),lFast,0,lOffset-SCANRANGE_IONBIN_FROM_ION,lOffset+SCANRANGE_IONBIN_FROM_ION,1,"",lNoFixSign);
          if (finite(lParams.at(0)) == 0) {lParams[0]=0; lParams[2]=0;}
          (lData.WienerSyncAmpl)[k]=lParams.at(0);
          (lData.WienerSyncChi2)[k]=lParams.at(2);
        } else {
          (lData.WienerSyncAmpl)[k]=0;
          (lData.WienerSyncChi2)[k]=0;
        }
      }
    }
  }
  
  // This step must be done in the end because the processed trace is modified
  // by the filtering operation!
  if (aFixTimes == NULL && (fAmplFlag == AMPL_FITFILT || fAmplFlag == AMPL_ALL)) {
    lPulse.FilterTrace(aTmplt.GetDirectFilter(),aTmplt.GetInverseFilter());
    vector<Float_t> lTimeFitParams;
    if (aOffsetScanMin != NOCENTER) {
      //      Float_t lOffsetMin = aCenterScanBin-SCANRANGE_IONBIN_FROM_HEAT ;
      //      Float_t lOffsetMax = aCenterScanBin+SCANRANGE_IONBIN_FROM_HEAT ;
      lTimeFitParams = lPulse.GetFitAmpl(aTmplt.FilteredTrace(),1,1,aOffsetScanMin,aOffsetScanMax);
    } else {
      lTimeFitParams = lPulse.GetFitAmpl(aTmplt.FilteredTrace());
    }
    lData.FitFilterAmpl=lTimeFitParams.at(0);
    lData.FitFilterBin=lTimeFitParams.at(1);
    lData.FitFilterChi2=lTimeFitParams.at(2);
    lData.FitFilterAmplErr=lTimeFitParams.at(3);
    lData.FitFilterZeroAmpl=(lPulse.GetFitAmpl(aTmplt.FilteredTrace(),0))[0];
  }
  
  
  return lData;
  
}




void EdwNtuple::SingleChannelEventLoop(UInt_t jchannel, TChain* aChain, EdwNtpBuffer& aBuffer, EdwAnaDB* aDB, EdwRawDB* aRawDB, EdwEvent* aEvt, EdwPulse* aPulse, ULong_t aOffset, Int_t aSynchroFlag) {

  string lChannel = fChannels[jchannel];
  string lSyncChannel; vector<string> lSyncChannels; vector<Float_t> lSyncParams;
  UInt_t numSyncChannel; vector<UInt_t> numSyncChannels;
  if (aSynchroFlag == 1) { // center scan chal --> ion
    lSyncChannel = this->GetSyncHeatChannel(lChannel);
    lSyncParams = this->GetSyncParams(lSyncChannel); // prend la chaleur en argument
    numSyncChannel = vectnum(fChannels,lSyncChannel);
  }
  if (aSynchroFlag == 2) {
    lSyncChannels = this->GetSyncIonChannels(lChannel);
    lSyncParams = this->GetSyncParams(lChannel); // prend la chaleur en argument
    for (UInt_t j=0; j<lSyncChannels.size(); j++)
      numSyncChannels.push_back(vectnum(fChannels,lSyncChannels[j]));
  }
  if (aSynchroFlag == 3) { // ion collectrice --> ion veto
    lSyncChannels = this->GetSyncIonChannels(lChannel);
    for (UInt_t j=0; j<lSyncChannels.size(); j++)
      numSyncChannels.push_back(vectnum(fChannels,lSyncChannels[j]));
  }
  UInt_t lIonLength;
  Int_t lHeatOffsetMin, lHeatOffsetMax = 0;
  if (aSynchroFlag != 1 && aSynchroFlag != -1 && isheat(lChannel) && // static limit to scan range due to ion window
      (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL) ) {
    lSyncParams = this->GetSyncParams(lChannel); // prend la chaleur en argument
    lSyncChannels = this->GetSyncIonChannels(lChannel);
    lIonLength = (aDB->GetTemplate(lSyncChannels[0])).Trace().size();
  }
  EdwTemplate lTemplate;
  EdwPreprocessData lPreprocess;
  UInt_t lBufSize = aBuffer.BufferSize;
  for (UInt_t ievt=0; ievt<lBufSize; ievt++) {
    ULong_t p = aOffset+(ULong_t)ievt;
    if (p >= (ULong_t)(aChain->GetEntries())) {
      cerr<<"Ntuple: error in event num. Exiting" << endl; exit(-1);
    }
    aChain->GetEntry(p);
    time_t EvtDate = aEvt->Header()->DateSec();
    if (jchannel == 0) {
      aBuffer.EvtNum[ievt] = aEvt->Header()->Num();
      aBuffer.Run[ievt] = aEvt->Header()->Run();
      aBuffer.DateMuSec[ievt] = aEvt->Header()->DateMuSec();
      aBuffer.DateSec[ievt] = (ULong64_t)EvtDate;
      aBuffer.TriggerBit1[ievt] = (Int_t)(aEvt->Header()->TriggerBit(1));
      aBuffer.TriggerBit2[ievt] = (Int_t)(aEvt->Header()->TriggerBit(2));
    }
    aPulse = aEvt->Pulse(lChannel);
    //    aBuffer.ClearPulseVar(ievt,jchannel);
    if (aPulse != NULL) { // Changement de position de cette condition aout 09
      // Application: ID2 est HS pendant quelques temps.
      // pas de pulse-> il faut surtout pas chercher la anadb qui est a zero!
      if (fAmplFlag != AMPL_RAW) {
	// decide if new spectrum needs to be reloaded...
	if (!((lTemplate.Trace()).size())) {
	  lTemplate = aDB->GetTemplate(lChannel);
	  if (aSynchroFlag != 1 && aSynchroFlag != -1 && isheat(lChannel) &&
	      (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL)) {
	    lHeatOffsetMin = (Int_t)lSyncParams[1]-lTemplate.PulseBin();
	    lHeatOffsetMax = lHeatOffsetMin+(Int_t)(lSyncParams[0]*lIonLength);
	  }
	}
	if (lPreprocess.StartTime() == 0 || !lPreprocess.IsTimeOk(EvtDate)) {
	  lPreprocess = aDB->GetPreprocess(EvtDate,lChannel);
	  if (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL) {
	    if (KVERBOSE) cout <<"* EdwNtuple: Computing kernels"<< endl;
	    lTemplate.SetTStartKernel(lPreprocess.StartTime());
	    lTemplate.SetTEndKernel(lPreprocess.EndTime());
	    lTemplate.ComputeKernels(lPreprocess.NoiseSpectrum());
	  }
	}
      }
      Short_t lSign = 0;
      if (aRawDB != NULL && !aPulse->IsHeat())
	lSign = -(aRawDB->GetVoltageSign(aBuffer.Run[ievt],lChannel));
      // Le pattern: depend de rawdb et d'un eventuel pattern specifie
      // dans anadb.
      Int_t lPattern = 0;
      if (aRawDB != NULL && !aPulse->IsHeat())
	lPattern = aRawDB->GetPatternLength(aBuffer.Run[ievt],lChannel);
      if (lPreprocess.NbPtsPatterns().size() == 1) { 
	// pas de cas a plus que 1 pour l'instant
	Int_t more_pattern=(lPreprocess.NbPtsPatterns())[0];
	if (more_pattern != 0 && more_pattern > lPattern) 
	  lPattern = more_pattern; // on depatterne avec le pattern le plus long!
      }
      PulseVariables lData;
      if (aSynchroFlag == -1) {
	lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern);
	
      } else if (aSynchroFlag == 0) {
	if (isheat(lChannel) && (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL)) {
	  Int_t lMin = FIT_TIMEMIN_HEAT;
	  Int_t lMax = FIT_TIMEMAX_HEAT;
	  if (lMin < lHeatOffsetMin) lMin = lHeatOffsetMin;
	  if (lMax > lHeatOffsetMax) lMax = lHeatOffsetMax;
	  lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern,lMin,lMax);
	} else lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern);


      } else if (aSynchroFlag == 1) { // centerscanbin (chal-->ion)
	// tchal=a*tion+b
	Float_t t_chal = (aBuffer.WienerBin)[ievt][numSyncChannel];
	Float_t t_ion = (t_chal-lSyncParams[1])/lSyncParams[0];
	Int_t bin_ion = (Int_t)t_ion - lTemplate.PulseBin();
	if (t_chal < 0.1) bin_ion = 0;
	Int_t lOffsetMin = bin_ion-SCANRANGE_IONBIN_FROM_HEAT ;
	Int_t lOffsetMax = bin_ion+SCANRANGE_IONBIN_FROM_HEAT ;
	lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern,lOffsetMin,lOffsetMax);

      } else if(aSynchroFlag == 2) { // fixtimes (ion-->chal)
	vector<Float_t> t_chal;
	for (UInt_t k=0; k<numSyncChannels.size();k++) {	  
	  Float_t t_ion = (aBuffer.WienerBin)[ievt][numSyncChannels[k]];
	  if (t_ion != 0) t_chal.push_back(lSyncParams[0]*t_ion + lSyncParams[1]);
	  else t_chal.push_back(0); // dans ce cas on va mettre l'ampl sync a zero...
	}
	if (isheat(lChannel) && (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL)) {
	  Int_t lMin = FIT_TIMEMIN_HEAT;
	  Int_t lMax = FIT_TIMEMAX_HEAT;
	  if (lMin < lHeatOffsetMin) lMin = lHeatOffsetMin;
	  if (lMax > lHeatOffsetMax) lMax = lHeatOffsetMax;
	  lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern,lMin,lMax,&t_chal);
	} else lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern,NOCENTER,NOCENTER,&t_chal); // nomrmalement jamais!

      } else if (aSynchroFlag == 3) { // narrow scan range (ion-->ion : same timing)
	vector<Float_t> t_syncion;
	for (UInt_t k=0; k<numSyncChannels.size();k++)
	  t_syncion.push_back( (aBuffer.WienerBin)[ievt][numSyncChannels[k]] );
	lData = this->ComputeSinglePulse(aPulse,lTemplate,lPreprocess,lSign,lPattern,0,0,&t_syncion);

      }
      if (aSynchroFlag == 2 || aSynchroFlag == 3) {
	aBuffer.UpdateFromPulseVar(lData,ievt,jchannel); // add only the "sync" variables
      } else {
	aBuffer.ClearPulseVar(ievt,jchannel); // Now only here!
	aBuffer.FillFromPulseVar(lData,ievt,jchannel);
      }

    }
    aEvt->Clear();
    if ( KVERBOSE && ievt%100 == 0 ) cout << "* Event "<< p << endl;
  }

}


/** Main drivers */

void EdwNtuple::FillDataFromChain(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB, ULong_t iStart, ULong_t iStop, EdwRawDB* aRawDB) {
  // Routine basique et assez obsolete, pas mise a jour

  if (aDB != NULL) {
    if (aDB->IsWritable()) cerr << "EdwNtuple: Warning, AnaDB is writable." << endl;
  }

  // Read events
  EdwEvent* lEvt=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvt);
  //  UInt_t nevts=aChain->GetEntries();
  UInt_t lChainLength = aChain->GetEntries();
  if (iStop == 0 && iStart != 0) {cerr << "istop pbl. Exit"<<endl; exit(-1);}
  if (iStop == 0 || iStop > lChainLength) iStop = lChainLength;
  if (iStart > iStop) { cerr <<"istop-istart pbl. Exit"<<endl; exit(-1);}
  UInt_t nevts = (UInt_t)(iStop-iStart);
  fChannels = aChannels;
  this->PrepareNtp();

  // Fill the Ntuple
  EdwPulse* lEdwPulse = new EdwPulse();
  EdwTemplate* lTemplate = new EdwTemplate();
  EdwPreprocessData* lPreprocess = new EdwPreprocessData();
  for (UInt_t ievt=0; ievt<nevts; ievt++) {
    aChain->GetEntry(iStart+(ULong_t)ievt);
    fEvtNum = lEvt->Header()->Num();
    strcpy(fRun,(lEvt->Header()->Run()).c_str()) ;
    fDateMuSec = lEvt->Header()->DateMuSec();
    fTriggerBit1 = (Int_t)(lEvt->Header()->TriggerBit(1));
    fTriggerBit2 = (Int_t)(lEvt->Header()->TriggerBit(2));
    time_t lDate = lEvt->Header()->DateSec();
    fDateSec = (ULong64_t)lDate;
    for (UInt_t j=0; j<fChannels.size(); j++) {
      lEdwPulse = lEvt->Pulse(fChannels[j]);
      fRawBaseline[j]=0;
      fRawNoise[j]=0;
      fNbPeaks[j]=0;
      fRawAmpl[j]=0;
      fRawBin[j]=0;
      fFitAmpl[j]=0;
      fFitZeroAmpl[j]=0;
      fFitBin[j]=0;
      fFitChi2[j]=0;
      fFitAmplErr[j]=0;
      fFitFilterAmpl[j]=0;
      fFitFilterZeroAmpl[j]=0;
      fFitFilterBin[j]=0;
      fFitFilterChi2[j]=0;
      fFitFilterAmplErr[j]=0;
      fWienerAmpl[j]=0;
      fWienerZeroAmpl[j]=0;
      fWienerBin[j]=0;
      fWienerChi2[j]=0;
      fWienerAmplErr[j]=0;
      fRiseTime[j]=0;
      fFallTime[j]=0;
      fFWHM[j]=0;
      if (lEdwPulse != NULL) {
	FitPulse lPulse(lEdwPulse);
	// First basic processing
	// Fit operations, using the preprocessing database
	if (aRawDB != NULL && !lPulse.IsHeat()) {
	  lPulse.SetSign(-(aRawDB->GetVoltageSign(lEvt->Header()->Run(),fChannels[j]))); //NEW
	  lPulse.SetPatternLength((UShort_t)aRawDB->GetPatternLength(lEvt->Header()->Run(),fChannels[j]));
	}
	if (fAmplFlag != AMPL_RAW) {
	  lTemplate = aDB->GetTemplate(lDate,fChannels[j]);
	  lPreprocess = aDB->GetPreprocessPtr(lDate,fChannels[j]);
	  lPulse.SetBaseFromPeakBin(lTemplate->PulseBin()); // NEW!!
	}
	lPulse.BasicPreprocess();
	lPulse.FindPeaks(FP_STRICT); // to debatte
	fRawBaseline[j]=lPulse.MeanBase();
	fRawNoise[j]=lPulse.ComputeNoise();
	fNbPeaks[j]=lPulse.GetPeakBins().size();
	fRawAmpl[j]=lPulse.GetSimpleAmpl();
	fRawBin[j]=lPulse.GetSimpleAmplBin();
	if (fShapeFlag == SHAPE_RISE || fShapeFlag == SHAPE_ALL) 
	  fRiseTime[j]=lPulse.GetRiseTime();
	if (fShapeFlag == SHAPE_FALL || fShapeFlag == SHAPE_ALL) 
	  fFallTime[j]=lPulse.GetFallTime();
	if (fShapeFlag == SHAPE_FWHM || fShapeFlag == SHAPE_ALL) 
	  fFWHM[j]=lPulse.GetFWHM();
	if (fAmplFlag == AMPL_FIT || fAmplFlag == AMPL_ALL) {
	  vector<Float_t> lTimeFitParams = lPulse.GetFitAmpl(lTemplate->Trace());
	  fFitAmpl[j]=lTimeFitParams.at(0);
	  fFitBin[j]=lTimeFitParams.at(1);
	  fFitChi2[j]=lTimeFitParams.at(2);
	  fFitAmplErr[j]=lTimeFitParams.at(3);
	  fFitZeroAmpl[j] = (lPulse.GetFitAmpl(lTemplate->Trace(),0))[0];
	}
	if (fAmplFlag == AMPL_WIENER || fAmplFlag == AMPL_ALL) {
	  if ((lPreprocess->NoiseSpectrum()).size() != 0) {
	    Bool_t lFast =0;
	    vector<Float_t> lWienerFitParams = lPulse.GetFitWiener(lTemplate,lPreprocess->NoiseSpectrum(),lFast);
	    fWienerAmpl[j]=lWienerFitParams.at(0);
	    fWienerBin[j]=lWienerFitParams.at(1);
	    fWienerChi2[j]=lWienerFitParams.at(2);
	    fWienerAmplErr[j]=lWienerFitParams.at(3);
	    fWienerZeroAmpl[j]=(lPulse.GetWienerAmpl(lTemplate->TraceFFT(),lPreprocess->NoiseSpectrum(),1))[0];
	  }
	}
	// This step must be done in the end because the processed trace is modified
	// by the filtering operation!
	if (fAmplFlag == AMPL_FITFILT || fAmplFlag == AMPL_ALL) {
	  lPulse.FilterTrace(lTemplate->GetDirectFilter(),lTemplate->GetInverseFilter());
	  vector<Float_t> lTimeFitParams = lPulse.GetFitAmpl(lTemplate->FilteredTrace());
	  fFitFilterAmpl[j]=lTimeFitParams.at(0);
	  fFitFilterBin[j]=lTimeFitParams.at(1);
	  fFitFilterChi2[j]=lTimeFitParams.at(2);
	  fFitFilterAmplErr[j]=lTimeFitParams.at(3);
	  fFitFilterZeroAmpl[j]=(lPulse.GetFitAmpl(lTemplate->FilteredTrace(),0))[0];
	}
      }
    }
    if ( KVERBOSE && ievt%100 == 0 ) cout << "* Event "<<(ULong_t)ievt+iStart << endl;
    fFile->cd(); Int_t lFlag = fTree->Fill();
    if (lFlag <= 0) cerr << "EdwNtuple / TTree::Fill(): error "<<lFlag<<" for event "<<ievt<<endl;
    lEvt->Clear();
  }
  fFile->cd(); fTree->Write("",kOverwrite);

  delete lTemplate;
  delete lPreprocess;
}

void EdwNtuple::FillDataByChannelOrder(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB, ULong_t iStart, ULong_t iStop, EdwRawDB* aRawDB) {

  if (aRawDB->IsWritable()) cerr << "EdwNtuple: Warning, RawDB is writable." << endl;
  if (aDB->IsWritable()) cerr << "EdwNtuple: Warning, AnaDB is writable." << endl;

  // Read events
  EdwEvent* lEvt=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvt);
  ULong_t lChainLength = aChain->GetEntries();
  if (iStop == 0 && iStart != 0) {cerr << "istop pbl. Exit"<<endl; exit(-1);}
  if (iStop == 0 || iStop > lChainLength) iStop = lChainLength;
  if (iStart > iStop) { cerr <<"istop-istart pbl. Exit"<<endl; exit(-1);}
  UInt_t nevts = (UInt_t)(iStop-iStart);
  fChannels = aChannels;
  UInt_t NbChannels = fChannels.size();
  this->PrepareNtp();
  
  // Loop on "buffers" (for each buffer there is a loop on channels first)
  EdwPulse* lEdwPulse = new EdwPulse();
  Int_t NbBuffers = nevts/fEvtBufferSize ;
  UInt_t LastBufferSize = (UInt_t)(nevts - NbBuffers*fEvtBufferSize) ;
  if (LastBufferSize != 0) NbBuffers+=1;
  cout << "*** EdwNtuple: nb of buffers: "<<NbBuffers<<" - nb of evts: "<<nevts<<endl;

  for (Int_t ibuffer=0; ibuffer<NbBuffers; ibuffer+=1) {
    //  for (Int_t ibuffer=0; ibuffer<1; ibuffer+=1) {
    UInt_t lBufferSize = (ibuffer == NbBuffers-1 && LastBufferSize != 0) ? LastBufferSize : fEvtBufferSize ;
    EdwNtpBuffer lBuf(lBufferSize,NbChannels);
    // Loop on channels
    for (UInt_t j=0; j<NbChannels; j++) {
      string lChannel = fChannels[j];
      cout << "*** EdwNtuple: Buffer "<<ibuffer<<" - Channel "<<lChannel<<endl;
      ULong_t lOffset = iStart+(ULong_t)(ibuffer*fEvtBufferSize);
      this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset);
    }

    if (KVERBOSE) cout << "* EdwNtuple: Filling ntp"<<endl;
    // Now fill the ntuple with the buffer matrices
    for (UInt_t ievt=0; ievt<lBufferSize; ievt++) {
      this->FillEvtFromBuffer(ievt,lBuf);
      fFile->cd(); fTree->Fill();
    }
  }
  fFile->cd(); fTree->Write("",kOverwrite);

}

// todo in the end: also compute fit at fixed time for wiener...

void EdwNtuple::FillDataWithChannelSynchro(TChain* aChain, vector<string> aChannels, EdwAnaDB* aDB, ULong_t iStart, ULong_t iStop, EdwRawDB* aRawDB) {

  if (aRawDB->IsWritable()) cerr << "EdwNtuple: Warning, RawDB is writable." << endl;
  if (aDB->IsWritable()) cerr << "EdwNtuple: Warning, AnaDB is writable." << endl;
  if (fHeatIonSynchroFile == "None") cerr << "EdwNtuple: no synchrofile.. pbl.."<<endl;

  // Read events
  EdwEvent* lEvt=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvt);
  ULong_t lChainLength = aChain->GetEntries();
  if (iStop == 0 && iStart != 0) {cerr << "istop pbl. Exit"<<endl; exit(-1);}
  if (iStop == 0 || iStop > lChainLength) iStop = lChainLength;
  if (iStart > iStop) { cerr <<"istop-istart pbl. Exit"<<endl; exit(-1);}
  UInt_t nevts = (UInt_t)(iStop-iStart);
  fChannels = aChannels;
  UInt_t NbChannels = fChannels.size();
  this->PrepareNtp();
  
  // Loop on "buffers" (for each buffer there is a loop on channels first)
  EdwPulse* lEdwPulse = new EdwPulse();
  Int_t NbBuffers = nevts/fEvtBufferSize ;
  UInt_t LastBufferSize = (UInt_t)(nevts - NbBuffers*fEvtBufferSize) ;
  if (LastBufferSize != 0) NbBuffers+=1;
  cout << "*** EdwNtuple: nb of buffers: "<<NbBuffers<<" - nb of evts: "<<nevts<<endl;

  for (Int_t ibuffer=0; ibuffer<NbBuffers; ibuffer+=1) {
    //  for (Int_t ibuffer=0; ibuffer<1; ibuffer+=1) {
    UInt_t lBufferSize = (ibuffer == NbBuffers-1 && LastBufferSize != 0) ? LastBufferSize : fEvtBufferSize ;
    EdwNtpBuffer lBuf(lBufferSize,NbChannels);
    ULong_t lOffset = iStart+(ULong_t)(ibuffer*fEvtBufferSize);

    // 1st Loop : heat channels
    for (UInt_t j=0; j<NbChannels; j++) {
      if (isheat(fChannels[j])) {
	string lChannel = fChannels[j];
	cout << "*** EdwNtuple: Buffer "<<ibuffer<<" - Channel "<<lChannel<<endl;
	if ((this->GetSyncIonChannels(lChannel)).size() != 0) {
	  this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset,0);
	} else {
	  this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset,-1);
	}
      }
    }
    // 2nd Loop : ion channels, from heat timing
    for (UInt_t j=0; j<NbChannels; j++) {
      if ( ! isheat(fChannels[j]) ) {
	string lChannel = fChannels[j];
	cout << "*** EdwNtuple: Buffer "<<ibuffer<<" - Channel "<<lChannel<<endl;
	this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset,1);
      }
    }

    // 3rd loop : heat channels, from ion timing 
    // and, in addition, "veto" channels from collecting electrode timing.
    for (UInt_t j=0; j<NbChannels; j++) {
      string lChannel = fChannels[j];
      if ( isheat(lChannel) ) {
	if ((this->GetSyncIonChannels(lChannel)).size() != 0) {
	  cout << "*** EdwNtuple: Buffer "<<ibuffer<<" - Enhance channel "<<lChannel<<endl;
	  this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset,2);
	}
      } else {
	if ((this->GetSyncIonChannels(lChannel)).size() != 0) {
	  cout << "*** EdwNtuple: Buffer "<<ibuffer<<" - Enhance channel "<<lChannel<<endl;
	  this->SingleChannelEventLoop(j,aChain,lBuf,aDB,aRawDB,lEvt,lEdwPulse,lOffset,3);
	}
      }
    }

    if (KVERBOSE) cout << "* EdwNtuple: Filling ntp"<<endl;
    // Now fill the ntuple with the buffer matrices
    for (UInt_t ievt=0; ievt<lBufferSize; ievt++) {
      this->FillEvtFromBuffer(ievt,lBuf);
      fFile->cd(); fTree->Fill();
    }
  }
  fFile->cd(); fTree->Write("",kOverwrite);

}


void EdwNtuple::PlotSelectEvents(TChain* aChain, vector<string> aChannels, string aEvtListFile, string aPlotDir, EdwAnaDB* aDB, EdwRawDB* aRawDB) {

  // Warning: the fits done in this routine do not include complete channel synchronization..
  if (aRawDB->IsWritable()) cerr << "EdwNtuple: Warning, RawDB is writable." << endl;
  if (aDB->IsWritable()) cerr << "EdwNtuple: Warning, AnaDB is writable." << endl;
  if (aPlotDir == "None") {
    cerr << "PlotSelectEvents: plot dir not defined. Exiting" << endl; exit(-1);
  }

  // Read the event list
  ifstream lFile(aEvtListFile.c_str(),ios::in);
  if (!lFile) {
    cerr << "Error opening event list file " << aEvtListFile << endl; exit(-1);
  }
  vector<string> lRunList;
  vector<UInt_t> lEvtList;
  string line;
  while (getline(lFile,line)) {
    istringstream ss(line);
    string run; ss >> run; lRunList.push_back(run);
    UInt_t i; ss >> i; lEvtList.push_back(i);
  }
  lFile.close();

  EdwEvent* lEvt=new EdwEvent();
  aChain->SetBranchAddress("Event",&lEvt);
  EdwPulse* lEdwPulse = new EdwPulse();
  EdwTemplate* lTemplate = new EdwTemplate();
  EdwPreprocessData* lPreprocess = new EdwPreprocessData();
  for (UInt_t ievt=0; ievt<aChain->GetEntries(); ievt++) {
    aChain->GetEntry(ievt);
    UInt_t lEvtNum = (UInt_t)(lEvt->Header()->Num());
    string lRun = lEvt->Header()->Run();
    Int_t jsel = uintnum(lEvtList,lEvtNum);
    if (jsel != -1 && lRun == lRunList[jsel]) { // Display this event
      if (KVERBOSE)	cerr << "Evt "<<lEvtNum<<" in "<<lRun<<" (chain: "<<ievt<<")"<<endl;
      time_t lDate = lEvt->Header()->DateSec();
      for (UInt_t k=0; k<lEvt->NbPulses(); k++) {
	lEdwPulse = lEvt->Pulse(k);
	if (lEdwPulse != NULL) {
	  FitPulse lPulse(lEdwPulse);
	  string lChannel = lPulse.Channel();
	  if (stringnum(aChannels,lChannel) != -1) { // Display this channel
	    lTemplate = aDB->GetTemplate(lDate,lChannel);
	    lPreprocess = aDB->GetPreprocessPtr(lDate,lChannel);
	    string lPlot = aPlotDir+"/"+lChannel+"_"+lRun+"_"+int2str(lEvtNum)+"_";
	    if (aRawDB != NULL && !lPulse.IsHeat()) {
	      lPulse.SetSign(-(aRawDB->GetVoltageSign(lRun,lChannel)));
	      lPulse.SetPatternLength((UShort_t)aRawDB->GetPatternLength(lRun,lChannel));
	    }
	    lPulse.SetBaseFromPeakBin(lTemplate->PulseBin());
	    lPulse.BasicPreprocess();
	    lPulse.FindPeaks(FP_STRICT);
	    // Plot Rise/fall times
	    lPulse.GetRiseTime(0.1,0.9,0,lPlot+"Rise.gif");
	    lPulse.GetFallTime(0.1,0.9,0,lPlot+"Fall.gif");
	    lPulse.GetFWHM(0.5,2,lPlot+"FWHM.gif");
	    lPulse.GetFitAmpl(lTemplate->Trace(),1,1,0,0,lPlot+"Fit.gif");
	    Bool_t lFast = 0; Bool_t lSubBin = 1;
	    lPulse.GetFitWiener(lTemplate,lPreprocess->NoiseSpectrum(),lFast,lSubBin,0,0,0,lPlot+"WienerFit.gif");
	    lPulse.FilterTrace(lTemplate->GetDirectFilter(),
			       lTemplate->GetInverseFilter());
	    lPulse.GetFitAmpl(lTemplate->FilteredTrace(),1,1,0,0,lPlot+"FitFilter.gif");
	  }
	}
      }
      
    }
  }

}

