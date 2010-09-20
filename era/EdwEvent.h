/**
   @file    EdwEvent.h
   @author  Eric Armengaud, armengau@in2p3.fr
   @brief   The standard Edelweiss event, containing all the raw event information from Samba necessary for the analysis
*/

#ifndef _EDWEVENT_H_
#define _EDWEVENT_H_

#include "EdwUtils.h"
#include "EdwPulse.h"
#include "EdwEventHeader.h"

/**
   @class   EdwEvent
   @brief   The standard Edelweiss event, containing all the raw event information from Samba necessary for the analysis

   An EdwEvent is made of two parts: an EdwEventHeader object containing all the global event information, and a TObjArray of EdwPulses, containing the raw traces as recorded by Samba.

*/
class EdwEvent : public TObject {
 public:
  EdwEvent() ;
  ~EdwEvent() ;
  UInt_t NbPulses() const { return fPulses->GetEntries(); } 
  /**< Number of pulses recorded in the event. "Pulses" with no recorded trace as sometimes mentionned in Samba headers are not taken into account here. */
  EdwPulse* Pulse(Int_t num) { return (EdwPulse*)fPulses->At(num); }
  /**< Returns a given pulse from its entry number */
  EdwEventHeader* Header() { return fHeader; } /**< Event header */ 
  EdwPulse* Pulse(string aChannel);
  /**< Returns a pulse from its channel name. If no pulse with such a name is found in the event, the NULL pointer is returned. */
  void PlotTraces(string aPlotName, Bool_t aPerBolo=0);
  /**< Plots all raw traces */
  virtual void Dump(string aFile="None") ; /**< Prints header and all traces to standard output */
  
  void AddPulse(EdwPulse* aPulse) { fPulses->Add(aPulse); } 
  /**< Adds an EdwPulse in the TObjArray structure */
  void Clear() ; /**< Resets the event */

 private:
  EdwEventHeader* fHeader ; /**< Event header */
  TObjArray* fPulses ; /**< Array of pulses (each event may have an arbitrary number of pulses) */

  ClassDef(EdwEvent,1)
};

vector<time_t> GetMinMaxTimes(TChain* aEvtChain, Bool_t aFast = 1) ; 
/**< get t_min and t_max from a chain of EdwEvent objects. Fast = take 1st and last evt (consider they are time-ordered!) */

ULong_t GetFirstEventAfter(TChain* aEvtChain, time_t aT0) ; 
/**< Get the first event after a given time in a TChain. The routine uses a dichotomy and therefore assumes that the events in the chain are time-ordered. If all events are before the time aT0, return N=nb of entries in the chain */
ULong_t GetLastEventBefore(TChain* aEvtChain, time_t aT0) ; 
/**< Get the last event before a given time in a TChain. The events in the chain must be time-ordered. */

#endif
