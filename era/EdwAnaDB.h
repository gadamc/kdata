/**
   @file    EdwAnaDB.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   Class encapsulating the data trees that are necessary to build before running the main analysis
*/

#ifndef _EDWANADB_H_
#define _EDWANADB_H_

#include "EdwUtils.h"
#include "EdwDB.h"
#include "EdwTemplate.h"
#include "EdwPreprocessData.h"
#include "EdwQualityFlags.h"

/**
   @class EdwAnaDB
   @brief   Class encapsulating the data trees that are necessary to build before running the main analysis

   The EdwAnaDB contains the tree and leaves storing data that are typically obtained after preprocessing of raw data: pulse templates, and noise spectra are the most important ones.

*/
class EdwAnaDB : public EdwDB {
 public:
  EdwAnaDB();
  ~EdwAnaDB();  
  EdwAnaDB(string aFile, string aRead = "READ") ;
  /**< Standard constructor available either in "READ" mode (default) or "RECREATE" */

  void FillFromDBs(vector<string> aDBs); /**< Build a single anadb object from a list of DB files. Useful to concatenate a list of DBs created independently into a single "big" DB that will be used for data processing. */
  void FillFromDBsAndRuns(vector<string> aDBs, vector<string> aRunList); /**< In addition to FillFromDBs, fills only the preprocessdata of aDBs whose run is in aRunList (if the run is defined). */

  void FillFromASCIITemplates(vector<string> aChannels, string aASCIIDir, string aPlotDir="None"); /**< Add templates built from ascii files */
  void FillTemplate(EdwTemplate aTemplate) ; /**< Add a template object to the DB */
  void FillTemplates(vector<EdwTemplate> aTemplates); /**< Add a list of template objects to the DB */
  void FillPreprocess(EdwPreprocessData aPreprocess); /**< Add a preprocess object to the DB */
  void FillPreprocess(vector<EdwPreprocessData> aPreprocess); /**< Add a list of preprocess to the DB */
  EdwTemplate* GetTemplate(time_t aTime, string aChannel); /**< Retrieve a template pointer from the DB, corresponding to a given channel and time. If no template is found for these criteria then the NULL pointer is returned. */
  EdwTemplate GetTemplate(string aChannel) const; /**< Retrieve a template from the DB, corresponding to a given channel */

  EdwPreprocessData* GetPreprocessPtr(time_t aTime, string aChannel);
  /**< Returns the preprocessing data associated to a given time and channel.

  If no preprocessing data is found for these criteria then the NULL pointer is returned.
 */
  EdwPreprocessData GetPreprocess(time_t aTime, string aChannel); /**< Returns the preprocess data corresponding to a given time and channel */

  EdwQualityFlags* GetQuality(time_t aTime, string aChannel);
  /**< Returns the quality "flags" associated to a given channel and time. */
  void BasicFillPreprocessData(TChain* aChain, vector<string> aChannels, vector<Int_t> aBins, vector<Int_t> aPatterns, time_t aStart, time_t aStop, string aPlotSpectrum = "");
  /**< Basic preprocessing of data related to a list of channels in a given time range, using a chain of EdwEvent objects. FFT computation, basic decorrelation... The preprocessing data is then filled to the fPreprocessBranch. */
  //  void BuildPreprocessCorrels(TChain* aChain, time_t aStart, time_t aStop, string aPlotProfiles="");
  //  Subroutine to compute correlations between channels. Used after BasicFillPreprocessData... (to specify)
  void BasicFillQuality(TChain* aChain, vector<string> aChannels, time_t aStart, time_t aStop) ;
  /**< Fills the fQualityBranch by computing essentially baselines from a given chain of events. Corresponding templates and preprocessing data must already exist in the database!! */

  vector<Float_t> ComputeFrequencyWeights(string aChannel) const; 
  /**< Compute the vector of frequency weights using 1) the template T of the input channel, and 2) the noise spectrum N of the *current* fPreprocess object. Returns the vector V^2/N in Fourier space */
  void DumpFrequencyWeights(string aFileName, UInt_t aFreqStart, UInt_t aFreqStop); 
  /**< Computes the average frequency weight in the range [FreqStart,FreqStop] for all channels and noise spectra available */
  void DumpFrequencyWeights(string aFileName, vector<string> aChannelList, UInt_t aFreqStart, UInt_t aFreqStop);
  /**< Computes the average frequency weight in the range [FreqStart,FreqStop] for all noise spectra available for the input list of channels */

  void DumpTemplatesInfo(string aASCIITemplateDir = "None") const ;
  /**< Dump basic info on all templates of the DB. If an ASCII template dir is given, templates are saved to ascii format in this directory */
  void DumpPreprocessInfo() const;
  /**< Dump basic info on all preprocessing data of the DB. */

  void PlotWienerSpectra(string aChannel, string aPlotName) const;
  /**< Plot spectra of template, noise, and opt filter for a given channel */
  void PlotNoiseSpectra(string aChannel, string aPlotName) const;


  // A mettre en protected...
  TTree* fTemplateTree; /**< Template tree */
  TTree* fPreprocessTree; /**< Preprocessing data tree */
  TTree* fQualityTree; /**< "Quality" data and flags tree */
  TBranch* fTemplateBranch; /**< Branch for template data */
  TBranch* fPreprocessBranch; /**< Branch for preprocessing data */
  TBranch* fQualityBranch; /**< Branch for quality data and flags */
  EdwTemplate* fTemplate; /**< Template object filled in fTemplateBranch */
  EdwPreprocessData* fPreprocess; /**< PreprocessData object filled in fPreprocessBranch */
  EdwQualityFlags* fQuality; /**< QualityFlags object filled in fQualitBranch */

 private:

  ClassDef(EdwAnaDB,1)
};

#endif
