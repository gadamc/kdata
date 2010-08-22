
{
  // rootlogon.. inspire de minos

  //set the background color to white
  //  gStyle->SetFillColor(0); // CA CA EMPECHE DE FAIRE DES HISTO 2D couleur
  gStyle->SetFrameFillColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(0);
  
  //dont put a colored frame around the plots
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetLegendBorderSize(0);
  
  //use the primary color palette
  gStyle->SetPalette(1);
  
  //make the axis labels black
  gStyle->SetLabelColor(kBlack,"xyz");
  
  //set the default title color to be black
  gStyle->SetTitleColor(kBlack);
  
  //set axis label and title text sizes
  gStyle->SetLabelFont(72,"xyz");
  gStyle->SetTitleFont(72,"xyz");
  gStyle->SetStatFont(72);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetStatBorderSize(0);
  gStyle->SetTextFont(72);
  
  //set line widths
  gStyle->SetFrameLineWidth(2);
  gStyle->SetLineWidth(2); 

  // time axis
  gStyle->SetTimeOffset(0);

  // Macros pour analyse edw
  TTree pokmv; // Necessaire pour une raison inconnue...
  gROOT->LoadMacro("GlobalVariables.C");
  gROOT->LoadMacro("BuildPeriodList.C");
  gROOT->LoadMacro("BuildTimestamp.C");
  gROOT->LoadMacro("IonCrossTalk.C");
  gROOT->LoadMacro("PulseBin.C");
  gROOT->LoadMacro("HeatVsIon.C");
  gROOT->LoadMacro("HeatIonEvol.C");
  gROOT->LoadMacro("Chi2Distri.C");
  gROOT->LoadMacro("BaselineEvol.C");
  gROOT->LoadMacro("Qplot.C");
  gROOT->LoadMacro("FillRunList.C");
  gROOT->LoadMacro("Collectrodes.C");
  gROOT->LoadMacro("BuildIonEnergy.C");
  gROOT->LoadMacro("CutIonSign.C");
  gROOT->LoadMacro("BuildHeatEnergy.C");
  gROOT->LoadMacro("BuildCutFile.C");
  gROOT->LoadMacro("CutVetos.C");
  gROOT->LoadMacro("PlotIonBaselines.C");
  gROOT->LoadMacro("PlotHeatBaseline.C");
  gROOT->LoadMacro("BuildBoloRunList.C");
  gROOT->LoadMacro("ExposureEvol.C");
  gROOT->LoadMacro("AcceptanceFunction.C");
  gROOT->LoadMacro("DeltaPlot.C");
  gROOT->LoadMacro("GetSigma356keV.C");
  gROOT->LoadMacro("CosmogenicLine.C");
  gROOT->LoadMacro("NoiseThreshold.C");
  gROOT->LoadMacro("DumpEvent.C");
  gROOT->LoadMacro("QplotGlobal.C");
  gROOT->LoadMacro("AcceptGlobal.C");
  gROOT->LoadMacro("EventClass.C");
  gROOT->LoadMacro("BuildDST.C");
  gROOT->LoadMacro("UpdateDST.C");
  gROOT->LoadMacro("BuildDSTRed.C");

  string ERAPATH="/afs/in2p3.fr/home/a/armengau/EdwRootAna/";
  string DATAPATH="/sps/edelweis/EdwRootAna/";
  string MACROPATH="/afs/in2p3.fr/home/a/armengau/Macros/";

}
