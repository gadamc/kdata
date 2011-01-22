/*
 *  KDataTypeParser.h
 *
 *  Created by Daniel Wegner on 1/21/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 * This class parses a datatype expression, which could be a template, for standard and user classes
 *
 */

#ifndef __KDATATYPEPARSER_H__
#define __KDATATYPEPARSER_H__

#include "Rtypes.h"
#include <string>
#include <set>
#include <vector>
#include <iostream>
using namespace std;

class KDataTypeParser {
	private:
		set<string> fStandardClasses;
		set<string> fUserClasses;
		
	public:
		bool IsInStandardClasses(string aClass) { return(fStandardClasses.find(aClass)!=fStandardClasses.end()); }
		bool IsInUserClasses(string aClass) { return(fUserClasses.find(aClass)!=fUserClasses.end()); }
		
		void Parse(string anExpression);
		void ShowUserClasses();
		void ShowStandardClasses();
	ClassDef(KDataTypeParser,0);
};

#endif