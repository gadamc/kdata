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
using namespace std;

class KDataTypeParser {
	private:
		set<string> fStandardClasses;
		set<string> fUserClasses;
		
	public:
	
		void Parse(string anExpression);
	ClassDef(KDataTypeParser,0);
};

#endif