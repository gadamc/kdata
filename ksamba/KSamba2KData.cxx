//______________________________________________________________________
//
// KSamba2KData.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
// This class reads the raw Samba files and converts into 
// a KData Raw file. 
//
//
// Currently, the header information of the Samba
// file is read out. But this is problematic if the format of the Samba
// file changes significantly. Therefore, it is planned that this 
// class will have the option to read the header information from 
// the database, which will be more robust since it will be indepenent
// from the actual formatting styles of the raw data file. The raw
// waveforms will still be read from the raw Samba files since it would
// be prohibitively expensive to store all of the data in the couchdb. 
//


#include "KSamba2KData.h"
#include "RConfig.h"
#include "KRawBolometerRecord.h"
#include "KRawSambaRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KRawEvent.h"
#include "Byteswap.h"
#include "TObjString.h"
#include "TObjArray.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "KDataProcessingInfo.h"


ClassImp(KSamba2KData);

KSamba2KData::KSamba2KData(void)
{
  fSambaFileName = "";
  fKdataFileName = "";
  InitializeMembers();
}

KSamba2KData::KSamba2KData(const char* inputSambaFile, const char* outputKRawEventFile)
{
  SetInputSambaFile(inputSambaFile);
  SetOutputKdataFile(outputKRawEventFile);
  InitializeMembers();
}


KSamba2KData::~KSamba2KData(void)
{
 
}

void KSamba2KData::InitializeMembers(void)
{
  
#ifdef R__BYTESWAP
  fLocalBigEndian = false;
#else
  fLocalBigEndian = true;
#endif
  
  //fSambaBigEndian = true;  //we assume big endianness by default.
                           //but this will be read from the Samba File
 
  fSambaFileLine = "";
  fSambaRunName = "";
  fSambaFileNum = -1;
  TString value = fLocalBigEndian==true ? "big" : "little";

  cout << "Local machine endianness: " <<  value.Data() << endl;
}

void  KSamba2KData::SetInputSambaFile(const char* fileName)
{
  fSambaFileName = fileName; 
}

void  KSamba2KData::SetOutputKdataFile(const char* fileName)
{
  fKdataFileName = fileName;
  
}

Bool_t  KSamba2KData::ConvertFile(void)
{
  //
  
  if(fSambaFileName == "" || fKdataFileName == ""){
    cerr << "KSamba2KData::ConvertFile. Input and Output file names are not set." << endl;
    return false;
  }
  
  cout << "Converting File. " << endl;
  cout << "   " << fSambaFileName << " -> " << fKdataFileName << endl;
  
  if(!OpenSambaFileStream()){
    cout << "Open Samba File Stream Fail." << endl;
    return false;
  }
    
  if(!OpenKdataFile()){
    cout << "Open Kdata File Fail." << endl;
    return false;
  }
  
  /*KDataProcessingInfo *info = (KDataProcessingInfo *)(fKdataOutput.GetTTree()->GetUserInfo()->Last());
  info->AddModule("ksamba");
  info->AddCommand("KSamba2KData::ConvertFile");
  */
  if(!CheckStartOfSambaFile()){
    cout << "Check Start of Samba File Fail." << endl;
    return false;
  }
    
  if(ReadSambaHeaderGeneral())
    if(ReadSambaDetectorConfigurations())
      if(ReadSambaChannelConfigurations()){
        if(ReadSambaRunHeader())
          if(ReadSambaData())
            if(CloseSambaFileStream())
              return CloseKdataFile();
      }
        
    
  
  return false;
}

Bool_t  KSamba2KData::ConvertFile(const char* inputSamba, const char* outputKRawEventFile)
{
  SetInputSambaFile(inputSamba);
  SetOutputKdataFile(outputKRawEventFile);
  InitializeMembers();
  return ConvertFile();
}

TString KSamba2KData::GetStringFromTokenizedStringResult(TObjArray *arr, Int_t index)
{
  char tab = 9;

  TString t ( ((TObjString *)arr->At(index))->GetString().Strip(TString::kBoth) );
  //sometimes there is a 'tab' character found in the string after the value we want
  //so we have to manually strip it off here because i can't figure out how to ask
  //TString to do it for me automatically
  //however, i assume that there is no tab at the beginning!
  //
  Ssiz_t tt = t.First(tab);
  TString s = t;
  if (tt != -1) {
    s= t(0,  tt  );
  }
  TString ss( s.Strip(TString::kBoth) ); //now remove any extra trailing/preceeding spaces

  return ss;
}

Int_t KSamba2KData::GetIntegerFromTokenizedStringResult(TObjArray *arr, Int_t index)
{
  TString s = GetStringFromTokenizedStringResult(arr, index);
  return s.Atoi();
}

Long64_t KSamba2KData::GetLongIntFromTokenizedStringResult(TObjArray *arr, Int_t index)
{
  TString s = GetStringFromTokenizedStringResult(arr, index);
  return s.Atoll();
}

Double_t KSamba2KData::GetFloatFromTokenizedStringResult(TObjArray *arr, Int_t index)
{
  TString s = GetStringFromTokenizedStringResult(arr, index);
  return s.Atof();
}

Bool_t KSamba2KData::CheckStartOfSambaFile(void)
{
  cout << "Check Start of Samba File" << endl;
  string startSamba = "* Archive SAMBA";
  
  fSambaFileLine.ReadToDelim(fSambaFileStream);  
  return fSambaFileLine.BeginsWith(startSamba.c_str()); 
}

Bool_t KSamba2KData::ReadSambaHeaderGeneral(void)
{
  // General Run header
  
  cout << "Reading Samba General Header." << endl;
  
  string endOfGeneralHeader = "* ----------" ; // 
  
  //set the run name based on the name of the file
  //given in fSambaFile. Then we look in the file directly
  //to get the run name based on the value given by
  //Fichier.
  size_t posOfFileName = fSambaFileName.find_last_of("/\\"); //the \\ should support windows file system as well as unix
  fSambaRunName = fSambaFileName.substr(posOfFileName == string::npos ? 0 : posOfFileName+1,
                    fSambaFileName.find_last_of("_"));
  fSambaFileNum = atoi(fSambaFileName.substr(fSambaFileName.find_last_of("_")+1).c_str());
  
  //loop through the general header section. we finish when we find
  //the endOfGeneralHeader keyword. 
  while ( !fSambaFileLine.BeginsWith(endOfGeneralHeader) 
         && !fSambaFileStream.eof()) {
    
    if(fSambaFileLine.BeginsWith("Byte-order")){
      cout << "Samba data: " << fSambaFileLine.Data() << endl;
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      TString s = GetStringFromTokenizedStringResult(arr, 1);
      
      if(s.Contains("big")){
        cout << "Setting samba endian: big" << endl;
        fSambaHeader.SetEndian(true);
      }
      else {  
        cout << "Setting samba endian: little" << endl;
        fSambaHeader.SetEndian(false);
      }
      
      delete arr;

      if(fSambaHeader.GetEndian() != fLocalBigEndian)
        cout << "     will swap bytes" << endl;
      else
        cout << "     no byte swap" << endl;
    
    }
    
    else if( fSambaFileLine.BeginsWith("Fichier") ) {
      TObjArray *path = fSambaFileLine.Tokenize("=#");
      TString s = GetStringFromTokenizedStringResult(path, 1);
      TObjArray *arr = s.Tokenize("/");
      fSambaHeader.SetRunName( GetStringFromTokenizedStringResult(arr, arr->GetLast()) ); 
      delete path;
      delete arr;
    }
       
    else if(fSambaFileLine.BeginsWith("Date") ) {
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetDate( GetStringFromTokenizedStringResult(arr, 1) );
      delete arr;
    }
  
    else if(fSambaFileLine.BeginsWith("Version") ){
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetVersion( GetStringFromTokenizedStringResult(arr, 1) );
      delete arr;
    }

    else if(fSambaFileLine.BeginsWith("Release") ){
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetRelease( GetStringFromTokenizedStringResult(arr, 1) );
      delete arr;
    }
    
    else if(fSambaFileLine.BeginsWith("Intitule") ){
      TObjArray *arr = fSambaFileLine.Tokenize("\""); //this one is special because of the quotes
      fSambaHeader.SetIntitule( ((TObjString *)arr->At(1))->GetString() );
      delete arr;
    }
    
    else if(fSambaFileLine.BeginsWith("Echantillonage") ){
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetEchantillonage ( GetFloatFromTokenizedStringResult(arr, 1) );
      delete arr;
    }
    
    else if(fSambaFileLine.BeginsWith("Bolo.nb") ){
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetNumBolos( GetIntegerFromTokenizedStringResult(arr, 1) );
      delete arr;
    }
    
    else if(fSambaFileLine.BeginsWith("Voies.nb") ){
      TObjArray *arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetNumChannels( GetIntegerFromTokenizedStringResult(arr, 1) );
      delete arr;
    }
    
    
    if (!fSambaFileStream.good()) {
      cerr << "ReadSambaHeaderGeneral. Unexpcted end of General Header line \""<< endOfGeneralHeader<< "\" not found" << endl;
      return false;
    }
                              
    fSambaFileLine.ReadToDelim(fSambaFileStream);
  } 
  return !fSambaFileStream.eof();
  
}

