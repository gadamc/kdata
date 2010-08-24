//______________________________________________________________________
//
// KBaselineRemoval.cxx
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved.
//
//


#include "KBaselineRemoval.h"

//ClassImp(KBaselineRemoval);

KBaselineRemoval::KBaselineRemoval(void)
{

  InitializeMembers();
}

KBaselineRemoval::~KBaselineRemoval(void)
{

}

void KBaselineRemoval::InitializeMembers(void)
{
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
}

bool KBaselineRemoval::RunProcess(void)
{
	cout << "KBaselineRemoval process : " << fProcessorName << endl;
	return true;
}