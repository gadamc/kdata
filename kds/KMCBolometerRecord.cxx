//_____________________________________________
//
// KMCBolometerRecord.cxx
// KDataStructure
//
// Author: Alexander Wunderle <mailto:alexander.wunderle@kit.edu> on 6/15/10.
//
// * Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
//
// The Monte Carlo Bolometer Record. Holds the monte carlo generated data files.
// This class is used in the KHLaMCEvent class. 
//

#include "KMCBolometerRecord.h"
#include <stdio.h>
#include <iostream>

ClassImp(KMCBolometerRecord);

KMCBolometerRecord::KMCBolometerRecord(void)
{
	InitializeMembers();
	
	//we must take care of members on the heap outside of 
	//InitializeMembers...
	fParticleName=0;
	fParticleNameSize=0;
	
	fProcess=0;
	fProcessSize=0;
	
	fCreationProcess=0;
	fCreationProcessSize=0;
	
}

KMCBolometerRecord::KMCBolometerRecord(const KMCBolometerRecord &aRec)
: KHLABolometerRecord(aRec)
{
	CopyLocalMembers(aRec);
	fParticleName=0;
	fParticleNameSize=0;
	
	fProcess=0;
	fProcessSize=0;
	
	fCreationProcess=0;
	fCreationProcessSize=0;

}

KMCBolometerRecord& KMCBolometerRecord::operator=(const KMCBolometerRecord &aRec)
{
	if(&aRec == this) return *this;
	
	this->KSubRecord::operator=(aRec);
	CopyLocalMembers(aRec);
	
	
	
	return *this;
}

void KMCBolometerRecord::CopyLocalMembers(const KMCBolometerRecord &aRec)
{
	SetEventID(aRec.GetEventID()) ;
	SetTrackID(aRec.GetTrackID()) ;
	SetParentID(aRec.GetParentID()) ;
	SetTime(aRec.GetTime());
	SetEnergy(aRec.GetEnergy());
	SetRecoilEnergy(aRec.GetRecoilEnergy());
	SetParticleName(aRec.GetParticleName().c_str(), aRec.GetParticleName().length()) ;
	SetProcess(aRec.GetProcess().c_str(), aRec.GetProcess().length()) ;
	SetCreationProcess(aRec.GetCreationProcess().c_str(), aRec.GetCreationProcess().length());
	SetXpos(aRec.GetXpos());
	SetYpos(aRec.GetYpos());
	SetZpos(aRec.GetZpos());
	SetMCQvalue(aRec.GetMCQvalue());
}

KMCBolometerRecord::~KMCBolometerRecord(void)
{
  //
  
	//Does calling clear at destruction take too much computing time?
  Clear("C");
	
	
}

void KMCBolometerRecord::Clear(Option_t *opt)
{
	//Clear the base classes and then clear/delete any local
  //members. Its necessary for this Clear method to exist
  //in the case that instances of this object are stored
  //inside of a TClonesArray
  //Also, if this class holds any TClonesArrays, it must call
  //TClonesArray::Clear("C")
	KHLABolometerRecord::Clear(opt);
	
 
	//Clear and delete local objects here. 
	if(fParticleName!=0){
		delete[] fParticleName;
		fParticleName=0;
		fParticleNameSize=0;
	}
	
	if(fProcess!=0){
		delete[] fProcess;
		fProcess=0;
		fProcessSize=0;
	}
	
	if(fCreationProcess!=0){
		delete[] fCreationProcess;
		fCreationProcess=0;
		fCreationProcessSize=0;
	}
	
  //Re initialize local members here and prepare for the next use of this class.
 	 InitializeMembers();
	
}

void KMCBolometerRecord::InitializeMembers(void)
{
	//init local members
	
  //WARNING - THIS METHOD SHOULD NEVER ALLOCATE SPACE FOR POINTERS
  //ONLY SET MEMBERS ON THE STACK TO THEIR INITIAL VALUES
	SetEventID(-99) ;
	SetTrackID(-99) ;
	SetParentID(-99) ;
	SetNumHits(-99) ;
	SetTime(-99);
	SetEnergy(-99);
	SetRecoilEnergy(-99);
	/*SetParticleName("-99",3) ;
	SetProcess("-99",3) ;
	SetCreationProcess("-99",3);*/
	SetXpos(-99);
	SetYpos(-99);
	SetZpos(-99);
	SetMCQvalue(-99);
	
}

void KMCBolometerRecord::SetParticleName(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fParticleNameSize){
		if(fParticleName!=0){
			delete[] fParticleName;	
			fParticleName=0;
			fParticleNameSize=0;
		}
		if(aSize>0)fParticleName= new Char_t[aSize];
		fParticleNameSize=aSize;
	}
	if(aSize>0)memcpy(fParticleName, aWord, fParticleNameSize*sizeof(Char_t));
}

