
// Global variables used to access easily to calibration parameters, etc...
// Avoid to expand this list too much!

// NB: Normalement il semble que ces declarations suffisent a initialiser ces
// tableaux avec des valeurs = 0.

#define MAX_NBVOLTAGES 10 // soit 5 polars en +/-

Float_t gNbVoltages = 1;

Float_t gVref_Col1[MAX_NBVOLTAGES];
Float_t gVref_Col2[MAX_NBVOLTAGES];
Float_t gVref_Vet1[MAX_NBVOLTAGES];
Float_t gVref_Vet2[MAX_NBVOLTAGES];
Float_t gVref_Gar1[MAX_NBVOLTAGES];
Float_t gVref_Gar2[MAX_NBVOLTAGES];
Float_t gVolts[MAX_NBVOLTAGES];

Float_t gCalib_Col1[MAX_NBVOLTAGES];
Float_t gCalib_Col2[MAX_NBVOLTAGES];
Float_t gCalib_Vet1[MAX_NBVOLTAGES];
Float_t gCalib_Vet2[MAX_NBVOLTAGES];
Float_t gCalib_Gar1[MAX_NBVOLTAGES];
Float_t gCalib_Gar2[MAX_NBVOLTAGES];

Float_t gCT_V1C1[MAX_NBVOLTAGES];
Float_t gCT_C1V1[MAX_NBVOLTAGES];
Float_t gCT_V2C2[MAX_NBVOLTAGES];
Float_t gCT_C2V2[MAX_NBVOLTAGES];
Float_t gCT_G2G1[MAX_NBVOLTAGES];
Float_t gCT_G1G2[MAX_NBVOLTAGES];
Float_t gCTlin_HG1[MAX_NBVOLTAGES];
Float_t gCTquad_HG1[MAX_NBVOLTAGES];
Float_t gCTlin_HG2[MAX_NBVOLTAGES];
Float_t gCTquad_HG2[MAX_NBVOLTAGES];

Float_t gCT_V1sync[MAX_NBVOLTAGES]; // crosstalk c1->v1 when v1 is synchronized to c1
Float_t gCT_V2sync[MAX_NBVOLTAGES];
Float_t gCT_G1sync[MAX_NBVOLTAGES];
Float_t gCT_G2sync[MAX_NBVOLTAGES];

Float_t gSpecialCT[MAX_NBVOLTAGES];
string gSpecialCTalk="", gSpecialCTalked="";

Float_t gCalib_Heat0[MAX_NBVOLTAGES];
Float_t gCalib_Heat1[MAX_NBVOLTAGES];
Float_t gCalib_Heat2[MAX_NBVOLTAGES];
Float_t gCalib_Heat3[MAX_NBVOLTAGES];

Float_t gKappaIon=0, gKappaHeat=0;

Float_t gChi2Cut0_Heat=0; Float_t gChi2Cut1_Heat=0;
Float_t gChi2Cut0_Col1=0; Float_t gChi2Cut1_Col1=0;
Float_t gChi2Cut0_Col2=0; Float_t gChi2Cut1_Col2=0;
Float_t gChi2Cut0_Vet1=0; Float_t gChi2Cut1_Vet1=0;
Float_t gChi2Cut0_Vet2=0; Float_t gChi2Cut1_Vet2=0;
Float_t gChi2Cut0_Gar1=0; Float_t gChi2Cut1_Gar1=0;
Float_t gChi2Cut0_Gar2=0; Float_t gChi2Cut1_Gar2=0;

Float_t gSigmaCut_Efid=2; // used in dst+qplot+acceptance
Float_t gSigmaCut_Heat=3.5; // used in qplot+acceptance (dst flag=1 is lighter)
Float_t gSigmaCut_Gamma=3.71; // used in qplot+acceptance
// 3.29=99.9; 3.71=99.98 (=99.99 single sided)
Float_t gNRWidth=1.645; // used in qplot+acceptance (+final exposure calculation!)
// 90%=1.645, 95%=1.960
Float_t gLdbCut_Efid=2; // used in qplot+exposure
Float_t gLdbCut_Ecol=4;
Float_t gLdbCut_Evet=1.5; // used in qplot+exposure
// (if gLdbCut_Ecol does not pass then check both Evet)
Float_t gLdbCut_Eheat=2.5;
Float_t gLdbCut_Egar=2.5; // used in qplot+exposure
