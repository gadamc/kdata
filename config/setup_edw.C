// setup_kdata.C
//
// Macro to define the kdata libraries in the root
// interpreter.
//
// This macro should be called from your .rootlogonRC
// file using the code:
//     
//
// This code should be placed after the load commands for the
// KDATA libraries.
//


{

  //Load KDATA libraries
  char *__kdataroot_env = getenv("KDATA_ROOT");
  char __kdatalib[256];
  sprintf(__kdatalib,"%s/lib/libkdata.so",__qsnoroot_env);
  gSystem->Load(__kdatalib);
  

/* Load additional libraries in the same way

  //Load the QOCA libraries
  sprintf(__qsnolib,"%s/lib/libqoca.so",__qsnoroot_env); 
  gSystem->Load(__qsnolib);

  //Load QPhysics Libraries
  char *__qphysics_env = getenv("QPHYSICS");
  sprintf(__qsnolib,"%s/setup_qphysics.C",__qphysics_env);
  gROOT->Macro(__qsnolib);
*/

  printf("KDATA libraries loaded. \n"); // web reference??

  //Set the current style to save on ink.
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);
  
  TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
  plain->SetCanvasBorderMode(0);
  plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
  plain->SetLabelFont(131,"x");
  plain->SetLabelFont(131,"y");
  plain->SetTextFont(131);
  plain->GetAttDate()->SetTextColor(1);
  gROOT->SetStyle("Plain");

  gStyle->GetAttDate()->SetTextColor(1);	
  gStyle->SetOptDate(1);
  gStyle->SetStatBorderSize(1.0);
  gStyle->SetTitleBorderSize(1.0);
}






