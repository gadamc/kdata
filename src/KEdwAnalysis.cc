//_____________________________________________
//
// KEdwAnalysis.cc
// KEdwDataStructure
//
// Author: Adam Cox <mailto:adam.cox@ik.fzk.de> on 3/25/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// This class could be developed into an interface for many different
// types of analysis algorithms. 
//
// Right now it just opens a file for you and allows you to progress through
// the file, event-by-event, and then gives you the option to save any 
// interesting events (SaveThisEvent) to an output file, in the same KEdwDS format. 
//
// It also has a method that is used with an KEdwEventCollection object.

#include "KEdwAnalysis.h"
#include "KEdwDSReader.h"
#include "KEdwDSWriter.h"
#include "KEdwEventCollection.h"
#include <typeinfo>
#include <exception>
#include <iostream>
#include <string>
#include <math.h>

using namespace std;

ClassImp(KEdwAnalysis);


KEdwAnalysis::KEdwAnalysis(const Char_t* fileName)
{
	fFileReader = 0;
	fFileWriter = 0;
  InitializeMembers();
	
	if(fileName != 0) 
		OpenEdsFile(fileName);
}

KEdwAnalysis::~KEdwAnalysis(void)
{
  Close();
}

void KEdwAnalysis::InitializeMembers(void)
{
	//
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}


void KEdwAnalysis::Close(Option_t *anOpt)
{
	CloseOutput(anOpt);
	CloseInput(anOpt);
}

void KEdwAnalysis::CloseOutput(Option_t *anOpt)
{
	if(fFileWriter != 0){
		fFileWriter->Close(anOpt);
		delete fFileWriter;
		fFileWriter = 0;
	}
}

void KEdwAnalysis::CloseInput(Option_t *anOpt)
{
	if(fFileReader != 0){
		fFileReader->Close(anOpt);
		delete fFileReader;
		fFileReader = 0;
	}
}

Int_t KEdwAnalysis::OpenEdsFile(const Char_t* fileName)
{
	fFileReader = new KEdwDSReader(fileName);
	if(fFileReader != 0){
		return fFileReader->GetEntries();
	}
	else return -1;
}

KEdwEventBase*  KEdwAnalysis::GetEvent(void)
{
	if(fFileReader != 0)
		return fFileReader->GetEvent();
	
	else return 0;
}

Int_t KEdwAnalysis::GetCurrentEntryNumber(void) const
{
	if(fFileReader != 0)
		return fFileReader->GetCurrentEntryNumber();
	
	else return -1;
}

Int_t KEdwAnalysis::GetNextEntry(void) const
{
	if(fFileReader != 0)
		return fFileReader->GetNextEntry();
	
	else return -1;
}

Int_t KEdwAnalysis::GetPreviousEntry(void) const
{
	if(fFileReader != 0)
		return fFileReader->GetPreviousEntry();
	
	else return -1;
}

Int_t KEdwAnalysis::GetEntry(Int_t anEntry) const
{
	if(fFileReader != 0)
		return fFileReader->GetEntry(anEntry);
	
	else return -1;
}

Int_t KEdwAnalysis::GetEntries(void) const
{
	if(fFileReader != 0)
		return fFileReader->GetEntries();
	
	else return -1;
}

