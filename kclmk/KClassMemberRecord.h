/*
 *  KClassMemberRecord.h
 *
 *  Created by Daniel Wegner on 1/20/11.
 *  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
 *
 *
 * This class stores datatype and member name information for a single member in the class produced by KClassMaker
 *
 */

#ifndef __KCLASSMEMBERRECORD_H__
#define __KCLASSMEMBERRECORD_H__

#include <string>
using namespace std;

class KClassMemberRecord {
	private:
		string fDataType;
		string fMemberName;
	public:
		//Getters
		const Char_t* GetDataType() const { return fDataType.c_str(); }
		const Char_t* GetMemberName() const { return fMemberName.c_str(); }
		
		//Setters
		void SetDataType(const Char_t* aDataType) { fDataType = aDataType; }
		void SetMemberName(const Char_t* aMemberName)  { fMemberName = aMemberName; }
};

#endif