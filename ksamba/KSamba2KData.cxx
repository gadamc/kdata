//______________________________________________________________________
//
// KSamba2KData.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KSamba2KData.h"
#include "RConfig.h"
#include "KRawBolometerRecord.h"
#include "KRawSambaRecord.h"
#include "KRawBoloPulseRecord.h"
#include "KRawEvent.h"
#include "Byteswap.h"
#include <iostream>
#include <cstdlib>


ClassImp(KSamba2KData);

KSamba2KData::KSamba2KData(void)
{
  fSambaFile = "";
  fKdataFile = "";
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
  
#ifdef R_BYTESWAP
  fLocalBigEndian = false;
#else
  fLocalBigEndian = true;
#endif
  
  fSambaBigEndian = true;  //we assume big endianness by default.
                           //but this will be read from the Samba File
 
  fSambaFileLine = "";
  fSambaRunName = "";
  fSambaFileNum = -1;
}

void  KSamba2KData::SetInputSambaFile(const char* fileName)
{
  fSambaFile = fileName; 
}

void  KSamba2KData::SetOutputKdataFile(const char* fileName)
{
  fKdataFile = fileName;
  
}

Bool_t  KSamba2KData::ConvertFile(void)
{
  //
  
  
  if(fSambaFile == "" || fKdataFile == ""){
    cerr << "KSamba2KData::ConvertFile. Input and Output file names are not set." << endl;
    return false;
  }
  
  cout << "Converting File. " << endl;
  cout << "   " << fSambaFile << " -> " << fKdataFile << endl;
  
  if(!OpenSambaFileStream()){
    cout << "Open Samba File Stream Fail." << endl;
    return false;
  }
    
  
  if(!OpenKdataFile()){
    cout << "Open Kdata File Fail." << endl;
    return false;
  }
    
  
  if(!CheckStartOfSambaFile()){
    cout << "Check Start of Samba File Fail." << endl;
    return false;
  }
    
  
  if(ReadSambaHeaderGeneral())
    if(ReadSambaDetectorConfigurations())
      if(ReadSambaData())
        if(CloseSambaFileStream())
          return CloseKdataFile();
    
  
  return false;
}

Bool_t  KSamba2KData::ConvertFile(const char* inputSamba, const char* outputKRawEventFile)
{
  SetInputSambaFile(inputSamba);
  SetOutputKdataFile(outputKRawEventFile);
  return ConvertFile();
}

Bool_t KSamba2KData::CheckStartOfSambaFile(void)
{
  cout << "Check Start of Samba File" << endl;
  string startSamba = "* Archive SAMBA";
  
  getline(fSambaFileStream,fSambaFileLine);
  return fSambaFileLine.compare(0, startSamba.size(), startSamba) == 0 ? true: false; 
}

Bool_t KSamba2KData::ReadSambaHeaderGeneral(void)
{
  // General Run header
  
  cout << "Reading Samba General Header." << endl;
  
  string endOfGeneralHeader = "* ----------" ; // 
  size_t endOfGeneralHeaderSize = endOfGeneralHeader.size();
  

  //set the run name based on the name of the file
  //given in fSambaFile. Then we look in the file directly
  //to get the run name based on the value given by
  //Fichier.
  size_t posOfFileName = fSambaFile.find_last_of("/\\");
  fSambaRunName = fSambaFile.substr(posOfFileName == string::npos ? 0 : posOfFileName,
                    fSambaFile.find_last_of("_"));
  fSambaFileNum = atoi(fSambaFile.substr(fSambaFile.find_last_of("_")+1).c_str());
  
  //loop through the general header section. we finish when we find
  //the * Detecteur keyword. 
  while ( fSambaFileLine.compare(0,endOfGeneralHeaderSize,endOfGeneralHeader) ) {
    
    if(fSambaFileLine.compare(0,10,"Byte-order") == 0){
      if(fSambaFileLine.substr(fSambaFileLine.find_last_of("=")+2,3) == "big")
        fSambaBigEndian = true;
      else 
        fSambaBigEndian = false;
    }
    
    if(fSambaFileLine.compare(0,10,"Fichier = ") == 0)
      fSambaRunName = fSambaFileLine.substr(fSambaFileLine.find_last_of("/\\")+1); //the \\ should support windows file system as well as unix
    
    if (!fSambaFileStream.good()) {
      cerr << "ReadSambaHeaderGeneral. Unexpcted end of General Header line \""<< endOfGeneralHeader<< "\" not found" << endl;
      return false;
    }
    getline(fSambaFileStream,fSambaFileLine);
  } 
  
  return true;
  
}

