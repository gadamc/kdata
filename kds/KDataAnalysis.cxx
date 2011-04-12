//_____________________________________________
//
// KDataAnalysis.cxx
// KDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class could be developed into an interface for many different
// types of analysis algorithms. 
//
// Right now it just opens a file for you and allows you to progress through
// the file, event-by-event. It also gives you the option to save any 
// interesting events (SaveThisEvent) to an output file, in the same KEvent format.
// You could then save only a subset of the events in your original file
// for faster analysis, or for more focused investigation.
//
// It also has a method that is used with an KEventCollection object.

#include "KDataAnalysis.h"
#include "KDataReader.h"
#include "KDataWriter.h"
#include "KEventCollection.h"
#include <typeinfo>
#include <exception>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;

ClassImp(KDataAnalysis);


KDataAnalysis::KDataAnalysis(const Char_t* fileName)
{
  //Open 'fileName' for analysis and event-looping.
  
	fFileReader = 0;
	fFileWriter = 0;
  //InitializeMembers();
	
	if(fileName != 0) 
		OpenKDataFile(fileName);
}

KDataAnalysis::~KDataAnalysis(void)
{
  Close();
}

/*void KDataAnalysis::InitializeMembers(void)
{
	//
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}
*/

void KDataAnalysis::Close(Option_t *anOpt)
{
  //close the file. anOpt is passed to TFile::Close().
  
	CloseOutput(anOpt);
	CloseInput(anOpt);
}

void KDataAnalysis::CloseOutput(Option_t *anOpt)
{
  //close the output file. The output file is the file that holds 
  //any events that you thought were interesting and decided 
  //to save. anOpt is passed to TFile::Close(Option_t*).

  
	if(fFileWriter != 0){
		fFileWriter->Close(anOpt);
		delete fFileWriter;
		fFileWriter = 0;
	}
}

void KDataAnalysis::CloseInput(Option_t *anOpt)
{
  //close the input file. pass 'anOpt' to TFile::Close(Option_t*)
  
	if(fFileReader != 0){
		fFileReader->Close(anOpt);
		delete fFileReader;
		fFileReader = 0;
	}
}

Int_t KDataAnalysis::OpenKDataFile(const Char_t* fileName)
{
  //open a file for reading/analyzing. this returns the number
  //of entries found in the KDataFile, otherwise it returns -1 on 
  //a failure
  
	fFileReader = new KDataReader(fileName);
	if(fFileReader != 0){
		return fFileReader->GetEntries();
	}
	else return -1;
}

KEvent*  KDataAnalysis::GetEvent(void)
{
  //Once you've opened your KDataFile, use this method to get a 
  //pointer to the event object. This method returns a KEvent*, but
  //you will probably want to cast that pointer into the appropriate type
  //either a KRawEvent or KHLAEvent. For example, let's say you're 
  //doing analysis with a KHLAEvent, you would do the following
  //
  //KDataAnalysis myAna("/path/to/myKHLADataFile.root");
  //KHLAEvent *e = (KHLAEvent *)myAna.GetEvent();
  //
  
  
	if(fFileReader != 0)
		return fFileReader->GetEvent();
	
	else return 0;
}

Int_t KDataAnalysis::GetCurrentEntryNumber(void) const
{
  //returns the entry number of the tree that was most 
  //recently accessed. 
  
	if(fFileReader != 0)
		return fFileReader->GetCurrentEntryNumber();
	
	else return -1;
}

Int_t KDataAnalysis::GetNextEntry(void) const
{
  //goes to the next entry in the tree. returns the size of the 
  //entry in bytes (like TTree::GetEntry(Int_t i))
  
	if(fFileReader != 0)
		return fFileReader->GetNextEntry();
	
	else return -1;
}

Int_t KDataAnalysis::GetPreviousEntry(void) const
{
  //goes to the previos entry in the tree. returns the size of the 
  //entry in bytes (like TTree::GetEntry(Int_t i))
  
	if(fFileReader != 0)
		return fFileReader->GetPreviousEntry();
	
	else return -1;
}

Int_t KDataAnalysis::GetEntry(Int_t anEntry) const
{
  //goes to entry, anEntry, in the tree. returns the size of the 
  //entry in bytes (like TTree::GetEntry(Int_t i))
  
	if(fFileReader != 0)
		return fFileReader->GetEntry(anEntry);
	
	else return -1;
}

Int_t KDataAnalysis::GetEntries(void) const
{
  //returns the number of entries in the Tree in the input file that 
  //was opened. Returns -1 if the file doesn't seem to have been opened. 
  
	if(fFileReader != 0)
		return fFileReader->GetEntries();
	
	else return -1;
}

