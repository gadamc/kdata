
{

  int modulation = 100;

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


  if (bolo == "ID401") {
    int bolonum=21; // samba num (used for bit-trigger)

    gNbVoltages=4;

    // NB: NOW THE CALIB COEF SIGN IS exactly the one from the plot!
    // on fait plus le micmac _p ou _n...
    gVolts[0]=4;
    gVref_Col1[0]=2; gVref_Col2[0]=-2;   gVref_Vet1[0]=-0.75; gVref_Vet2[0]=0.75;
    gVref_Gar1[0]=0.5; gVref_Gar2[0]=-0.5;
    gCalib_Col1[0]=356./1.93;                gCalib_Col2[0]=gCalib_Col1[0]/0.89;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.18;      gCalib_Vet2[0]=gCalib_Col2[0]/1.297;
    gCalib_Gar1[0]=356/1.845;                gCalib_Gar2[0]=356./1.79;
    gCT_V1C1[0]=-0.0012; gCT_C1V1[0]=-0.0034;gCT_V2C2[0]=-0.0015; gCT_C2V2[0]=-0.0012;
    gCTquad_HG1[0]=0.001; gCTquad_HG2[0]=0.002;
    gCT_V1sync[0]=-0.035; gCT_V2sync[0]=-0.032;
    gCT_G1sync[0]=-0.044; gCT_G2sync[0]=0.005;

    gVolts[1]=4;
    gVref_Col1[1]=-2; gVref_Col2[1]=2;   gVref_Vet1[1]=0.75; gVref_Vet2[1]=-0.75;
    gVref_Gar1[1]=-0.5; gVref_Gar2[1]=0.5;
    gCalib_Col1[1]=-356./1.948;              gCalib_Col2[1]=gCalib_Col1[1]/0.85;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.172;     gCalib_Vet2[1]=gCalib_Col2[1]/1.307;
    gCalib_Gar1[1]=-356/1.84;                gCalib_Gar2[1]=-356./1.78;
    gCT_V1C1[1]=-0.0017; gCT_C1V1[1]=-0.0017;gCT_V2C2[1]=-0.0021; gCT_C2V2[1]=-0.0025;
    gCTlin_HG1[1]=-0.0028; gCTquad_HG2[1]=0.005;
    gCT_V1sync[1]=-0.022; gCT_V2sync[1]=-0.052;
    gCT_G1sync[1]=-0.045; gCT_G2sync[1]=0.010;

    gVolts[2]=8;
    gVref_Col1[2]=4; gVref_Col2[2]=-4;   gVref_Vet1[2]=-1.5; gVref_Vet2[2]=1.5;
    gVref_Gar1[2]=1; gVref_Gar2[2]=-1;
    gCalib_Col1[2]=356./1.956;               gCalib_Col2[2]=gCalib_Col1[2]/0.884;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.178;     gCalib_Vet2[2]=gCalib_Col2[2]/1.295;
    gCalib_Gar1[2]=356/1.88;                 gCalib_Gar2[2]=356./1.825;
    gCT_V1C1[2]=-0.0014; gCT_C1V1[2]=-0.0029;gCT_V2C2[2]=-0.0015; gCT_C2V2[2]=-0.0022;
    gCTquad_HG1[2]=0.006; gCTquad_HG2[2]=0.001;
    gCT_V1sync[2]=-0.028; gCT_V2sync[2]=-0.031;
    gCT_G1sync[2]=-0.043; gCT_G2sync[2]=0.006;

    gVolts[3]=8;
    gVref_Col1[3]=-4; gVref_Col2[3]=4;   gVref_Vet1[3]=1.5; gVref_Vet2[3]=-1.5;
    gVref_Gar1[3]=-1; gVref_Gar2[3]=1;
    gCalib_Col1[3]=-356./1.966;              gCalib_Col2[3]=gCalib_Col1[3]/0.860;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.171;     gCalib_Vet2[3]=gCalib_Col2[3]/1.303;
    gCalib_Gar1[3]=-356/1.87;                gCalib_Gar2[3]=-356./1.806;
    gCT_V1C1[3]=-0.0012; gCT_C1V1[3]=-0.002;gCT_V2C2[3]=-0.0012; gCT_C2V2[3]=-0.0029;
    gCTquad_HG1[3]=0.003; gCTquad_HG2[3]=0.002;
    gCT_V1sync[3]=-0.018; gCT_V2sync[3]=-0.043;
    gCT_G1sync[3]=-0.044; gCT_G2sync[3]=0.01;

    gCalib_Heat0[0]=1.428;             gCalib_Heat1[0]=-0.1302; 
    gCalib_Heat2[0]=1.136;             gCalib_Heat3[0]=0.467;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=1.406;             gCalib_Heat1[2]=-0.1252; 
    gCalib_Heat2[2]=1.161;             gCalib_Heat3[2]=0.464;
    gCalib_Heat0[3]=gCalib_Heat0[2];  gCalib_Heat1[3]=gCalib_Heat1[2]; 
    gCalib_Heat2[3]=gCalib_Heat2[2];  gCalib_Heat3[3]=gCalib_Heat3[2]; 

    gChi2Cut0_Col1=1.15; gChi2Cut1_Col1=5.7e-5;
    gChi2Cut0_Col2=1.08; gChi2Cut1_Col2=2.8e-5;
    gChi2Cut0_Heat=1.5;  gChi2Cut1_Heat=1.4e-4;
    gChi2Cut0_Vet1=1.18; gChi2Cut1_Vet1=3.6e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=2.5e-5;
    gChi2Cut0_Gar1=1.6;  gChi2Cut1_Gar1=1.6e-5;
    gChi2Cut0_Gar2=1.2;  gChi2Cut1_Gar2=3.0e-5;

    gKappaIon=0.0007;    gKappaHeat=0.0008;

    //    if (polar == "4V") {
    Float_t cutbaselines[7]={2.3,1,1.5,1,1.5,1.8,1.2};
    //    } else if (polar == "8V") {
    //      Float_t cutbaselines[7]={1.4,1,1.5,1,1.5,1.8,1.2};
    //    }

  } else if (bolo == "ID3") {
    int bolonum=24;

    gNbVoltages=3;
    
    gVolts[0]=4;
    gVref_Col1[0]=2; gVref_Col2[0]=-2;   gVref_Vet1[0]=-0.75; gVref_Vet2[0]=0.75;
    gVref_Gar1[0]=0.5; gVref_Gar2[0]=-0.5;
    gCalib_Col2[0]=356./1.633;               gCalib_Col1[0]=gCalib_Col2[0]/1.152;
    gCalib_Vet1[0]=gCalib_Col1[0]/0.955;     gCalib_Vet2[0]=gCalib_Col2[0]/1.149;
    gCalib_Gar1[0]=356/2.11;                 gCalib_Gar2[0]=356./0.5;
    gCT_V1C1[0]=-0.0013; gCT_C1V1[0]=-0.0026;gCT_V2C2[0]=0; gCT_C2V2[0]=0.0042;
    gCTquad_HG1[0]=0.009;
    gCT_V1sync[0]=-0.032; gCT_V2sync[0]=0; // 0 pour l'instant...
    gCT_G1sync[0]=0.010;  gCT_G2sync[0]=0;

    gVolts[1]=4;
    gVref_Col1[1]=-2; gVref_Col2[1]=2;   gVref_Vet1[1]=0.75; gVref_Vet2[1]=-0.75;
    gVref_Gar1[1]=-0.5; gVref_Gar2[1]=0.5;
    gCalib_Col1[1]=-356./1.94;               gCalib_Col2[1]=gCalib_Col1[1]/0.817;
    gCalib_Vet1[1]=gCalib_Col1[1]/0.949;     gCalib_Vet2[1]=gCalib_Col2[1]/1.165;
    gCalib_Gar1[1]=-356/2.1;                 gCalib_Gar2[1]=-356./0.5;
    gCT_V1C1[1]=0.0005; gCT_C1V1[1]=-0.0012; gCT_V2C2[1]=0.02; gCT_C2V2[1]=0.0007;
    gCTlin_HG1[1]=0.0019;
    gCT_V1sync[1]=-0.015; gCT_V2sync[1]=-0.029;
    gCT_G1sync[1]=0.0;    gCT_G2sync[1]=0;

    gVolts[2]=6.4;
    gVref_Col1[2]=-3.2; gVref_Col2[2]=3.2;   gVref_Vet1[2]=1.2; gVref_Vet2[2]=-1.2;
    gVref_Gar1[2]=-0.8; gVref_Gar2[2]=0.8;
    gCalib_Col1[2]=-356./1.949;              gCalib_Col2[2]=gCalib_Col1[2]/0.824;
    gCalib_Vet1[2]=gCalib_Col1[2]/0.948;     gCalib_Vet2[2]=gCalib_Col2[2]/1.163;
    gCalib_Gar1[2]=-356/2.11;                gCalib_Gar2[2]=-356./0.5;
    gCT_V1C1[2]=-0.0006; gCT_C1V1[2]=-0.0015; gCT_V2C2[2]=0.023; gCT_C2V2[2]=-0.0014;
    gCTlin_HG1[2]=0.0013;
    gCT_V1sync[2]=-0.015; gCT_V2sync[2]=-0.028;
    gCT_G1sync[2]=0.0;    gCT_G2sync[2]=0;

    gCalib_Heat0[0]=1.285;             gCalib_Heat1[0]=-9.51e-2;
    gCalib_Heat2[0]=1.057;             gCalib_Heat3[0]=0.557;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0]; 
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0]; 
    gCalib_Heat0[2]=1.286;             gCalib_Heat1[2]=-9.55e-2;
    gCalib_Heat2[2]=1.062;             gCalib_Heat3[2]=0.381;

    gChi2Cut0_Col1=1.12; gChi2Cut1_Col1=4.e-5;
    gChi2Cut0_Col2=1.1;  gChi2Cut1_Col2=4.e-5;
    gChi2Cut0_Heat=1.3;  gChi2Cut1_Heat=4.e-5;
    gChi2Cut0_Vet1=1.13; gChi2Cut1_Vet1=4.e-5;
    gChi2Cut0_Vet2=1.1;  gChi2Cut1_Vet2=4.5e-5;
    gChi2Cut0_Gar1=1.5;  gChi2Cut1_Gar1=3.e-5;
    gChi2Cut0_Gar2=2.0;  gChi2Cut1_Gar2=0;

    gKappaIon=0.0007;    gKappaHeat=0.0008;

    //    if (polar == "4V") {
    Float_t cutbaselines[7]={5,1.5,1.5,1.5,2.2,2.1,20000};
    //    } else if (polar == "8V") {
    //      Float_t cutbaselines[7]={3,1.5,1.5,1.5,1.5,1.7,20000};
    //    }

  } else if (bolo == "ID201") {
    int bolonum=27;

    gNbVoltages=4;

    gVolts[0]=4;
    gVref_Col1[0]=2; gVref_Col2[0]=-2;   gVref_Vet1[0]=-0.75; gVref_Vet2[0]=0.75;
    gVref_Gar1[0]=0.5; gVref_Gar2[0]=-0.5;
    gCalib_Col2[0]=356./1.874;           gCalib_Col1[0]=gCalib_Col2[0]/0.970;
    gCalib_Vet1[0]=gCalib_Col1[0]/1.20;  gCalib_Vet2[0]=gCalib_Col2[0]/1.260;
    gCalib_Gar1[0]=356./1.13;            gCalib_Gar2[0]=356./1.66;
    gCT_C1V1[0]=-0.0007; gCT_V2C2[0]=-0.00014; gCT_C2V2[0]=0;
    gCTlin_HG1[0]=0.0029; gCTlin_HG2[0]=-0.0006;
    gCT_V1sync[0]=-0.025; gCT_V2sync[0]=0;
    gCT_G1sync[0]=0.0;    gCT_G2sync[0]=0;

    gVolts[1]=4;
    gVref_Col1[1]=-2; gVref_Col2[1]=2;   gVref_Vet1[1]=0.75; gVref_Vet2[1]=-0.75;
    gVref_Gar1[1]=-0.5; gVref_Gar2[1]=0.5;
    gCalib_Col1[1]=-356./1.884;          gCalib_Col2[1]=gCalib_Col1[1]/0.963;
    gCalib_Vet1[1]=gCalib_Col1[1]/1.20;  gCalib_Vet2[1]=gCalib_Col2[1]/1.275;
    gCalib_Gar2[1]=-356./1.66;           gCalib_Gar1[1]=gCalib_Gar2[1]/0.8;
    gCT_C1V1[1]=-0.0002;  gCT_C2V2[1]=-0.0008;
    gCTlin_HG1[1]=0.0036; gCTlin_HG2[1]=0.0001;
    gCT_V1sync[1]=0;      gCT_V2sync[1]=-0.027;
    gCT_G1sync[1]=0.0;    gCT_G2sync[1]=0;

    gVolts[2]=8;
    gVref_Col1[2]=4; gVref_Col2[2]=-4;   gVref_Vet1[2]=-1.5; gVref_Vet2[2]=1.5;
    gVref_Gar1[2]=1; gVref_Gar2[2]=-1;
    gCalib_Col1[2]=356./1.875;           gCalib_Col2[2]=gCalib_Col1[2]/1.019;
    gCalib_Vet1[2]=gCalib_Col1[2]/1.198; gCalib_Vet2[2]=gCalib_Col2[2]/1.258;
    gCalib_Gar1[2]=356./1.13;            gCalib_Gar2[2]=356./1.69;
    gCT_C1V1[2]=-0.0005;  gCT_C2V2[2]=0; gCT_V1C1[2]=0.017;
    gCTlin_HG1[2]=0.0032; gCTlin_HG2[2]=-0.0003;
    gCT_V1sync[2]=-0.017; gCT_V2sync[2]=-0.0027;
    gCT_G1sync[2]=0.007;  gCT_G2sync[2]=0;

    gVolts[3]=8;
    gVref_Col1[3]=-4; gVref_Col2[3]=4;   gVref_Vet1[3]=1.5; gVref_Vet2[3]=-1.5;
    gVref_Gar1[3]=-1; gVref_Gar2[3]=1;
    gCalib_Col1[3]=-356./1.904;          gCalib_Col2[3]=gCalib_Col1[3]/0.98;
    gCalib_Vet1[3]=gCalib_Col1[3]/1.196; gCalib_Vet2[3]=gCalib_Col2[3]/1.269;
    gCalib_Gar1[3]=-356./1.12;           gCalib_Gar2[3]=-356./1.68;
    gCT_C1V1[3]=0;  gCT_C2V2[3]=-0.00092; gCT_V1C1[3]=0.011;
    gCTlin_HG1[3]=0.0019; gCTlin_HG2[3]=0;
    gCT_V1sync[3]=-0.004; gCT_V2sync[3]=-0.020;
    gCT_G1sync[3]=0;  gCT_G2sync[3]=0.0089;

    gCalib_Heat0[0]=1.21;             gCalib_Heat1[0]=-0.075; 
    gCalib_Heat2[0]=1.07;             gCalib_Heat3[0]=0.55;
    gCalib_Heat0[1]=gCalib_Heat0[0];  gCalib_Heat1[1]=gCalib_Heat1[0];
    gCalib_Heat2[1]=gCalib_Heat2[0];  gCalib_Heat3[1]=gCalib_Heat3[0];
    gCalib_Heat0[2]=1.26;             gCalib_Heat1[2]=-0.088; 
    gCalib_Heat2[2]=1.09;             gCalib_Heat3[2]=0.70;
    gCalib_Heat0[3]=1.24;             gCalib_Heat1[3]=-0.085; 
    gCalib_Heat2[3]=1.10;             gCalib_Heat3[3]=0.70;

    gChi2Cut0_Heat=1.5;      gChi2Cut1_Heat=6.e-5;
    gChi2Cut0_Col1=1.3;      gChi2Cut1_Col1=7.e-5;
    gChi2Cut0_Col2=1.35;     gChi2Cut1_Col2=8.e-5;
    gChi2Cut0_Vet1=1.2;      gChi2Cut1_Vet1=7.e-5;
    gChi2Cut0_Vet2=1.35;     gChi2Cut1_Vet2=4.e-5;
    gChi2Cut0_Gar1=1.5;      gChi2Cut1_Gar1=6.e-6;
    gChi2Cut0_Gar2=1.4;      gChi2Cut1_Gar2=3.5e-5;

    Float_t cutbaselines[7]={3.0,1.5,1.5,2.3,1.5,900.,1.4};

  } else cout << "Error in bolo name" << endl;

}
