//
// KDataProcessingInfo.h
// KDataStructure
//
// Created by Mike G. Marino 2011. Part of the EXO offline analysis code base.
// Some minor modifications were made by Adam Cox (such as changing the name)
//
//

#ifndef KDataProcessingInfo_hh
#define KDataProcessingInfo_hh

#include "TObject.h"
#include "TTimeStamp.h"
#include "TSystem.h"
#include <string> 

class KDataProcessingInfo : public TObject {

  public:
    typedef std::vector<std::string> StrVec;
    KDataProcessingInfo() : TObject(), fCalibrationsFromDatabase(true), fPrevProc(NULL) {} 
    KDataProcessingInfo(const KDataProcessingInfo& other);
    KDataProcessingInfo& operator=(const KDataProcessingInfo& other);
    virtual ~KDataProcessingInfo() { if(fPrevProc) delete fPrevProc; }

    const StrVec& GetModulesUsed()        const { return fVectorOfModules; }
    const StrVec& GetCommandsCalled()     const { return fCommandsCalled; }
    const std::string& GetSVNRev()        const { return fSVNRevision; }
    const TTimeStamp&  GetStartTimeOfProcess() const { return fStartTimeOfProcess; }
    const bool& GetCalibrationsFromDatabase() const { return fCalibrationsFromDatabase; }
    const std::string& GetHostName()    const { return fProcessHostName;}
    const std::string& GetHostInfo()    const { return fProcessHostInfo;}
    const std::string& GetUserInfo()    const { return fUser;}
    void AddModule( const std::string& amod ) { fVectorOfModules.push_back( amod ); } 
    void ClearModules() { fVectorOfModules.clear(); } 
    void AddCommand( const std::string& cmd ) { fCommandsCalled.push_back( cmd ); }
    void ClearCommands() { fCommandsCalled.clear(); } 
    
    void SetSVNRev( const std::string& rev ) { fSVNRevision = rev; }
    void SetAllModules( const StrVec& mods ) { fVectorOfModules = mods; }
    void SetAllCommands( const StrVec& cmds ) { fCommandsCalled = cmds; }
    void SetAllModules( const std::string& mods );
    void SetAllCommands( const std::string& cmds );
    void SetStartTimeOfProcess( TTimeStamp &aTimeStamp );
    void SetCalibrationsFromDatabase(const bool& input) { fCalibrationsFromDatabase = input; }
    void SetHostName( const std::string& host ) { fProcessHostName = host;}
    void SetHostInfo( const std::string& hostinfo ) { fProcessHostInfo = hostinfo;}
    void SetUserInfo( const std::string& user ){ fUser = user;}
    virtual void Print(Option_t * = "") const;
    void SetPrevProcInfo( const KDataProcessingInfo& prev );
    const KDataProcessingInfo* GetPrevProcInfo() const; 

  protected:

    std::string fSVNRevision;     // SVN revision of the build 
    StrVec fVectorOfModules;      // List of modules used in order of usage
    StrVec fCommandsCalled;       // List of commands used
    TTimeStamp fStartTimeOfProcess;    // Start time of processing
    std::string fProcessHostName;    //hostname of computer where process took place
    std::string fProcessHostInfo;               
    std::string fUser;
    bool fCalibrationsFromDatabase;    // false if any calibrations used default values rather than database values
    KDataProcessingInfo* fPrevProc; // Previous processing information

  ClassDef( KDataProcessingInfo, 1 )
};

//---- inlines -----------------------------------------------------------------

inline const KDataProcessingInfo* KDataProcessingInfo::GetPrevProcInfo() const
{
  // Returns processing information from previous runs over the data.  This can
  // be chained so that the following will work to loop through the previous
  // processing information:
  //  
  //  const KDataProcessingInfo* proc_info = currentInfo;
  //  const KDataProcessingInfo* prev_info;
  //  while ( (prev_info = proc_info->GetPrevProcInfo() ) ) {
  //      // Do something with prev_info 
  //      proc_info = prev_info; 
  //  } 
  //
  // returns NULL when no previous info exists.
  return fPrevProc;
}

#endif /* KDataProcessingInfo_hh */

