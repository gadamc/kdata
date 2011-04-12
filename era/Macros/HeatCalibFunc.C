
Double_t HeatCalibFunc(Double_t* x,Double_t* p) {

  /* Ancien:
  Double_t xx=x[0];
  Double_t f;
  if (p[1] == 0) f = p[0];
  else {
    Double_t logx = TMath::Log10(TMath::Abs(xx));
    Double_t x0=((p[2]/p[0])-1)/p[1];
    if (logx < x0) f=p[2];
    else f = p[0]*(1+p[1]*logx);
    if (p[3] != 0 && TMath::Abs(logx-x0) <= p[3]) {
      // Peut-etre formule a simplifier!
      Float_t zz = logx+p[3];
      f=p[2]*(x0-logx+p[3])+p[0]*(zz-x0)+0.5*p[0]*p[1]*(zz*zz-x0*x0);
      f/=(2*p[3]);
    }
  }
  return f*xx; */

  // Nouveau
  // x est deja un log
  // y=[0](1+[1]x) a haute E; y=[2] a basse E
  // lissage parabolique de demi-largeur [3]
  // Retourne le ratio "E/A" ie la fonction NL du log.
  Double_t xx=x[0];
  Double_t f;
  if (p[1] == 0) f = p[0];
  else {
    Double_t x0=((p[2]/p[0])-1)/p[1];
    if (xx < x0) f=p[2];
    else f = p[0]*(1+p[1]*xx);
    if (p[3] != 0 && TMath::Abs(xx-x0) <= p[3]) {
      Float_t zz = xx+p[3];
      f=p[2]*(x0-xx+p[3])+p[0]*(zz-x0)+0.5*p[0]*p[1]*(zz*zz-x0*x0);
      f/=(2*p[3]);
    }
  }
  return f;
}
