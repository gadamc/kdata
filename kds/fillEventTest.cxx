/*
 *  fillEventTest.cxx
 *  KDataStructure
 *
 *  Created by Adam Cox on 3/18/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include "KHLAEvent.h"
#include "KHLABolometerRecord.h"
#include "KHLASambaRecord.h"
#include "TProcessID.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>

using namespace std;

void makeInputFile1(Int_t &counter);
void makeInputFile2(Int_t &counter);
void RunTest(const char* file, bool opt);


int main(int , char** ) 
{
	cout << "fillEventTest" << endl;
	
	
	//THIS FIRST PART JUST CREATES TWO ROOT FILES TO USE FOR 
	//DEMONSTRATING THE BUG LATER. 
	Int_t j = 0;
	makeInputFile1(j);
	makeInputFile2(j);
	
	cout << "OKAY - INPUT FILES HAVE BEEN CREATED. NOW I MAKE TWO TESTS TO COPY EVENTS" << endl;
	cout << "FROM THESE TWO FILES INTO A THIRD FILE. In the first attempt (test1)," << endl;
	cout << "I copy events from input1.root and input2.root into the file without setting" << endl;
	cout << "the TRef in the KHLAEvent::operator= method. In test2, I set a flag so that" << endl;
	cout << "KHLAEvent::operator= method behaves differently. I expect them to be exactly the" << endl;
	cout << "same, but they are NOT. Can you tell me why and how to fix this?" << endl;
	
	cout << "run test1" << endl;
	RunTest("test1.root",true);
	
	cout << "run test2" << endl;
	cout << "now I do the same thing, but this time use the fAssignmenOptionNoTref == false setting in the KHLAEvent::operator= method" << endl;
	cout << endl;
	
	RunTest("test2.root",false);
	
	return 1;
	
}



void makeInputFile1(Int_t &j)
{
	
	cout << "Create input1.root" << endl;
	//create file 1. print out via Scan.
	
	TFile *f1 = new TFile("input1.root","recreate");
	KHLAEvent *e1 = new KHLAEvent;
	TTree *t = new TTree("t","K tree");
	t->Branch("event", &e1);
	
	for(Int_t i = 0; i < 8; i++)
	{
		//Int_t ObjectNumber = TProcessID::GetObjectCount();
		
		KHLABolometerRecord *bolo = e1->AddBolo();
		bolo->SetEnergyHeat(10*i+1);
		bolo->SetDetectorNumber(1);
		e1->SetStamp(j++);
		
		KHLASambaRecord *samba = e1->AddSamba();
		samba->SetSambaEventNumber(i);
		samba->SetSambaDAQNumber(7);
		
		bolo->SetSambaRecordNum(e1->GetNumSambas()-1);  
		
		t->Fill();
		
		//TProcessID::SetObjectCount(ObjectNumber);
		
		e1->Clear("C");
	}
	t->Write();
	f1->Close();
	delete f1;
	delete e1;
	cout << endl;
	
}

void makeInputFile2(Int_t &j)
{
	cout << "Create input2.root" << endl;
	//create file 2. print out via Scan.
	TFile *f2 = new TFile("input2.root","recreate");
	KHLAEvent *e2 = new KHLAEvent;
	TTree *t = new TTree("t","K tree");
	t->Branch("event", &e2);
	
	for(Int_t i = 0; i < 8; i++)
	{
		//Int_t ObjectNumber = TProcessID::GetObjectCount();
		
		KHLABolometerRecord *bolo = e2->AddBolo();
		bolo->SetEnergyHeat(10*i+201);
		bolo->SetDetectorNumber(2);
		e2->SetStamp(j++);
		
		KHLASambaRecord *samba = e2->AddSamba();
		samba->SetSambaEventNumber(i);
		samba->SetSambaDAQNumber(17);
		
		bolo->SetSambaRecordNum(e2->GetNumSambas()-1);  
		
		t->Fill();
		//TProcessID::SetObjectCount(ObjectNumber);
		
		e2->Clear("C");
	}
	t->Write();
	f2->Close();
	delete f2;
	delete e2;
	//delete t;
}

void RunTest(const char *file, bool /*option*/)
{
	
	cout << endl;
	
	//create file 3, add events from file 1 and 2. 
	//do NOT use TRefs. print out via Scan.
	//THIS SEEMS TO WORK FINE BECAUSE I TURN OFF TRYING TO SET THE TREF
	//IN THE KHLAEVENT::OPERATOR= METHOD.
	//
	TFile *f3 = new TFile(file,"recreate");
	KHLAEvent *e3 = new KHLAEvent;
	TTree *t3 = new TTree("t","K tree");
	t3->Branch("event", &e3);
	
	TFile *fr1 = new TFile("input1.root","read");
	TTree *t1 = (TTree *)fr1->Get("t");
	KHLAEvent *e1 = new KHLAEvent;
	t1->SetBranchAddress("event", &e1);
	//t1->GetBranch("event")->SetAutoDelete(true);
	//e1->Clear("C");
	
	
	TFile *fr2 = new TFile("input2.root","read");
	TTree *t2 = (TTree *)fr2->Get("t");
	KHLAEvent *e2 = new KHLAEvent;
	t2->SetBranchAddress("event", &e2);
	//t2->GetBranch("event")->SetAutoDelete(true);

	//e2->Clear("C");
	t2->GetEntry(0);
	t1->GetEntry(0);
	
	cout << "TTree::Scan for input1.root" << endl;
	t1->Scan("fEnergyHeat:fDetectorNumber:fSambaEventNumber:fSambaDAQNumber","","colsize = 30");
	cout << endl;
	
	cout << "TTree::Scan for input2.root" << endl;
	t2->Scan("fEnergyHeat:fDetectorNumber:fSambaEventNumber:fSambaDAQNumber","","colsize = 30");
	cout << endl;
	
	t2->GetEntry(0);
	t1->GetEntry(0);
	
	//e3->SetAssignmentOptionNoTref(option);  
	
	e3->Clear("C");
	cout << "starting to fill" << endl;
	
	Int_t entry1 = 0;
	Int_t entry2 = 0;
	for(Int_t i = 0; i < 3; i++){
		
		
		//uncomment the next line  to see the difference. This line is
		//required for the assignment operator to work properly!
		//t1->GetEntry(entry1);  //I've already called this!
		
		
		*e3 = *e1;  //SEE THE ASSIGNMENT OPERATOR. 
		t3->Fill();
		t1->GetEntry(++entry1);
		e3->Clear("C");
		
	}
	
	//uncomment the next line  to see the difference. This line is
	//required for the assignment operator to work properly!
	//t2->GetEntry(entry2);
	
	*e3 = *e2; 
	t3->Fill();
	t2->GetEntry(++entry2);	
	e3->Clear("C");
	
	
	
	for(Int_t i = 0; i < 3; i++){
		//uncomment the next line  to see the difference. This line is
		//required for the assignment operator to work properly!
		//t1->GetEntry(entry1);
		
		*e3 = *e1;
		t3->Fill();
		t1->GetEntry(++entry1);
		e3->Clear("C");
	}
	
	
	for(Int_t i = 0; i < 3; i++){
		//uncomment the next line  to see the difference. This line is
		//required for the assignment operator to work properly!
		//t2->GetEntry(entry2);
		
		*e3 = *e2;
		t3->Fill();
		t2->GetEntry(++entry2);		
		e3->Clear("C");
	}
	
	f3->cd();  //run-time segmentation violation if I don't call this!
	t3->Write();
	f3->Close();
	fr1->Close();
	fr2->Close();
	delete fr1;
	delete fr2;
	delete f3;
	delete e1;
	delete e2;
	delete e3;

	
	TFile *fr3 = new TFile(file,"read");
	TTree *tr3 = (TTree *)fr3->Get("t");
	KHLAEvent *er3 = new KHLAEvent;
	tr3->SetBranchAddress("event", &er3);
	
	cout << "TTree::Scan for " << file << endl;
	cout << "Should be in order first 3 events from input1, one event from input2, next 3 events from input1, next 3 events from input2" << endl;
	tr3->Scan("fEnergyHeat:fDetectorNumber:fSambaEventNumber:fSambaDAQNumber","","colsize = 30");
	cout << endl;
	
}