Bool_t KSamba2KData::ReadSambaDetectorConfigurations(void)
{
  cout << "Reading Detector Configurations." << endl;
  
  string endOfDetectorConfig = "* ----------" ; // 
  size_t endOfDetectorConfig = endOfDetectorConfig.size();
  
  
  return true;
}

Bool_t KSamba2KData::ReadSambaData(void)
{
    
  cout << "Reading Samba Data " << endl;
  
  string kBeginEvent = "* Evenement" ; // empirical flags in the raw Edelweiss files
  string kBeginChannel = "* Voie" ;
  string kSeparator = "* ----------" ;
  short kBeginEventSize = kBeginEvent.size() ;
  short kBeginChannelSize = kBeginChannel.size() ;
  short kSeparatorSize = kSeparator.size() ;
  
  KRawEvent *event = dynamic_cast<KRawEvent *>(fKdataOutput.GetEvent());
  
  if(event == 0){
    cerr << "KSamba2KData::ReadSambaData. Raw Event Pointer is NULL" << endl;
    return false;
  }
  
  
  //skip ahead to the first event.
  while ( fSambaFileLine.compare(0,kBeginEventSize,kBeginEvent) ) {
    if (!fSambaFileStream.good()) {
      cerr << "KSamba2KData::ReadSambaData: "<< kBeginEvent<< " not found" << endl;
      return false;
    }
    getline(fSambaFileStream,fSambaFileLine);
  } 
  
  Int_t eventCount = 0;
  Int_t pulseCount = 0;
  // Loop on events
  while ( fSambaFileStream.good() ) {
    
    event->Clear("C");
    
    KRawBolometerRecord *bolo = event->AddBolo();
    KRawSambaRecord *samba = event->AddSamba();
    
    samba->SetRunName(fSambaRunName.c_str());
    samba->SetFileNumber(fSambaFileNum);
    samba->SetSambaDAQNumber(fSambaRunName.compare(4,1,"a") + 1); 
    UInt_t gigaStamp = 0;
    UInt_t smallStamp = 0;
    
    while ( fSambaFileLine.compare(0,kBeginChannelSize,kBeginChannel) ) {
      getline(fSambaFileStream,fSambaFileLine);
      size_t diese_pos = fSambaFileLine.find("#",0);
      size_t eq_pos = fSambaFileLine.find("=",0);
      if (eq_pos != string::npos) {
        string lField = fSambaFileLine.substr(0,eq_pos-1);
        string lValue = fSambaFileLine.substr(eq_pos+1);
        if (diese_pos != string::npos) lValue = fSambaFileLine.substr(eq_pos+1,diese_pos-1-eq_pos) ;
        //samba event number
        if (lField == "Numero") samba->SetSambaEventNumber( atol(lValue.c_str()) );
        if (lField == "Date.secondes") {
          samba->SetNtpDateSec( atol(lValue.c_str()) );
          event->SetEventTriggerTime( atol(lValue.c_str()) );
        }
        if (lField == "Date.microsecs") samba->SetNtpDateMicroSec( atoi(lValue.c_str()) );
        if (lField == "Delai") samba->SetSambaDelay( atof(lValue.c_str()) );
        if (lField == "Liste:31-0") samba->SetTriggerBit1((UInt_t)atoi(lValue.c_str()));
        if (lField == "Liste:63-32") samba->SetTriggerBit2((UInt_t)atoi(lValue.c_str()));
        
        if(lField == "GigaStamp") gigaStamp = (UInt_t)atoi(lValue.c_str());
        if(lField == "TimeStamp") smallStamp = (UInt_t)atoi(lValue.c_str());
        
      } // else cerr << "Strange line for event header: " << fSambaFileLine << endl;
    }
    event->SetStamp(gigaStamp*10^9 + smallStamp);
    
    bolo->SetSambaRecord(samba);
    
    // Loop on pulses within a given event
    int lNopulse=0;
    while (lNopulse == 0) {
      KRawBoloPulseRecord *pulse = event->AddBoloPulse();
      
      pulse->SetChannelName(fSambaFileLine.substr(8,fSambaFileLine.size()-9).c_str());
      Short_t lPulseSize=0;
      int lPtsFiltre=0;
      while ( fSambaFileLine.compare(0,kSeparatorSize,kSeparator) ) { // Pulse header
        getline(fSambaFileStream,fSambaFileLine);
        string::size_type diese_pos = fSambaFileLine.find("#",0);
        string::size_type eq_pos = fSambaFileLine.find("=",0);
        if (eq_pos != string::npos) {
          string lField = fSambaFileLine.substr(0,eq_pos-1);
          lField = lField.substr(lField.find_first_not_of("\t"));
          string lValue = fSambaFileLine.substr(eq_pos+1);
          if (diese_pos != string::npos) lValue = fSambaFileLine.substr(eq_pos+1,diese_pos-1-eq_pos) ;
          if (lField == "Dimension") lPulseSize=atoi(lValue.c_str());
          if (lField == "Horloge") pulse->SetPulseTimeWidth( (ULong_t)atof(lValue.c_str())*1000000 ) ; // ms to ns
          if (lField == "Filtre.nb") lPtsFiltre=atoi((lValue).c_str());
          if (lField == "Base") pulse->SetAmplitudeBaseline(atof(lValue.c_str()));
          if (lField == "Bruit") pulse->SetAmplitudeBaselineNoise(atof(lValue.c_str()));
          if (lField == "Amplitude") pulse->SetAmplitude(atof(lValue.c_str()));
        } // else cerr << "Strange line for pulse header: " << fSambaFileLine << endl;
        
        if (!fSambaFileStream.good()) 
          cerr << "Error reading pulse header" << endl;
      }
      
      if ( lPtsFiltre ) 
        fSambaFileStream.ignore(8*lPtsFiltre); // filtered data = 8 bytes
                                               // First check if the pulse is here indeed!
      long lPos = fSambaFileStream.tellg();
      getline(fSambaFileStream,fSambaFileLine);
      fSambaFileStream.seekg(lPos);
      
      if ( (!fSambaFileLine.compare(0,kBeginEventSize,kBeginEvent)) || 
          (!fSambaFileLine.compare(0,kBeginChannelSize,kBeginChannel))) {
        // No pulse, just a header.. need to record the pulse in the tree but with no trace?
      } 
      else { // Read the pulse
        Short_t* lArray = new Short_t[lPulseSize];
        fSambaFileStream.read((char*)lArray,lPulseSize*2);
        
        if (fSambaFileStream.fail()) 
          cerr << "KSamba2KData::ReadSambaData. Error reading a pulse for event " << samba->GetSambaEventNumber()<< endl;
        
        if (fSambaBigEndian != fLocalBigEndian)  //reverse order if necessary 
          for (Short_t i=0;i<lPulseSize;i++) lArray[i]=R__bswap_16(lArray[i]);
        
        pulse->SetTrace(lPulseSize,lArray);
        delete[] lArray;
        
      }
      
      pulse->SetBolometerRecord(bolo);
      bolo->AddPulseRecord(pulse);
      pulseCount++;
      
      char toto; // Je comprends pas pourquoi il faut faire ca. Admettons...
      long pos=fSambaFileStream.tellg();
      fSambaFileStream.get(toto);
      if ( fSambaFileStream.good() ) {
        fSambaFileStream.seekg(pos);
        getline(fSambaFileStream,fSambaFileLine);
        //	cout << fSambaFileLine << endl;
        if ( ! fSambaFileLine.compare(0,kBeginEventSize,kBeginEvent) ) 
          lNopulse=1;
      } 
      else 
        lNopulse=1;
      
      if (fSambaFileStream.bad()) 
        cerr << "Error reading the Samba file.." <<endl;
    }
    
    fKdataOutput.Fill();
    eventCount++;
  }
  
  cout << "    Found " << eventCount << " Bolometer Events" << endl;
  cout << "           and " << pulseCount << " Pulses" << endl;
  
  fKdataOutput.Write();
  
  return true;

}

Bool_t KSamba2KData::CloseKdataFile(void)
{
  cout << "Closing KData File." << endl;
  return fKdataOutput.Close();
}

Bool_t KSamba2KData::OpenSambaFileStream(void)
{
  fSambaFileStream.open(fSambaFile.c_str(), ios::in);
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
  return fKdataOutput.OpenFile(fKdataFile.c_str(), "Raw");
}