Bool_t KSamba2KData::ReadSambaDetectorConfigurations(void)
{
  cout << "Reading Detector Configurations." << endl;
  
  string startOfChannelConfig = "* Voie";
  string startOfNewDetectorConfig = "* Detecteur";
  
  //size_t endOfDetectorConfigSize = endOfDetectorConfig.size();
  
  while (!fSambaFileStream.eof() && !fSambaFileLine.BeginsWith(startOfChannelConfig)){
    
    if(fSambaFileLine.BeginsWith(startOfNewDetectorConfig)){
      
      TString detector = GetDetectorName(fSambaFileLine.Data());
      TObjArray *arr = fSambaFileLine.Tokenize(" ");
      TString sub = GetStringFromTokenizedStringResult(arr, 2);
      delete arr;

      if (!fSambaHeader.IsInDetectorList(detector.Data())){
        KSambaDetector *fNewDetector =  fSambaHeader.AddDetector();
        fNewDetector->SetName(detector.Data());
        cout << "Adding new detector to list: " << detector.Data() << endl;
        AddDetectorInfo(fNewDetector);
      }
      else  {  //we have already started adding this information
        AddDetectorInfo(detector.Data());
      }
    }
          
    fSambaFileLine.ReadToDelim(fSambaFileStream);
  }
  
  return !fSambaFileStream.eof();
}

const char* KSamba2KData::GetDetectorName(const char* line)
{
  //This expectes that line == * Detecteur (F)IDAB or 
  // * Detecteur Gc
  //
  // or in samba versions 9.20 and greater
  // * Detecteur "Name"  where Name = is the actual detector name - no parsing required.
  
  TString sline = line;
  TObjArray *arr = sline.Tokenize(" ");
  TString sub = GetStringFromTokenizedStringResult(arr, 2);
  delete arr;
   
  if(GetMajorVersion() >= 9 && GetMinorVersion() >= 19){
    return sub.Data();
  }
  else{
    TString detector;
    if (sub.BeginsWith("FID") || sub.BeginsWith("ID")){
      detector = sub(0, sub.Length()-2);  //I make assumptions about the structure of the Samba data
    }
    else if (sub.BeginsWith("Gc")){
      detector = sub(0,3);
    }
    return detector.Data();
  }
  
  
}

Bool_t KSamba2KData::AddDetectorInfo(KSambaDetector *detector)
{
  
  if(GetMajorVersion() >= 9 && GetMinorVersion() >= 19){
    return AddDetectorInfoPost919(detector);
  }
  else{
    return AddDetectorInfoPre919(detector);
  }
  
}

