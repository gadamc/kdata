/*
 *  KClassMaker.cxx
 *
 *  Created by Daniel Wegner on 1/20/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 * This class produces the header and source file for a class with specified classname and list of members
 *
 */

#include "KClassMaker.h"

ClassImp(KClassMaker);


KClassMaker::KClassMaker(string aClassName, Int_t aClassVersion, string anAuthor,string aTargetDir)  
{
	fTime.Set(); //set current date and time
	fBufferSize = 255;
	fClassVersion = aClassVersion;
	fAuthor = anAuthor;
	fTargetDir = aTargetDir;
	if(aClassName=="")
		fClassName = "TestClass";
		else
		fClassName = aClassName;
	//data types
	fDataTypes.insert("short");
	fDataTypes.insert("int");
	fDataTypes.insert("uint");
	fDataTypes.insert("long");
	fDataTypes.insert("ulong");
	fDataTypes.insert("long long");
	fDataTypes.insert("float");
	fDataTypes.insert("double");
	fDataTypes.insert("long double");
	fDataTypes.insert("bool");
	fDataTypes.insert("char");
	fDataTypes.insert("wchar_t");
}

void KClassMaker::ClearMembers()
{
	fMembers.clear();
}

void KClassMaker::ReadMembers(string aFileName) 
{
	//reads members from an ASCII file
	//each line in form "$DATATYPE $MEMBERNAME" with exactly one blank between them
	string aFilePath;
	if(fTargetDir=="")
		aFilePath = aFileName;
		else
		aFilePath = fTargetDir + "/" + aFileName;
	ifstream aReadStream(aFilePath.c_str());
	Char_t aBuffer[fBufferSize];
	Char_t aDataType[fBufferSize];
	Char_t aMemberName[fBufferSize];
	do {
		aReadStream.getline(aBuffer,fBufferSize);
		sscanf(aBuffer,"%s %s",aDataType,aMemberName);
		this->AppendMember(aDataType,aMemberName);
	} while(!aReadStream.eof());
	fMembers.pop_back();
}

void KClassMaker::AppendMember(const Char_t* aClassType, const Char_t* aMemberName)
{
	//appends a member to the member list
	fMembers.push_back(KClassMemberRecord(aClassType,aMemberName));
	if(fDataTypes.find(aClassType)==fDataTypes.end())
		fClassTypes.insert(aClassType);
}

void KClassMaker::ShowMembers()
{
	//shows all the members with datatype for the class to be generated
	for(vector<KClassMemberRecord>::iterator it = fMembers.begin(); it!=fMembers.end(); ++it)
		cout << it->GetDataType() << " " << it->GetMemberName() << endl;
}

const Char_t* KClassMaker::GetMemberDataType(UInt_t anIndex)
{
	//gets a member's data/class type
	if(anIndex<fMembers.size())
		return fMembers[anIndex].GetDataType();
	else
	{
		if(fMembers.size())
			cerr << "KClassMaker::GetMemberDataType: invalid index, must be in [0," << fMembers.size()-1 << "]" << endl;
		else
			cerr << "KClassMaker::GetMemberDataType: invalid index, list of members is empty" << endl;
		return "";
	}
}

const Char_t* KClassMaker::GetMemberName(UInt_t anIndex) 
{
	//gets a member's name
	if(anIndex<fMembers.size())
		return fMembers[anIndex].GetMemberName();
	else
	{
		if(fMembers.size())
			cerr << "KClassMaker::GetMemberName: invalid index, must be in [0," << fMembers.size()-1 << "]" << endl;
		else
			cerr << "KClassMaker::GetMemberName: invalid index, list of members is empty" << endl;
		return "";
	}
}

void KClassMaker::RemoveBaseClass(const Char_t* aBaseClass) 
{
	//remove a base class name from the set of base class names
	set<string>::iterator it = fBaseClasses.find(aBaseClass);
	fBaseClasses.erase(it);
}

void KClassMaker::AppendBaseClass(const Char_t* aBaseClass)
{
	//append a base class name to the set of base class names
	fBaseClasses.insert(aBaseClass);
}

