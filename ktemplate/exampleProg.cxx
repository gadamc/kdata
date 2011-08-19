//
//  exampleProg.cxx
//  kData
//
//  Created by Adam Cox on 8/5/11.
//  Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.
//

#include <iostream>
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
#include "KRawEvent.h"
#include "KDataReader.h"
#include "KBaselineRemoval.h"

using namespace std;

int main(int argc, char* argv[])
{
	KDataReader f("myFile.root");
  KBaselineRemoval bas;
  
  KRawEvent *e = (KRawEvent *)f.GetEvent();
  
  for(int i = 0; i < f.GetEntries(); i++){
    f.GetEntry(i);

  }
		
	
}