Bool_t KSamba2KData::AddDetectorInfoPost919(KSambaDetector *detector)
{
  string startOfChannelConfig = "* Voie";
  string startOfRun = "# ===== Entete de run ====="; 
  string endOfDetecorHeader = "* ----------" ; 
  
  TString bolo = GetDetectorName(fSambaFileLine.Data());
  
  if(bolo != detector->GetName()){
    cerr << "KSamba2KData::AddDetectorInfo. Mismatch!"<< endl;
    cerr << bolo.Data() << " != " << detector->GetName() << endl;
    return false; //this should be really really impossible. like, more impossible than normal.
  }
    
  TString aHackJobStartDateBecauseSambaChangedFormatOfGc1Header = "11/12/01";
  TString dateOfThisFile = fSambaHeader.GetDate();
  if(bolo.BeginsWith("Gc")){
    if (bolo.EqualTo("Gc1") && dateOfThisFile <= aHackJobStartDateBecauseSambaChangedFormatOfGc1Header){
      AddChannelToDetectorWithNamePost919(0, bolo, detector);
    }
    else{
      AddChannelToDetectorWithNamePost919("chalA", bolo, detector);
      AddChannelToDetectorWithNamePost919("chalB", bolo, detector);
    }
  }
  else if (bolo.BeginsWith("FID") || bolo.BeginsWith("ID")){
    AddChannelToDetectorWithNamePost919("chalA", bolo, detector);
    AddChannelToDetectorWithNamePost919("chalB", bolo, detector);
    AddChannelToDetectorWithNamePost919("ionisA", bolo, detector);
    AddChannelToDetectorWithNamePost919("ionisB", bolo, detector);
    AddChannelToDetectorWithNamePost919("ionisC", bolo, detector);
    AddChannelToDetectorWithNamePost919("ionisD", bolo, detector);
    if(bolo.BeginsWith("ID")){
      AddChannelToDetectorWithNamePost919("ionisG", bolo, detector);
      AddChannelToDetectorWithNamePost919("ionisH", bolo, detector);
    }
  } 
  else if (bolo.BeginsWith("ZM")) {
    AddChannelToDetectorWithNamePost919("H1", bolo, detector);
    AddChannelToDetectorWithNamePost919("H2", bolo, detector);
    AddChannelToDetectorWithNamePost919("LT1", bolo, detector);
    AddChannelToDetectorWithNamePost919("LT2", bolo, detector);
  }

  else if (bolo.BeginsWith("veto")) {
    AddChannelToDetectorWithNamePost919("hits", bolo, detector);
    AddChannelToDetectorWithNamePost919("stamp", bolo, detector);
  }
  
  while (!fSambaFileLine.BeginsWith(endOfDetecorHeader) && !fSambaFileStream.eof()){
    
    if(fSambaFileLine.BeginsWith("Bolo.etat") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      for(UInt_t i = 0; i < detector->GetChannelListSize(); i++){
        KSambaDetectorChannel* chan = detector->GetChannelFromList(i);
        chan->SetState( GetStringFromTokenizedStringResult(larr, 1)  );
      }
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.position") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      int x;
      std::stringstream ss;
      ss << std::hex << GetStringFromTokenizedStringResult(larr, 1).Data(); 
      ss >> x;
      for(UInt_t i = 0; i < detector->GetChannelListSize(); i++){
        KSambaDetectorChannel* chan = detector->GetChannelFromList(i);
        chan->SetPosition( x );
      }
      
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.masse") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      for(UInt_t i = 0; i < detector->GetChannelListSize(); i++){
        KSambaDetectorChannel* chan = detector->GetChannelFromList(i);
        chan->SetMass( GetFloatFromTokenizedStringResult(larr, 1) );
      }
      
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.hote") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      for(UInt_t i = 0; i < detector->GetChannelListSize(); i++){
        KSambaDetectorChannel* chan = detector->GetChannelFromList(i);
        chan->SetMac( GetStringFromTokenizedStringResult(larr, 1) );
      }
      
      delete larr;
    }
    
    
    else if(fSambaFileLine.BeginsWith("Bolo.reglages")){  //the bolometer
      Bool_t foundGoodKey = true;
      fSambaFileLine.ReadToDelim(fSambaFileStream);  //go to the next line
      
      if (fSambaFileLine.BeginsWith(endOfDetecorHeader.c_str()) ) return true;  //probably the veto configuration
      //return here if the Bolo.reglages section is empty.

      while (foundGoodKey){  //see below. i return when i get to the end. 
        TObjArray *regarr = fSambaFileLine.Tokenize("{}");  
        TString regSub = GetStringFromTokenizedStringResult(regarr, 1);
        //cout << regSub.Data() << endl;
        TObjArray *regSubArr = regSub.Tokenize(":=");
        TString key = GetStringFromTokenizedStringResult(regSubArr, 0);
        TString val = "indetermine";
        if (regSubArr->GetEntries() > 1) val = GetStringFromTokenizedStringResult(regSubArr, 1);
        
        delete regarr;
        delete regSubArr;
        
        if(key.BeginsWith("polar") && !key.BeginsWith("polar-fet") ) {
          
          TString cName = "ionis";
          cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetPolarCentre(-9999);
          else chan->SetPolarCentre(val.Atof());
        }
        
        
        else if(key.BeginsWith("gain") &&  !key.BeginsWith("gain-chal") &&  !key.BeginsWith("gain-H") &&  !key.BeginsWith("gain-L") ) {
          //ignore the new ZM detectors. why are the names ALWAYS changing. so stupid...
          TString cName = "ionis";
          cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainCentre(-9999);
          else chan->SetGainCentre(val.Atof());
        }
        
        else if(key.BeginsWith("gain-chal") && !key.EqualTo("gain-chaleur")) {
          
          TString cName = "chal";
          cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        else if(key.EqualTo("gain-chaleur") ) {
          //special Gc1 detector in Run16... or any detector with just one heat.
          KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        else if(key.EqualTo("gain-H1") || key.EqualTo("gain-LT1")  || key.EqualTo("gain-LT2") ) {
          
          TString cName = key(5, key.Length()-5); cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        else if(key.BeginsWith("polar-fet") || key.BeginsWith("fet-chal") ) {
          if(key.EqualTo("polar-fet")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            chan->SetPolarFet(val.Data());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            chan->SetPolarFet(val.Data());
          }          
        }
        
        else if(key.EqualTo("fet-H1") || key.EqualTo("fet-LT1")  || key.EqualTo("fet-LT2") ) {
          
          TString cName = key(4, key.Length()-4); cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        else if(key.EqualTo("corr-pied") || key.BeginsWith("corr-chal")) {
          //seems that in samba version 9.20 this field only exists for detectors with one 

          if(key.EqualTo("corr-chal")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrPied(-9999);
            else chan->SetCorrPied(val.Atof());
          }
          if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrPied(-9999);
            else chan->SetCorrPied(val.Atof());
          }
        }
        
        else if(key.BeginsWith("corr-trng") || key.BeginsWith("corr-chal") ) {
          
          if(key.EqualTo("corr-trng")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetCorrTrngl(val.Atof());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetCorrTrngl(val.Atof());
          }
          
        }
        
        else if(key.EqualTo("corr-H1") || key.EqualTo("corr-LT1")  || key.EqualTo("corr-LT2") ) {
          
          TString cName = key(5, key.Length()-5); cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        
        else if(key.BeginsWith("comp-modul") || key.BeginsWith("comp-chal")  ) {
          
          if(key.EqualTo("comp-modul")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetCompModul(val.Atof());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetCompModul(val.Atof());
          }
          
        }
        
        else if(key.EqualTo("comp-H1") || key.EqualTo("comp-LT1")  || key.EqualTo("comp-LT2") ) {
          
          TString cName = key(5, key.Length()-5); cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        
        else if(key.BeginsWith("ampl-modul") || key.BeginsWith("ampl-chal") ) {
          
          if(key.EqualTo("ampl-modul")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetAmplModul(val.Atof());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetAmplModul(val.Atof());
          }
          
        }
        
        
        else if(key.EqualTo("ampl-H1") || key.EqualTo("ampl-LT1")  || key.EqualTo("ampl-LT2") ) {
          
          TString cName = key(5, key.Length()-5); cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        
        
        
        else if(key.BeginsWith("d2") ) {
          
          if(key.EqualTo("d2")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD2(val.Atof());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD2(val.Atof());
          }
          else if(key.EndsWith("H1") || key.EndsWith("LT1") || key.EndsWith("LT2")){
            TString cName = key(2, key.Length()-2);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD2(val.Atof());
          }
          
        }
        
        
        else if(key.BeginsWith("d3") ) {
          
          if(key.EqualTo("d3")){
            //special Gc1 with only one heat in Run16... or any detector with just one heat.
            
            KSambaDetectorChannel* chan = detector->GetChannelFromList(bolo.Data());
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD3(val.Atof());
          }
          else if(key.EndsWith("A") || key.EndsWith("B")){
            TString cName = "chal";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD3(val.Atof());
          }
          else if(key.EndsWith("H1") || key.EndsWith("LT1") || key.EndsWith("LT2")){
            TString cName = key(2, key.Length()-2);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
            if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
            else chan->SetDiviseurD2(val.Atof());
          }
          
        }
        
      
        else if(key.BeginsWith("Rt") ) {
          
          TString cName = "ionis";
          cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
            chan->SetConvergencePeriod(val.Atof());
            chan->SetBoloBoxVersion(2.0);
          }
          
          //The Rt field contains two different channels in the name :P
          cName = "ionis";
          cName += key(key.Length()-2, 1);  cName += " "; cName += bolo;
          chan = detector->GetChannelFromList(cName);
          
          if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
            chan->SetConvergencePeriod(val.Atof());
            chan->SetBoloBoxVersion(2.0);
          }
        }
        
        
        else if(key.BeginsWith("rlay1") ) {
          
          TString cName = "ionis";
          cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
          KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);
          
          if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){ 
            chan->SetRelay1Status(val.Data());
            chan->SetBoloBoxVersion(2.0);
          }
          
          //The rlay field contains two different channels in the name :P
          cName = "ionis";
          cName += key(key.Length()-2, 1);  cName += " "; cName += bolo;
          chan = detector->GetChannelFromList(cName);
          
          if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
            chan->SetRelay1Status(val.Data());
            chan->SetBoloBoxVersion(2.0);
          }
        }
        
        
         else if(key.BeginsWith("rlay2") ) {

            TString cName = "ionis";
            cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
            KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);

            if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")) {
              chan->SetRelay2Status(val.Data());
              chan->SetBoloBoxVersion(2.0);
            }
            //The rlay field contains two different channels in the name :P
            cName = "ionis";
            cName += key(key.Length()-2, 1);  cName += " "; cName += bolo;
            chan = detector->GetChannelFromList(cName);

            if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
              chan->SetRelay2Status(val.Data());
              chan->SetBoloBoxVersion(2.0);
            }
          }
        
          else if(key.BeginsWith("Rdac") ) {

              TString cName = "ionis";
              cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
              KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);

              if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
                chan->SetFetDac(val.Atof());
                chan->SetBoloBoxVersion(2.0);
              }

          }
          
          else if(key.BeginsWith("Rg") ) {

              TString cName = "ionis";
              cName += key(key.Length()-1, 1);  cName += " "; cName += bolo;
              KSambaDetectorChannel* chan = detector->GetChannelFromList(cName);

              if (!val.BeginsWith("indetermine") && !val.BeginsWith("inconnu")){
                chan->SetBoloGain(val.Atof());
                chan->SetBoloBoxVersion(2.0);
              }

          }
        
        else  {
          cerr << "Unknown key Reading Voie header: " << key << endl;
          //foundGoodKey = false; //just keep reading... Samba is allowed to insert a new key
        }

        fSambaFileLine.ReadToDelim(fSambaFileStream);
        
        if(fSambaFileLine.BeginsWith(")") || fSambaFileStream.eof())
          return true; // I know this is the last thing in the Detector Header Part. So, I quit when I get to the end.
      }
    }
    
    fSambaFileLine.ReadToDelim(fSambaFileStream);
    
  }
 
  return !fSambaFileStream.eof(); 
}

