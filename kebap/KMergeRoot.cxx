//
// KMergeRoot.cxx  
// KDataStructure
//
// Created by Benjamin Schmidt on 4/18/11.
// Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This is a derived class from KIndex. It is supposed to get a KDataReader*
// object (at present created with the python script ksamba2kdata.py) and a 
// char array as outputfilename. All entries of the reader will be sorted 
// according to stamp and then written into the new file specified.
//

#include "KMergeRoot.h"
#include <iostream>
#include "KRawEvent.h"
#include "TTimeStamp.h"
#include "KSambaRecord.h"

ClassImp(KMergeRoot);

KMergeRoot::KMergeRoot(void)
{
    fSorted=false;
    fW=0;
    fEOut=0;
    fOffset=0;
	fOutPath="";
	fDate="";
	fLastStamp=0;
	fDay=0;
	fLastDay=0;
}


KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay):KIndex(r)
{
    fLastDay=lastDay;
	cout << "KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay): Opening KDataReader r and a new KDataWriter " << chOut << endl;
    fOffset=0;
    fSorted=false;
    fEOut=0;// Do I have to check per dynamic cast what the fE Event looks like because I cant create a purely abstract object Thus I can't really create fEOut
    fEOut=fE;
	fOutPath=chOut;
	if(dynamic_cast<KRawEvent*>(fEOut)!=0) cout << "KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay): fEOut points to KRawEvent" << endl;
    fW= new KDataWriter(chOut, &fEOut);
	fLastStamp=stamp;
	if(CheckStampSpans2DaysOfData()){
		cout << "KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay): There are more than 36h of data passed to KMergeRoot me.\n Please start Merging with a call of me.Merge()"<< endl;;
	}
	else{
		cout << "KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay): The data passed is not in the expected format of 2 days of data, out of which one could be selected.\n"
		<< "KMergeRoot::KMergeRoot(KDataReader* r, const char* chOut, Int_t stamp, Int_t lastDay): In particular it is less than 36h of data. You're adviesd to check the data!" << endl;
	}
    //GetStampToUnixTimeOffset();
}


Bool_t KMergeRoot::IsReady() //not doing anything at the moment
{
    if(fSorted==true) return true;
    return false;
    
}


Int_t KMergeRoot::Merge()
{	
	if(fR==0){
		cout << "KMergeRoot::Merge(): No ReaderObject initialized! Exit Merge()!" << endl;
		return -1;
	}
	if(fOutPath==""){
		cout << "KMergeRoot::Merge(): No OutputPath for new file specified! Please specify first. Exit Merge()!" << endl;
		return -1;
	}
	cout << "KMergeRoot::Merge(): Building and sorting index with " << fR->GetEntries() << " entries." << endl;
    Int_t nEntries=BuildIndex(fR, true); //calling KIndex::BuildIndex() which also takes care of detecting a restart and removing all events after a restart
    //second option false specifies not to try any restart handling
	cout << "KMergeRoot::Merge(): BuildIndex(fR) successfull! "  << endl;
    fEOut=fE; //pointer assignment
    fW= new KDataWriter(fOutPath.Data(), &fEOut);
    if(nEntries==0) return 0;
    fSorted=true;
	Int_t lastStampOfCurrentData=0;
    //GetDayOfData();
    
    if( fW->IsReady()){
        for (Int_t i=0; i< nEntries; i++){
            
			if(i%1000==0) cout << "KMergeRoot::Merge(): Working on entry "<< i << " of " << nEntries << endl;
           // if(GetSelectedEntry(GetIndexValue(i))){
                fR->GetEntry(GetIndex(i));
                fEOut=fE;
                fW->Fill();
            //}
        }
        lastStampOfCurrentData=fE->GetStamp();

        fW->Write();
        fW->Close();
		fW=0;
        if(fNeedFurtherProcessing){
     		TTimeStamp ti;
     		fOutPath.Replace(fOutPath.Index(".root"),5 , "-restart");
     		fOutPath+=ti.GetSec();
     		fOutPath+=".root";
     		cout << "KMergeRoot::Merge(): Going into further processing after restart calling Merge(fR, "<< fOutPath.Data() <<  ")"<< endl;
     		Merge(fR, fOutPath.Data());
     	}
		fOutPath="";
		fSorted=false;
		cout << "KMergeRoot::Merge(): Done building outfile " << fOutPath.Data() << ". \n Cleaning up KDataWriter and resetting KMergeObject." << endl;
    }

	fR->Close();
	fR=0;
	cout << "KMergeRoot::Merge(): Done building all outfiles.  Cleaning up KDataReader also." << endl;
	return lastStampOfCurrentData;
}


Int_t KMergeRoot::Merge(KDataReader *r, const char* chOut )
{	
	if(r==0){
		if(fR==0){
			cout << "KMergeRoot::Merge(KDataReader *r, char* chOut ): No ReaderObject initialized! Exit Merge(KDataReader *r, char* chOut)!" << endl;
			return -1;
		}
	}
	if(strcmp(chOut, "")==0){
		if(fOutPath==""){
			cout << "KMergeRoot::Merge(KDataReader *r, char* chOut ): No OutputPath for new file specified! Please specify first. Exit Merge(KDataReader *r, char* chOut)!" << endl;
			return -1;
		}
	}
	fR=r;
	fOutPath=chOut;
	return Merge();
}



