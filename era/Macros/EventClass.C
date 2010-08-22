
#include <cmath>
#include <algorithm>

int EventClass(float* energies, float* ldbs, int* bins, float* esync, int* ionflags, int nbvoies, int& toffset, float& eion) {
  // syntaxe actuelle:
  // (1 et 0 = bruits)
  // 2 bulk
  // 3 pure surface
  // 4 pure garde
  // 5 autre
  // channel order: col1,col2,vet1,vet2,gar1,gar2

  // TO ADDRESS: CHECK ALL POSSIBLE DEAD CHANNELS!!!
  // en particulier pbl pour les bolos a une garde manquante.
  // aussi les ldb=0 qui apparaissent rarement en cas de trop courte periode..

  eion=0; toffset=0;
  for (int i=0; i<nbvoies; i++) ionflags[i]=0;
  // - Cas d'une "mauvaise periode" pour une collectrice:
  if (ldbs[0] == 0 || ldbs[1] == 0) return 1;
  //  if (ldbs[4] == 0 && ldbs[5] == 0) return 0;
  //  if (ldbs[2] == 0 && ldbs[3] == 0) return 0;
  // - Cas d'un evt avec trace manquante (ou NaN):
  if (energies[0]==0 && energies[1]==0 && energies[2]==0 && energies[3]==0) return 1;

  int flag=1;
  if (nbvoies != 4 && nbvoies != 6) cout << "error nbvoies" <<endl;

  // variables de coupure ATTENTION EN DUR
  int deltabin_col=3;
  int deltabin_vet=5;
  float sigmacut_efid=2; // ATTENTION DOIT ETRE = gSigmaCut_IonFid
  float vetopercentcut=5;
  float sigmacut_delta=2;
  float sigmacut_vet=2;
  float atten=2; // cf CutVetos... a modifier surement;

  // Test bulk
  if (fabs(bins[0]-bins[1]) <= deltabin_col) {
    float invdenom=1./(ldbs[0]*ldbs[0]+ldbs[1]*ldbs[1]);
    float ldbfid=ldbs[0]*ldbs[1]*sqrt(invdenom);
    float w1=ldbs[1]*ldbs[1]*invdenom;
    float w2=ldbs[0]*ldbs[0]*invdenom;
    float Efid = w1*energies[0]+w2*energies[1];
    if (Efid > sigmacut_efid*ldbfid) {
      int BinFid=(bins[0]+bins[1])/2;
      float truc=0.01*vetopercentcut*Efid;
      float DeltaE = fabs(energies[0]-energies[1]);
      float Ldb_DeltaE = sqrt(ldbs[1]*ldbs[1]+ldbs[0]*ldbs[0]);
      if (DeltaE < sigmacut_delta*Ldb_DeltaE || DeltaE < truc) {
	// collectrodes are ok, now check other channels
	bool ok=1;
	for (int p=2; p<nbvoies; p++) {
	  if (ldbs[p] != 0) {
	    float ecut = ldbs[p]*sigmacut_vet;
	    // test Esync en plus:
	    if (esync[p-2] > ecut && esync[p-2] > truc) ok=0;
	    if (fabs(BinFid-bins[p]) <= deltabin_vet) ecut /= atten;
	    if (energies[p] > ecut && energies[p] > truc) ok=0;
	  }
	}
	if (ok) {
	  // Evt fiduciel. (tfid,Efid) a deja ete calculee..
	  flag=2;
	  toffset = BinFid-4000; // PARAM EN DUR... (RUN 10?)
	  eion=Efid;
	  ionflags[0]=1; ionflags[1]=1;
	}
      }
    }
  }

  // Test surface haut : il faut que la veto haut soit fonctionnelle
  if (flag <= 1 && ldbs[2] != 0) {
    if (fabs(bins[0]-bins[2]) <= deltabin_col) { // surface haut
      int BinFid = (bins[0]+bins[2])/2;
      if (energies[0] >= sigmacut_efid*ldbs[0] && energies[2] >= sigmacut_efid*ldbs[2]) {
	bool ok=1;
	float truc=0.01*vetopercentcut*max(energies[0],energies[2]);
	for (int p=3; p<nbvoies; p++) { // test pas de signal ailleurs que sur col2..
	  if (ldbs[p] != 0) {
	    float ecut = ldbs[p]*sigmacut_vet;
	    if (fabs(BinFid-bins[p]) <= deltabin_vet) ecut /= atten;
	    if (energies[p] > ecut && energies[p] > truc) ok=0;
	  }
	}
	if (ok) {
	  // Evt surface haut.
	  flag=3;
	  toffset = BinFid-4000; // PARAM EN DUR... (RUN 10?)
	  if (fabs(BinFid-bins[1]) <= deltabin_vet && energies[1] > truc && 
	      energies[1] > ldbs[1]*sigmacut_vet/atten) { // Evt surface 3 electrodes
	    // La on fait encore un truc simple: on teste pas Ec1=Ec2+Ev1...
	    // et pour l'instant on fait une eion pas bien ponderee
	    eion=(energies[0]+energies[1]+energies[2])/2.;
	    ionflags[0]=1; ionflags[1]=1; ionflags[2]=1;
	  } else { // Evt surface 2 electrodes
	    float invdenom=1./(ldbs[0]*ldbs[0]+ldbs[2]*ldbs[2]);
	    float w1=ldbs[2]*ldbs[2]*invdenom;
	    float w2=ldbs[0]*ldbs[0]*invdenom;
	    eion=w1*energies[0]+w2*energies[2];
	    ionflags[0]=1; ionflags[2]=1;
	  }
	}
      }
    }
  }

  // Test surface bas
  if (flag <= 1 && ldbs[3] != 0) {
    if (fabs(bins[1]-bins[3]) <= deltabin_col) { // surface haut
      int BinFid = (bins[1]+bins[3])/2;
      if (energies[1] >= sigmacut_efid*ldbs[1] && energies[3] >= sigmacut_efid*ldbs[3]) {
	bool ok=1;
	float truc=0.01*vetopercentcut*max(energies[1],energies[3]);
	for (int p=2; p<nbvoies; p++) { // test pas de signal ailleurs que sur col2..
	  if (p!=3 && ldbs[p] != 0) {
	    float ecut = ldbs[p]*sigmacut_vet;
	    if (fabs(BinFid-bins[p]) <= deltabin_vet) ecut /= atten;
	    if (energies[p] > ecut && energies[p] > truc) ok=0;
	  }
	}
	if (ok) {
	  // Evt surface haut.
	  flag=3;
	  toffset = BinFid-4000; // PARAM EN DUR... (RUN 10?)
	  if (fabs(BinFid-bins[0]) <= deltabin_vet && energies[0] > truc && 
	      energies[0] > ldbs[0]*sigmacut_vet/atten) { // Evt surface 3 electrodes
	    // La on fait encore un truc simple: on teste pas Ec1=Ec2+Ev1...
	    // et pour l'instant on fait une eion pas bien ponderee
	    eion=(energies[0]+energies[1]+energies[3])/2.;
	    ionflags[0]=1; ionflags[1]=1; ionflags[3]=1;
	  } else { // Evt surface 2 electrodes
	    float invdenom=1./(ldbs[1]*ldbs[1]+ldbs[3]*ldbs[3]);
	    float w1=ldbs[3]*ldbs[3]*invdenom;
	    float w2=ldbs[1]*ldbs[1]*invdenom;
	    eion=w1*energies[1]+w2*energies[3];
	    ionflags[1]=1; ionflags[3]=1;
	  }
	}
      }
    }
  }


  // Test pure garde : structure semblable au test des bulks
  // Il faudrait peut-etre des parameters differents (?)
  if (flag <= 1 && nbvoies == 6 && ldbs[4] != 0 && ldbs[5] != 0) {
    if (fabs(bins[5]-bins[4]) <= deltabin_col) {
      float invdenom=1./(ldbs[5]*ldbs[5]+ldbs[4]*ldbs[4]);
      float ldbfid=ldbs[4]*ldbs[5]*sqrt(invdenom);
      float w1=ldbs[5]*ldbs[5]*invdenom;
      float w2=ldbs[4]*ldbs[4]*invdenom;
      float Efid = w1*energies[4]+w2*energies[5];
      if (Efid > sigmacut_efid*ldbfid) {
	int BinFid=(bins[4]+bins[5])/2;
	float truc=0.01*vetopercentcut*Efid;
	float DeltaE = fabs(energies[4]-energies[5]);
	float Ldb_DeltaE = sqrt(ldbs[4]*ldbs[4]+ldbs[5]*ldbs[5]);
	if (DeltaE < sigmacut_delta*Ldb_DeltaE || DeltaE < truc) {
	  // guards are ok, now check other channels
	  bool ok=1;
	  for (int p=0; p<4; p++) {
	    if (ldbs[p] != 0) {
	      float ecut = ldbs[p]*sigmacut_vet;
	      if (fabs(BinFid-bins[p]) <= deltabin_vet) ecut /= atten;
	      if (energies[p] > ecut && energies[p] > truc) ok=0;
	    }
	  }
	  if (ok) {
	    // Evt pure garde. (tfid,Efid) a deja ete calculee..
	    flag=4;
	    toffset = BinFid-4000; // PARAM EN DUR... (RUN 10?)
	    eion=Efid;
	    ionflags[4]=1; ionflags[5]=1;
	  }
	}
      }
    }
  }
  
  if (flag <= 1) {
    // Last check if at least one channel has signal
    for (int p=0; p<nbvoies; p++) {
      if (energies[p] > 1.5*sigmacut_efid*ldbs[p] && ldbs[p]!=0) { // MALUS 1.5...
	flag=5;
	toffset=bins[p]-4000; // PARAM EN DUR
	eion+=(energies[p]/2.);
	ionflags[p]=1;
	// ALGO A AMELIORER... il faut tester les bins...
      }
    }
  }

  return flag;
}
