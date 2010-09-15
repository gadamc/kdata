//______________________________________________________________________
//
// KMergeKds.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//
// Currently, this class is completely empty and does nothing!
//


#include "KMergeKds.h"
#include  "KEvent.h";
#include  "KDataReader.h";
#include  "KDataWriter.h";
#include <iostream>

ClassImp(KMergeKds);

KMergeKds::KMergeKds(const char* file1, const char *file2, const char *fileOut)
{
	fInputFile1 = file1;
	fInputFile2 = file2;
	fOutputFile = fileOut;
	
  InitializeMembers();
	
	fEvent1 = 0;
	fEvent2 = 0;
	fOutputEvent = 0;
	
}

KMergeKds::~KMergeKds(void)
{
  //Does calling clear at destruction take too much computing time?
  Clear("C");

}

void KMergeKds::Clear(Option_t* /* opt */)
{
  //Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")

  //Clear and delete local objects here. 

  //Re initialize local members here and prepare for the next use of this class.
  InitializeMembers();

}

void KMergeKds::InitializeMembers(void)
{
	
}

void KMergeKds::MergeFiles(void)
{
	while(!EndOfFile(fReaderFile1) || !EndOfFile(fReaderFile2))
	{
		if(fGoToNextEntryFile1)
			fReaderFile1->GetNextEntry();
		
		if(fGoToNextEntryFile2)
			fReaderFile2->GetNextEntry();
		
		if(EndOfFile(fReaderFile1)){
			FillRemainingFromFile(fReaderFile2);
			FinishAndSummary();
			return; // done, leave the loop. 
		}
		
		if(EndOfFile(fReaderFile2)){
			FillRemainingFromFile(fReaderFile1);
			FinishAndSummary();
			return; // done, leave the loop. 
		}
		
		if(RestartFound()){
			HandleRestart();
		}
		else if(fEvent1->GetStamp() == fEvent2->GetStamp()){
			MergeEvents();
		}
		else if(fEvent1->GetStamp() < fEvent2->GetStamp()){
			FillEvent(fReaderFile1);
		}
		else if(fEvent1->GetStamp() > fEvent2->GetStamp()){
			FillEvent(fReaderFile2);
		}
		else {
			cout << "Danger Danger. " << endl;
		}
	}
	
	FinishAndSummary();
}

void KMergeKds::AddTimeReset(Int_t pcDateSec, Int_t year, Int_t month, Int_t day)
{
  timeReset aReset;
  aReset.pcDateSeconds = pcDateSec;
  aReset.year = year;
  aReset.month = month;
  aReset.day = day;
  
  fListOfResets.push_back(aReset);
}

Bool_t KMergeKds::EndOfFile(KDataReader* /*aReaderFile*/)
{
  return true;
}

void KMergeKds::FillRemainingFromFile(KDataReader* /*aReaderFile*/)
{
  
}

void KMergeKds::FinishAndSummary(void)
{
  
}

Bool_t KMergeKds::RestartFound(void)
{
  return false;
}

void KMergeKds::HandleRestart(void)
{
  
}

void KMergeKds::MergeEvents(void)
{
  
}

void KMergeKds::FillEvent(KDataReader* /*aReaderFile*/)
{
  
}
