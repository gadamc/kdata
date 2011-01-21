/*
 *  KDataTypeParser.cxx
 *
 *  Created by Daniel Wegner on 1/21/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 * This class parses a datatype expression, which could be a template, for standard and user classes
 *
 */

#include "KDataTypeParser.h"

ClassImp(KDataTypeParser);

void KDataTypeParser::Parse(string anExpression)  
{
	//parses a datatype expression
	
	//scanning for brackets in multi parametrized templates
	Int_t aBracketCounter = 0;
	vector<unsigned int> theCommaPositions;
	for(unsigned int k = 0; k<anExpression.size(); ++k) {
		switch(anExpression[k]) {
			case '<': ++aBracketCounter; break;
			case '>': --aBracketCounter; break;
			case ',': if(aBracketCounter==0)
									theCommaPositions.push_back(k); break;
			default: break;
		}
	}
	if(theCommaPositions.size()) {
		Parse(anExpression.substr(0,theCommaPositions[0]));
		for(int k = 1; k<theCommaPositions.size()-1; ++k)
			Parse(anExpression.substr(theCommaPositions[k]+1,theCommaPositions[k+1]-theCommaPositions[k]-1));
		Parse(anExpression.substr(theCommaPositions[theCommaPositions.size()-1]+1,anExpression.size()-theCommaPositions.size()-1));
	}
	
	//standard C++ templates
	if(anExpression.substr(0,6)=="bitset") {
		fStandardClasses.insert("bitset");
		Parse(anExpression.substr(7,anExpression.size()-8));
	}
	else
		if(anExpression.substr(0,5)=="deque") {
			fStandardClasses.insert("deque");
			Parse(anExpression.substr(6,anExpression.size()-7));
		}
		else
			if(anExpression.substr(0,4)=="list") {
				fStandardClasses.insert("list");
				Parse(anExpression.substr(5,anExpression.size()-6));
			}
			else
				if(anExpression.substr(0,3)=="map") {
					fStandardClasses.insert("map");
					Parse(anExpression.substr(4,anExpression.size()-5));
				}
				else
					if(anExpression.substr(0,8)=="multimap") {
						fStandardClasses.insert("multimap");
						Parse(anExpression.substr(9,anExpression.size()-10));
					}
					else
						if(anExpression.substr(0,8)=="multiset") {
							fStandardClasses.insert("multiset");
							Parse(anExpression.substr(9,anExpression.size()-10));
						}
						else
							if(anExpression.substr(0,14)=="priority_queue") {
								fStandardClasses.insert("priority_queue");
								Parse(anExpression.substr(15,anExpression.size()-16));
							}
							else
								if(anExpression.substr(0,5)=="queue") {
									fStandardClasses.insert("queue");
									Parse(anExpression.substr(6,anExpression.size()-7));
								}
								else
									if(anExpression.substr(0,3)=="set") {
										fStandardClasses.insert("set");
										Parse(anExpression.substr(4,anExpression.size()-5));
									}
									else
										if(anExpression.substr(0,5)=="stack") {
											fStandardClasses.insert("stack");
											Parse(anExpression.substr(6,anExpression.size()-7));
										}
										else
											if(anExpression.substr(0,6)=="vector") {
												fStandardClasses.insert("vector");
												Parse(anExpression.substr(7,anExpression.size()-8));
											}
}


