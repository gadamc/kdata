//______________________________________________________________________
//
// KMergeKds.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KMERGEKDS_H__
#define __KMERGEKDS_H__

#include "Rtypes.h"
#include <string>
#include <list>

using namespace std;

class KEvent;
class KDataReader;
class KDataWriter;

struct timeReset {
	Int_t pcDateSeconds;
	Int_t year;
	Int_t month;
	Int_t day;	
};

class KMergeKds  { 

public:
  //Constructors
  KMergeKds(const char* file1, const char *file2, const char *fileOut);
  virtual ~KMergeKds(void);
  virtual void Clear(Option_t *opt = "C");

  void AddTimeReset(Int_t pcDateSec, Int_t year, Int_t month, Int_t day); //what other information?
	void MergeFiles(void);

	//getters
	Double_t GetMinStampDiffForRestart(void) {return fMinStampDiffForRestart;	}
  //setters
  void SetMinStampDiffForResetart(Double_t aStampDiff) { fMinStampDiffForRestart = aStampDiff;}
  
private:

	string fInputFile1;
	string fInputFile2;
	string fOutputFile;
	
	Double_t fMinStampDiffForRestart;  
	
	KEvent* fEvent1;
	KEvent* fEvent2;
	KEvent* fOutputEvent;
	
	KDataReader *fReaderFile1;
	KDataReader *fReaderFile2;
	KDataWriter *fWriterOutputFile;
	
	Bool_t fGoToNextEntryFile1;
	Bool_t fGoToNextEntryFile2;
	
	list<timeReset> fListOfResets;
	
	Long64_t fPreviousEventStamp1;
	Long64_t fPreviousEventStamp2;
	
  //private methods
  void InitializeMembers(void);
  Bool_t EndOfFile(KDataReader* aReaderFile);
  void FillRemainingFromFile(KDataReader* aReaderFile);
  void FinishAndSummary(void);
  Bool_t RestartFound(void);
  void HandleRestart(void);
  void MergeEvents(void);
  void FillEvent(KDataReader* aReaderFile);
  
  ClassDef(KMergeKds,1);
};


#endif // __KMERGEKDS_H__
