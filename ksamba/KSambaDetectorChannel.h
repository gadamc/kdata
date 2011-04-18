//______________________________________________________________________
//
// KSambaDetectorChannel.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KSAMBADETECTORCHANNEL_H__
#define __KSAMBADETECTORCHANNEL_H__

#include "Rtypes.h"
#include "TObject.h"
#include <string>

using namespace std;

class KSambaDetectorChannel : public TObject { 
  
public:
  //Constructors
  KSambaDetectorChannel(void);
  virtual ~KSambaDetectorChannel(void);
  
  const char* GetName(void) const { return fName.c_str();}
  const char* GetState(void) const { return fState.c_str();}
  Int_t GetPosition(void) const { return fPosition;}
  Double_t GetMass(void) const { return fMass;}
  const char* GetMac(void) const { return fMac.c_str();}
  Double_t GetDiviseurD2(void) const { return fDiviseurD2;}
  Double_t GetDiviseurD3(void) const { return fDiviseurD3;}
  Double_t GetPolarCentre(void) const { return fPolarCentre;}
  Double_t GetPolarGarde(void) const { return fPolarGarde;}
  Double_t GetGainCentre(void) const { return fGainCentre;}
  Double_t GetGainGarde(void) const { return fGainGarde;}
  const char* GetPolarFet(void) const { return fPolarFet.c_str();}
  Double_t GetCorrPied(void) const { return fCorrPied;}
  Double_t GetCompModul(void) const { return fCompModul;}
  Double_t GetCorrTrngl(void) const { return fCorrTrngl;}
  Double_t GetAmplModul(void) const { return fAmplModul;}
  Double_t GetGainChaleur(void) const { return fGainChaleur;}

  
  void SetName(const char* aVal) {fName = aVal;}
  void SetState(const char* aVal) {fState = aVal;}
  void SetPosition(Int_t aVal) {fPosition = aVal;}
  void SetMass(Double_t aVal) {fMass = aVal;}
  void SetMac(const char* aVal) {fMac = aVal;}
  void SetDiviseurD2(Double_t aVal) {fDiviseurD2 = aVal;}
  void SetDiviseurD3(Double_t aVal) {fDiviseurD3 = aVal;}
  void SetPolarCentre(Double_t aVal) {fPolarCentre = aVal;}
  void SetPolarGarde(Double_t aVal) {fPolarGarde = aVal;}
  void SetGainCentre(Double_t aVal) {fGainCentre = aVal;}
  void SetGainGarde(Double_t aVal) {fGainGarde = aVal;}
  void SetPolarFet(const char* aVal) {fPolarFet = aVal;}
  void SetCorrPied(Double_t aVal) {fCorrPied = aVal;}
  void SetCompModul(Double_t aVal) {fCompModul = aVal;}
  void SetCorrTrngl(Double_t aVal) {fCorrTrngl = aVal;}
  void SetAmplModul(Double_t aVal) {fAmplModul = aVal;}
  void SetGainChaleur(Double_t aVal) {fGainChaleur = aVal;}

  
private:
  
  //private methods
  void InitializeMembers(void);
  string fName;
  string fState;
  Int_t fPosition;
  Double_t fMass;
  string fMac;
  Double_t fDiviseurD2;
  Double_t fDiviseurD3;
  Double_t fPolarCentre;
  Double_t fPolarGarde;
  Double_t fGainCentre;
  Double_t fGainGarde;
  string fPolarFet;
  Double_t fCorrPied;
  Double_t fCompModul;
  Double_t fCorrTrngl;
  Double_t fAmplModul;
  Double_t fGainChaleur;

  
  ClassDef(KSambaDetectorChannel,1);

};


#endif // __KSAMBADETECTORCHANNEL_H__