Bool_t KEdwAnalysis::OpenOutputEdsFile(const Char_t* fileName)
{
	if(fFileWriter == 0){
		fFileWriter = new KEdwDSWriter(fileName, GetEventClassName());
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}

Bool_t KEdwAnalysis::OpenOutputEdsFile(const Char_t* fileName, const Char_t* aLevel)
{
	if(fFileWriter == 0){
		fFileWriter = new KEdwDSWriter(fileName, aLevel);
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}

Bool_t KEdwAnalysis::OpenOutputEdsFile(const Char_t* fileName, KEdwEventBase **anEvent)
{
	if(fFileWriter == 0){
		fFileWriter = new KEdwDSWriter(fileName, anEvent);
		return fFileWriter->IsReady();
	}
	else {
		return false;
	}
}


Int_t KEdwAnalysis::SaveThisEvent(KEdwEventBase* e)
{
	//Save a particular event into the output Eds File. 
	//The type of the event need not be the same type of
	//event expected by the output Eds file (HLA, Raw, etc..)
	//However, the output Eds file will contain a single tree
	//with a single branch of a particular event type. For example,
	//if you call OpenOutputEdsFile("file.root","HLA"), then
	//all of the events in file.root will be of type KEdwHLAEvent.
	//If you call this method and pass in an event of type KEdwHLAEvent,
	//then it will copy it normally into your output file. If you call
	//this method and pass in an event of type KEdwRawEvent, then
	//the KEdwHLAEvent class will copy all of the base information 
	//AND compute any High Level parameters that it can based upon 
	//the information that is available in the RawEvent class. 
	//The other example is if you call OpenOutputEdsFile("file.root", "Raw").
	//If you then call this method and pass in an event of type KEdwRawEvent
	//it will copy the event normally. If you call this method
	//and pass in an event of type KEdwHLAEvent, it will only copy the
	//data members that both classes share - namely, the data members
	//that exist in the KEdwEventBase class. Any data members 
	//specifically in the KEdwRawEvent
	//class (and not in the base class) will not be set. 
	//
	//Returns -1 if it fails to save the event in your output EDS file
	//otherwise returns from KEdwDSWriter::Fill. (TTree::Fill)
	
	Int_t theRet = -1;
		
	if(fFileWriter != 0){
		if(fFileWriter->IsReady()){
			if(e == 0){
				cout << "KEdwAnalysis::SaveThisEvent(KEdwEventBase *). Argument is NULL" << endl;
				return theRet;
			}
			else if(fFileWriter->GetEvent() == 0){
				cout << "KEdwAnalysis::SaveThisEvent(KEdwEventBase *). KEdwDSWriter::GetEvent() is NULL" << endl;
				return theRet;
			}
			//the type of event (KEdwEventBase, KEdwHLAEvent, KEdwRawEvent, etc..) will
			//be handled appropriatedly by the virtual assignment operator=
			
			//BUG! nope, i don't think that this works!
			*fFileWriter->GetEvent() = *e;
			theRet = fFileWriter->Fill();
			
		}
	}
	
	return theRet;
}

Int_t KEdwAnalysis::SaveThisEvent(void)
{
	//Saves the current event into the output Eds file.
	return SaveThisEvent(GetEvent());
}

Int_t KEdwAnalysis::Write(const Char_t* name, Int_t option, Int_t bufsize)
{
	//The options are the same as those options in TObject::Write. 
    //This writes the output KEdwDS data file to disk.
	
	if(fFileWriter != 0){
		return fFileWriter->Write(name, option, bufsize);
	}
	else return -1;
}

const char* KEdwAnalysis::GetEventClassName(void) const
{
	if(fFileReader != 0) {
		return fFileReader->GetEventClassName();
	}
	
	else return "";
}


KEdwEventCollection* KEdwAnalysis::GetCoincEvent(KEdwEventCollection *mCoincE, Double_t fForwardTime,
																						 Double_t fBackwardTime)
{
	//This routine finds, for the current event that the file is pointing to,
	//nearby events that match a particular criteria set by the input 
	//arguments and packages them into an KEdwEventCollection object. 
	//The current set of criteria is only a time window defined by the 
	//two variables fForwardTime and fBackwardTime
	//If the pointer to the KEdwEventCollection is NULL, this routine will
	//make a new object, which you must delete when you're done with it. 
	/*
	 version: 0
	 This routine only looks for coincidences within a specific time frame.
	 
	 In version 0, we only look for coincidences with the time stamp parameter.***
	 The primary event must have an Energy and Q value in a specified range. 
	 Then, once that event is found, it searches backwards and forward in time
	 searching for any other type of event that is specified by another set of 
	 conditions. 
	 
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
	 in time that is significantly large - beyond what is reasonably expected -
	 and then analyze that subset in more detail. 
	 Another possible work around is to write a new program that will break
	 up the HLA event structure and re-organize it into an event structure
	 that is based upon the physics event time ( = time stamp + ion pulse time),
	 forcing that the events are arranged in the file in order. 
	 
	 
	 */
	
	KEdwEventBase *anEvent = GetEvent();
	
	if(anEvent == 0){
		return mCoincE;
	}
	if(mCoincE == 0) mCoincE = new KEdwEventCollection;
	
	Int_t fSaveEntry = GetCurrentEntryNumber();
	
	try {
		
		//NOTE - probably need to come up with an algorithm that
		//will put these coincidence events in chronological order.
		//Could probably just write a method the sorts for you. 
		//could make it happen automatically inside of KEdwEventCollection::AddEvent
		//Or, we just know that the first event in the CoincEvent is always
		//the initial event of interest. 
		//Or, see the WishList for the idea of making objects "Sortable"
		
		
		if(KEdwHLAEvent *mHlaEv = dynamic_cast<KEdwHLAEvent *>(anEvent)){
			//need a routine here to look for events near this particular event
			//given the criteria above. 
			Double_t deltaT = 0;
			Double_t fCurrentTime = mHlaEv->GetStampTime();
			Double_t fTempTime = fCurrentTime;
			deltaT = fabs(fCurrentTime - fTempTime);
			
			mCoincE->AddEvent(mHlaEv);
			
			Bool_t keepGoing = true;
			while(keepGoing){
				if(GetPreviousEntry() > 0){
					//cout << "..searching backward...." << endl;
					mHlaEv = dynamic_cast<KEdwHLAEvent *> (GetEvent());
					fTempTime = mHlaEv->GetStampTime();
					deltaT = fabs(fCurrentTime - fTempTime);
					if(deltaT < fBackwardTime){  
						//cout << "adding event" << endl;
						mCoincE->AddEvent(mHlaEv);
					}
					else
						keepGoing = false;
				}
				else {
					keepGoing = false;
				}
			}
			
			GetEntry(fSaveEntry);
			fCurrentTime = mHlaEv->GetStampTime();
			fTempTime = fCurrentTime;
			deltaT = fabs(fCurrentTime - fTempTime);
			
			keepGoing = true;
			while(keepGoing){
				if(GetNextEntry() > 0){
					//cout << "..searching forward...." << endl;
					mHlaEv = dynamic_cast<KEdwHLAEvent *> (GetEvent());
					fTempTime = mHlaEv->GetStampTime();
					deltaT = fabs(fCurrentTime - fTempTime);
					if(deltaT < fForwardTime) {
						//cout << "adding event" << endl;
						mCoincE->AddEvent(mHlaEv);
					}
					else
						keepGoing = false;
				}
				else {
					keepGoing = false;
				}
			}
			
		}
		else {
			cout << "KEdwAnalysis::GetPhysicsEvent. Sorry, you can only do this with High Level Analysis Events at the moment" << endl;
		}
		
	}
	catch (bad_cast) {
		//
	}
	
	GetEntry(fSaveEntry);
	
	return mCoincE;
}
 

Double_t KEdwAnalysis::GetBoloPhysicsEventTime(Int_t i, KEdwHLAEvent *ev) 
{
	if(ev == 0)
		ev = dynamic_cast< KEdwHLAEvent *>( this->GetEvent() );
	
	if(ev != 0){
		KEdwHLASingleBoloSubRecord* bolo = ev->GetBolo(i);
		if(bolo != 0)
			return ev->GetSecPerStamp()*(ev->GetStamp() + bolo->GetIonPulseTimeOffset());
		else 
			return -9999.;
	}
	else return -9999.;
}

