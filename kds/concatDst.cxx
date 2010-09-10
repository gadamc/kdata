/*
 *  concatDst.cc
 *  KDataStructure 
 *  Preprocessing of Erics two different dst Files into 1, for the whole of Run12.
 *
 *  Created by Benjamin Schmidt 26.04.10 .
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 *  Revisions
 *  1 - Adam Cox. Used new KDataReader/Writer and KEventFactory objects
 *                This calls the KDataWriter::MergeTrees method
 *                which simply calls the TTree::MergeTrees method.
 *  2 - Adam Cox. Using the TTree::MergeTrees in the concatDst function.
 *                
 */

#include "TFile.h"
#include "KHLAEvent.h"
#include "KHLAMuonModuleRecord.h"
#include "KHLABolometerRecord.h"
#include "KHLASambaRecord.h"
#include "TTree.h"
#include <iostream>
#include <fstream>
#include "TRandom.h"
#include <string>
#include "KEventFactory.h"
#include "KDataReader.h"
#include "KDataWriter.h"
#include "TList.h"

//outputFile =firstFile , inputPath=second read only file
int concatDst(string inputFile1, string inputFile2, string outputFile){
	
	cout << "Concatenate two DST files: "<< outputFile 
	<< " = " << inputFile1 << " + " << inputFile2 << endl;
	
	TFile *inFile1 = new TFile(inputFile1.c_str(),"read");
	TFile *inFile2 = new TFile(inputFile2.c_str(),"read");
	TFile *f = new TFile(outputFile.c_str(),"recreate");
	if(!inFile1->IsOpen() || !inFile2->IsOpen()){
		cout << "Could not open Input Files: " << inputFile1 << " \n"<< inputFile2 << endl;
		return 1;
	}
	TTree *inTree1 =(TTree*)inFile1->Get("dsttree");
	TTree *inTree2 =(TTree*)inFile2->Get("dsttree");
	
	TList *list = new TList;
	list->Add(inTree1);
	list->Add(inTree2);

	f->cd();
	TTree *outTree = TTree::MergeTrees(list, "fast");
	
	f->cd();
	outTree->Write();
	f->Close();
	inFile2->Close();
	inFile1->Close();
	return 0;
}

int concatEheat(string inputFile1, string inputFile2, string outputFile){
	
	cout << "Concatenate two eheat files: "<< outputFile 
	<< " = " << inputFile1 << " + " << inputFile2 << endl;
	
	TFile *inFile1 = new TFile(inputFile1.c_str(),"read");
	TFile *inFile2 = new TFile(inputFile2.c_str(),"read");
	TFile *f = new TFile(outputFile.c_str(),"recreate");
	
	TTree *inTree1 =(TTree*)inFile1->Get("energytree");
	TTree *inTree2 =(TTree*)inFile2->Get("energytree");
	
	TList *list = new TList;
	list->Add(inTree1);
	list->Add(inTree2);
	
	f->cd();
	TTree *outTree = TTree::MergeTrees(list, "fast");
	
	f->cd();
	outTree->Write();
	f->Close();
	inFile2->Close();
	inFile1->Close();
	return 0;
}

int concatEion(string inputFile1, string inputFile2, string outputFile){
	
	cout << "Concatenate two eion files: "<< outputFile 
	<< " = " << inputFile1 << " + " << inputFile2 << endl;
	
	TFile *inFile1 = new TFile(inputFile1.c_str(),"read");
	TFile *inFile2 = new TFile(inputFile2.c_str(),"read");
	TFile *f = new TFile(outputFile.c_str(),"recreate");
	
	TTree *inTree1 =(TTree*)inFile1->Get("energytree");
	TTree *inTree2 =(TTree*)inFile2->Get("energytree");
	
	TList *list = new TList;
	list->Add(inTree1);
	list->Add(inTree2);
	
	f->cd();
	TTree *outTree = TTree::MergeTrees(list, "fast");
	
	f->cd();
	outTree->Write();
	f->Close();
	inFile2->Close();
	inFile1->Close();
	return 0;
}