void KSamba2KData::AddChannelToDetectorWithNamePost919(const char* prefix, TString bolo, KSambaDetector *detector)
{
  KSambaDetectorChannel *chan = detector->AddChannel();
  TString cName;
  if(prefix){
    cName = prefix; cName += " "; cName += bolo;
  }
  else
    cName = bolo;
  chan->SetName(cName.Data());
  cout << "    Adding channel: " << cName.Data() << " to " << detector->GetName() << " configuration list." << endl;
}

Bool_t KSamba2KData::AddDetectorInfoPre919(KSambaDetector *detector)
{
  string startOfChannelConfig = "* Voie";
  string startOfRun = "# ===== Entete de run ====="; 
  string endOfDetecorHeader = "* ----------" ; 
  
  TString bolo = GetDetectorName(fSambaFileLine.Data());
             
  if(bolo != detector->GetName()){
    cerr << "KSamba2KData::AddDetectorInfo. Mismatch!"<< endl;
    cerr << bolo.Data() << " != " << detector->GetName() << endl;
    return false; //this should be impossible. 
  }
    
  TObjArray *arr = fSambaFileLine.Tokenize(" ");
  TString sub = GetStringFromTokenizedStringResult(arr, 2);
  delete arr;
  
  TString channelcentre;
  TString channelgarde;
  TString channelName;
  
  if (bolo.BeginsWith("ID") || bolo.BeginsWith("FID")){  //okay for FID and ID detectors that have the format FID809AB
                                //assume the sub is something like "chaleur FID809AB"
    TString cc ( sub(sub.Length()-2, 1) );
    TString cg ( sub(sub.Length()-1, 1) );
    channelcentre = cc;
    channelgarde = cg;
    channelName = channelcentre + channelgarde;
    
    //need to deal with channels. AB, CD, GH are standard, but Ch also is allowed
    
    if(channelcentre != "A" && channelcentre != "C" && channelcentre != "G"){
      cerr << "Unknown Center Channel: " << channelcentre.Data() << " Skipping Configuration." << endl;
      return true;  //still return true so that data is read
    }
    
    if (channelcentre == "A") {
      if(channelgarde != "B"){
        cerr << "Unknown Channel Pattern reading Samba Header:";
        cerr << channelcentre.Data() << channelgarde.Data() << endl;
        cerr << "Skipping Configuration" << endl;
        return true; //still return true so that data is read
      }
    }
    
    if (channelcentre == "G") {
      if(channelgarde != "H"){
        cerr << "Unknown Channel Pattern reading Samba Header:";
        cerr << channelcentre.Data() << channelgarde.Data() << endl;
        cerr << "Skipping Configuration" << endl;
        return true; //still return true so that data is read
      }
    }
    
    if (channelcentre == "C") {
      if (channelgarde != "D" && channelgarde != "h"){
        cerr << "Unknown Channel Pattern reading Samba Header:";
        cerr << channelcentre.Data() << channelgarde.Data() << endl;
        cerr << "Skipping Configuration" << endl;
        return true; //still return true so that data is read
      }
    }
  }
  else{
    if(bolo.BeginsWith("Gc")){
      //assume the sub is something like "Gc2B"
      TString cg ( sub(sub.Length()-1, 1) );
      channelName = cg;
    }
  }
    
  KSambaDetectorChannel *chan;
  //then add the appropriate number of 
  if(!detector->IsChannelInList(channelName.Data())){ 
    //unless there's a bug in Samba and the same channel is printed out twice in the header
    //this will always create a new channel.
    chan = detector->AddChannel();
    chan->SetName(channelName.Data());
    cout << "    Adding channel: " << channelName.Data() << " to " << detector->GetName() << " configuration list." << endl;
  }
  else {
    chan = detector->GetChannelFromList(channelName.Data());
  }
  
  
  while (!fSambaFileLine.BeginsWith(endOfDetecorHeader) && !fSambaFileStream.eof()){
    
    if(fSambaFileLine.BeginsWith("Bolo.etat") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetState( GetStringFromTokenizedStringResult(larr, 1)  );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.position") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      int x;
      std::stringstream ss;
      ss << std::hex << GetStringFromTokenizedStringResult(larr, 1).Data(); 
      ss >> x;
      chan->SetPosition( x );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.masse") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetMass( GetFloatFromTokenizedStringResult(larr, 1) );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.hote") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetMac( GetStringFromTokenizedStringResult(larr, 1) );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.d2") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetDiviseurD2( GetIntegerFromTokenizedStringResult(larr, 1) );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.d3") ) {
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetDiviseurD3( GetIntegerFromTokenizedStringResult(larr, 1) );
      delete larr;
    }  
    else if(fSambaFileLine.BeginsWith("Bolo.modulation") ) {  //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetDiviseurD2( GetIntegerFromTokenizedStringResult(larr, 1) );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.synchronization") ) { //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      chan->SetDiviseurD3( GetIntegerFromTokenizedStringResult(larr, 1) );
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.ampl.modul") ) {  //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      TString val = GetStringFromTokenizedStringResult(larr, 1);
      if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetAmplModul(-9999);
      else chan->SetAmplModul(val.Atof());
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.comp.modul") ) {  //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      TString val = GetStringFromTokenizedStringResult(larr, 1);
      if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
      else chan->SetCompModul(val.Atof());
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.comp.trngl") ) {  //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      TString val = GetStringFromTokenizedStringResult(larr, 1);
      if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrTrngl(-9999);
      else chan->SetCorrTrngl(val.Atof());
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.comp.pied") ) {  //support old Samba header
      TObjArray *larr = fSambaFileLine.Tokenize("=#");
      TString val = GetStringFromTokenizedStringResult(larr, 1);
      if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrPied(-9999);
      else chan->SetCorrPied(val.Atof());
      delete larr;
    }
    else if(fSambaFileLine.BeginsWith("Bolo.reglages")){
      Bool_t foundGoodKey = true;
      fSambaFileLine.ReadToDelim(fSambaFileStream);  //go to the next line
      
      if (fSambaFileLine.BeginsWith(endOfDetecorHeader.c_str()) ) return true;  //probably the veto configuration
      //return here if the Bolo.reglages section is empty.

      while (foundGoodKey){  //see below. i return when i get to the end. 
        TObjArray *regarr = fSambaFileLine.Tokenize("{}");  
        TString regSub = GetStringFromTokenizedStringResult(regarr, 1);
        //cout << regSub.Data() << endl;
        TObjArray *regSubArr = regSub.Tokenize(":=");
        TString key = GetStringFromTokenizedStringResult(regSubArr, 0);
        TString val = "indetermine";
        if (regSubArr->GetEntries() > 1) val = GetStringFromTokenizedStringResult(regSubArr, 1);
        
        delete regarr;
        delete regSubArr;
        
        if(key == "polar-centre") {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetPolarCentre(-9999);
          else chan->SetPolarCentre(val.Atof());
        }
        else if(key == "polar-garde" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetPolarGarde(-9999);
          else chan->SetPolarGarde(val.Atof());
        }
        else if(key == "gain-centre" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainCentre(-9999);
          else chan->SetGainCentre(val.Atof());
        }
        else if(key == "gain-chaleur" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainChaleur(-9999);
          else chan->SetGainChaleur(val.Atof());
        }
        else if(key == "gain-garde" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetGainGarde(-9999);
          else chan->SetGainGarde(val.Atof());
        }
        else if(key == "polar-fet" ) {
          chan->SetPolarFet(val.Data());
        }
        else if(key == "corr-pied" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrPied(-9999);
          else chan->SetCorrPied(val.Atof());
        }
        else if(key == "comp-modul" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCompModul(-9999);
          else chan->SetCompModul(val.Atof());
        }
        else if(key == "corr-trngl" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetCorrTrngl(-9999);
          else chan->SetCorrTrngl(val.Atof());
        }
        else if(key == "ampl-modul" ) {
          if (val.BeginsWith("indetermine") || val.BeginsWith("inconnu")) chan->SetAmplModul(-9999);
          else chan->SetAmplModul(val.Atof());
        }
        else if(key == "d2" ) {
          if (val != "indetermine" && val != "inconnu")
            chan->SetDiviseurD2(val.Atof());
        }
        else if(key == "d3" ) {
          if (val != "indetermine" && val != "inconnu")
            chan->SetDiviseurD3(val.Atof());
        }
        else if(key != "d2" && key != "d3") {
          cerr << "Unknown key Reading Voie header: " << key << endl;
          //foundGoodKey = false; //just keep reading... Samba is allowed to insert a new key
        }

        fSambaFileLine.ReadToDelim(fSambaFileStream);
        
        if(fSambaFileLine.BeginsWith(")") || fSambaFileStream.eof())
          return true; // I know this is the last thing in the Detector Header Part. So, I quit when I get to the end.
      }
    }
    
    fSambaFileLine.ReadToDelim(fSambaFileStream);
    
  }
  
  return !fSambaFileStream.eof();
}
  