void KClassMaker::MakeClass(string aClassName)
{
	if(aClassName!="")
		fClassName = aClassName;
	
	//this generates the header and source file for the class
	ofstream aHeaderStream(TString::Format("%s.h",fClassName.c_str()).Data());
	ofstream aSourceStream(TString::Format("%s.cxx",fClassName.c_str()).Data());
	string aMemberName;
	string aDataType;
	
	//writing the header file
	aHeaderStream << "/*" << endl;
	aHeaderStream << TString::Format(" * %s.h",fClassName.c_str()).Data() << endl;
	aHeaderStream << " *" << endl;
	aHeaderStream << TString::Format(" * Created by %s on %s",fAuthor.c_str(),fTime.AsString()).Data() << endl;
	aHeaderStream << TString::Format(" * Copyright %i Karlsruhe Institute of Technology. All Rights reserved.",fTime.GetYear()).Data() << endl;
	aHeaderStream << " *" << endl;
	aHeaderStream << " *" << endl;
	//TODO class description
	aHeaderStream << " */" << endl;
	aHeaderStream << endl;
	

	
	//defining clauses
	string aClassNameUpperCase = fClassName;
	transform(aClassNameUpperCase.begin(),aClassNameUpperCase.end(),aClassNameUpperCase.begin(), ::toupper);
	aHeaderStream << TString::Format("#ifndef __%s_H__",aClassNameUpperCase.c_str()).Data() << endl;
	aHeaderStream << TString::Format("#define __%s_H__",aClassNameUpperCase.c_str()).Data() << endl;
	aHeaderStream << endl;
	
	
	//forward declarations
	for(set<string>::iterator it = fClassTypes.begin(); it!=fClassTypes.end(); ++it)
		aHeaderStream << "class " << *it << ";" << endl;
	aHeaderStream << endl;
	for(set<string>::iterator it = fBaseClasses.begin(); it!=fBaseClasses.end(); ++it)
		aHeaderStream << "class " << *it << ";" << endl;
	aHeaderStream << endl;
	
	//including ROOT types
	aHeaderStream << "#include \"Rtypes.h\"" << endl;
	aHeaderStream << endl;
	

	
	//class declaration
	aHeaderStream << TString::Format("class %s",fClassName.c_str()).Data();
	if(fBaseClasses.size())
		aHeaderStream << " : ";
	UInt_t aSetCounter = 0;
	for(set<string>::iterator it = fBaseClasses.begin(); it!=fBaseClasses.end(); ++it) {
		++aSetCounter;
		aHeaderStream << " public " << *it;
		if(aSetCounter<fBaseClasses.size())
			aHeaderStream << " ,";

	}
	aHeaderStream << " {" << endl;
	aHeaderStream << "\tprivate:" << endl;
	for(vector<KClassMemberRecord>::iterator it = fMembers.begin(); it!= fMembers.end(); ++it)
		aHeaderStream << "\t\t" << it->GetDataType() << " f" << it->GetMemberName() << endl;
	aHeaderStream << endl;
	aHeaderStream << "\tpublic:" << endl;
	aHeaderStream << "\t\t\\\\constructors" << endl;
	aHeaderStream << TString::Format("\t\t%s();",fClassName.c_str()).Data() << endl;
	aHeaderStream << endl;
	aHeaderStream << "\t\t\\\\getters" << endl;
	for(vector<KClassMemberRecord>::iterator it = fMembers.begin(); it!=fMembers.end(); ++it) {
		aMemberName = it->GetMemberName();
		aDataType = it->GetDataType();
		if(aDataType=="string")
			aHeaderStream << TString::Format("\t\tconst char* Get%s() const { return f%s.c_str(); }",aMemberName.c_str(),aMemberName.c_str()).Data() << endl;
			else
			if(aDataType=="TString")
				aHeaderStream << TString::Format("\t\tconst char* Get%s() const { return f%s.Data(); }",aMemberName.c_str(),aMemberName.c_str()).Data() << endl;
				else
				aHeaderStream << TString::Format("\t\t%s Get%s() const { return f%s; }",aDataType.c_str(),aMemberName.c_str(),aMemberName.c_str()).Data() << endl;

	}
	aHeaderStream << endl;
	aHeaderStream << "\t\t\\\\setters" << endl;
	for(vector<KClassMemberRecord>::iterator it = fMembers.begin(); it!= fMembers.end(); ++it) {
		aMemberName = it->GetMemberName();
		aDataType = it->GetDataType();
		if(aDataType=="string"||aDataType=="TString")
			aHeaderStream << TString::Format("\t\tvoid Set%s(const char* a%s) { f%s=a%s; }",aMemberName.c_str(),aMemberName.c_str(),aMemberName.c_str(),aMemberName.c_str()).Data() << endl;
			else
			aHeaderStream << TString::Format("\t\tvoid Set%s(const %s& a%s) { f%s=a%s; }",aMemberName.c_str(),aDataType.c_str(),aMemberName.c_str(),aMemberName.c_str(),aMemberName.c_str()).Data() << endl;
	}
	aHeaderStream << TString::Format("\tClassDef(%s,%i)",fClassName.c_str(),fClassVersion).Data() << endl;
	aHeaderStream << "};" << endl;
	aHeaderStream << endl;
	aHeaderStream << "#endif" << endl;
	aHeaderStream << endl;
	
	//writing the source file
	aSourceStream << "/*" << endl;
	aSourceStream << TString::Format(" * %s.cxx",fClassName.c_str()).Data() << endl;
	aSourceStream << " *" << endl;
	aSourceStream << TString::Format(" * Created by %s on %s",fAuthor.c_str(),fTime.AsString()).Data() << endl;
	aSourceStream << TString::Format(" * Copyright %i Karlsruhe Institute of Technology. All Rights reserved.",fTime.GetYear()).Data() << endl;
	aSourceStream << " *" << endl;
	aSourceStream << " *" << endl;
	aSourceStream << " */" << endl;
	aSourceStream << endl;
	
	aSourceStream << "#include \"Rtypes.h\"" << endl;
	aSourceStream << TString::Format("#include \"%s.h\"",fClassName.c_str()).Data() << endl;
	for(set<string>::iterator it = fClassTypes.begin(); it!=fClassTypes.end(); ++it)
		aSourceStream << TString::Format("#include \"%s.h\"",(*it).c_str()).Data() << endl;
	for(set<string>::iterator it = fBaseClasses.begin(); it!=fBaseClasses.end(); ++it)
		aSourceStream << TString::Format("#include \"%s.h\"",(*it).c_str()).Data() << endl;
	aSourceStream << endl;
	aSourceStream << TString::Format("%s::%s()",fClassName.c_str(),fClassName.c_str()).Data() << endl;
	aSourceStream << "{" << endl;
	aSourceStream << endl;
	aSourceStream << "}" << endl;
	
	
	
}
