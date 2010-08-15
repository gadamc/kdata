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
#include "KHLAMuonModuleSubRecord.h"
#include "KHLASingleBoloSubRecord.h"
#include "KHLASambaSubRecord.h"
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
int concatDst(string inputPath1, string inputPath2, string outputFile){
	
	cout << "Concatenate two DST files: "<< outputFile 
	<< " = " << inputPath1 << " + " << inputPath2 << endl;
	
	TFile *inFile1 = new TFile(inputPath1.c_str(),"read");
	TFile *inFile2 = new TFile(inputPath2.c_str(),"read");
	TFile *f = new TFile(outputFile.c_str(),"recreate");
	if(!inFile1->IsOpen() || !inFile2->IsOpen()){
		cout << "Could not open Input Files: " << inputPath1 << " \n"<< inputPath2 << endl;
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

int concatEheat(string inputPath1, string inputPath2, string outputFile){
	
	cout << "Concatenate two eheat files: "<< outputFile 
	<< " = " << inputPath1 << " + " << inputPath2 << endl;
	
	TFile *inFile1 = new TFile(inputPath1.c_str(),"read");
	TFile *inFile2 = new TFile(inputPath2.c_str(),"read");
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

int concatEion(string inputPath1, string inputPath2, string outputFile){
	
	cout << "Concatenate two eion files: "<< outputFile 
	<< " = " << inputPath1 << " + " << inputPath2 << endl;
	
	TFile *inFile1 = new TFile(inputPath1.c_str(),"read");
	TFile *inFile2 = new TFile(inputPath2.c_str(),"read");
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


int concatCuts(string inputPath1, string inputPath2, string outputFile){
	
	cout << "Concatenate two cuts files: "<< outputFile 
	<< " = " << inputPath1 << " + " << inputPath2 << endl;
	
	TFile *inFile1 = new TFile(inputPath1.c_str(),"read");
	TFile *inFile2 = new TFile(inputPath2.c_str(),"read");
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
			cout << " woops! File doesn't seem to be an EDS File. Quitting." << endl;
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
