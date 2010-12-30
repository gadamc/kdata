/**
   @file    EdwRawDB.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class encapsulating the data tree containing raw informations on the runs, automate data and setup.
*/

#ifndef _EDWRAWDB_H_
#define _EDWRAWDB_H_

#include "EdwUtils.h"
#include "EdwAutomateData.h"
#include "EdwRunData.h"
#include "EdwSetupData.h"
#include "EdwDB.h"
#include "EdwEvent.h"

/**
   @class   EdwRawDB
   @brief   Class encapsulating the data tree containing raw informations on the runs, automate data and setup.

*/
class EdwRawDB : public EdwDB {
 public:
  EdwRawDB() ;
  ~EdwRawDB() ;
  EdwRawDB(string aFile, string aRead = "READ") ; /**< Standard constructor */
  void FillAutomateData(string) ; /**< Add data from a file containing automate data */
  void FillRunData(string aRunDir, string aRunName, ByteOrderType aByteOrder = kBigEndian) ;
  /**< Adds a single run to the database */
  void FillRunData(string aRunDir, ByteOrderType aByteOrder = kBigEndian);
  /**< Adds all the runs in a given directory to the database */
  void FillRunData(string aRunDir, vector<string> aRunList, ByteOrderType aByteOrder = kBigEndian); /**< fill run data */
  void FillAllData(string aAutomateDir, string aRunDir);
  /**< Adds all automate data and all runs from both directories */
  void DumpRunData() ; /**< Dumps all run data from the DB to standard output */
  void DumpEvent(string aRun, UInt_t aSambaNum, string aRootDir, string aFile="None", string aChannel="all") ;
  /**< Dumps the raw traces of a given event from the corresponding Root file */
  void PlotEvent(string aRun, UInt_t aSambaNum, string aRootDir, string aPlotName) ;
  /**< Plots the raw traces of a given event from the corresponding Root file */

  Float_t GetTBolo(time_t) ; /**< Temperature from automate data */
  Float_t GetVoltage(string aRun, string aChannel) ; /**< Voltage */
  Short_t GetVoltageSign(string aRun, string aChannel) ; /**< Voltage sign: useful to know the sign of the pulse we are looking for */
  Int_t GetPatternLength(string aRun, string aChannel);
  vector<string> GetRunList(); /**< List of runs in the fRunBranch */
  vector<string> GetRunChannels(vector<string> aRunNames) ;
  /**< Returns the list of channels used in given runs */
  TChain* GetEventChain(vector<string> aRunNames, string aDir) ;
  /**< Returns a chain of EdwEvents from root files located in aDir, for a list of given runs. [Uses the operator NEW] */
  TChain* GetEventSubChain(vector<string>& aRunNames, string aDir, ULong_t istart, ULong_t istop, ULong_t& i_offset, vector<string>& runnames_restrict, string aLocalDir="");

  TTree* fAutomateTree; /**< Automate data: eg. Tbolo sources etc.. vs time */
  TTree* fRunTree; /**< Run data : eg. run name, channels, voltages, thresholds.. */
  TTree* fSetupTree; /**< Setup : more or less empty currently */
  TBranch* fAutomateBranch; /**< Automate data: eg. Tbolo sources etc.. vs time */
  TBranch* fRunBranch; /**< Run data : eg. run name, channels, voltages, thresholds.. */
  TBranch* fSetupBranch; /**< Automate data: eg. Tbolo sources etc.. vs time */
  EdwAutomateData* fAutomateData; /**< Automate data: eg. Tbolo sources etc.. vs time */
  EdwRunData* fRunData; /**< Run data : eg. run name, channels, voltages, thresholds.. */
  EdwSetupData* fSetupData;   /**< Automate data: eg. Tbolo sources etc.. vs time */

 private:


  ClassDef(EdwRawDB,1)
};

#endif
