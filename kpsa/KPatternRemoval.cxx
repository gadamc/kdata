//______________________________________________________________________
//
// KPatternRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KPatternRemoval.h"

//ClassImp(KPatternRemoval);

KPatternRemoval::KPatternRemoval(void)
{

  InitializeMembers();
}

KPatternRemoval::~KPatternRemoval(void)
{

}

void KPatternRemoval::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

bool KPatternRemoval::RunProcess(void)
{
	cout << "KPatternRemoval process : " << fProcessorName << endl;
	return true;
}