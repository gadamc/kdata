//_____________________________________________
//
// KQAdjustPanel.cxx
// KDataStructure
//
// Author: Daniel Wegner <mailto:Daniel.Wegner@student.kit.edu> on 1/13/11.
//
// * Copyright 2011 Karlsruhe Institute of Technology. All rights reserved.

#include "KQAdjustPanel.h"

ClassImp(KQAdjustPanel);

KQAdjustPanel::KQAdjustPanel(const TGWindow *p, UInt_t w, UInt_t h,void (*func)(Int_t,Double_t),void (*func2)(Int_t)) : TGMainFrame(p, w,h), fCorrBound(func), fStoreImage(func2) {
	SetCleanup(kDeepCleanup);
	Connect("CloseWindow()","KQAdjustPanel",this,"DoExit()");
	DontCallClose();

	TGHorizontalFrame * fHL2 = new TGHorizontalFrame(this,70,100);
	fButtonGroup = new TGVButtonGroup(fHL2,"boundaries");
	fBoundaryButtons[0] = new TGRadioButton(fButtonGroup, new TGHotString("Q_n,low"),IDs.GetUnID());
	fBoundaryButtons[1] = new TGRadioButton(fButtonGroup, new TGHotString("Q_n,high"),IDs.GetUnID());
	fBoundaryButtons[2] = new TGRadioButton(fButtonGroup, new TGHotString("Q_g,low"),IDs.GetUnID());
	fBoundaryButtons[3] = new TGRadioButton(fButtonGroup, new TGHotString("Q_g,high"),IDs.GetUnID());
	fButtonGroup->Show();
	fHL2->AddFrame(fButtonGroup, new TGLayoutHints(kLHintsLeft,1,1,1,1));
	AddFrame(fHL2);

	TGHorizontalFrame* fHL3 = new TGHorizontalFrame(this,70,200);
	TGLabel * label = new TGLabel(fHL3,"Step:");
	fHL3->AddFrame(label, new TGLayoutHints(kLHintsLeft, 25,5,5,2));
	fStep = new TGNumberEntry(fHL3,0.01,5,0,
					TGNumberFormat::kNESRealTwo,
					TGNumberFormat::kNEAAnyNumber,
					TGNumberFormat::kNELNoLimits,0.,1.);
	fHL3->AddFrame(fStep,new TGLayoutHints(kLHintsNormal));
	AddFrame(fHL3);
	
	TGHorizontalFrame* fHL4 = new TGHorizontalFrame(this,70,200);
	fAdjust = new TGTextButton(fHL4,"&Adjust",IDs.GetUnID());
	fHL4->AddFrame(fAdjust,new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	fSave = new TGTextButton(fHL4,"&Save",IDs.GetUnID());
	fHL4->AddFrame(fSave, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
	AddFrame(fHL4);
	fAdjust->Connect("Clicked()","KQAdjustPanel",this,"Adjust()");
	fSave->Connect("Clicked()","KQAdjustPanel",this,"Save()");

	fButtonGroup->SetState(true);
	
	SetWindowName("KQAdjustPanel");
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();

	fButtonGroup->SetRadioButtonExclusive(true);
	fBoundaryButtons[0]->SetOn();

};

KQAdjustPanel* adjustpanel;

KQAdjustPanel::~KQAdjustPanel() {
	Cleanup();
}

void KQAdjustPanel::DoExit() {
	gApplication->Terminate();
}

void KQAdjustPanel::SetGroupEnabled(Bool_t on) {
	fButtonGroup->SetState(on);
}

void KQAdjustPanel::Adjust() {
	Int_t b = 1;
	if(fBoundaryButtons[0]->IsOn()) b = 1;
	else
	if(fBoundaryButtons[1]->IsOn()) b = 2;
	else
	if(fBoundaryButtons[2]->IsOn()) b = 3;
	else
	if(fBoundaryButtons[3]->IsOn()) b = 4;
	fCorrBound(b,fStep->GetNumber());
}

void KQAdjustPanel::Save(Int_t anIndex) {
	fStoreImage(anIndex);
}