Bool_t KSamba2KData::AddDetectorInfo(const char* detname)
{
  KSambaDetector *det = fSambaHeader.GetDetectorFromList(detname);
  return AddDetectorInfo(det);
}

Bool_t KSamba2KData::ReadSambaChannelConfigurations(void)
{
  return true; //skip this for now
}

void KSamba2KData::ReadSambaRecordLine(KRawSambaRecord *samba, TString &aLine, UInt_t &gigaStamp, UInt_t &smallStamp)
{
  if(aLine.BeginsWith("Numero")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetSambaEventNumber( GetIntegerFromTokenizedStringResult(arr, 1) );
    delete arr;
  }
  else if(aLine.BeginsWith("Regeneration")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetRegenerationFlag( GetStringFromTokenizedStringResult(arr, 1) == "non" ? false : true );
    delete arr;
  }
  else if(aLine.BeginsWith("Date.secondes")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetNtpDateSec( GetIntegerFromTokenizedStringResult(arr, 1) );
    delete arr;
  }
  else if(aLine.BeginsWith("Date.microsecs")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetNtpDateMicroSec( GetIntegerFromTokenizedStringResult(arr, 1) );
    delete arr;
  }
  else if(aLine.BeginsWith("GigaStamp")){
    TObjArray *arr = aLine.Tokenize("=#");
    gigaStamp = GetIntegerFromTokenizedStringResult(arr, 1);
    delete arr;
  }
  else if(aLine.BeginsWith("TimeStamp")){
    TObjArray *arr = aLine.Tokenize("=#");
    smallStamp = GetIntegerFromTokenizedStringResult(arr, 1);
    delete arr;
  }
  else if(aLine.BeginsWith("TempsMort.secondes")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetTotalDeadTimeSec( GetIntegerFromTokenizedStringResult(arr, 1) );
    delete arr;
  }
  else if(aLine.BeginsWith("TempsMort.microsecs")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetTotalDeadTimeMicroSec(  GetIntegerFromTokenizedStringResult(arr, 1) );
    delete arr;
  }
  else if(aLine.BeginsWith("Delai")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetSambaDelay(GetFloatFromTokenizedStringResult(arr, 1));
    delete arr;
  }
  //else if(aLine.BeginsWith("Trigger")){
  //TObjArray *arr = aLine.Tokenize("=#");
  //Is this any different than Delai?
  //samba->SetSambaTrigger(GetFloatFromTokenizedStringResult(arr, 1));
  //}
  else if(aLine.BeginsWith("Temperature")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetTemperature(GetFloatFromTokenizedStringResult(arr, 1));
    delete arr;
  }
  else if(aLine.BeginsWith("Liste:31-0")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetTriggerBit1(GetIntegerFromTokenizedStringResult(arr, 1));
    delete arr;
  }
  else if(aLine.BeginsWith("Liste:63-32")){
    TObjArray *arr = aLine.Tokenize("=#");
    samba->SetTriggerBit2(GetIntegerFromTokenizedStringResult(arr, 1));
    delete arr;
  }
  
}