int concatCuts(string inputFile1, string inputFile2, string outputFile){
	
	cout << "Concatenate two cuts files: "<< outputFile 
	<< " = " << inputFile1 << " + " << inputFile2 << endl;
	
	TFile *inFile1 = new TFile(inputFile1.c_str(),"read");
	TFile *inFile2 = new TFile(inputFile2.c_str(),"read");
	TFile *f = new TFile(outputFile.c_str(),"recreate");
	
	TTree *inTree1 =(TTree*)inFile1->Get("cuttree");
	TTree *inTree2 =(TTree*)inFile2->Get("cuttree");
	
	TList *list = new TList;
	list->Add(inTree1);
	list->Add(inTree2);
	
	f->cd();
	TTree *outTree = TTree::MergeTrees(list, "fast");
	
	f->cd();
	outTree->Write();
	f->Close();
	inFile2->Close();
	inFile1->Close();
	return 0;
}



int concatKDS(string *inputFiles, Int_t numFiles, string outputFile){
	//You MUST add these to the list in the proper order.
	//If there are a large numFiles, then expect this routine
	//to take up a LOT of memory. A work around is to combine 
	//two files at once and iterate over the entire set of files
	//that you wish to cacatenate together. 
	KDataReader* mFilesToMerge[numFiles];
	TList *mList = new TList;
	
	cout << "Concatenating Files into the output file " << outputFile << "." << endl;
	cout << "Note, the following files will be added in the order they are found in the input array!" << endl;
	cout << "    Adding the following files: " << endl;
	for(Int_t i=0; i< numFiles; i++){
		cout << "          " << inputFiles[i] << endl;
		mFilesToMerge[i] = new KDataReader(inputFiles[i].c_str());
		
		if(mFilesToMerge[i]->GetTTree() == 0){
			cout << " woops! File doesn't seem to be an KData File. Quitting." << endl;
			return  -1;
		}
		mList->Add(mFilesToMerge[i]->GetTTree());
	}
	
	KDataWriter f(outputFile.c_str());
	
	cout << "Concatenating the Trees... have a beer, this could take a while." << endl;
	f.ConcatenateTrees(mList, "fast");
	
	f.Write();
	f.Close();
	
	//delete mList; 
	for(Int_t i = 0; i < numFiles; i++)
	{
		delete mFilesToMerge[i];
		mFilesToMerge[i] = 0;
	}
	delete mList;
	mList = 0;
	
	
	return 0;
}


#ifndef __CINT__
//we only compile with the main if we're not in the CINT.
//you can still compile this file with the ACLiC compiler when you start a ROOT session, but then you 
//should call MuonVetoToDS directly since a function titled 'main' cannot be used in this case. 
int main(int argc, char* argv[])
{
	if(argc == 1) {
		cout << "Can't run without args!" << endl;
		return -1;
	}

	string argOne, argTwo, argThree;

	argOne = argv[2];
	argTwo = argv[3];
	argThree = argv[4];
	
	switch (atoi(argv[1])) {
		case 1:
			
			return concatDst(argOne, argTwo, argThree);
			break;
			
		case 2:
			return concatCuts(argOne, argTwo, argThree);
			break;
			
		case 3:
			return concatEheat(argOne, argTwo, argThree);
			break;
			
		case 4:
			return concatEion(argOne, argTwo, argThree);
			break;
			
		case 5:
			//concatDst 5 outputFile inFile1 inFile2 ....
			//this case will be used for the muon veto concatenation!
			if(argc >= 4){
				string *listOfFiles = new string[argc-3];
				for(Int_t i = 0; i < argc-3; i++){
					listOfFiles[i] = argv[i+3];
				}
				Int_t theRet = concatKDS(listOfFiles, argc-3, argTwo);
				delete [] listOfFiles;
				return theRet;
			}
			else {
				cout << "Not enough arguments for case 5" << endl;
			}
			break;
			
			
		default:
			cout << "Unknown case " << atoi(argv[1]) << endl;
			cout << "1 = concatDst. 2 = concatCuts. 3 = concatEheat. 4 = concatEion. 5 = concatKDS" << endl;
			return -1;
			break;
	}
	
	return -1;
}
#endif


