//_____________________________________________
//
// KIndex.cxx
// KDataStructure
//
// Author: Benjamin Schmidt <mailto:benjamin.schmidt3@kit.edu> on 04/14/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//
// This class is a little help tool to sort and merge the raw data into daily
// combined files. You pass a KDataReaderObject to it and it creates an index
// from the TimeStamp (This could be easily changed to be more flexible.)
// 
// 

#include "KIndex.h"
#include <exception>
#include <iostream>
#include "TMath.h"
#include "KRawEvent.h"
#include "KSambaRecord.h"

using namespace std;

ClassImp(KIndex);

KIndex::KIndex(void)
{
  InitializeMembers();
    
}

KIndex::KIndex(KDataReader* r)
{
	cout << "KIndex(KDataReader* r): Initializing KIndex and reading the index(stamp) into float arrays" << endl;
    InitializeMembers();
    fR=r;
    fN=fR->GetEntries();
    fE=r->GetEvent();
    fIndex= new Int_t[fN];
	fIndexValue= new Long64_t [fN];
    ReadToIndex();
	if(dynamic_cast<KRawEvent*>(fE)!=0) cout << "KIndex(KDataReader* r): fE points to KRawEvent" << endl;
}

KIndex::~KIndex(void)
{
  delete fIndex;
  delete fIndexValue;
}

void KIndex::InitializeMembers(void)
{
  //
   fN=0;
   fIndex=0; //Null Pointer
   fIndexValue=0; //Null Pointer
   fR=0;
   fE=0;
   fEntriesToRemove=0; //Null Pointer
   fNEntries=0; 
   fNeedFurtherProcessing=false;
}

void KIndex::ReadToIndex(void)
{
    
    if(fR!=0){
        fE=fR->GetEvent();
        if(fE!=0){
            for(Int_t i=0; i< fR->GetEntries(); i++){
                fR->GetEntry(i);
                fIndex[i]=i;
                fIndexValue[i]=fE->GetStamp();
            }
        }
    }
}



Int_t KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts) 
{
	cout << "KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts) entered" << endl;
	if(fNeedFurtherProcessing){
		cout << "KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts) fNeedFurtherProcessing==true" << endl;
		delete fIndex;
		delete fIndexValue;
		fIndexValue=new Long64_t[fNEntries];
		fIndex=fEntriesToRemove;
		for (Int_t i=0; i< fNEntries; i++){
			fR->GetEntry(fIndex[i]);
			fIndexValue[i]=fE->GetStamp();
		}
		fN=fNEntries;
		cout << "KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts) Copied fEntriesToRemove to fIndex. "<< fN << "entries to be processed"<< endl;
		//cout << fIndex.size() << fIndexValue.size() << fN << fIndex.size()/sizeof(Int_t) << fIndexValue.size()/sizeof(Long64_t) << endl;
		Short_t *macs=GetNumberOfAcquisitionMacs();
		Int_t currentEntries=CheckTimeRestarts(macs);
		if(currentEntries > 0){
			fN=RemoveEntriesFromList();
			fNeedFurtherProcessing=true;
		}
		else{
			fNeedFurtherProcessing=false;
		}
		SortIndex();
		return fN;
	}
    if(r==0){
        if(fR != 0 && fN==0){ // should never happen
            ReadToIndex();
        }
    }
    else{
        if(fR != 0){
            delete fIndex;
            delete fIndexValue;
            InitializeMembers();
        
        }
        fR=r;
        fN=fR->GetEntries();
        fIndex= new Int_t[fN];
        fIndexValue= new Long64_t[fN];
        ReadToIndex();
    }
    if(boolRestarts){ //check for restart default =true
		cout << "KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts) boolRestarts==true" << endl;
		Short_t *macs=GetNumberOfAcquisitionMacs();
		Int_t currentEntries=CheckTimeRestarts(macs);
		if(currentEntries > 0){
			fN=RemoveEntriesFromList();
			fNeedFurtherProcessing=true;
			cout << "KIndex::BuildIndex(KDataReader* r, Bool_t boolRestarts): Removed entries after restart from current merging. To be processed later" << endl;
		}
	}
    SortIndex();
	return fN;
}

Int_t KIndex::RemoveEntriesFromList(void){
	if(fNEntries>0 && fIndex!=0){
		Int_t *newArray=new Int_t[fN-fNEntries];
		Long64_t *newArrayValue=new Long64_t[fN-fNEntries];
		Int_t j=0;
		for(Int_t i=0; i<fN; i++){
			if(fEntriesToRemove[j]==i){
				j++;
			}
			else{
				newArray[i-j]=fIndex[i];
				newArrayValue[i-j]=fIndexValue[i];
			}
		}
		delete fIndex;
		delete fIndexValue;
		fIndex=newArray;
		fIndexValue=newArrayValue;
		fN-=fNEntries;
		fNeedFurtherProcessing=true;
		cout << "KIndex::RemoveEntriesFromList(void): Set fNeedFurtherProcessing to true" << endl;
	}
	return fN;
}

