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

#include "KDataWriter.h";
#include "Rtypes.h"
#include <fstream>
#include <string>

using namespace std;

class KSamba2KData  { 
  
public:
  //Constructors
  KSamba2KData(void);
  KSamba2KData(const char* inputSambaFile, const char* outputKRawEventFile);
  virtual ~KSamba2KData(void);
  
  void SetInputSambaFile(const char* fileName);
  void SetOutputKdataFile(const char* fileName);
  
  Bool_t ConvertFile(void);
  Bool_t ConvertFile(const char* inputSamba, const char* outputKRawEventFile);
  
  //Bool_t ConvertStream(ifstream& inputSambaStream, const char* outputKRawEventFile);
  
  //getters
  
  //setters
private:
  string fSambaFile;
  string fKdataFile;
  ifstream fSambaFileStream;
  string fSambaFileLine; //most recently read line in Samba File.
  string fSambaRunName;
  Int_t fSambaFileNum;
  
  Bool_t fSambaBigEndian; //equals 1 if big, 0 if little
  Bool_t fLocalBigEndian; //equals 1 if big, 0 if little.
  
  KDataWriter fKdataOutput; 
  
  //private methods
  void InitializeMembers(void);
  void DetermineSambaByteOrder(void);
  Bool_t OpenSambaFileStream(void);
  Bool_t OpenKdataFile(void);
  Bool_t CheckStartOfSambaFile(void);
  Bool_t ReadSambaHeaderGeneral(void);
  Bool_t ReadSambaDetectorConfigurations(void);
  Bool_t ReadSambaData(void);
  Bool_t CloseSambaFileStream(void);
  Bool_t CloseKdataFile(void);

  ClassDef(KSamba2KData,1);

};


#endif // __KSAMBA2KDATA_H__
