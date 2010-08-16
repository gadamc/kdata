/*
 *  filldstEvents.cc
 *  KDataStructure
 *
 *  Created by Adam Cox on 4/6/10 and Benjamin Schmidt 14.04.10 .
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include <iostream>
#include <fstream>


#include "TFile.h"
#include "TBits.h"
#include "TTree.h"
#include "TRandom.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TProcessID.h"

#include "Rtypes.h"

#include "KHLAEvent.h"
#include "KHLAMuonModuleRecord.h"
#include "KHLAMuonVetoSysRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLASambaRecord.h"
#include "KDataReader.h"
#include "KDataWriter.h"


const Char_t kEDSTree[] = "t";
const Char_t kEDSTreeTitle[] = "KDSTree"; 
const Char_t kEDSBranchClass[] = "KHLAEvent";
const Char_t kEDSBranchName[] = "event";

using namespace std;



KHLAEvent* mergeBoloSysRecord(KHLAEvent* ev1, KHLAEvent* ev2){
	string n1, n2;
	string h1, h2;
	n1=ev1->GetBoloSystemRecord()->GetRunName();
	n2=ev2->GetBoloSystemRecord()->GetRunName();
	h1=n1.substr(0,4);
	h2=n2.substr(0,4);
	if(n1=="" && n2!=""){
		ev1->GetBoloSystemRecord()->SetRunName(n2);
	}
	else{
		 if((n2=="" && n1!="" )|| h2==h1){// do notihng
		 }
		 else{
			 cout << "No Run name or different run names found please check manually: RunNames "
				<< n1 << " : "<< n2 << endl;
		 }
	}
	return ev1;		
	//mEv->GetBoloSystemRecord()->SetRunStartTime(Double_t)
	//mEv->GetBoloSystemRecord()->SetRunEndTime(Double_t) //does it make sense for bolos?
}

// no handling of Events where both have muonSysRecord information, implemented so far, only checking the new // event and copying if it has information
KHLAEvent* mergeMuonVetoSysRecord(KHLAEvent* ev1, KHLAEvent* ev2){
	if(ev2->GetMuonVetoSystemRecord()->GetRunNumber()!=0){
		*ev1->GetMuonVetoSystemRecord() = *ev2->GetMuonVetoSystemRecord();
	}
	return ev1;
}


KHLAEvent* mergeSubRecords(KHLAEvent* ev1, KHLAEvent* ev2){ 
	Int_t samba2, bolo2, muon2, pulse2;
	samba2 = ev2->GetNumSambas();
	bolo2 = ev2->GetNumBolos();
	muon2 = ev2->GetNumMuonModules();
	pulse2 = ev2->GetNumBoloPulses();
	
	Int_t ObjectNumber = TProcessID::GetObjectCount();
	
	TClonesArray *boloArray2 = ev2->GetBoloSubRecords();
	for(Int_t i =0; i<bolo2;i++){
		KHLABolometerRecord* singleBoloSub=ev1->AddBolo();
		KHLABolometerRecord* singleBoloSub2=(KHLABolometerRecord*)boloArray2->At(i);
		*singleBoloSub = *singleBoloSub2; 
		KHLASambaRecord* sambaSub2 = singleBoloSub2->GetSambaRecord();
		KHLASambaRecord* sambaSub = 0;
		
		if(sambaSub2 != 0){
			sambaSub = ev1->AddSamba();
			*sambaSub = *sambaSub2;
			if(sambaSub) singleBoloSub->SetSambaRecord(sambaSub);
		}
	}
	//the same for pulse muon etc.
	TClonesArray *muonArray2 = ev2->GetMuonModuleRecords();
	for(Int_t i =0; i<muon2;i++){
		KHLAMuonModuleRecord* muonModuleSub=ev1->AddMuonModule();
		KHLAMuonModuleRecord* muonModuleSub2=(KHLAMuonModuleRecord*)muonArray2->At(i);
		*muonModuleSub = *muonModuleSub2;
	}
	
	//Restore Object count                                                                                                     
	//To save space in the table keeping track of all referenced objects 
	//and computation time,
	//we assume that our events DO NOT address each other. We reset the                                                        
	//object count to what it was at the beginning of the event.                                                               
	TProcessID::SetObjectCount(ObjectNumber);
	
	return ev1;	
}

//at the moment it is strictly necessary that a muon event is handled as event 2
KHLAEvent* mergeEdsEvent(KHLAEvent* ev1, KHLAEvent* ev2){
	ev1 = mergeBoloSysRecord(ev1, ev2);//okay
	ev1 = mergeMuonVetoSysRecord( ev1,  ev2);//always takes ev2 if this one has information other than default
	ev1 = mergeSubRecords( ev1, ev2);// to check
	//mergeEventInformation?
	return ev1; 
}



void printInfo(KHLAEvent* ev){
	ev->myPrint();
	ev->myPrintB();
	if((ev->GetNumSambas())!=0){
		TClonesArray* sambaArray=ev->GetSambaRecords();
		KHLASambaRecord* sam=(KHLASambaRecord*)sambaArray->At(0);
		cout <<  "FileName: "<< sam->GetRunName()	<< " Samba Event Number: "<< sam->GetSambaEventNumber() <<endl;
	}
	else cout << "No Samba information available." << endl;
}



bool checkSpecialCase(string in, KHLAEvent* ev ){
	if((ev->GetNumSambas())!=0){ //GetNumSambaRecs  GetSambaRecords
		TClonesArray* sambaArray = ev->GetSambaRecords();
		KHLASambaRecord* sam=(KHLASambaRecord*)sambaArray->At(0);
		if((sam->GetRunName())==in) return true;
		else return false;
	}
	else {
		cout << "Event without Samba Record" << endl;
		return false;
	}
}



int mergeKEdsTree(string inputPath1, string inputPath2, string outputPath, string log){

	ofstream fout(log.c_str());
	streambuf* cout_rdbuf=cout.rdbuf();
	cout << "Redirecting cout to output file "<< log <<"." << endl;
	streambuf* outBuf=fout.rdbuf();
	cout.rdbuf(outBuf);

	//string specialFile="jd29c005";// stamp and samba numbers restart during file, but not pcTime
	//string specialFile2="jl14c001";//831 events that show up only in the dstFiles not in the samba ntps
	Int_t returnValue=0, globalEntry=0;
	cout << "Building Eds file " << outputPath << " out of "<< inputPath1 
		<< " and " << inputPath2 << endl;

	KDataReader inFile1(inputPath1.c_str());
	KHLAEvent *oldEv1 = dynamic_cast<KHLAEvent *>(inFile1.GetEvent());
	if(oldEv1 == 0) return -1;
	
	KDataReader inFile2(inputPath2.c_str());
	KHLAEvent *oldEv2 = dynamic_cast<KHLAEvent *>(inFile2.GetEvent());
	if(oldEv2 == 0) return -1;
		
	KDataWriter f(outputPath.c_str());
	KHLAEvent *mEv = dynamic_cast<KHLAEvent *>(f.GetEvent());
	if(mEv == 0) return -1;
	f.GetTTree()->BranchRef(); //creates an extra Branch table filled with Branches that have TRefs or TRefArrays

	Long64_t oldStamp1=0, stamp1=0;
	Long64_t oldStamp2=0, stamp2=0;
	Int_t entry1=0, entry2=0, entries1, entries2;
	Bool_t nextEntry1=true, nextEntry2=true;
	entries1=inFile1.GetEntries();
	entries2= inFile2.GetEntries();
	Int_t timeResetLoop=1, mergeLoop=0, insertLoop=0, insertLoop1=0, insertLoop2=0;

	cout <<"Created all Files and Trees!" << endl;
	
	while( (entry1+entry2) < (entries1+entries2) ){
		if(nextEntry1){
			oldEv1->Clear();
			if(inFile1.GetEntry(entry1)<=0) { //we ran into the end of file 1, thus just fill in the rest of file 2
				cout << "Entry does not exist entry1: " << entry1 <<endl;
				for(; entry2 < entries2;entry2++){
					inFile2.GetEntry(entry2);
					*mEv=*oldEv2;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				goto end; // I want to have the summary infomartion!

			}
/*			while(checkSpecialCase(specialFile, oldEv1)){// just looping over the events and not filling them into the Eds structure
				oldEv1->Clear();			//jd29c005 samba restart during file, stamps reset to zero not corrected in Anas files -> new file version available
				entry1++;
				inFile1.GetEntry(entry1);
			}
			while(checkSpecialCase(specialFile2, oldEv1) && entry1>=3092627){// 831 events only showing up in erics dstFiles, nowhere else runName jl14c001, detector ID4
				oldEv1->Clear();
				entry1++;
				inFile1.GetEntry(entry1);	
			}
*/  //testing if these special cases are no longer necessary (at least the first one should be obsolete) 10.06.2010 The first one is obsolete took care of the second one in the fillEraMethod 17.06.10
		}
		if(nextEntry2){
			oldEv2->Clear();
			if(inFile2.GetEntry(entry2)<=0) { //we ran into the end of file 2, thus just fill in the rest of file 1
				cout << "Entry does not exist entry2: "<< entry2 << endl;
				for(; entry1 < entries1; entry1++){
					inFile1.GetEntry(entry1);
					*mEv=*oldEv1;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				goto end;

			}
/*
			while(checkSpecialCase(specialFile, oldEv2)){// just looping over the events and not filling them into the Eds structure
				oldEv2->Clear();
				entry2++;
				inFile2.GetEntry(entry2);	
			}
			while(checkSpecialCase(specialFile2,oldEv2) && entry2>=3092627){// 831 events only showing up in erics dstFiles, nowhere else runName jl14c001 detector ID4
				oldEv2->Clear();
				entry2++;
				inFile2.GetEntry(entry2);	
			}
*/
		}

		stamp1=oldEv1->GetStamp();
		stamp2=oldEv2->GetStamp();
		nextEntry1=false;nextEntry2=false;
		//if (TBit bad stamp place it directly here and grab the next event)
		if((stamp1-oldStamp1)< -1e7 || (stamp2-oldStamp2)< -1e7){ // handling of time restarts should be checked afterwards (manually) 1e7=100s
			if((stamp1-oldStamp1)< -1e7 ){
				Int_t i=0;
				while((oldStamp2-stamp2)<1e7){
					i++;
					oldStamp2=stamp2;
					inFile2.GetEntry(entry2+i);
					stamp2=oldEv2->GetStamp();
					if(i==10000){
						cout << "Scanned 10000 events in File2 and did not find the time restart of File 1; i:"<< i << endl;
						cout << "entry1 : entry2 : oldStamp1 : stamp1 : oldStamp2 : stamp2 \n"<< entry1 << " : "<< entry2 
							<< " : "<< oldStamp1 << " : "<< stamp1 <<" : "<< oldStamp2 << " : "<< stamp2 << endl;
						printInfo(oldEv1);
						printInfo(oldEv2);
						//char d; cin >> d;		
					}
				}
				cout <<i<<" iterations needed to find restart in second file\n This was the "<< timeResetLoop << " time a time restart occured"<< endl;
				oldStamp2=0;
				for(Int_t k=0; k < i; k++){
					inFile2.GetEntry(entry2+k);
					*mEv=*oldEv2;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				nextEntry2=true;
				entry2+=i;
				oldStamp1=0;
			}
			else{
				Int_t i=0;
				while((oldStamp1-stamp1)<1e7){
					i++;
					oldStamp1=stamp1;
					inFile1.GetEntry(entry1+i);
					stamp1=oldEv1->GetStamp();
					if(i==10000){
						cout << "Scanned 10000 events in File1 and did not find the time restart of File 2; i:"<< i << endl;
						cout << "entry1 : entry2 : oldStamp1 : stamp1 : oldStamp2 : stamp2 \n"<< entry1 << " : "<< entry2 
								<< " : "<< oldStamp1 << " : "<< stamp1 <<" : "<< oldStamp2 << " : "<< stamp2 << endl;
						printInfo(oldEv1);
						printInfo(oldEv2);
						//char d; cin >> d;		
					}
				}
				cout <<i<<" iterations needed to find restart in first file\n This was the "<< timeResetLoop << " time a time restart occured"<< endl;
				oldStamp1=0;
				for(Int_t k=0; k < i; k++){
					inFile1.GetEntry(entry1+k);
					*mEv=*oldEv1;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				nextEntry1=true;
				entry1+=i;
				oldStamp2=0;
			}
			timeResetLoop++;
		} // end of time restart handling
		else{ // normal case check according to time stamp which event to insert first or if you have got to merge the events
			if(stamp1 == stamp2){
				//if(entry1==236278){ //some special file issue
				//	cout << "Mit Entry1 = 236278 sind wir in der merge loop gelandet!!! \n";
				//}
				*mEv=*mergeEdsEvent(oldEv1, oldEv2); //seems to work fine (except after time reset loop)
				f.Fill();
				mEv->Clear();
				globalEntry++;
				nextEntry1=true;nextEntry2=true;
				entry1++; entry2++;
				oldStamp1=stamp1;
				oldStamp2=stamp2;
				mergeLoop++;
				if(mergeLoop%100000==0){
					cout << "Merge loop hit: "<< mergeLoop <<endl;
				}
			}
			else{ 
				//if(entry1>=14654)cout << "stamps were not equal" <<endl;
				if(stamp1 < stamp2){
					//if(entry1>=14654)cout << "Entry1 to fill, entry2: "<< entry2 << endl;
					*mEv=*oldEv1;
					//mEv->IsSame(*oldEv1,true);
					f.Fill();
					mEv->Clear();
					globalEntry++;
					nextEntry1=true;
					entry1++;
					oldStamp1=stamp1;
					insertLoop1++;
					if(insertLoop1%100000==0){ // %10000 is a rather good value
					cout << "Insert loop1 hit: "<< insertLoop1 << endl;
					}
					
				}
				else{ 
					if(stamp1 > stamp2){
						*mEv=*oldEv2;
						//mEv->IsSame(*oldEv2,true);
						f.Fill();
						mEv->Clear();
						globalEntry++;
						nextEntry2=true;
						entry2++;
						oldStamp2=stamp2;
						insertLoop2++;
						if(insertLoop2%100000==0){
							cout << "Insert loop2 hit: "<< insertLoop2 << endl;
						}
					}
					else{	insertLoop++;
						cout << "Danger Danger Danger Insert loop hit, without case: "<< insertLoop << endl;
						returnValue=1; // should lead to an infinite loop anyway
					}
				}
				
			}
		}
		
	}
	end:
	cout << "In total "<<globalEntry<<" events filled." << endl;
	f.Write();
	cout << "Write Tree" << endl;
	f.Close();
	cout << "Closing File" << endl;
	inFile1.Close();
	inFile2.Close();
	
	cout.rdbuf(cout_rdbuf);
	cout << "quit mergeKEdsTree "<< endl;
	return returnValue;
}




Bool_t checkTBit(KHLAEvent* ev){
	KHLAMuonVetoSysRecord *mMvSysRec = ev->GetMuonVetoSystemRecord();	
	if(mMvSysRec->TestEventQualityBit(0)){
		 return true;
	}
	else return false;
}

//overloaded function that gets a merged uVetoFiles and additionally handles uVeto events with bad or no stamps and which can leave out the Big Neutron calibration data
//and which does not care about special bolometer files
Int_t mergeKEdsTree(string inputPath1, string inputPath2, string outputPath, string log, Int_t kEntryOfBoloStart, Bool_t bigNeutron, Int_t debugAt){

	ofstream fout(log.c_str());
	streambuf* cout_rdbuf=cout.rdbuf();
	cout << "Redirecting cout to output file "<< log <<"." << endl;
	streambuf* outBuf=fout.rdbuf();
	cout.rdbuf(outBuf);
	
	cout.setf(ios::scientific, ios::floatfield);
	cout.precision(9); //set digits to display on screen

	Int_t returnValue=0, globalEntry=0;
	Long64_t bigNeutronCounter=0;
	cout << "Building Eds file " << outputPath << " out of "<< inputPath1 << " and " << inputPath2 << "." << endl;

	KDataReader inFile2(inputPath2.c_str()); //calls GetEntry and leads to a segFault if one opens the bolometer file first, thus keep this sequence
	//cout << "File 2.1"<< endl;
	KHLAEvent *oldEv2 = dynamic_cast<KHLAEvent *>(inFile2.GetEvent());
	if(oldEv2 == 0) return -1;
	//cout << "File 2.2"<< endl;		

	KDataReader inFile1(inputPath1.c_str());
	KHLAEvent *oldEv1 = dynamic_cast<KHLAEvent *>(inFile1.GetEvent());
	if(oldEv1 == 0) return -1;
	cout << "File 1"<< endl;

	
	KDataWriter f(outputPath.c_str());
	KHLAEvent *mEv = dynamic_cast<KHLAEvent *>(f.GetEvent());
	if(mEv == 0) return -1;
	f.GetTTree()->BranchRef(); //creates an extra Branch table filled with Branches that have TRefs or TRefArrays
	cout << "outFile" << endl;

	Long64_t oldStamp1=0, stamp1=0;
	Long64_t oldStamp2=0, stamp2=0;
	Int_t entry1=0, entry2=0, entries1, entries2;
	Bool_t nextEntry1=true, nextEntry2=true;
	entries1=inFile1.GetEntries();
	entries2=inFile2.GetEntries();
	Int_t timeResetLoop=0, mergeLoop=0, insertLoop=0, insertLoop1=0, insertLoop2=0, badVetoStamp=0;
	Bool_t goodStampBool=true;
	cout <<"Created all Files and Trees!" << endl;
	Bool_t debug=false;

	inFile1.GetEntry(entry1);
	inFile2.GetEntry(entry2);
	
	//fillMuonEventsUntilBolosStart not a nice way to do this, but fast and has to be changed of course whenever producing other data then uVeto Run54 with Bolo Run12
	while(entry2< kEntryOfBoloStart){
		inFile2.GetEntry(entry2);
		*mEv=*oldEv2;
		f.Fill();
		mEv->Clear();
		globalEntry++;
		entry2++;
	}
	cout << "Filled everything till bolo start! We are at the filling of entry" <<entry1+entry2 <<" out of " << entries1+entries2 << endl;
	
	while( (entry1+entry2) < (entries1+entries2) ) {
		goodStampBool=true;
		if(entry1+entry2==debugAt )debug=true;
		//cout << "While loop!"<< endl;
		if(nextEntry1){
			oldEv1->Clear();
			if(debug)cout << "nextEntry1 " << entry1<< endl;
			if(inFile1.GetEntry(entry1)<=0) { //we ran into the end of file 1, thus just fill in the rest of file 2
				cout << "Entry does not exist entry1: " << entry1 <<endl;
				for(; entry2 < entries2;entry2++){
					inFile2.GetEntry(entry2);
					*mEv=*oldEv2;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				goto end;
			}
		}
		if(nextEntry2){
			oldEv2->Clear();
			if(debug)cout << "nextEntry2 " << entry2 << " Entry1: "<< entry1 << endl;
			if(inFile2.GetEntry(entry2)<=0) { //we ran into the end of file 2, thus just fill in the rest of file 1
				cout << "Entry does not exist entry2: "<< entry2 << endl;
				for(; entry1 < entries1; entry1++){
					inFile1.GetEntry(entry1);
					*mEv=*oldEv1;
					f.Fill();
					mEv->Clear();
					globalEntry++;
				}
				goto end;
			}
		}
		stamp1=oldEv1->GetStamp();
		stamp2=oldEv2->GetStamp();
		nextEntry1=false;nextEntry2=false;
		//next if bigNeutron=false ->leave this data out, there is no wimp search bolo data anyway
		if(!bigNeutron){
			if(oldEv2->GetNumMuonModules()>0 ){
				if((oldEv2->GetMuonVetoSystemRecord()->GetRunNumber() ==65 && oldEv2->GetMuonVetoSystemRecord()->GetFileNumber()>=96) || (oldEv2->GetMuonVetoSystemRecord()->GetRunNumber() ==66 && oldEv2->GetMuonVetoSystemRecord()->GetFileNumber()<=15) ){
					nextEntry2=true;
					entry2++;
					bigNeutronCounter++;
					if(bigNeutronCounter%100000==0) cout << "Big Neutron loop: "<< bigNeutronCounter << endl;
					if(debug)cout << "bigNeutron" << endl;
				}
				else {
					goto myElse;
				}
			}
			else{
				goto myElse;
			}
		}
		else{ //muonEvent is not located in the big Neutron source Run
			myElse:
			if(entry2==298193)cout << "arrived at myElse" << endl;
			//next: bad or unnecessary muon veto periods with restarts and without bolo data -> just filling them in where they come (Run12 speciality)
			if( (oldEv2->GetEventTriggerTime()>=1239.095e6 && oldEv2->GetEventTriggerTime()<=1239.116e6) ||  (oldEv2->GetEventTriggerTime() >= 1238.662366e6 && oldEv2->GetEventTriggerTime() <= 1238.675884e6) ) {
				if(debug)cout << "bad muon period" << endl;
				*mEv=*oldEv2;
				f.Fill();
				mEv->Clear();
				globalEntry++;
				entry2++;
				nextEntry2=true;
			}
			else{ //muonEvent is not located in a bad period of run 12
				//if (TBit bad stamp place it directly here and grab the next event)
				if(oldEv2->GetMuonVetoSystemRecord()->GetRunNumber()<=60){
					if(!checkTBit(oldEv2)){
						if(debug)cout << "bad Stamp" << endl;
						goodStampBool=false;
						// bad stamp,thus we fill the event in right here and get the next uVeto event
						*mEv=*oldEv2;
						f.Fill();

						nextEntry2=true;
						entry2++;
						globalEntry++;
						badVetoStamp++;
						if (badVetoStamp%10000==0) cout << badVetoStamp << " bad uVeto Stamps filled." << endl;
					}
					else {
						goodStampBool=true;
					}
				}
				if(oldEv2->GetStamp()>5e13){ //there is no routine checking for bad stamps with the new time board so far, doing a bit of this here
					if(debug)cout << "bad Stamp 2" << endl;
					goodStampBool=false;
					// bad stamp,thus we fill the event in right here and get the next uVeto event
					*mEv=*oldEv2;
					mEv->GetMuonVetoSystemRecord()->SetEventQualityBit(0,0);
					f.Fill();
					nextEntry2=true;
					entry2++;
					globalEntry++;
					badVetoStamp++;
					if (badVetoStamp%10000==0) cout << badVetoStamp << " bad uVeto Stamps filled." << endl;
				}
				if(goodStampBool){ //inserting of normal muon and bolo events
					if(((stamp1-oldStamp1)< -2e8 && stamp1 < 5e10) || ((stamp2-oldStamp2)< -1e7 && stamp2 < 5e10) ||  ((stamp2-oldStamp2)< -1e7 && stamp2 < 5e11 && oldEv2->GetEventTriggerTime()>1245e6  && oldEv2->GetEventTriggerTime()<1248e6) ){ // handling of time restarts should be checked afterwards (manually) 1e7=100s 1e10=100000s=27h //changed -1e7 to -2e8 2000s= ca 33minuten
						//Double_t pcTime2=0, oldPcTime2=0; // pc Time of uVeto
						//pcTime2=oldEv2->GetEventTime();
						//Bool_t muonTiming=false;
						if(debug)cout << "time restart" << endl;
						if((stamp1-oldStamp1)< -1e7 ){
							Int_t i=0;
							while((oldStamp2-stamp2)<1e7  || (stamp2>5e10)){
								i++;
								entry2++;
								oldStamp2=stamp2;
								//oldPcTime2=pcTime2;
								inFile2.GetEntry(entry2);
								if(!checkTBit(oldEv2)){
									// bad stamp,thus we fill the event in right here and get the next uVeto event
									*mEv=*oldEv2;
									f.Fill();
									entry2++;
									badVetoStamp++;
									inFile2.GetEntry(entry2);
									if (badVetoStamp%10000==0) cout << badVetoStamp << " bad uVeto Stamps filled." << endl;
								}
								else{
									stamp2=oldEv2->GetStamp();
									//pcTime=oldEv2->GetPcTime();
									*mEv=*oldEv2;
									f.Fill();
									//muonTiming=timingIsReliabale(oldStamp2, stamp2, oldPcTime2, pcTime2, oldEv1 );
								}
								if(i==10000){
									cout << "Scanned "<< i <<" events in File2 and did not find the time restart of File 1." << endl;
									cout << "entry1 : entry2 : oldStamp1 : stamp1 : oldStamp2 : stamp2  : pcTime1 : pcTime2 : numBolos1 : numMuon1 : numBolos2 : numMuon2 \n"<< entry1 << " : "<< entry2 << " : "<< oldStamp1 << " : "<< stamp1 <<" : "<< oldStamp2 << " : "<< stamp2 << " : " << oldEv1->GetEventTriggerTime() << " : " << oldEv2->GetEventTriggerTime()   << " : " << oldEv1->GetNumBolos()  << " : " << oldEv1->GetNumMuonModules() << " : " << oldEv2->GetNumBolos()  << " : " << oldEv2->GetNumMuonModules() <<endl ;
									cout << "Pointer 1 and Pointer 2 " << oldEv1 << " and " << oldEv2 << endl;
									oldEv1->myPrint();
									oldEv1->myPrintB();
									oldEv2->myPrint();
									oldEv2->myPrintB();
									//char d; cin >> d;		
								}
								mEv->Clear();
								globalEntry++;
							}
							cout <<i<<" iterations needed to find restart in second file.\n This was the "<< timeResetLoop << " time a time restart occured."<< endl;
							oldStamp2=0;
							nextEntry2=true;
							entry2++;
							oldStamp1=0;
						}
						else{
							Int_t i=0;
							while((oldStamp1-stamp1)<1e7  || (stamp1>5e10)){
								i++;
								entry1++;
								oldStamp1=stamp1;
								inFile1.GetEntry(entry1);
								*mEv=*oldEv1;
								f.Fill();
								stamp1=oldEv1->GetStamp();
								if(i==10000){
									cout << "Scanned "<< i <<" events in File1 and did not find the time restart of File 2." << endl;
									cout << "entry1 : entry2 : oldStamp1 : stamp1 : oldStamp2 : stamp2 : pcTime1 : pcTime2  \n"<< entry1 << " : "<< entry2 << " : "<< oldStamp1 << " : "<< stamp1 <<" : "<< oldStamp2 << " : "<< stamp2 << " : " << oldEv1->GetEventTriggerTime() << " : " << oldEv2->GetEventTriggerTime() <<  endl;
									oldEv1->myPrint();
									oldEv1->myPrintB();
									oldEv2->myPrint();
									oldEv2->myPrintB();
									//char d; cin >> d;		
								}
								mEv->Clear();
								globalEntry++;
							}
							cout <<i<<" iterations needed to find restart in first file.\n This was the "<< timeResetLoop << " time a time restart occured."<< endl;
							oldStamp1=0;
							nextEntry1=true;
							entry1++;
							oldStamp2=0;
						}
						timeResetLoop++;
					} // end of time restart handling
					else{ // normal case check according to time stamp which event to insert first or if you have got to merge the events
						if(stamp1 == stamp2){
							if(debug)cout << "merge into same event" << endl;
							//if(entry1==236278){ //some special file issue
							//	cout << "Mit Entry1 = 236278 sind wir in der merge loop gelandet!!! \n";
							//}
							*mEv=*mergeEdsEvent(oldEv1, oldEv2); //seems to work fine (except after time reset loop)
							f.Fill();
							//mEv->Clear();
							globalEntry++;
							nextEntry1=true;nextEntry2=true;
							entry1++; entry2++;
							oldStamp1=stamp1;
							oldStamp2=stamp2;
							mergeLoop++;
							if(mergeLoop%10000==0){
								cout << "Merge loop hit: "<< mergeLoop <<endl;
							}
						}
						else{ 
							//if(entry1>=14654)cout << "stamps were not equal" <<endl;
							if(stamp1 < stamp2){
								if(debug)cout << "fill bolo" << endl;
								//if(entry1>=14654)cout << "Entry1 to fill, entry2: "<< entry2 << endl;
								*mEv=*oldEv1;
								f.Fill();
								globalEntry++;
								nextEntry1=true;
								entry1++;
								oldStamp1=stamp1;
								insertLoop1++;
								if(insertLoop1%10000==0){ // %10000 is a rather good value
									cout << "Insert loop1 hit: "<< insertLoop1 << endl;
								}
								
							}
							else{ 
								if(stamp1 > stamp2){
									if(debug)cout << "fill Muon" << endl;
									*mEv=*oldEv2;
									f.Fill();
									globalEntry++;
									nextEntry2=true;
									entry2++;
									oldStamp2=stamp2;
									insertLoop2++;
									if(insertLoop2%10000==0){
										cout << "Insert loop2 hit: "<< insertLoop2 << endl;
									}
								}
								else{	insertLoop++;
									cout << "Danger Danger Danger Insert loop hit, without case: "<< insertLoop << endl;
									returnValue=1; // should lead to an infinite loop anyway
								}
							}
							
						}
					}
				}
			}
		}
	}
	end:
	cout << "In total "<<globalEntry<<" events filled." << endl;
	f.Write();
	cout << "Write Tree" << endl;
	f.Close();
	cout << "Closing File" << endl;
	inFile1.Close();
	inFile2.Close();

	cout.rdbuf(cout_rdbuf);
	cout << "quit mergeKEdsTree "<< endl;
	return returnValue;
}