Bool_t KSamba2KData::ReadSambaRunHeader(void)
{
  cout << "Reading Run Header Data " << endl;
  
  string kBeginEvent = "# ===== Entete d'evenement =====" ; // empirical flags in the raw Edelweiss files
  string kBeginEvent2 = "* Evenement";
  string kBeginRun = "# ===== Entete de run =====" ; // empirical flags in the raw Edelweiss files
  string kBeginRun2 = "* Run";
  
  //skip ahead until you find the start of the Run (or the start of the events in case
  //the run header isn't found in the data for some reason!)
  while(!fSambaFileLine.BeginsWith(kBeginEvent) && !fSambaFileLine.BeginsWith(kBeginRun) &&
        !fSambaFileLine.Contains(kBeginEvent2) && !fSambaFileLine.Contains(kBeginRun2) &&
        !fSambaFileStream.eof())
    fSambaFileLine.ReadToDelim(fSambaFileStream);
  
  if(fSambaFileLine.BeginsWith(kBeginEvent) || fSambaFileLine.Contains(kBeginEvent2))
    return true;  //found the start of events... i guess there just isn't a run header for these data.
  
  TObjArray *arr = 0; //holds tokenized strings in the while loop
  Long64_t gigaStampStartRun = 0;
  Long64_t smallStampStartRun = 0;
  
  while (!fSambaFileStream.eof() && !fSambaFileLine.BeginsWith(kBeginEvent) && 
         !fSambaFileLine.Contains(kBeginEvent2)) {
    
    
    /*if(fSambaFileLine.BeginsWith("Run")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else*/
    if(fSambaFileLine.BeginsWith("Type")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetRunType( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Condition")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetRunCondition( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    /*else if(fSambaFileLine.BeginsWith("Starter")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }*/
    /*else if(fSambaFileLine.BeginsWith("Tubes-pulses")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }*/
    else if(fSambaFileLine.BeginsWith("Date.secondes")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetRunStartPcTimeSec( GetLongIntFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Date.microsecs")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetRunStartPcTimeMicroSec( GetIntegerFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("GigaStamp0")){
      arr = fSambaFileLine.Tokenize("=#");
      gigaStampStartRun = GetLongIntFromTokenizedStringResult(arr, 1);
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("TimeStamp0")){
      arr = fSambaFileLine.Tokenize("=#");
      smallStampStartRun = GetLongIntFromTokenizedStringResult(arr, 1);
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Source.1.regen")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetSource1Regen( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Source.2.regen")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetSource2Regen( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Source.1.calib")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetSource1Calib( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Source.2.calib")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetSource2Calib( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Donnees.source")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetDataSource( GetStringFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Duree.tampon")){
      arr = fSambaFileLine.Tokenize("=#");
      fSambaHeader.SetDataBufferLength( GetFloatFromTokenizedStringResult(arr, 1));
      delete arr;
    }
    /*else if(fSambaFileLine.BeginsWith("Duree.synchronisation")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }*/
    /*else if(fSambaFileLine.BeginsWith("Lect.start.synchro")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Lect.taux.seuil")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Lect.delai.mini")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }*/
    else if(fSambaFileLine.BeginsWith("Trigger.actif")){
      arr = fSambaFileLine.Tokenize("=#");
      if(GetStringFromTokenizedStringResult(arr, 1) == "oui")
        fSambaHeader.SetIsStreamMode(false);
      else fSambaHeader.SetIsStreamMode(true);
      delete arr;
    }
    /*else if(fSambaFileLine.BeginsWith("Trigger.Type")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trigger.prgm")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.calcul")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.datation")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.altivec")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.pattern")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.sans_fltr")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.saute_evt")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Trmt.evt.calage")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Sauvegarde.stream")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Sauvegarde.evt")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }
    else if(fSambaFileLine.BeginsWith("Sauvegarde.regen")){
      arr = fSambaFileLine.Tokenize("=#");
      delete arr;
    }*/
    
    fSambaFileLine.ReadToDelim(fSambaFileStream);
  }
  
  fSambaHeader.SetRunStartTriggerStamp( (Long64_t)(gigaStampStartRun*(1e9) + smallStampStartRun) );

  return !fSambaFileStream.eof();
}

Bool_t KSamba2KData::ReadSambaData(void)
{
    
  cout << "Reading Samba Data " << endl;
  
  string kBeginEvent = "# ===== Entete d'evenement =====" ; // empirical flags in the raw Edelweiss files
  string kBeginEvent2 = "* Evenement";
  string kBeginChannel = "* Voie " ;
  string kSeparator = "* ----------" ;
  string kMuonVetoIgnore = "veto";
  
  KRawEvent *event = dynamic_cast<KRawEvent *>(fKdataOutput.GetEvent());
  
  if(event == 0){
    cerr << "KSamba2KData::ReadSambaData. Raw Event Pointer is NULL" << endl;
    return false;
  }
  
  //skip ahead to the first event.
  while ( !fSambaFileLine.BeginsWith(kBeginEvent) ) {
    if (!fSambaFileStream.good()) {
      cerr << "KSamba2KData::ReadSambaData: "<< kBeginEvent<< " not found" << endl;
      return false;
    }
    fSambaFileLine.ReadToDelim(fSambaFileStream);
  } 
  
  Int_t eventCount = 0;
  Int_t pulseCount = 0;
  // Loop on events
  while ( fSambaFileStream.good() ) {
    
    if(fSambaFileLine.BeginsWith(kBeginEvent)){  //(given the while loop above, this if statement check should be redundant.)
      
      //an event is found. the following needs to be done.
      //read the samba record information. read every channel stored in the 
      //event, and pack each
      //channels pulse trace and other information into the appropriate bolometer
      //record. 
      
      event->Clear("C");
      KRawSambaRecord *samba = event->AddSamba();
      samba->SetRunName(fSambaHeader.GetRunName());
      samba->SetFileNumber(fSambaFileNum);
      samba->SetSambaDAQNumber(fSambaRunName[4] - 'a' + 1); 
      AddSambaInformationFromHeader(samba);
      UInt_t gigaStamp = 0;
      UInt_t smallStamp = 0;
      
      fSambaFileLine.ReadToDelim(fSambaFileStream);  //read the next line. it should be a comment. #
      
      while(!fSambaFileLine.Contains(kBeginChannel) && !fSambaFileStream.eof()){
        //continue reading the file to get all of the samba information, exiting the loop when the first 
        //channel record is found (or end of file)
        ReadSambaRecordLine(samba, fSambaFileLine, gigaStamp, smallStamp);
        fSambaFileLine.ReadToDelim(fSambaFileStream);
      } 
      
      if (fSambaFileStream.eof()) {
        cout << "Unexpected end of file after reading event" << endl;
	cerr << "Unexpected end of file after reading event" << endl;

	cout << "  Found a total of " << eventCount << " Bolometer Events" << endl;
	cout << "    a total of and " << pulseCount << " Pulses" << endl;
  
	fKdataOutput.Write();

        return true;
      }
      
      
      while (!fSambaFileLine.BeginsWith(kBeginEvent) && !fSambaFileLine.BeginsWith(kBeginEvent2)
             && !fSambaFileStream.eof() ) { //keep reading channel recordss until we reach the start of the next event. 
                                            //we are at the start of a channel record in the samba file now. 
        if (!fSambaFileStream.eof()) {
          
          TString subStr( fSambaFileLine(fSambaFileLine.Index(kBeginChannel), fSambaFileLine.Length() - fSambaFileLine.Index(kBeginChannel)) );
         
          // cout << subStr.Data() << endl;
          TObjArray *arr = subStr.Tokenize(" ");
          TString sub, detector;
          
          // make changes here for new samba version
          if(GetMajorVersion() >= 9 && GetMinorVersion() >= 19){
            //this works for all channels, it seems
            sub = GetStringFromTokenizedStringResult(arr, arr->GetEntries()-1);
            detector = sub.Strip(TString::kBoth,'"');   //strip off the quotes
          }
          else{
            if(arr->GetEntries() == 4){
              sub = GetStringFromTokenizedStringResult(arr, 3);
              detector = sub(0,sub.Length()-3);  //assuming the samba file format always give this! subtract 3 (instead of 2) because of the quotation 
            }
            else if(arr->GetEntries() == 3){
              sub = GetStringFromTokenizedStringResult(arr, 2);
              detector = sub(1,sub.Length()-3);  //for Gc detectors. assuming the samba file format always give this! start with string position 1 and subtract 2 (instead of 1) because of the quotation 
            }
          }
          delete arr;
          
          //need to get the proper string for detector

          //cout << "Adding event for " << detector << endl;
          //check to see if bolo already exists in the event, or else, add a new bolometer record. 
          KRawBolometerRecord *bolo = 0;
          Bool_t newBolo = true;
          for(Int_t i = 0; i < event->GetNumBolos(); i++){
            bolo = static_cast<KRawBolometerRecord *>(event->GetBolo(i));
            if(detector == bolo->GetDetectorName()){
              newBolo = false;
              break;
            }
          }
          if (newBolo) {
            bolo = event->AddBolo();
            bolo->SetDetectorName(detector.Data());
            
            bolo->SetSambaRecord(samba);
          }
          
          if(bolo == 0){
            cout << "Didn't get a proper pointer to the KRawBolometerRecord!" << endl;
            return false;  //this should never happen!
          }
          
          KRawBoloPulseRecord *pulse = event->AddBoloPulse();
          //set the pulse name 
          //don't worry about figuring out the exact channel, just record the pulse
          //name as whatever is found inside the quotes after the Voie
          
          //i think this should be okay for samba version 9.20 and greater.
          //this should be the correct channel name.
          arr = subStr.Tokenize("\"");
          pulse->SetChannelName( GetStringFromTokenizedStringResult(arr, 1));
          delete arr;
          pulse->SetBolometerRecord(bolo);
          bolo->AddPulseRecord(pulse);
          AddPulseInformationFromHeader(pulse); //must do this after setting the bolometer record pointer
          
          pulseCount++;
          
          Short_t lPulseSize=0;
          int lPtsFiltre=0;
          
          fSambaFileLine.ReadToDelim(fSambaFileStream); //should be at the first line still of the channel record
                                                        //so read the next line and start reading the channel record
          
          while( !fSambaFileLine.Contains(kBeginChannel) && !fSambaFileStream.eof() && !fSambaFileLine.Contains(kBeginEvent)){
            
            if (fSambaFileLine.Contains(kMuonVetoIgnore)) cout << fSambaFileLine.Data() << endl;

            //read line from the pulse channel and store them appropriately
            if(fSambaFileLine.BeginsWith("Dimension")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetPulseLength( GetIntegerFromTokenizedStringResult(arr, 1) );
              lPulseSize = GetIntegerFromTokenizedStringResult(arr, 1);  //to be used later to read the pulse
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Horloge")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetPulseTimeWidth( GetFloatFromTokenizedStringResult(arr, 1)*1000000.0 ); //size of a single point in ns
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Base")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetDaqAmplitudeBaseline( GetFloatFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Bruit")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetDaqAmplitudeBaselineNoise( GetFloatFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Amplitude")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetDaqAmplitude( GetFloatFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Trigger.ampl.pos")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetPositiveTriggerAmp( GetFloatFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Trigger.ampl.neg")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetNegativeTriggerAmp( GetFloatFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Pretrigger")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetPretriggerSize( GetIntegerFromTokenizedStringResult(arr, 1) );
              delete arr;
            }
            else if(fSambaFileLine.BeginsWith("Montee")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetPulseRiseTime( GetFloatFromTokenizedStringResult(arr, 1) );
              
            }
            else if(fSambaFileLine.BeginsWith("Filtre.nb")){
              arr = fSambaFileLine.Tokenize("=#");
              pulse->SetFilterSize( GetIntegerFromTokenizedStringResult(arr, 1) );
              lPtsFiltre = GetIntegerFromTokenizedStringResult(arr, 1);
              delete arr;
            }
            
            else if(fSambaFileLine.BeginsWith(kSeparator)){
              //we've reached the pulse.. the next line could be the 
              //pulse, or it could be the start of another event or channel record.
              
              long lPos = fSambaFileStream.tellg();
              fSambaFileLine.ReadToDelim(fSambaFileStream); //read the line, which includes the pulse trace and possibly the next channel info
              fSambaFileStream.seekg(lPos);  //but back up the stream pointer
              
              if ( !fSambaFileLine.BeginsWith(kBeginEvent) && !fSambaFileLine.BeginsWith(kBeginEvent2) &&
                  !fSambaFileLine.BeginsWith(kBeginChannel) ){  //make sure we're really not at the start of a next event or channel info and that the pulse is really there
                
                if ( lPtsFiltre )  //skip the filter information
                  fSambaFileStream.ignore(8*lPtsFiltre); // filtered data = 8 bytes
                
                Short_t* lArray = new Short_t[lPulseSize];
                fSambaFileStream.read((char*)lArray,lPulseSize*2); //read the pulse from the stream. always 2 bytes for each point. 
                
                if (fSambaFileStream.fail()) 
                  cerr << "KSamba2KData::ReadSambaData. Possible error reading a pulse for event " << samba->GetSambaEventNumber()<< endl;
                
                if (fSambaHeader.GetEndian() != fLocalBigEndian)  {//reverse order if necessary.
		              for (Short_t i=0;i<lPulseSize;i++) lArray[i]=R__bswap_16(lArray[i]);
                }

                pulse->SetTrace(lPulseSize,lArray);
                delete[] lArray;
              } 
              
              fSambaFileStream.seekg(lPos);  //now that we've read the pulse, back the stream up again
                                             //so that the line is read and checked in the next
                                             //iteration of the while loop for the start of a new channel
                                             //or new event record.
              
            }
            
            fSambaFileLine.ReadToDelim(fSambaFileStream);
            
          }  
          
          //reached the end of this channel's record.
        }
      
        // else {
        //   //found a muon channel record, so must skip ahead to either the next channel
        //   //or the next event. 
        //   //cout << "Found Muon Veto Event." << fSambaFileLine << endl;
        //   fSambaFileLine.ReadToDelim(fSambaFileStream);
          
        //   while( !fSambaFileLine.BeginsWith(kBeginEvent) && !fSambaFileLine.BeginsWith(kBeginEvent2) &&
        //         !fSambaFileLine.BeginsWith(kBeginChannel) && !fSambaFileStream.eof() )
        //     fSambaFileLine.ReadToDelim(fSambaFileStream);
        // }
        
      }
      
      //here because the while loop above found the start of the next
      //event. so,  finally save all of the event information that's been collected
      
      event->SetEventTriggerStamp((Long64_t)(gigaStamp*(1e9) + smallStamp));
      fKdataOutput.Fill();
      eventCount++;
      //cout << eventCount << endl;
      if (eventCount % 10000 == 0){
        cout << "......" << eventCount << " Bolometer Events" << endl;
        cout << "......" << pulseCount << " Pulses" << endl;
      }
      //print out Event Info for debugging. 
      /*
      Long64_t a = gigaStamp*(1e9) + smallStamp;
       int stop;
       event->Dump();
       cout << "gigastamp: smallStamp: Sum: " << gigaStamp << " " << smallStamp << " " << a << endl;
       cout << event->GetNumBolos() << " bolometer records" << endl;
       for(Int_t ii = 0; ii < event->GetNumBolos(); ii++){
        KRawBolometerRecord *b = event->GetBolo(ii);
        cout << endl << "detector name: " << b->GetDetectorName() << endl;
        b->Dump();
        KRawSambaRecord *s = b->GetSambaRecord();
        cout << "run: " << s->GetRunName() << endl;
        s->Dump();
        cout <<  b->GetNumPulseRecords() << " pulse records" << endl;
        for(Int_t jj = 0; jj < b->GetNumPulseRecords(); jj++){
          KRawBoloPulseRecord *p = b->GetPulseRecord(jj);
          cout << "channel name: \"" << p->GetChannelName() << "\"" << endl;
          p->Dump();
        }
      }
      cin >> stop;
      */
    }  
    else fSambaFileLine.ReadToDelim(fSambaFileStream); //if the line doesn't begin with a new event, read the next line
    
  }
  
  cout << "  Found a total of " << eventCount << " Bolometer Events" << endl;
  cout << "    a total of and " << pulseCount << " Pulses" << endl;
  
  fKdataOutput.Write();
  
  return true;
}
void KSamba2KData::AddSambaInformationFromHeader(KRawSambaRecord* samba)
{
  samba->SetRunType(fSambaHeader.GetRunType());
  samba->SetRunCondition(fSambaHeader.GetRunCondition());
  samba->SetRunStartPcTimeSec(fSambaHeader.GetRunStartPcTimeSec());
  samba->SetRunStartPcTimeMicroSec(fSambaHeader.GetRunStartPcTimeMicroSec());
  samba->SetRunStartTriggerStamp(fSambaHeader.GetRunStartTriggerStamp());
  samba->SetSource1Regen(fSambaHeader.GetSource1Regen());
  samba->SetSource2Regen(fSambaHeader.GetSource2Regen());
  samba->SetSource1Calib(fSambaHeader.GetSource1Calib());
  samba->SetSource2Calib(fSambaHeader.GetSource2Calib());
  samba->SetDataSource(fSambaHeader.GetDataSource());
  samba->SetDataBufferLength(fSambaHeader.GetDataBufferLength());
  samba->SetIsStreamMode(fSambaHeader.GetIsStreamMode());
  
}

void KSamba2KData::AddPulseInformationFromHeader(KRawBoloPulseRecord *p)
{
  
  
  KRawBolometerRecord *b = p->GetBolometerRecord();
  if(b != 0){
    TString detName = b->GetDetectorName();
    //cout << "detector name from pulse record: " << detName.Data() << endl;
    KSambaDetector *det = fSambaHeader.GetDetectorFromList(detName.Data());
    if(det != 0){
      TString pulseChannelName = p->GetChannelName();
      //cout << "pulse channel name from pulse: " << pulseChannelName.Data() << endl;
      
      TString chanName;
      if(GetMajorVersion() >= 9 && GetMinorVersion() >= 19){
        chanName = pulseChannelName.Strip(TString::kBoth,'"'); //need to strip off any quotes in the case of Gc1
        
      }
      
      else{
        if (detName.BeginsWith("FID") || detName.BeginsWith("ID")) {
          chanName = pulseChannelName(pulseChannelName.Length()-2,2);  //ASSUME format, the last two letters are always AB, GH, CD or Ch
                                                                        //cout << "sub name from pulse: " << chanNameSub.Data() << endl;
        }
        else if (detName.BeginsWith("Gc")){
          chanName = pulseChannelName(pulseChannelName.Length()-1,1);
        }
        else {
          cout << "AddPulseInformationFromHeader. Didn't recognize the bolometer name." << endl;
          return;
        }
      }
      
      KSambaDetectorChannel *chan = det->GetChannelFromList(chanName.Data());
      if(chan != 0){
        p->SetState(chan->GetState());
        p->SetCryoPosition(chan->GetPosition());
        p->SetHeatPulseStampWidth(chan->GetDiviseurD2());
        p->SetIsHeatPulse(0);
        p->SetCorrPied(0); 
        p->SetCompModul(0);
        p->SetCorrTrngl(0);
        p->SetAmplModul(0);
        p->SetBoloBoxVersion(chan->GetBoloBoxVersion());
        p->SetConvergencePeriod(chan->GetConvergencePeriod());
        p->SetRelay1Status(chan->GetRelay1Status());
        p->SetRelay2Status(chan->GetRelay2Status());  
        p->SetFetDac(chan->GetFetDac());
        p->SetBoloGain(chan->GetBoloGain());
        
        if(pulseChannelName.BeginsWith("chal") || detName.BeginsWith("Gc")){  //changed 'chaleur' to 'chal' to accommodate samba version >= 9.20
          p->SetIsHeatPulse(1);
          p->SetVoltage(0.0);
          p->SetPolarFet(chan->GetPolarFet());
          p->SetGain(chan->GetGainChaleur());
          p->SetCorrPied(chan->GetCorrPied()); 
          p->SetCompModul(chan->GetCompModul());
          p->SetCorrTrngl(chan->GetCorrTrngl());
          p->SetAmplModul(chan->GetAmplModul());
        }
        else if(pulseChannelName.BeginsWith("centre")){
          p->SetVoltage(chan->GetPolarCentre());
          p->SetGain(chan->GetGainCentre());
        }
        else if(pulseChannelName.BeginsWith("garde")){
          p->SetVoltage(chan->GetPolarGarde());
          p->SetGain(chan->GetGainGarde());
        }
        else if (pulseChannelName.BeginsWith("ionis")){ //added for samba version >= 9.20
          p->SetVoltage(chan->GetPolarCentre());
          p->SetGain(chan->GetGainCentre());
        }
        
        //set properties for the bolometer associated with this pulse!  
        //this is kind of a weird place to do it and we could have 
        //problems if the data isn't entered into the samba configuration properly
        //and one channel's information conflicts with a different channel's information.
        b->SetMass(chan->GetMass());
      }
      else {
        cout << "AddPulseInformationFromHeader. Couldn't find channel in list. Searched for: " << chanName.Data() << endl;
      }
    }
    else {
      cout << "AddPulseInformationFromHeader. Could not find detector in list. Searched for: " << detName.Data() << endl;
    }
  }
  else {
    cout << "AddPulseInformationFromHeader. KRawBoloPulseRecord -- no bolometer record pointer set." << endl;
  }
  
}


Bool_t KSamba2KData::CloseKdataFile(void)
{
  cout << "Closing KData File." << endl;
  return fKdataOutput.Close();
}

Bool_t KSamba2KData::OpenSambaFileStream(void)
{
  fSambaFileStream.open(fSambaFileName.c_str(), ios::in);
  return ( fSambaFileStream.is_open() && !fSambaFileStream.fail() );
}


Bool_t KSamba2KData::CloseSambaFileStream(void)
{
  cout << "Closing Samba File Stream" << endl;
  fSambaFileStream.close();
  return !fSambaFileStream.good();
}

Bool_t KSamba2KData::OpenKdataFile(void)
{
  fKdataOutput.Close(); //make sure we've closed the file.
  return fKdataOutput.OpenFile(fKdataFileName.c_str(), KRawEvent::GetClassName());

}

Int_t KSamba2KData::GetMajorVersion(void)  
{
  return GetReleaseIndex(0);
}

Int_t KSamba2KData::GetMinorVersion(void) 
{
  return GetReleaseIndex(1);
}

Int_t KSamba2KData::GetSubVersion(void) 
{
  return GetReleaseIndex(2);
}

Int_t KSamba2KData::GetReleaseIndex(Int_t i) 
{
  TString currentRelease  = fSambaHeader.GetRelease();
  TObjArray *arr = currentRelease.Tokenize(".");
  Int_t theVer = -1;
  if(arr == 0) return -1;
  
  if (arr->GetEntries() > i)
    theVer =  GetIntegerFromTokenizedStringResult(arr,i);
  
  if(arr) delete arr;
  return theVer;
}