/*
Int_t KMergeRoot::GetStampToUnixTimeOffset(Int_t macToSync)
{ //always calibrate the stamp according to the unix Time of Mac S1 (a) nonsense
    Int_t mimac, i, miunix;
	mimac=0; i=0; miunix=0;
    if(fR==0){
        cout << "No KDataReader object initialized!";
        return 0;
    }
    //need a pointer to a highel level event class to access the samba records and put a correct timing in here
    
    while(mimac!=macToSync && i<fR->GetEntries()){
        fR->GetEntry(i);
        for(Int_t j=0; j< fE->GetNumSambas();j++){
            mimac=fE->GetSamba(j)->GetSambaDAQNumber();
            if(mimac==macToSync){
                miunix=fE->GetSamba(j)->GetNtpDateSec();
                fBaseStamp=fE->GetStamp();
                fOffset=unix-fBaseStamp;
                return fOffset;
            }
        }
    }
    
    cout << "Could not find a single event out of Mac S1 to synchronize stamp to UnixTime in "<< fR->GetEntries() << " entries";
    return 0;
}
*/

Bool_t KMergeRoot::GetSelectedEntry(Int_t stamp){
	//selects one day of data
	if (fLastStamp==0){
		cout << "KMergeRoot::GetSelectedEntry(Int_t stamp): fLastStamp not initialized! Putting first stamp of currentFile as fLastStamp" << endl;
		GetFirstStampToSetfLastStamp();
		
	}
	if(stamp-fLastStamp<(24*3600*fE->GetSecPerStamp())){
		return true;
	}
	if(stamp-fLastStamp < 0) //restart of 10 mus stamp 
		return true;// only happens at a particular sort of time restart
	return false;
}

Int_t KMergeRoot::GetFirstStampToSetfLastStamp(){
	// return value set to fLastStamp
	if(fR==0){
		cout << "KMergeRoot::GetSelectedEntry(Int_t stamp): Error in GetFirstStampToSetfLastStamp(): No KDataReader initialized fR==0." << endl;
		return 0;
	}
	fR->GetEntry(0);
	fLastStamp=fE->GetEventTriggerStamp();
	return fLastStamp;
}

/*
Bool_t KMergeRoot::GetSelectedEntry(Int_t pcTimeSec, Int_t stamp){
    if (fDay==0){
        cout << "No Day initialized. Run GetDayOfData() first!" << endl;
        return false;
    }
    TTimeStamp tim(pcTimeSec);
	Int_t ti=(Int_t) (tim.GetDate()%100);

    if(stamp > fLastStamp && ti <=fDay){
		 return true;
    }
    if(stamp < fLastStamp && fDay>fLastDay) //restart of 10 mus stamp
		return true;
    return false;
}
*/

Int_t KMergeRoot::CheckStampSpans2DaysOfData()
{ //Check that the KDataReader fR spans over 2 days of data usually from 9 o'clock on day x to 9 o'clock on day x+2 return x+1 the day to merge
    if(fR==0){
		cout << "KMergeRoot::CheckStampSpans2DaysOfData(): No KDataReader object initialized!" << endl;
        return 0;
    }
    //Int_t unix1,unix2; // would require the implementation of  a virtual method GetPcTimeSec() or GetEventTriggerTime() in KEvent
	//See KHLAEvent about the implementation of such a method //Do we want to force the implementation of such a method until the DAQ works?
	Int_t stampTime1, stampTime2;
    fR->GetEntry(0);
    //unix1=GetStampToUnixTime(fE->GetStamp());
	//unix1=fE->GetPcTimeSec();
	stampTime1=fE->GetStampTime();
    fR->GetEntry(fR->GetEntries());
    //unix2=GetStampToUnixTime(fE->GetStamp());
	//unix2=fE->GetPcTimeSec();
	stampTime2=fE->GetStampTime();
    if((stampTime2-stampTime1)/3600.0< 36){
		if(stampTime2-stampTime1<0){
			cout << "KMergeRoot::CheckStampSpans2DaysOfData(): There seems to be a restart of the stamp values! Can't correctly merge the data. Please take a look manually!" << endl;
			return 0;
		}
		cout << "KMergeRoot::CheckStampSpans2DaysOfData(): The data is not in the expected form of ca. 2 days of data. The data spans a " << (stampTime2-stampTime1)/3600.0 << " h period." << endl;
        return 0;
    }
    //fDay=(t1.GetDate()%100)+1; //GetDate()-> 20110510 fDay=Day of month
	//fOutFile
    //cout << "Two days of data passed to the merge tool. Creating a new file with the data from day "<< fDay << endl;
    return(1);
}


KMergeRoot::~KMergeRoot(void)
{
    delete fW;
    //delete fEOut;
    fW=0;
    fEOut=0;
}