Short_t* KIndex::GetNumberOfAcquisitionMacs(){
//Checking all events of a tree for the number of acquisition macs return value is a Short array with 100 elements.
//If an acquisition mac is working a 1 is written into the corresponding array position, 0 else.
//This method should be slow but fine as long as we have less than 100 acquisition macs
	Short_t *macs= new Short_t[100];
	for(Int_t k=0; k<100; k++){
		macs[k]=0;
	}
	Int_t nMacs=0;
	if(fR==0){
		cout << "KIndex::GetNumberOfAcquisitionMacs():No KDataReader initialized!" << endl; 
		return 0;
	}
	for(Int_t i=0; i<fR->GetEntries(); i++){
		fR->GetEntry(i);
		for(Int_t j=0; j<fE->GetNumSambas(); j++){
			if(macs[fE->GetSamba(j)->GetSambaDAQNumber()]!=1){
				macs[fE->GetSamba(j)->GetSambaDAQNumber()]=1;
				nMacs++;
			}
		}
	}
	cout << "KIndex::GetNumberOfAcquisitionMacs(): "<< nMacs << " acquisition Macs found." << endl;
	return macs;
}

Int_t KIndex::CheckTimeRestarts(Short_t* macs){
// Okay input is a KDataReader with data that is sorted according to file name which means 1st all sorted data of Mac S1 than S2 ...
// Check the KDataReaderFile for stamp restarts if true find them put all data after the first restart in the deselection list	
// Steps needed to be done. Get the number of macs. Check restarts in each mac separately. If a restart occurs only in one Mac 
	if(fR==0 || macs==0){
		cout << "KIndex::CheckTimeRestarts(Short_t* macs): Either no KDataReader or no array of Macs: The pointer to the macs array has the value: " << macs << endl;
		return -1;
	}
	cout << "KIndex::CheckTimeRestarts(Short_t* macs) entered" << endl;
	Bool_t restart=false;
	fEntriesToRemove= new Int_t[fR->GetEntries()];
	Long64_t *stampOfRestart=new Long64_t[100];
	Long64_t *lastStamp=new Long64_t[100];
	for(Int_t i=0; i<100; i++){
			stampOfRestart[i]=0;
			lastStamp[i]=0;
		}
	Int_t l=0;
	for(Int_t i=0; i<100; i++){
		if(macs[i]==1){
			cout << "KIndex::CheckTimeRestarts(Short_t* macs) Mac["<<i<<"] is present in the data." << endl;
		}
	}
	cout << fN << " " << fIndex[1] <<  endl;
	//char goOn;
	for(Int_t j=0; j<fN; j++){
		fR->GetEntry(fIndex[j]);
		//if(fIndex[j]!=j)cout << j << " "<< fIndex[j] << endl;
		Int_t k=0;
			if((-lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()]+fE->GetEventTriggerStamp())<-1e5){//time Restart stamp jump back of 1 second or more
					cout << "KIndex::CheckTimeRestarts(Short_t* macs): restart at Mac" << fE->GetSamba(k)->GetSambaDAQNumber()<<" " << fE->GetSamba(k)->GetSambaEventNumber()<<" " << lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()] <<" " << fE->GetStamp() << " "<< fE->GetSamba(k)->GetNtpDateSec() << endl;
					//cin >> goOn;
					restart=true;
					stampOfRestart[fE->GetSamba(k)->GetSambaDAQNumber()]=lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()];
			}
			//cout << "KIndex::CheckTimeRestarts(Short_t* macs): lastStamp at Mac" << fE->GetSamba(k)->GetSambaDAQNumber() <<" " << fE->GetSamba(k)->GetSambaEventNumber()<< " " << lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()] <<" " << fE->GetStamp() << " "<< fE->GetSamba(k)->GetNtpDateSec() << endl;
			lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()]=fE->GetEventTriggerStamp();
			//cout << "KIndex::CheckTimeRestarts(Short_t* macs): lastStamp at Mac" << fE->GetSamba(k)->GetSambaDAQNumber() <<" " << fE->GetSamba(k)->GetSambaEventNumber()<< " " << lastStamp[fE->GetSamba(k)->GetSambaDAQNumber()] <<" " << fE->GetStamp() << " "<< fE->GetSamba(k)->GetNtpDateSec() << endl;
			if(stampOfRestart[fE->GetSamba(k)->GetSambaDAQNumber()]>0){//add event to exclude list
				fEntriesToRemove[l]=fIndex[j];
				l++;
			}
	}
	cout << "KIndex::CheckTimeRestarts(Short_t* macs) for loop processed successful" << endl;
	//Int_t *tempArray=new Int_t[fR->GetEntries()];
	//maybe I have to assign values here...
	//TMath::Sort(l, fEntriesToRemove, tempArray, false); // I suppose this works
	delete stampOfRestart;
	delete lastStamp;
	if(restart==true){
		cout << "KIndex::CheckTimeRestarts(Short_t* macs): There was a time restart. " << l <<" entries singled out to remove from current merging process." << endl;
		fNEntries=l;
		return l;
	}	
	return 0;
}

void KIndex::SortIndex(void)
{
    if(fN>0){
        TMath::Sort(fN, fIndexValue, fIndex, false);
    }
}

void KIndex::PrintIndex(void)
{
	cout << "KIndex::PrintIndex(void): index : indexValue" << endl;
    for(Int_t i=0; i<fN; i++){
		cout << "KIndex::PrintIndex(void):" << fIndex[i] << " : "<< fIndexValue[i] << endl;
        
    }
}