void KMCBolometerRecord::SetProcess(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fProcessSize){
		if(fProcess!=0){
			delete[] fProcess;	
			fProcess=0;
			fProcessSize=0;
		}
		if(aSize>0)fProcess= new Char_t[aSize];
		fProcessSize=aSize;
	}
	if(aSize>0)memcpy(fProcess, aWord, fProcessSize*sizeof(Char_t));
}

void KMCBolometerRecord::SetCreationProcess(const Char_t* aWord, Int_t aSize)
{
	if(aSize!=fCreationProcessSize){
		if(fCreationProcess!=0){
			delete[] fCreationProcess;	
			fCreationProcess=0;
			fCreationProcessSize=0;
		}
		if(aSize>0)fCreationProcess= new Char_t[aSize];
		fCreationProcessSize=aSize;
	}
	if(aSize>0)memcpy(fCreationProcess, aWord, fCreationProcessSize*sizeof(Char_t));
}



Bool_t KMCBolometerRecord::IsSame(const KMCBolometerRecord &aRec, Bool_t bPrint) const
{
	//Compares two objects and their member variables to test for equality.
	//If bPrint is set to true, then a message for each member variable that is different
	//will print to standard out. Otherwise, this method will return false and quit
	//checking member variables as soon as it finds a unequal data member.
	
	Bool_t bIsEqual = true; //assume its true, then test for differences
	
	//call the base class's IsSame methods
	if(!this->KSubRecord::IsSame(aRec,bPrint)){
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fEventID != aRec.fEventID){
		if (bPrint) 
			cout << "KMCBolometerRecord fEventID Not Equal. lhs: " 
			<< fEventID << " != rhs " << aRec.fEventID << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fTrackID != aRec.fTrackID){
		if (bPrint) 
			cout << "KMCBolometerRecord fTrackID Not Equal. lhs: " 
			<< fTrackID << " != rhs " << aRec.fTrackID << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	
	if(fParentID != aRec.fParentID){
		if (bPrint) 
			cout << "KMCBolometerRecord fParentID Not Equal. lhs: " 
			<< fParentID << " != rhs " << aRec.fParentID << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fTime != aRec.fTime){
		if (bPrint) 
			cout << "KMCBolometerRecord fTime Not Equal. lhs: " 
			<< fTime << " != rhs " << aRec.fTime << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fEnergy != aRec.fEnergy){
		if (bPrint) 
			cout << "KMCBolometerRecord fEnergy Not Equal. lhs: " 
			<< fEnergy << " != rhs " << aRec.fEnergy << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fRecoilEnergy != aRec.fRecoilEnergy){
		if (bPrint) 
			cout << "KMCBolometerRecord fRecoilEnergy Not Equal. lhs: " 
			<< fRecoilEnergy << " != rhs " << aRec.fRecoilEnergy << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(strncmp(fParticleName, aRec.fParticleName, sizeof(Char_t)*fParticleNameSize)){
		if (bPrint) 
			cout << "KMCBolometerRecord fParticleName Not Equal. lhs: " 
			<< fParticleName << " != rhs " << aRec.fParticleName << endl;	
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(strncmp(fProcess, aRec.fProcess, sizeof(Char_t)*fProcessSize)){
		if (bPrint) 
			cout << "KMCBolometerRecord fProcess Not Equal. lhs: " 
			<< fProcess << " != rhs " << aRec.fProcess << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(strncmp(fCreationProcess, aRec.fCreationProcess, sizeof(Char_t)*fCreationProcessSize)){
		if (bPrint) 
			cout << "KMCBolometerRecord fParticleName Not Equal. lhs: " 
			<< fCreationProcess << " != rhs " << aRec.fCreationProcess << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fXpos != aRec.fXpos){
		if (bPrint) 
			cout << "KMCBolometerRecord fXpos Not Equal. lhs: " 
			<< fXpos << " != rhs " << aRec.fXpos << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fYpos != aRec.fYpos){
		if (bPrint) 
			cout << "KMCBolometerRecord fYpos Not Equal. lhs: " 
			<< fYpos << " != rhs " << aRec.fYpos << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	if(fZpos != aRec.fZpos){
		if (bPrint) 
			cout << "KMCBolometerRecord fZpos Not Equal. lhs: " 
			<< fZpos << " != rhs " << aRec.fZpos << endl;	
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}

	if(fMCQvalue != aRec.fMCQvalue){
		if (bPrint) 
			cout << "KMCBolometerRecord fMCQvalue Not Equal" << endl;		
		bIsEqual = false;
		if(!bPrint)
			return false;  //if we're not printing out, just return false at first failure
		//the operator== method uses this functionality.
	}
	
	return bIsEqual;
}

