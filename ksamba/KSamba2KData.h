//______________________________________________________________________
//
// KSamba2KData.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBA2KDATA_H__
#define __KSAMBA2KDATA_H__

#include "Rtypes.h"
#include "TString.h"
#include "KDataWriter.h"
#include "KSambaHeader.h"
#include "KSambaDetector.h"
#include <fstream>
#include <string>
#include "TObject.h"
#include <exception>

class TObjArray;
class KRawSambaRecord;
class KRawBoloPulseRecord;

using namespace std;

class KSamba2KData : public TObject { 
  
public:
  //Constructors
  KSamba2KData(void);
  KSamba2KData(const char* inputSambaFile, const char* outputKRawEventFile);
  virtual ~KSamba2KData(void);
    
  Bool_t ConvertFile(void);
  Bool_t ConvertFile(const char* inputSamba, const char* outputKRawEventFile);
  
  //Bool_t ConvertStream(ifstream& inputSambaStream, const char* outputKRawEventFile);
  
 
private:
    
  string fSambaFileName;
  string fPreviousSambaFileLine;
  string fKdataFileName;
  ifstream fSambaFileStream;
  TString fSambaFileLine; //most recently read line in Samba File.
  string fSambaRunName;
  Int_t fSambaFileNum;
  
  Bool_t fLocalBigEndian; //equals 1 if big, 0 if little.
  
  KDataWriter fKdataOutput; 
  KSambaHeader fSambaHeader;

  //private methods
  void SetInputSambaFile(const char* fileName);
  void SetOutputKdataFile(const char* fileName);

  void InitializeMembers(void);
  Bool_t OpenSambaFileStream(void);
  Bool_t OpenKdataFile(void);
  Bool_t CheckStartOfSambaFile(void);
  Bool_t ReadSambaHeaderGeneral(void);
  Bool_t ReadSambaDetectorConfigurations(void);
  Bool_t ReadSambaChannelConfigurations(void);
  Bool_t ReadSambaData(void);
  Bool_t ReadSambaRunHeader(void);

  Bool_t CloseSambaFileStream(void);
  Bool_t CloseKdataFile(void);
  Bool_t AddDetectorInfo(KSambaDetector *detector);
  Bool_t AddDetectorInfo(const char* detname);
  Bool_t AddDetectorInfoPre919(KSambaDetector *detector);
  Bool_t AddDetectorInfoPost919(KSambaDetector *detector);
  
  void AddChannelToDetectorWithNamePost919(const char*, TString, KSambaDetector*);
  
  void ReadSambaRecordLine(KRawSambaRecord *samba, TString &aLine, UInt_t &gigaStamp, UInt_t &smallStamp);
  void AddPulseInformationFromHeader(KRawBoloPulseRecord *p);
  void AddSambaInformationFromHeader(KRawSambaRecord* samba);
  const char* GetDetectorName(const char* line);

  TString GetStringFromTokenizedStringResult(TObjArray *arr, Int_t index);
  Int_t GetIntegerFromTokenizedStringResult(TObjArray *arr, Int_t index);
  Long64_t GetLongIntFromTokenizedStringResult(TObjArray *arr, Int_t index);
  Double_t GetFloatFromTokenizedStringResult(TObjArray *arr, Int_t index);

  Int_t GetMajorVersion(void);
  Int_t GetMinorVersion(void);
  Int_t GetSubVersion(void);
  Int_t GetReleaseIndex(Int_t i);
  
  void PrintExceptionInfo(exception& e, int linenumber = -1);
  void ReadNextSambaFileLine(void);


  ClassDef(KSamba2KData,1);

};


#endif // __KSAMBA2KDATA_H__
