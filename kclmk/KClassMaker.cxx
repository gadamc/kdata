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

KClassMaker::KClassMaker()
{
	fTime.Set(); // set current date and time
}

KClassMaker::ClearMembers()
{
	fMembers.Clear();
}

KClassMaker::AppendMember(const Char_t* aDataType, const Char_t* aMemberName)
{
	//this appends a member to the member list
	fMembers.pushback(KClassMemberRecord(aDataType,aMemberName));
	fDataTypes.insert(aDataType);
}

KClassMaker::ShowMembers()
{
	//shows all the members with datatype for the class to be generated
	for(int k = 0; k<fMembers.size(); ++k)
		cout << fMembers[k].GetDataType() << " " << fMembers[k].GetMemberName() << endl;
}

const Char_t* KClassMaker::GetMemberDataType(Int_t anIndex)
{
	if(anIndex>=0&&anIndex<fMembers.size())
		return fMembers.GetDataType();
	else
	{
		cerr << "KClassMaker::GetMemberDataType: invalid index " << endl;
		return "";
	}
}

const Char_t* KClassMaker::GetMemberName(Int_t anIndex) 
{
	if(anIndex>=0&&anIndex<fMembers.size())
		return fMembers.GetMemberName();
	else
	{
		cerr << "KClassMaker::GetMemberName: invalid index " << endl;
		return "";
	}
}

KClassMaker::MakeClass()
{
	//this generates the header and source file for the class
	ofstream aHeaderStream(TString::Format("%s.h",fClassName.c_str()).Data());
	ofstream aSourceStream(TString::Format("%s.cxx",fClassName.c_str()).Data());
	
	//writing the header file
	aHeaderStream << "/*" << endl;
	aHeaderStream << TString(" * %s.h",fClassName).Data() << endl;
	aHeaderStream << " *" << endl;
	aHeaderStream << TString(" Created by %s on %s",fAuthor.c_str(),fDate.AsString).Data() << endl;
	aHeaderStream << TString("Copyright %i Karlsruhe Institute of Technology. All Rights reserverd.",fTime.GetYear()).Data() << endl;
	aHeaderStream << " *" << endl;
	aHeaderStream << " *" << endl;
	//TODO class description
	aHeaderStream << " */" << endl;
	
	//forward declarations
	for(set<string>::iterator it = fDataTypes.begin(); it!=fDataTypes.end(); ++it)
		aHeaderStream << "class " << *it << ";" << endl;
	
	//class declaration
	aHeaderStream << TString::Format("class %s {",fClassName.c_str()).Data() << endl;
	aHeaderStream << "\tprivate:" << endl;
	aHeaderStream << "\tpublic:" << endl;
	aHeaderStream << "};" << endl;
	
	
	
}
