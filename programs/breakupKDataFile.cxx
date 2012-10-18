/*
 *  breakupKDataFile.cxx
 *  KDataStructure
 *
 *  Created by Adam Cox on 7/26/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */
#include <iostream>
#include <cstdlib>
#include "TString.h"
#include "TDatime.h"
#include "KHLAEvent.h"
#include "KDataReader.h"
#include "KDataWriter.h"

using namespace std;

int main(int argc, char* argv[])
{
	
	
	//here are the values that are needed during the program, but which 
	//may be altered at run-time by user input.
	Int_t numSubFiles = 20;
	cout << "There are " << argc << " arguments" << endl;
	//read in the arguements supplied by the user
	Int_t i = 1;
	while(i < argc-2) {
		
		if(strcmp(argv[i],"-n") == 0){
			numSubFiles = atoi(argv[++i]);
		}
		
		else {
			cout << "Invalid option: " << argv[i] << "... exiting" << endl;
			exit(0);
		}
        
        i++;
	}
	
	TString fInFileName(argv[argc-2]);
	TString fOutDir(argv[argc-1]);
	if(!fOutDir.EndsWith("/")) fOutDir += "/";
	
	//spit out the command to std out so the user knows the program read in the proper
	TDatime now;
	cout << now.AsSQLString() << ": ";
	i = 0;
	while(i < argc){
		cout << " " << argv[i++];
	}
	cout << endl << endl;
	cout << "Intput File: " << fInFileName << endl;
	cout << "Output Dir:  " << fOutDir << endl;
	
	
	
	//Now on to the work - read in the file and check for HLA Event.
	//In the future, it should be possible, via polymorphism, to 
	//to handle raw and hla events. 
	
	KDataReader fin(fInFileName.Data());
	KHLAEvent *fEventIn = dynamic_cast<KHLAEvent *>(fin.GetEvent());
	if(fEventIn == 0){
		cout << "Not an HLA File" << endl;
		return -1;
	}
	
	KDataWriter *fout = 0;
	KHLAEvent *fEventOut = 0;
	
	if(fInFileName.EndsWith(".root")){
		fInFileName.Remove(fInFileName.Length() - 5, 5);  //strip the .root off"
	}
	else {
		cout << "Input File Name does not end in .root" << endl;
		return -1;
	}
		 
	Int_t fBreakAfterThisNumEvents = fin.GetEntries()/numSubFiles;
	cout << "There will be " << fBreakAfterThisNumEvents << " events per sub file." << endl;
	cout << "There will be " << numSubFiles << " sub files." << endl;
	
	Int_t subFileCounter = 0;
	
	for(Int_t j = 0; j < fin.GetEntries(); j++){
		
		if(j % fBreakAfterThisNumEvents == 0){
			if(fout != 0){
				cout << "Writing and Closing sub file." << endl;
				fout->Write();
				fout->Close();
				delete fout; fout = 0;
			}
			
			TString fOutName = fOutDir + fInFileName + "_"; fOutName +=subFileCounter++;
			fOutName += ".root";
			
			cout << "Opening " << fOutName << endl;
			
			fout = new KDataWriter(fOutName.Data());
			fEventOut = dynamic_cast<KHLAEvent *> (fout->GetEvent());
			if(fEventOut == 0){
				cout << "Hmmmm KDataWriter gave me the wrong event type" << endl;
				return -1;
			}
			
		}
		
		fEventOut->Clear();
		fin.GetEntry(j);
		*fEventOut = *fEventIn;
		
		fEventOut->IsSame(*fEventIn,true);
		
		//fEventOut->Set(*fEventIn, fin);
		
		fout->Fill();
		
		/*
		Int_t ObjectNumber = TProcessID::GetObjectCount();
		
		//Restore Object count                                                                                                     
		//To save space in the table keeping track of all referenced objects 
		//and computation time,
		//we assume that our events DO NOT address each other. We reset the                                                        
		//object count to what it was at the beginning of the event.                                                               
		TProcessID::SetObjectCount(ObjectNumber);*/
		
	}
	
	if(fout != 0){
		cout << "Writing and Closing sub file." << endl;
		fout->Write();
		fout->Close();
		delete fout; fout = 0;
	}
	
	fin.Close();
	
	
}

