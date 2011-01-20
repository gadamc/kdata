/*
 *  KClassMaker.h
 *
 *  Created by Daniel Wegner on 1/20/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 * This class produces the header and source file for a class with specified classname and list of members
 *
 */

#ifndef __KCLASSMAKER_H__
#define __KCLASSMAKER_H__

#include "Rtypes.h"
#include "KClassMemberRecord.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include "TString.h"
#include "TDatime.h"
using namespace std;

class KClassMaker {
	private:
		string fTargetDir; //target directory for the class to be generated
		string fAuthor; //author's name
		fDatime fTime; // release date and time
		string fClassName; //class name for the class to be generated
		vector<KClassMemberRecord> fMembers; //member list of the class to be generated
		set<string> fDataTypes;
		
		
	public:
		//constructors
		KClassMaker();
		
		//Getters
		const Char_t* GetTargetDir() const { return fTargetDir.c_str(); }
		const Char_t* GetAuthor() const { return fAuthor.c_str(); }
		fDatime GetTime() const { return fDate; }
		const Char_t* GetClassName() const { return fClassName.c_str(); }
		Int_t GetNumMembers() const { return fMembers.size(); }
		const Char_t* GetMemberDataType(Int_t anIndex);
		const Char_t* GetMemberName(Int_t anIndex);
		
		//Setters
		void SetTargetDir(const Char_t* aTargetDir)  { fTargetDir = aTargetDir; }
		void SetAuthor(const Char_t* anAuthor) { fAuthor = anAuthor; }
		void SetTime(const fDatime& aTime) { fTime = aTime; }
		void SetClassName(const Char_t* aClassName) { fClassName = aClassName; }
		


		
		
		
		void ClearMembers();
		void AppendMember(const Char_t* aDataType, const Char_t* aMemberName);
		void ShowMembers();
		void MakeClass();
		ClassDef(KClassMaker,0);
};
