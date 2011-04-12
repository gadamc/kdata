
{
  
  int modulation=200; // used for DST (absolute time offset wrt samba time)

  // Initialisation de tous les gTruc a 0.
  for (int i=0; i<MAX_NBVOLTAGES; i++) {
    gVref_Col1[i]=0;gVref_Col2[i]=0;gVref_Vet1[i]=0;gVref_Vet2[i]=0;
    gVref_Gar1[i]=0;gVref_Gar2[i]=0;gVolts[i]=0;
    gCalib_Col1[i]=0;gCalib_Col2[i]=0;gCalib_Vet1[i]=0;gCalib_Vet2[i]=0;
    gCalib_Gar1[i]=0;gCalib_Gar2[i]=0;gCT_V1C1[i]=0;gCT_C1V1[i]=0;gCT_V2C2[i]=0;
    gCT_C2V2[i]=0;gCT_G2G1[i]=0;gCT_G1G2[i]=0;gCTlin_HG1[i]=0;gCTquad_HG1[i]=0;
    gCTlin_HG2[i]=0;gCTquad_HG2[i]=0;gSpecialCT[i]=0;
    gCalib_Heat0[i]=0;gCalib_Heat1[i]=0;gCalib_Heat2[i]=0;gCalib_Heat3[i]=0;
  }

  if (bolo == "FID401") {
    int bolonum=0;
    string mac="S1";
    modulation=400;

    gNbVoltages=2;

    gVolts[0]=3.2;
    gVref_Col1[0]=-1.6; gVref_Col2[0]=1.6;   gVref_Vet1[0]=0.4; gVref_Vet2[0]=-0.4;
    gCalib_Col1[0]=356./1.189;               gCalib_Col2[0]=gCalib_Col1[0]/0.8285;
    gCalib_Vet1[0]=gCalib_Col1[0]/0.253;     gCalib_Vet2[0]=gCalib_Col2[0]/1.53;
    gCT_V1C1[0]=-0.008; gCT_C1V1[0]=-0.0007; gCT_V2C2[0]=-0.0016; gCT_C2V2[0]=-0.0047;

    gVolts[1]=3.2;
    gVref_Col1[1]=-1.6; gVref_Col2[1]=1.6;   gVref_Vet1[1]=0.6; gVref_Vet2[1]=-0.6;
    gCalib_Col1[1]=356./1.189;               gCalib_Col2[1]=gCalib_Col1[1]/0.819;
    gCalib_Vet1[1]=gCalib_Col1[1]/0.28;     gCalib_Vet2[1]=gCalib_Col2[1]/1.53;
    gCT_C1V1[1]=-0.0004;

    gCalib_Heat0[0]=1.306;            gCalib_Heat1[0]=-9.918e-2; 
    gCalib_Heat2[0]=1.0797;           gCalib_Heat3[0]=0.254;
    gCalib_Heat0[1]=1.306;            gCalib_Heat1[1]=-9.918e-2; // inchange
    gCalib_Heat2[1]=1.0797;           gCalib_Heat3[1]=0.254;

    gChi2Cut0_Col1=1.04; gChi2Cut1_Col1=2.1e-5;
    gChi2Cut0_Col2=1.15; gChi2Cut1_Col2=2.e-5;
    gChi2Cut0_Vet1=1.15; gChi2Cut1_Vet1=1.e-5;
    gChi2Cut0_Vet2=1.2; gChi2Cut1_Vet2=1.7e-5;
    gChi2Cut0_Heat=1.4;  gChi2Cut1_Heat=3.5e-5;
    gKappaIon=0.0011;    gKappaHeat=0.0017;

    //    Float_t cutbaselines[5]={4.5,2.5,2.5,6,3.5}; // DUR
    Float_t cutbaselines[5]={8.,2.5,6,12,7}; // MOU

  } else if (bolo == "ID403") {
    int bolonum=2;
    string mac="S1";

    gNbVoltages=3;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.103;               gCalib_Col2[0]=gCalib_Col1[0]/0.913;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.014;     gCalib_Vet2[0]=gCalib_Col2[0]/1.708;
    gCalib_Gar2[0]=356/1.09;                 gCalib_Gar1[0]=gCalib_Gar2[0]/1.092;
    gCT_V1C1[0]=-0.0023; gCT_C1V1[0]=-0.0026;gCT_V2C2[0]=-0.0022; gCT_C2V2[0]=-0.0105;
    gCT_G2G1[0]=-0.0091; gCT_G1G2[0]=-0.0016;gCTlin_HG1[0]=-0.0023; gCTquad_HG2[0]=0.0025;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=-356./1.03;               gCalib_Col1[1]=gCalib_Col2[1]/1.037;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.03;      gCalib_Vet2[1]=gCalib_Col2[1]/1.69;
    gCalib_Gar2[1]=-356/1.08;                gCalib_Gar1[1]=gCalib_Gar2[1]/1.08;
    gCT_V1C1[1]=-0.0036; gCT_C1V1[1]=-0.0062;gCT_V2C2[1]=-0.0025; gCT_C2V2[1]=-0.006;
    gCT_G1G2[1]=-0.004;

    gVolts[2]=8;
    gVref_Col1[2]=4; gVref_Col2[2]=-4;       gVref_Vet1[2]=-2.5; gVref_Vet2[2]=2.5;
    gVref_Gar1[2]=1; gVref_Gar2[2]=-1;
    gCalib_Col2[2]=-356./1.02;               gCalib_Col1[2]=gCalib_Col2[2]/1.035;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.03;      gCalib_Vet2[2]=gCalib_Col2[2]/1.69;
    gCalib_Gar2[2]=-356/1.08;                gCalib_Gar1[2]=gCalib_Gar2[2]/1.07;
    gCT_V1C1[2]=-0.0032; gCT_C1V1[2]=-0.0029;gCT_V2C2[2]=-0.00013; gCT_C2V2[2]=-0.004;
    gCTlin_HG1[2]=-0.009;

    gCalib_Heat0[0]=1.437;            gCalib_Heat1[0]=-1.303e-1; 
    gCalib_Heat2[0]=1.13;             gCalib_Heat3[0]=0.36;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[1];  gCalib_Heat1[2]=gCalib_Heat1[1]; 
    gCalib_Heat2[2]=gCalib_Heat2[1];  gCalib_Heat3[2]=gCalib_Heat3[1]; 

    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=3.5e-5;
    gChi2Cut0_Col2=1.04; gChi2Cut1_Col2=4.e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=5.e-5;
    gChi2Cut0_Vet2=1.06; gChi2Cut1_Vet2=4.e-5;
    gChi2Cut0_Gar1=1.5;  gChi2Cut1_Gar1=2.e-5;
    gChi2Cut0_Gar2=1.15; gChi2Cut1_Gar2=5.e-5;

    gChi2Cut0_Heat=1.5;  gChi2Cut1_Heat=2e-4;
    gKappaIon=0.00086;   gKappaHeat=0.001;

    //    Float_t cutbaselines[7]={2.2,1.2,1.3,2.5,1.5,6,2}; // DUR
    Float_t cutbaselines[7]={3.,1.5,1.5,2.7,1.5,8,2} ; // MOU

  } else if (bolo == "ID5") {
    int bolonum=5;
    string mac="S1";
    // Voie gar2 morte

    gNbVoltages=3;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.003;               gCalib_Col2[0]=gCalib_Col1[0]/0.9921;
    gCalib_Vet1[0]=gCalib_Col1[0]/0.3052;    gCalib_Vet2[0]=gCalib_Col2[0]/1.0503;
    gCalib_Gar1[0]=356/1.296;
    gCT_V1C1[0]=-0.033; gCT_C1V1[0]=-0.0014; gCT_V2C2[0]=-0.0045; gCT_C2V2[0]=-0.004;
    gCTlin_HG1[0]=0.0009;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;   gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=-356./1.017;          gCalib_Col1[1]=gCalib_Col2[1]/0.970;
    gCalib_Vet1[1]=gCalib_Col1[1]/0.31;  gCalib_Vet2[1]=gCalib_Col2[1]/1.043;
    gCalib_Gar1[1]=-356/1.275;
    gCT_V1C1[1]=-0.022; gCT_C1V1[1]=-0.0025; gCT_V2C2[1]=-0.0055; gCT_C2V2[1]=-0.0031;
    gCTquad_HG1[1]=0.002;
    // sgn des ct hg a checker...

    gVolts[2]=8;
    gVref_Col1[2]=4; gVref_Col2[2]=-4;   gVref_Vet1[2]=-2.5; gVref_Vet2[2]=2.5;
    gVref_Gar1[2]=1; gVref_Gar2[2]=-1;
    gCalib_Col2[2]=-356./1.005;          gCalib_Col1[2]=gCalib_Col2[2]/0.954;
    gCalib_Vet1[2]=gCalib_Col1[2]/0.31;  gCalib_Vet2[2]=gCalib_Col2[2]/1.043;
    gCalib_Gar1[2]=-356/1.275;
    gCT_V1C1[2]=-0.018; gCT_C1V1[2]=-0.0019; gCT_V2C2[2]=-0.0025; gCT_C2V2[2]=-0.0015;
    gCTquad_HG1[2]=0.002;

    gCalib_Heat0[0]=1.253;            gCalib_Heat1[0]=-8.503e-2; 
    gCalib_Heat2[0]=1.054;            gCalib_Heat3[0]=0.584;
    // coef2 change au passage run12bis...
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[1];  gCalib_Heat1[2]=gCalib_Heat1[1]; 
    gCalib_Heat2[2]=gCalib_Heat2[1];  gCalib_Heat3[2]=gCalib_Heat3[1]; 

    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=2.5e-5;
    gChi2Cut0_Col2=1.07; gChi2Cut1_Col2=3.7e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=3.e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=2.8e-5;
    gChi2Cut0_Gar1=1.7;  gChi2Cut1_Gar1=2.2e-5;
    gChi2Cut0_Gar2=0;    gChi2Cut1_Gar2=0;
    gChi2Cut0_Heat=1.5;  gChi2Cut1_Heat=1e-4;
    gKappaIon=0.0008;    gKappaHeat=0.0005;

    //    Float_t cutbaselines[7]={3.,2,1.2,2,1.8,1.3,0}; // DUR
    Float_t cutbaselines[7]={3.,3.0,1.2,3,1.8,1.8,0}; // MOU

  } else if (bolo == "FID402") {
    int bolonum=9;
    string mac="S2";

    gNbVoltages=6;

    // NB: NOW THE CALIB COEF SIGN IS exactly the one from the plot!
    // on fait plus le micmac _p ou _n...

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;   gVref_Vet1[0]=1; gVref_Vet2[0]=-1;
    gCalib_Col1[0]=356./0.976;           gCalib_Col2[0]=gCalib_Col1[0]/1.242;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.071; gCalib_Vet2[0]=gCalib_Col2[0]/1.385;
    gCT_V1C1[0]=0; gCT_C1V1[0]=-0.0103;  gCT_V2C2[0]=0; gCT_C2V2[0]=-0.0031;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;   gVref_Vet1[1]=-1; gVref_Vet2[1]=1;
    // Signe - nouveau dans new du au fait qu'on a une polar "negative" ds l'ancienne convention
    gCalib_Col1[1]=-356./0.963;          gCalib_Col2[1]=gCalib_Col1[1]/1.274;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.073; gCalib_Vet2[1]=gCalib_Col2[1]/1.384;
    gCT_V1C1[1]=0; gCT_C1V1[1]=-0.007;   gCT_V2C2[1]=0; gCT_C2V2[1]=-0.0014;

    gVolts[2]=4;
    gVref_Col1[2]=2; gVref_Col2[2]=-2;   gVref_Vet1[2]=-0.5; gVref_Vet2[2]=0.5;
    // Calib sgn moins : du a signe template
    gCalib_Col2[2]=-356./1.224;          gCalib_Col1[2]=gCalib_Col2[2]/0.779;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.077; gCalib_Vet2[2]=gCalib_Col2[2]/1.381;
    gCT_V1C1[2]=0; gCT_C1V1[2]=-0.0054;  gCT_V2C2[2]=0; gCT_C2V2[2]=0.0011;

    gVolts[3]=8;
    gVref_Col1[3]=4; gVref_Col2[3]=-4;   gVref_Vet1[3]=-2; gVref_Vet2[3]=2;
    gCalib_Col1[3]=-356./0.957;          gCalib_Col2[3]=gCalib_Col1[3]/1.278;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.073; gCalib_Vet2[3]=gCalib_Col2[3]/1.384;
    gCT_V1C1[3]=0; gCT_C1V1[3]=-0.005;   gCT_V2C2[3]=0; gCT_C2V2[3]=0;

    gVolts[4]=8;
    gVref_Col1[4]=-4; gVref_Col2[4]=4;   gVref_Vet1[4]=2; gVref_Vet2[4]=-2;
    gCalib_Col1[4]=356./0.977;           gCalib_Col2[4]=gCalib_Col1[4]/1.236;
    gCalib_Vet1[4]=gCalib_Col1[4]/1.07;  gCalib_Vet2[4]=gCalib_Col2[4]/1.385;
    gCT_V1C1[4]=0; gCT_C1V1[4]=-0.0051;  gCT_V2C2[4]=0; gCT_C2V2[4]=0.0018;

    // Vflag=5 : polar asymetrique!
    gVolts[5]=8;
    gVref_Col1[5]=4.5; gVref_Col2[5]=-3.5; gVref_Vet1[5]=-1.5; gVref_Vet2[5]=0.5;
    gCalib_Col1[5]=-356./0.974;            gCalib_Col2[5]=gCalib_Col1[5]/1.272;
    gCalib_Vet1[5]=gCalib_Col1[5]/1.07;  gCalib_Vet2[5]=gCalib_Col2[5]/1.385;
    gCT_C1V1[5]=-0.0044;

    gCalib_Heat0[0]=1.218;            gCalib_Heat1[0]=-7.557e-2; 
    gCalib_Heat2[0]=1.087;            gCalib_Heat3[0]=0.60;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=1.435;            gCalib_Heat1[2]=-1.30e-1;
    gCalib_Heat2[2]=1.129;            gCalib_Heat3[2]=0.11;
    gCalib_Heat0[3]=1.275;            gCalib_Heat1[3]=-9.18e-2;
    gCalib_Heat2[3]=1.104;            gCalib_Heat3[3]=0.105;
    gCalib_Heat0[4]=1.268;            gCalib_Heat1[4]=-8.97e-2;
    gCalib_Heat2[4]=1.066;            gCalib_Heat3[4]=0.34; 
    gCalib_Heat0[5]=1.253;            gCalib_Heat1[5]=-8.596e-2; 
    gCalib_Heat2[5]=1.14;             gCalib_Heat3[5]=0.3;

    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=2.5e-5;
    gChi2Cut0_Col2=1.07; gChi2Cut1_Col2=2.8e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=3.5e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=2.6e-5;
    gChi2Cut0_Heat=1.2;  gChi2Cut1_Heat=6e-5;
    gKappaIon=0.0008;    gKappaHeat=0.0007;

    //    Float_t cutbaselines[5]={4.,1.4,1.6,1.4,1.7}; // DUR
    Float_t cutbaselines[5]={8.,2.1,1.8,1.8,2}; //  MOU

  } else if (bolo == "ID3") {
    int bolonum=11;
    string mac="S2";

    gNbVoltages=3;

    gVolts[0]=6.4;
    gVref_Col1[0]=-3.2; gVref_Col2[0]=3.2;   gVref_Vet1[0]=1.2; gVref_Vet2[0]=-1.2;
    gVref_Gar1[0]=-0.8; gVref_Gar2[0]=0.8;
    gCalib_Col1[0]=356./2.765;               gCalib_Col2[0]=gCalib_Col1[0]/0.419;
    gCalib_Vet1[0]=gCalib_Col1[0]/0.658;     gCalib_Vet2[0]=gCalib_Col2[0]/1.197;
    gCalib_Gar1[0]=356/1.38;                 gCalib_Gar2[0]=gCalib_Gar1[0]/0.399;
    gCT_V1C1[0]=-0.0045; gCT_C1V1[0]=-0.0014;gCT_V2C2[0]=-0.0023; gCT_C2V2[0]=-0.0028;
    gCT_G1G2[0]=0.0007;gCTquad_HG1[0]=0.0015; gCTquad_HG2[0]=0.0015;

    gVolts[1]=6.4;
    gVref_Col1[1]=3.2; gVref_Col2[1]=-3.2;   gVref_Vet1[1]=-1.2; gVref_Vet2[1]=1.2;
    gVref_Gar1[1]=0.8; gVref_Gar2[1]=-0.8;
    gCalib_Col2[1]=-356./1.192;              gCalib_Col1[1]=gCalib_Col2[1]/2.278;
    gCalib_Vet1[1]=gCalib_Col1[1]/0.665;     gCalib_Vet2[1]=gCalib_Col2[1]/1.185;
    gCalib_Gar1[1]=-356/1.385;               gCalib_Gar2[1]=-356/0.555;
    gCT_V1C1[1]=-0.0035; gCT_C1V1[1]=-0.0023;gCT_V2C2[1]=-0.002; gCT_C2V2[1]=-0.0015;
    gCT_G1G2[1]=0.0004; gCTlin_HG1[1]=0.0024; gCTlin_HG2[1]=0.0001;

    gVolts[2]=6.4;
    gVref_Col1[2]=-3.2; gVref_Col2[2]=3.2;   gVref_Vet1[2]=2; gVref_Vet2[2]=-2;
    gVref_Gar1[2]=-0.8; gVref_Gar2[2]=0.8;
    gCalib_Col1[2]=356./1.539;               gCalib_Col2[2]=gCalib_Col1[2]/0.747;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.184;     gCalib_Vet2[2]=gCalib_Col2[2]/1.20;
    gCalib_Gar1[2]=356/1.385;                gCalib_Gar2[2]=356/0.555;
    gCT_V1C1[2]=-0.0012; gCT_C1V1[2]=-0.0018; gCT_C2V2[2]=-0.0017;
    gCTlin_HG1[2]=0.001; gCTquad_HG2[2]=0.002;

    gCalib_Heat0[0]=1.392;            gCalib_Heat1[0]=-1.18e-1; 
    gCalib_Heat2[0]=1.059;             gCalib_Heat3[0]=0.57;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[0];  gCalib_Heat1[2]=gCalib_Heat1[0]; 
    gCalib_Heat2[2]=gCalib_Heat2[0];  gCalib_Heat3[2]=gCalib_Heat3[0]; 

    gChi2Cut0_Col1=1.07; gChi2Cut1_Col1=7.e-5;
    gChi2Cut0_Col2=1.08; gChi2Cut1_Col2=6.5e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=7.e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=8.e-5;
    gChi2Cut0_Gar1=2.;   gChi2Cut1_Gar1=3.e-5;
    gChi2Cut0_Gar2=1.15; gChi2Cut1_Gar2=6.e-5;
    gChi2Cut0_Heat=1.3;  gChi2Cut1_Heat=5e-4;
    gKappaIon=0.0009;    gKappaHeat=0.0005;

    //    Float_t cutbaselines[7]={1.4,1,2,1.5,1.3,2,1.7}; // DUR
    Float_t cutbaselines[7]={2.5,1.5,2.5,1.5,1.4,3,2.4}; // MOU

  } else if (bolo == "ID401") {
    int bolonum=14;
    string mac="S2";

    gNbVoltages=6;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./0.940;               gCalib_Col2[0]=gCalib_Col1[0]/1.182;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.853;     gCalib_Vet2[0]=gCalib_Col2[0]/2.03;
    gCalib_Gar2[0]=356/1.294;                gCalib_Gar1[0]=356/0.645;
    gCT_V1C1[0]=-0.001; gCT_C1V1[0]=-0.0045; gCT_V2C2[0]=-0.0021; gCT_C2V2[0]=-0.0061;
    gCT_G1G2[0]=0.0006; gCTquad_HG1[0]=0.002; gCTquad_HG2[0]=0.005;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=-356./1.132;              gCalib_Col1[1]=gCalib_Col2[1]/0.827;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.852;     gCalib_Vet2[1]=gCalib_Col2[1]/2.013;
    gCalib_Gar2[1]=-356/1.289;               gCalib_Gar1[1]=gCalib_Gar2[1]/0.495;
    gCT_V1C1[1]=-0.001; gCT_C1V1[1]=-0.0057; gCT_V2C2[1]=-0.0017; gCT_C2V2[1]=-0.0045;
    gCTlin_HG1[1]=-0.0002; gCTquad_HG2[1]=0.002;

    gVolts[2]=6.4;
    gVref_Col1[2]=3.2; gVref_Col2[2]=-3.2;   gVref_Vet1[2]=-1.2; gVref_Vet2[2]=1.2;
    gVref_Gar1[2]=0.8; gVref_Gar2[2]=-0.8;
    gCalib_Col2[2]=-356./1.13;               gCalib_Col1[2]=gCalib_Col2[2]/0.825;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.845;     gCalib_Vet2[2]=gCalib_Col2[2]/2.013;
    gCalib_Gar2[2]=-356/1.30;                gCalib_Gar1[2]=gCalib_Gar2[2]/0.495;
    gCT_V1C1[2]=-0.0006; gCT_C1V1[2]=-0.0042; gCT_V2C2[2]=-0.0011; gCT_C2V2[2]=-0.0041;
    gCTlin_HG1[2]=-0.0008;

    gVolts[3]=6.4;
    gVref_Col1[3]=-3.2; gVref_Col2[3]=3.2;   gVref_Vet1[3]=1.2; gVref_Vet2[3]=-1.2;
    gVref_Gar1[3]=-0.8; gVref_Gar2[3]=0.8;
    gCalib_Col1[3]=356./0.942;               gCalib_Col2[3]=gCalib_Col1[3]/1.176;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.851;     gCalib_Vet2[3]=gCalib_Col2[3]/2.035;
    gCalib_Gar2[3]=356/1.289;               gCalib_Gar1[3]=gCalib_Gar2[3]/0.495;
    gCT_V1C1[3]=-0.001; gCT_C1V1[3]=-0.0057; gCT_V2C2[3]=-0.0017; gCT_C2V2[3]=-0.0045;
    gCTlin_HG1[3]=0.004; gCTquad_HG2[3]=0.002;

    gVolts[4]=6.4;
    gVref_Col1[4]=3.2; gVref_Col2[4]=-3.2;   gVref_Vet1[4]=-2; gVref_Vet2[4]=2;
    gVref_Gar1[4]=0.8; gVref_Gar2[4]=-0.8;
    gCalib_Col2[4]=-356./1.12;               gCalib_Col1[4]=gCalib_Col2[4]/0.817;
    gCalib_Vet1[4]=gCalib_Col1[4]/1.851;     gCalib_Vet2[4]=gCalib_Col2[4]/2.022;
    gCalib_Gar2[4]=-356/1.297;               gCalib_Gar1[4]=gCalib_Gar2[4]/0.492;
    gCT_V1C1[4]=-0.001; gCT_C1V1[4]=-0.0047; gCT_V2C2[4]=-0.0012; gCT_C2V2[4]=-0.0035;
    gCTquad_HG2[4]=0.002;

    gVolts[5]=6.4;
    gVref_Col1[5]=-3.2; gVref_Col2[5]=3.2;   gVref_Vet1[5]=2; gVref_Vet2[5]=-2;
    gVref_Gar1[5]=-0.8; gVref_Gar2[5]=0.8;
    gCalib_Col1[5]=356./0.932;              gCalib_Col2[5]=gCalib_Col1[5]/1.164;
    gCalib_Vet1[5]=gCalib_Col1[5]/1.842;    gCalib_Vet2[5]=gCalib_Col2[5]/2.035;
    gCalib_Gar2[5]=356/1.295;               gCalib_Gar1[5]=gCalib_Gar2[5]/0.498;
    gCT_V1C1[5]=-0.0005; gCT_C1V1[5]=-0.004; gCT_V2C2[5]=-0.0012; gCT_C2V2[5]=-0.0057;
    gCTlin_HG1[5]=0.0035; gCTquad_HG2[5]=0.002;

    gCalib_Heat0[0]=1.386;            gCalib_Heat1[0]=-1.191e-1; 
    gCalib_Heat2[0]=1.153;            gCalib_Heat3[0]=0.442;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[0];  gCalib_Heat1[2]=gCalib_Heat1[0]; 
    gCalib_Heat2[2]=gCalib_Heat2[0];  gCalib_Heat3[2]=gCalib_Heat3[0]; 
    gCalib_Heat0[3]=gCalib_Heat0[0];  gCalib_Heat1[3]=gCalib_Heat1[0]; 
    gCalib_Heat2[3]=gCalib_Heat2[0];  gCalib_Heat3[3]=gCalib_Heat3[0]; 
    gCalib_Heat0[4]=gCalib_Heat0[0];  gCalib_Heat1[4]=gCalib_Heat1[0]; 
    gCalib_Heat2[4]=gCalib_Heat2[0];  gCalib_Heat3[4]=gCalib_Heat3[0]; 
    gCalib_Heat0[5]=gCalib_Heat0[0];  gCalib_Heat1[5]=gCalib_Heat1[0]; 
    gCalib_Heat2[5]=gCalib_Heat2[0];  gCalib_Heat3[5]=gCalib_Heat3[0]; 

    gChi2Cut0_Col1=1.1;  gChi2Cut1_Col1=1.6e-4;
    gChi2Cut0_Col2=1.06; gChi2Cut1_Col2=6.e-5;
    gChi2Cut0_Vet1=1.15; gChi2Cut1_Vet1=4.5e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=6.e-5;
    gChi2Cut0_Gar1=1.4;  gChi2Cut1_Gar1=2.5e-5;
    gChi2Cut0_Gar2=1.2;  gChi2Cut1_Gar2=5.e-5;
    gChi2Cut0_Heat=1.6;  gChi2Cut1_Heat=5.e-4;
    gKappaIon=0.0009;    gKappaHeat=0.0008;

    //    Float_t cutbaselines[7]={1.5,1.3,1.8,1.8,2,5,1.7}; // DUR
    Float_t cutbaselines[7]={2.5,1.5,2,2.5,2.5,9,1.7}; // MOU

  } else if (bolo == "ID6") {
    int bolonum=17;
    string mac="S3";

    gNbVoltages=3;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=-356./0.564;              gCalib_Col2[0]=gCalib_Col1[0]/2.195;
    // signe - ! Pbl signe template... 
    gCalib_Vet1[0]=-gCalib_Col1[0]/2.122;    gCalib_Vet2[0]=-gCalib_Col2[0]/0.997;
    gCalib_Gar2[0]=356/0.984;                gCalib_Gar1[0]=gCalib_Gar2[0]/1.127;
    gCT_C1V1[0]=0.0045; gCT_V2C2[0]=0.003; gCT_C2V2[0]=0.0035;
    gCTquad_HG2[0]=0.0015;
    
    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col1[1]=356./0.554;               gCalib_Col2[1]=gCalib_Col1[1]/2.291;
    // signe - ! Pbl signe template...
    gCalib_Vet1[1]=-gCalib_Col1[1]/2.125;    gCalib_Vet2[1]=-gCalib_Col2[1]/0.987;
    gCalib_Gar2[1]=-356/0.974;               gCalib_Gar1[1]=gCalib_Gar2[1]/1.12;
    gCT_C1V1[1]=0.0060; gCT_V2C2[1]=0.0051; gCT_C2V2[1]=0.0037;
    gCTlin_HG2[1]=0.0003;

    gVolts[2]=8;
    gVref_Col1[2]=-4; gVref_Col2[2]=4;       gVref_Vet1[2]=2.5; gVref_Vet2[2]=-2.5;
    gVref_Gar1[2]=-1; gVref_Gar2[2]=1;
    gCalib_Col1[2]=-356./0.558;               gCalib_Col2[2]=gCalib_Col1[2]/2.174;
    // signe - ! Pbl signe template...
    gCalib_Vet1[2]=-gCalib_Col1[2]/2.1205;    gCalib_Vet2[2]=-gCalib_Col2[2]/0.993;
    gCalib_Gar2[2]=356/0.976;               gCalib_Gar1[2]=gCalib_Gar2[2]/1.12;
    gCT_C1V1[2]=0.004; gCT_V2C2[2]=0.003; gCT_C2V2[2]=0.0036;
    gCTlin_HG2[2]=0.0003;

    //    gCalib_Heat0[0]=1.287;            gCalib_Heat1[0]=-9.537e-2; 
    //    gCalib_Heat2[0]=1.08;             gCalib_Heat3[0]=0.50;
    gCalib_Heat0[0]=1.346;            gCalib_Heat1[0]=-0.112; 
    gCalib_Heat2[0]=1.10;             gCalib_Heat3[0]=0.59;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[0];  gCalib_Heat1[2]=gCalib_Heat1[0]; 
    gCalib_Heat2[2]=gCalib_Heat2[0];  gCalib_Heat3[2]=gCalib_Heat3[0]; 

    gChi2Cut0_Heat=1.3;  gChi2Cut1_Heat=8.e-4;
    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=5.e-5;
    gChi2Cut0_Col2=1.07; gChi2Cut1_Col2=5.e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=5.e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=5.e-5;
    gChi2Cut0_Gar2=1.1;  gChi2Cut1_Gar2=5.5e-5;

    gKappaIon=0.0007;    gKappaHeat=0.0008;

    //    Float_t cutbaselines[7]={1.0,1.4,2.4,1.2,2.3,0,1.3} ; // DUR
    Float_t cutbaselines[7]={2.0,1.4,2.4,1.2,2.4,0,1.3} ; // MOU

  } else if (bolo == "ID404") {
    int bolonum=20;
    string mac="S3";

    gNbVoltages=5;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.0254;              gCalib_Col2[0]=-gCalib_Col1[0]/0.9514;
    gCalib_Vet1[0]=-gCalib_Col1[0]/1.1228;   gCalib_Vet2[0]=-gCalib_Col2[0]/1.4905;
    gCalib_Gar2[0]=-356/0.991;               gCalib_Gar1[0]=356/0.949;
    gCT_V1C1[0]=0.004; gCT_C1V1[0]=0.0058;   gCT_V2C2[0]=0.0023; gCT_C2V2[0]=0.0045;
    gCT_G2G1[0]=0.0011; gCTquad_HG2[0]=-0.001;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col1[1]=-356./1.018;              gCalib_Col2[1]=356/0.9880;
    gCalib_Vet1[1]=-gCalib_Col1[1]/1.1280;   gCalib_Vet2[1]=-gCalib_Col2[1]/1.4835;
    gCalib_Gar2[1]=356/1.009;                gCalib_Gar1[1]=-356/0.964;
    gCT_V1C1[1]=0.0025; gCT_C1V1[1]=0.0064;  gCT_V2C2[1]=0.002; gCT_C2V2[1]=0.0046;
    gCTlin_HG2[1]=0.0013;

    gVolts[2]=6.4;
    gVref_Col1[2]=-3.2; gVref_Col2[2]=3.2;   gVref_Vet1[2]=1.2; gVref_Vet2[2]=-1.2;
    gVref_Gar1[2]=-0.8; gVref_Gar2[2]=0.8;
    gCalib_Col1[2]=356./1.020;               gCalib_Col2[2]=-gCalib_Col1[2]/0.956;
    gCalib_Vet1[2]=-gCalib_Col1[2]/1.127;   gCalib_Vet2[2]=-gCalib_Col2[2]/1.498;
    gCalib_Gar2[2]=-356/1.003;               gCalib_Gar1[2]=356/0.960;
    gCT_V1C1[2]=0.004; gCT_C1V1[2]=0.0053;   gCT_V2C2[2]=0.0023; gCT_C2V2[2]=0.0035;
    gCTquad_HG2[2]=-0.001;

    gVolts[3]=6.4;
    gVref_Col1[3]=3.2; gVref_Col2[3]=-3.2;   gVref_Vet1[3]=-1.2; gVref_Vet2[3]=1.2;
    gVref_Gar1[3]=0.8; gVref_Gar2[3]=-0.8;
    gCalib_Col1[3]=-356./1.017;              gCalib_Col2[3]=-gCalib_Col1[3]/0.982;
    gCalib_Vet1[3]=-gCalib_Col1[3]/1.132;    gCalib_Vet2[3]=-gCalib_Col2[3]/1.493;
    gCalib_Gar2[3]=356/1.011;                gCalib_Gar1[3]=-356/0.965;
    gCT_V1C1[3]=0.0025; gCT_C1V1[3]=0.0064;  gCT_V2C2[3]=0.002; gCT_C2V2[3]=0.004;
    gCTlin_HG2[3]=0.0013;

    gVolts[4]=6.4;
    gVref_Col1[4]=3.2; gVref_Col2[4]=-3.2;   gVref_Vet1[4]=-2; gVref_Vet2[4]=2;
    gVref_Gar1[4]=0.8; gVref_Gar2[4]=-0.8;
    gCalib_Col1[4]=-356./1.005;              gCalib_Col2[4]=-gCalib_Col1[4]/0.98;
    gCalib_Vet1[4]=-gCalib_Col1[4]/1.132;    gCalib_Vet2[4]=-gCalib_Col2[4]/1.485;
    gCalib_Gar2[4]=356/1.005;                gCalib_Gar1[4]=-356/0.961;

    gCT_V1C1[4]=0.0019; gCT_C1V1[4]=0.005;  gCT_V2C2[4]=0.001; gCT_C2V2[4]=0.004;
    gCTlin_HG2[4]=0.0008;

    gCalib_Heat0[0]=1.416;            gCalib_Heat1[0]=-1.259e-1; 
    gCalib_Heat2[0]=1.173;            gCalib_Heat3[0]=0.25;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=1.416;            gCalib_Heat1[2]=-1.261e-1; 
    gCalib_Heat2[2]=1.144;            gCalib_Heat3[2]=0.21;
    gCalib_Heat0[3]=gCalib_Heat0[2];  gCalib_Heat1[3]=gCalib_Heat1[2]; 
    gCalib_Heat2[3]=gCalib_Heat2[2];  gCalib_Heat3[3]=gCalib_Heat3[2]; 
    gCalib_Heat0[4]=gCalib_Heat0[2];  gCalib_Heat1[4]=gCalib_Heat1[2]; 
    gCalib_Heat2[4]=gCalib_Heat2[2];  gCalib_Heat3[4]=gCalib_Heat3[2]; 

    gChi2Cut0_Heat=1.3;  gChi2Cut1_Heat=8.e-4;
    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=5.e-5;
    gChi2Cut0_Col2=1.08; gChi2Cut1_Col2=5.e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=4.5e-5;
    gChi2Cut0_Vet2=1.15; gChi2Cut1_Vet2=5.5e-5;
    gChi2Cut0_Gar1=1.3;  gChi2Cut1_Gar1=2.5e-5;
    gChi2Cut0_Gar2=1.1;  gChi2Cut1_Gar2=2.6e-5;

    gKappaIon=0.0007;    gKappaHeat=0.0008;

    //    Float_t cutbaselines[7]={1.3,1.7,2,2,2,2,1.7}; // DUR
    Float_t cutbaselines[7]={2,2.5,2.5,2,2.5,3,1.7} ; // MOU

  } else if (bolo == "ID4") {
    int bolonum=23;
    string mac="S3";

    gNbVoltages=2;

    gVolts[0]=8;
    gVref_Col1[0]=4; gVref_Col2[0]=-4;       gVref_Vet1[0]=-1.5; gVref_Vet2[0]=1.5;
    gVref_Gar1[0]=1; gVref_Gar2[0]=-1;
    gCalib_Col2[0]=356./0.880;               gCalib_Col1[0]=gCalib_Col2[0]/1.458;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.017;     gCalib_Vet2[0]=356./0.2; // voie morte
    gCalib_Gar1[0]=356/1.003;                gCalib_Gar2[0]=356/1.17;
    gCT_V1C1[0]=-0.0048; gCT_C1V1[0]=-0.0041;gCT_V2C2[0]=0.0022;
    gCTlin_HG1[0]=0.004;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-2.5; gVref_Vet2[1]=2.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=356./0.855;               gCalib_Col1[1]=gCalib_Col2[1]/1.463;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.017;     gCalib_Vet2[1]=356./0.2; // voie morte
    gCalib_Gar1[1]=356/1.003;                gCalib_Gar2[1]=356/1.17;
    gCT_V1C1[1]=-0.0035; gCT_C1V1[1]=-0.0037;
    gCTlin_HG1[1]=0.004;

    gCalib_Heat0[0]=1.246;            gCalib_Heat1[0]=-8.41e-2; 
    gCalib_Heat2[0]=1.0398;           gCalib_Heat3[0]=0.251;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 

    gChi2Cut0_Heat=1.4;  gChi2Cut1_Heat=1.2e-4;
    gChi2Cut0_Col1=1.07; gChi2Cut1_Col1=5.e-5;
    gChi2Cut0_Col2=1.1;  gChi2Cut1_Col2=4.5e-5;
    gChi2Cut0_Vet1=1.1;  gChi2Cut1_Vet1=4.5e-5;
    gChi2Cut0_Gar1=1.6;  gChi2Cut1_Gar1=2.5e-5;

    gKappaIon=0.0007;    gKappaHeat=0.0007;

    gSpecialCTalk="garde_ID404GH";
    gSpecialCTalked="chaleur_ID4AB";
    gSpecialCT[0]=5.2e-3; // pquoi yavait 2 valeurs avant..??
    //    special_ct.push_back(5.2e-3);special_ct.push_back(3.5e-3);
    gSpecialCT[1]=5.2e-3;

    //    Float_t cutbaselines[7]={0.9,1.6,2.2,1.1,1000,2.3,1000}; // DUR
    Float_t cutbaselines[7]={1.8,2.6,2.5,2.3,1000,3.5,1000}; // MOU

  } else if (bolo == "ID402") {
    int bolonum=26;
    string mac="S4";

    gNbVoltages=3;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.020;               gCalib_Col2[0]=gCalib_Col1[0]/1.007;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.133;     gCalib_Vet2[0]=gCalib_Col2[0]/1.238;
    gCalib_Gar1[0]=356/1.1;                  gCalib_Gar2[0]=356/1.266;
    gCT_V1C1[0]=-0.0014; gCT_C1V1[0]=-0.0014;gCT_V2C2[0]=-0.0005; gCT_C2V2[0]=-0.0022;
    gCTquad_HG1[0]=0.002;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=-356./1.055;              gCalib_Col1[1]=gCalib_Col2[1]/0.948;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.139;     gCalib_Vet2[1]=gCalib_Col2[1]/1.23;
    gCalib_Gar1[1]=-356/1.145;               gCalib_Gar2[1]=-356/1.3;
    gCT_V1C1[1]=-0.0014; gCT_C1V1[1]=-0.0009;gCT_C2V2[1]=-0.0012;
    gCTlin_HG1[1]=-0.0002;

    gVolts[2]=8;
    gVref_Col1[2]=-4; gVref_Col2[2]=4;       gVref_Vet1[2]=2.5; gVref_Vet2[2]=-2.5;
    gVref_Gar1[2]=-1; gVref_Gar2[2]=1;
    gCalib_Col2[2]=356./1.005;               gCalib_Col1[2]=gCalib_Col2[2]/1.007;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.131;     gCalib_Vet2[2]=gCalib_Col2[2]/1.239;
    gCalib_Gar1[2]=356/1.077;                gCalib_Gar2[2]=356/1.3;
    gCT_V1C1[2]=-0.0007; gCT_C1V1[2]=-0.0009;gCT_C2V2[2]=-0.0019;
    gCT_G1G2[2]=0.206;

    gCalib_Heat0[0]=1.447;            gCalib_Heat1[0]=-1.328e-1; 
    gCalib_Heat2[0]=1.223;            gCalib_Heat3[0]=0.374;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=gCalib_Heat0[0];  gCalib_Heat1[2]=gCalib_Heat1[0]; 
    gCalib_Heat2[2]=gCalib_Heat2[0];  gCalib_Heat3[2]=gCalib_Heat3[0]; 

    gChi2Cut0_Col1=1.08; gChi2Cut1_Col1=5.e-5;
    gChi2Cut0_Col2=1.1;  gChi2Cut1_Col2=4.e-5;
    gChi2Cut0_Vet1=1.2;  gChi2Cut1_Vet1=6.5e-5;
    gChi2Cut0_Vet2=1.15; gChi2Cut1_Vet2=5.e-5;
    gChi2Cut0_Gar1=1.7;  gChi2Cut1_Gar1=2.e-5;
    gChi2Cut0_Heat=1.4;  gChi2Cut1_Heat=1.5e-4;
    gKappaIon=0.0008;    gKappaHeat=0.0007;

    //    Float_t cutbaselines[7]={1.5,2,2,2,1.6,2.5,0}; // DUR
    Float_t cutbaselines[7]={3,2,2.6,2,1.6,3,0}; // MOU

  } else if (bolo == "ID405") {
    int bolonum=29;
    string mac="S4";

    gNbVoltages=6;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.020;               gCalib_Col2[0]=gCalib_Col1[0]/0.944;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.135;     gCalib_Vet2[0]=gCalib_Col2[0]/1.310;
    gCalib_Gar1[0]=356/1.09;                 gCalib_Gar2[0]=gCalib_Gar1[0]/0.93;
    gCT_V1C1[0]=-0.0055; gCT_C1V1[0]=-0.0036;gCT_V2C2[0]=-0.0034; gCT_C2V2[0]=-0.0086;
    gCTquad_HG1[0]=0.0005; gCTquad_HG2[0]=0.001;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col1[1]=-356./1.003;              gCalib_Col2[1]=gCalib_Col1[1]/0.9916;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.139;     gCalib_Vet2[1]=gCalib_Col2[1]/1.31;
    gCalib_Gar2[1]=-356/1.016;               gCalib_Gar1[1]=gCalib_Gar2[1]/1.071;
    gCT_V1C1[1]=-0.0053; gCT_C1V1[1]=-0.0067;gCT_V2C2[1]=-0.0051; gCT_C2V2[1]=-0.0060;
    gCTlin_HG1[1]=-0.001; gCTlin_HG2[1]=0.0005;

    gVolts[2]=6.4;
    gVref_Col1[2]=3.2; gVref_Col2[2]=-3.2;   gVref_Vet1[2]=-1.2; gVref_Vet2[2]=1.2;
    gVref_Gar1[2]=0.8; gVref_Gar2[2]=-0.8;
    gCalib_Col1[2]=-356./1.003;              gCalib_Col2[2]=gCalib_Col1[2]/1.008;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.142;     gCalib_Vet2[2]=gCalib_Col2[2]/1.29;
    gCalib_Gar2[2]=-356/1.03;                gCalib_Gar1[2]=-356./1.10;
    gCT_V1C1[2]=-0.0042; gCT_C1V1[2]=-0.0069;gCT_V2C2[2]=-0.005; gCT_C2V2[2]=-0.0064;
    gCTlin_HG1[2]=-0.002;

    gVolts[3]=6.4;
    gVref_Col1[3]=-3.2; gVref_Col2[3]=3.2;   gVref_Vet1[3]=1.2; gVref_Vet2[3]=-1.2;
    gVref_Gar1[3]=-0.8; gVref_Gar2[3]=0.8;
    gCalib_Col1[3]=356./1.020;               gCalib_Col2[3]=gCalib_Col1[3]/0.945;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.133;     gCalib_Vet2[3]=gCalib_Col2[3]/1.30;
    gCalib_Gar2[3]=356/1.022;                gCalib_Gar1[3]=356./1.099;
    gCT_V1C1[3]=-0.005; gCT_C1V1[3]=-0.0036; gCT_C2V2[3]=-0.009;
    gCTquad_HG2[3]=0.001;

    gVolts[4]=6.4;
    gVref_Col1[4]=-3.2; gVref_Col2[4]=3.2;   gVref_Vet1[4]=2; gVref_Vet2[4]=-2;
    gVref_Gar1[4]=-0.8; gVref_Gar2[4]=0.8;
    gCalib_Col1[4]=356./1.002;               gCalib_Col2[4]=gCalib_Col1[4]/0.935;
    gCalib_Vet1[4]=gCalib_Col1[4]/1.132;     gCalib_Vet2[4]=gCalib_Col2[4]/1.31;
    gCalib_Gar2[4]=356/1.01;                gCalib_Gar1[4]=356./1.09;
    gCT_V1C1[4]=-0.004; gCT_C1V1[4]=-0.0039; gCT_C2V2[4]=-0.009;
    gCTquad_HG2[4]=0.0013;

    gVolts[5]=6.4;
    gVref_Col1[5]=3.2; gVref_Col2[5]=-3.2;   gVref_Vet1[5]=-2; gVref_Vet2[5]=2;
    gVref_Gar1[5]=0.8; gVref_Gar2[5]=-0.8;
    gCalib_Col2[5]=-356./0.966;              gCalib_Col1[5]=gCalib_Col2[5]/1.00;
    gCalib_Vet1[5]=gCalib_Col1[5]/1.142;     gCalib_Vet2[5]=gCalib_Col2[5]/1.30;
    gCalib_Gar2[5]=-356/1.017;                gCalib_Gar1[5]=-356./1.09;
    gCT_V1C1[5]=-0.0046; gCT_C1V1[5]=-0.0054; gCT_C2V2[5]=-0.0041; gCT_V2C2[5]=-0.0034;
    gCTlin_HG1[5]=0.001;

    gCalib_Heat0[0]=1.393;            gCalib_Heat1[0]=-1.213e-1; 
    gCalib_Heat2[0]=1.16;             gCalib_Heat3[0]=0.45;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=1.410;            gCalib_Heat1[2]=-1.248e-1; 
    gCalib_Heat2[2]=1.14;             gCalib_Heat3[2]=0.24;
    gCalib_Heat0[3]=1.38;            gCalib_Heat1[3]=-1.185e-1; 
    gCalib_Heat2[3]=1.16;             gCalib_Heat3[3]=0.40;
    gCalib_Heat0[4]=gCalib_Heat0[3];  gCalib_Heat1[4]=gCalib_Heat1[3]; 
    gCalib_Heat2[4]=gCalib_Heat2[3];  gCalib_Heat3[4]=gCalib_Heat3[3]; 
    gCalib_Heat0[5]=gCalib_Heat0[3];  gCalib_Heat1[5]=gCalib_Heat1[3]; 
    gCalib_Heat2[5]=gCalib_Heat2[3];  gCalib_Heat3[5]=gCalib_Heat3[3]; 

    gChi2Cut0_Col1=1.1; gChi2Cut1_Col1=6.e-5;
    gChi2Cut0_Col2=1.1; gChi2Cut1_Col2=3.5e-5;
    gChi2Cut0_Vet1=1.15;gChi2Cut1_Vet1=4.e-5;
    gChi2Cut0_Vet2=1.2; gChi2Cut1_Vet2=1.3e-4;
    gChi2Cut0_Gar1=1.4; gChi2Cut1_Gar1=2.5e-5;
    gChi2Cut0_Gar2=1.15;gChi2Cut1_Gar2=4.5e-5;
    gChi2Cut0_Heat=1.5; gChi2Cut1_Heat=2.5e-4;
    gKappaIon=0.001;    gKappaHeat=0.0009;

    //    Float_t cutbaselines[7]={1.0,1.6,2,1.5,1.5,6,2.5}; // DUR
    Float_t cutbaselines[7]={1.4,1.8,7,2,2,6,2.5}; // MOU

  } else if (bolo == "ID2") {
    int bolonum=32;
    string mac="S4";

    gNbVoltages=5;

    gVolts[0]=8;
    gVref_Col1[0]=-4; gVref_Col2[0]=4;       gVref_Vet1[0]=1.5; gVref_Vet2[0]=-1.5;
    gVref_Gar1[0]=-1; gVref_Gar2[0]=1;
    gCalib_Col1[0]=356./1.00;                gCalib_Col2[0]=gCalib_Col1[0]/1.043;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.695;     gCalib_Vet2[0]=gCalib_Col2[0]/1.16;
    gCalib_Gar1[0]=356/1.089;                gCalib_Gar2[0]=gCalib_Gar1[0]/0.923;
    gCT_V1C1[0]=-0.008; gCT_C1V1[0]=-0.0036;
    gCTlin_HG1[0]=-0.001; gCTlin_HG2[0]=0.0005;

    gVolts[1]=8;
    gVref_Col1[1]=4; gVref_Col2[1]=-4;       gVref_Vet1[1]=-1.5; gVref_Vet2[1]=1.5;
    gVref_Gar1[1]=1; gVref_Gar2[1]=-1;
    gCalib_Col2[1]=-356./1.049;              gCalib_Col1[1]=gCalib_Col2[1]/0.931;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.704;     gCalib_Vet2[1]=gCalib_Col2[1]/1.164;
    gCalib_Gar1[1]=-356/1.09;                gCalib_Gar2[1]=gCalib_Gar1[1]/0.94;
    gCT_C1V1[1]=-0.0067;                     gCT_C2V2[1]=-0.0007;
    gCTlin_HG1[1]=-0.001; gCTlin_HG2[1]=-0.0017;

    gVolts[2]=8;
    gVref_Col1[2]=-4; gVref_Col2[2]=4;       gVref_Vet1[2]=1.5; gVref_Vet2[2]=-1.5;
    gVref_Gar1[2]=1; gVref_Gar2[2]=-1;
    gCalib_Col1[2]=356./1.003;               gCalib_Col2[2]=gCalib_Col1[2]/1.051;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.685;     gCalib_Vet2[2]=gCalib_Col2[2]/1.173;
    gCalib_Gar1[2]=-356/1.10;                gCalib_Gar2[2]=gCalib_Gar1[2]/0.935;
    gCT_V1C1[2]=-0.0006; gCT_C1V1[2]=-0.0021;gCT_C2V2[2]=0.0021;
    gCTquad_HG1[2]=0.007;

    gVolts[3]=8;
    gVref_Col1[3]=-4; gVref_Col2[3]=4;       gVref_Vet1[3]=2.5; gVref_Vet2[3]=-2.5;
    gVref_Gar1[3]=1; gVref_Gar2[3]=-1;
    gCalib_Col1[3]=356./0.987;               gCalib_Col2[3]=gCalib_Col1[3]/1.040;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.685;     gCalib_Vet2[3]=gCalib_Col2[3]/1.163;
    gCalib_Gar1[3]=-356/1.09;                gCalib_Gar2[3]=gCalib_Gar1[3]/0.935;
    gCT_V1C1[3]=-0.0006; gCT_C1V1[3]=-0.0023;gCT_C2V2[3]=0.0018; gCT_V2C2[3]=-0.0009;
    gCTquad_HG1[3]=0.007;

    gVolts[4]=8;
    gVref_Col1[4]=4; gVref_Col2[4]=-4;       gVref_Vet1[4]=-1.5; gVref_Vet2[4]=1.5;
    gVref_Gar1[4]=-1; gVref_Gar2[4]=1;
    gCalib_Col2[4]=-356./1.049;              gCalib_Col1[4]=gCalib_Col2[4]/0.931;
    gCalib_Vet1[4]=gCalib_Col1[4]/1.704;     gCalib_Vet2[4]=gCalib_Col2[4]/1.164;
    gCalib_Gar1[4]=-356/1.09;                gCalib_Gar2[4]=gCalib_Gar1[4]/0.94;
    gCT_C1V1[4]=-0.0067;                     gCT_C2V2[4]=-0.0007;
    gCTlin_HG1[4]=-0.001; gCTlin_HG2[4]=-0.0017;

    gCalib_Heat0[0]=1.325;            gCalib_Heat1[0]=-1.0415e-1; 
    gCalib_Heat2[0]=1.068;            gCalib_Heat3[0]=0.6;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    // params [2] en fait tres proches de [0]...
    gCalib_Heat0[2]=1.297;            gCalib_Heat1[2]=-9.818e-2; 
    gCalib_Heat2[2]=1.090;            gCalib_Heat3[2]=0.52;
    gCalib_Heat0[3]=gCalib_Heat0[0];  gCalib_Heat1[3]=gCalib_Heat1[0]; 
    gCalib_Heat2[3]=gCalib_Heat2[0];  gCalib_Heat3[3]=gCalib_Heat3[0]; 
    gCalib_Heat0[4]=gCalib_Heat0[1];  gCalib_Heat1[4]=gCalib_Heat1[1]; 
    gCalib_Heat2[4]=gCalib_Heat2[1];  gCalib_Heat3[4]=gCalib_Heat3[1]; 

    gChi2Cut0_Col1=1.1;    gChi2Cut1_Col1=4.e-5;
    gChi2Cut0_Col2=1.0795; gChi2Cut1_Col2=5.e-5;
    gChi2Cut0_Vet1=1.2;    gChi2Cut1_Vet1=4.5e-5;
    gChi2Cut0_Vet2=1.2;    gChi2Cut1_Vet2=3.e-4;
    gChi2Cut0_Gar1=1.1;    gChi2Cut1_Gar1=6.e-5;
    gChi2Cut0_Gar2=1.5;    gChi2Cut1_Gar2=3.e-5;
    gChi2Cut0_Heat=1.7;    gChi2Cut1_Heat=1.7e-4;
    gKappaIon=0.0006;      gKappaHeat=0.0008;

    //    Float_t cutbaselines[7]={1.5,1.5,1.8,1.8,1.2,1.6,1.5}; // DUR
    Float_t cutbaselines[7]={3,2,2,2,1.4,2,1.5}; // MOU


  } else if (bolo=="GGA4") {

    int bolonum=8;
    string mac="S1";

    gNbVoltages=1;

    gVolts[0]=7;
    gVref_Col1[0]=-7; gVref_Gar1[0]=-7;
    gCalib_Col1[0]=356./1.382;               gCalib_Gar1[0]=-356./1.027;
    gCT_V1C1[0]=-0.024; gCT_C1V1[0]=-0.018;

    gCalib_Heat0[0]=1.102;            gCalib_Heat1[0]=-4.2e-2; 
    gCalib_Heat2[0]=1.07;             gCalib_Heat3[0]=0.27;

    gChi2Cut0_Col1=1.1; gChi2Cut1_Col1=4.e-5;
    gChi2Cut0_Gar1=1.1; gChi2Cut1_Gar1=2.1e-5;
    gChi2Cut0_Heat=1.5;  gChi2Cut1_Heat=2.e-4;
    gKappaIon=0.0011;    gKappaHeat=0.0017;

    //    Float_t cutbaselines[5]={4.5,2.5,2.5,6,3.5}; // DUR
    Float_t cutbaselines[7]={2,3.5,0,0,0,4.5,0}; // MOU
    // NB pr GG on garde le meme ordre des voies ds cutbaselines..

  } else if (bolo == "Gc2") {

    int bolonum=35;
    string mac="S4";

    gNbVoltages=1;
    gVolts[0]=0;
    
    // pas de non-linearites supposees
    gCalib_Heat0[0]=356./1.64;
    gCalib_Heat1[0]=0; gCalib_Heat2[0]=1; gCalib_Heat3[0]=1;

  } else {
    cout << "wrong bolo name" << endl;
  }
 
}
