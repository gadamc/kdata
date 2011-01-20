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
#include "TDatime.h"
#include "KClassMemberRecord.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cstdio>
#include "TString.h"
#include "TDatime.h"
using namespace std;

class KClassMaker {
	private:
		UInt_t fBufferSize; // string buffer size for reading files
		string fTargetDir; //target directory for the class to be generated
		string fAuthor; //author's name
		TDatime fTime; // release date and time
		string fClassName; //class name for the class to be generated
		Int_t fClassVersion; // class version for the classdef macro
		vector<KClassMemberRecord> fMembers; //member list of the class to be generated
		set<string> fBaseClasses; //possible baseclasses, empty by default
		set<string> fClassTypes;
		set<string> fDataTypes;
		
		void InitializeMembers(); //initializing default values
		
		
	public:
		//constructors
		KClassMaker(string aClassName="", Int_t aClassVersion=0, string anAuthor="Anonymous",string aTargetDir="");
		
		//Getters
		const Char_t* GetTargetDir() const { return fTargetDir.c_str(); }
		const Char_t* GetAuthor() const { return fAuthor.c_str(); }
		TDatime GetTime() const { return fTime; }
		const Char_t* GetClassName() const { return fClassName.c_str(); }
		Int_t GetNumMembers() const { return fMembers.size(); }
		const Char_t* GetMemberDataType(UInt_t anIndex);
		const Char_t* GetMemberName(UInt_t anIndex);
		
		//Setters
		void SetTargetDir(const Char_t* aTargetDir)  { fTargetDir = aTargetDir; }
		void SetAuthor(const Char_t* anAuthor) { fAuthor = anAuthor; }
		void SetTime(const TDatime& aTime) { fTime = aTime; }
		void SetTime(Int_t aYear,Int_t aMonth,Int_t aDay, Int_t anHour, Int_t aMinute, Int_t aSecond) { fTime.Set(aYear,aMonth,aDay,anHour,aMinute,aSecond); }
		void SetClassName(const Char_t* aClassName) { fClassName = aClassName; }
		
		void ClearMembers();
		void AppendMember(const Char_t* aDataType, const Char_t* aMemberName);
		void ShowMembers();
		void ReadMembers(string aFileName);
		void RemoveBaseClass(const Char_t* aBaseClass);
		void AppendBaseClass(const Char_t* aBaseClass);
		void MakeClass(string aClassName="");
		ClassDef(KClassMaker,0);
};

#endif