Bool_t KDataAnalysis::OpenOutputKDataFile(const Char_t* fileName)
{
  //opens an output file that will hold a list of KDatafiles
  //of the same type as the input file. (this means, if you open
  //a KData file holding KRawEvents, this method will open up a file
  //that holds KRawEvents. And similarly for KHLAEvents.) 
  //returns true if successful.
  //you can then call SaveThisEvent to save the data found in object
  //pointed to by GetEvent(), which is most likely the data found in the
  //most recently accessed entry in the tree, to this output file.
  
	if(fFileWriter == 0){
		fFileWriter = new KDataWriter(fileName, GetEventClassName());
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}

Bool_t KDataAnalysis::OpenOutputKDataFile(const Char_t* fileName, const Char_t* aLevel)
{
  //opens an output file that will hold a list of KDatafiles
  //of the  type 'aLevel'. if you pass aLevel = "HLA", you'll get
  //a file with KHLAEvents in it. if aLevel = "Raw", you'll get a file
  //with KRawEvents.
  //returns true if successful.
  //you can then call SaveThisEvent to save the data found in object
  //pointed to by GetEvent(), which is most likely the data found in the
  //most recently accessed entry in the tree, to this output file
  
	if(fFileWriter == 0){
		fFileWriter = new KDataWriter(fileName, aLevel);
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}

Bool_t KDataAnalysis::OpenOutputKDataFile(const Char_t* fileName, KEvent **anEvent)
{
  //opens an output file that will hold a list of KDatafiles
  //of whatever type anEvent is. If anEvent points to a KHLAEvent, then
  //the output file will hold KHLAEvents. similarly for KRawEvents.
  //returns true if successful.
  //you can then call SaveThisEvent to save the data found in object
  //pointed to by GetEvent(), which is most likely the data found in the
  //most recently accessed entry in the tree, to this output file
  
	if(fFileWriter == 0){
		fFileWriter = new KDataWriter(fileName, anEvent);
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}


Int_t KDataAnalysis::SaveThisEvent(KEvent* e)
{
  // For standard event saving, use SaveThisEvent(void).
  //
	//This method save a particular event, e,  into the output KData File. 
	//The type of the event need not be the same type of
	//event held in the input KData file (HLA, Raw, etc..)
	//However, the output KData file will contain a single tree
	//with a single branch of a particular event type. For example,
	//if you call OpenOutputKDataFile("file.root","HLA"), then
	//all of the events in file.root will be of type KHLAEvent.
	//If you call this method and 'e' is an event of type KHLAEvent,
	//then it will copy it normally into your output file. If you call
	//this method and 'e' is an event of type KRawEvent, then
	//the KHLAEvent class will copy all of the base information 
	//AND compute any High Level parameters that it can based upon 
	//the information that is available in the RawEvent class. 
  //(However, at this moment, v3.0.1, no High Level Parameters are 
  //automatically calculated yet, so only the basic information would be copied.)
	//The other example is if you call OpenOutputKDataFile("file.root", "Raw").
	//If you then call this method and 'e' is an event of type KRawEvent
	//it will copy the all of the KRawEvent data normally. If you call this method
	//and pass in an event of type KHLAEvent, it will only copy the
	//data members that both classes share - namely, the data members
	//that exist in the KEvent base class. Any data members 
	//specifically in the KRawEvent
	//class (and not in the base class) will not be set. 
	//
	//Returns -1 if it fails to save the event in your output KData file
	//otherwise returns from KDataWriter::Fill. (TTree::Fill)
	
	Int_t theRet = -1;
		
	if(fFileWriter != 0){
		if(fFileWriter->IsReady()){
			if(e == 0){
				cout << "KDataAnalysis::SaveThisEvent(KEvent *). Argument is NULL" << endl;
				return theRet;
			}
			else if(fFileWriter->GetEvent() == 0){
				cout << "KDataAnalysis::SaveThisEvent(KEvent *). KDataWriter::GetEvent() is NULL" << endl;
				return theRet;
			}
			//the type of event (KEvent, KHLAEvent, KRawEvent, etc..) will
			//be handled appropriatedly by the virtual assignment operator=
			
			//BUG! nope, i don't think that this works!
			*fFileWriter->GetEvent() = *e;
			theRet = fFileWriter->Fill();
			
		}
	}
	
	return theRet;
}

Int_t KDataAnalysis::SaveThisEvent(void)
{
	//Saves the current event (the event held by the pointer returned
  //by the method KDataAnalysis::GetEvent() )into the output KData file.
  
	return SaveThisEvent(GetEvent());
}

Int_t KDataAnalysis::Write(const Char_t* name, Int_t option, Int_t bufsize)
{
	//The options are the same as those options in TObject::Write. 
    //This writes the output KEdwDS data file to disk.
	
	if(fFileWriter != 0){
		return fFileWriter->Write(name, option, bufsize);
	}
	else return -1;
}

const char* KDataAnalysis::GetEventClassName(void) const
{
  //returns the name of the event class that was found in the 
  //input KData file.
  
	if(fFileReader != 0) {
		return fFileReader->GetEventClassName();
	}
	
	else return "";
}


KEventCollection* KDataAnalysis::GetCoincEvent(KEventCollection *mCoincE, Double_t fForwardTime,
																						 Double_t fBackwardTime)
{
	//This routine finds, for the current event that the file is pointing to,
	//nearby events that match a particular criteria set by the input 
	//arguments and packages them into an KEventCollection object. 
	//The current set of criteria is only a time window defined by the 
	//two variables fForwardTime and fBackwardTime
	//If the pointer to the KEdwEventCollection is NULL, this routine will
	//make a new object, which you must delete when you're done with it. 
	/*
	 version: 0
	 This routine only looks for coincidences within a specific time frame. 
	 
	 ***In order to be more inclusive, this routine should be modified somehow
	 to search for physics event times by examining the ion pulse offset time.
	 However, this is difficult with the current data stream because we are
	 not guaranteed that events are in order in the data stream according to
	 their physics time. In fact, its probably likely to find events "out of
	 order" during high-rate situations. These are events where the heat trigger
	 times are sequenced in a different way that the full reconstructed event
	 time would have sequenced them. For example, event 1 has a stamp time
	 of 10000 and the reconstructed ion pulse time is 0. Event 2 has a 
	 stamp time of 10100, but the reconstructed ion pulse time is -200. So, 
	 in the data stream Event 2 would come after Event 1, even though 
	 physically Event 2 occurred before Event 1. Now, imagine a 
	 series of events like this. If one were to perform a coincidence search, 
	 its not clear when to stop searching for events away from the event
	 of interest because the order of events is not guaranteed to be chronological. 
	 The simplest thing to do would be to search for coincdences
	 in a significantly large time window - beyond what is reasonably expected -
	 and then analyze the resulting subset in more detail. 
	 Another possible work around is to write a new program that will break
	 up the HLA event structure and re-organize it into an event structure
	 that is based upon the physics event time ( = time stamp + ion pulse time),
	 forcing that the events are arranged in the file in order. 
	 
	 
	 */
	
	KEvent *anEvent = GetEvent();
	
	if(anEvent == 0){
		return mCoincE;
	}
	if(mCoincE == 0) mCoincE = new KEventCollection;
	
	Int_t fSaveEntry = GetCurrentEntryNumber();
  
  //need a routine here to look for events near this particular event
  //given the criteria above. 
  Double_t deltaT = 0;
  Double_t fCurrentTime = anEvent->GetStampTime();
  Double_t fTempTime = fCurrentTime;
  deltaT = fabs(fCurrentTime - fTempTime);
  
  mCoincE->AddEvent(anEvent);
  
  Bool_t keepGoing = true;
  while(keepGoing){
    if(GetPreviousEntry() > 0){
      //cout << "..searching backward...." << endl;
      
      anEvent =  GetEvent();
      fTempTime = anEvent->GetStampTime();
      deltaT = fabs(fCurrentTime - fTempTime);
      if(deltaT < fBackwardTime){  
        //cout << "adding event" << endl;
        mCoincE->AddEvent(anEvent);
      }
      else
        keepGoing = false;
    }
    else {
      keepGoing = false;
    }
  }

  
  //reset and search forward in time. 
  GetEntry(fSaveEntry);
  anEvent = GetEvent();
  fCurrentTime = anEvent->GetStampTime();
  fTempTime = fCurrentTime;
  deltaT = fabs(fCurrentTime - fTempTime);
  
  keepGoing = true;
  while(keepGoing){
    if(GetNextEntry() > 0){
      //cout << "..searching forward...." << endl;
      anEvent = GetEvent();
      fTempTime = anEvent->GetStampTime();
      deltaT = fabs(fCurrentTime - fTempTime);
      if(deltaT < fForwardTime) {
        //cout << "adding event" << endl;
        mCoincE->AddEvent(anEvent);
      }
      else
        keepGoing = false;
    }
    else {
      keepGoing = false;
    }
  }

	GetEntry(fSaveEntry);
	
	return mCoincE;
}
 

Double_t KDataAnalysis::GetBoloPhysicsEventTime(Int_t i, KHLAEvent *ev) 
{
  //If the input file is a high level file (holds KHLAEvents), 
  //it will look at the i^th bolometer record in the event
  //and return the stamp value + the ion pulse time offset in seconds. 
  //if the i^th record does not exist, or if its not a high level file
  //it will return -9999
  
	if(ev == 0)
		ev = dynamic_cast< KHLAEvent *>( this->GetEvent() );
	
	if(ev != 0){
		KHLABolometerRecord* bolo = ev->GetBolo(i);
		if(bolo != 0)
			return ev->GetSecPerStamp()*(ev->GetStamp() + bolo->GetIonPulseTimeOffset());
		else 
			return -9999.;
	}
	else return -9999.;
}

