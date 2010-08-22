
string CutIonSign(string* runs, Int_t* signs, Int_t nbruns, Int_t signcut) {

  string cutstring="( ";
  //  if (signcut != 1 && signcut != -1) cout << "signcut mauvais" <<endl;
  for (Int_t i=0; i<nbruns; i++) {
    int thesign=signs[i];
    if (thesign == signcut) cutstring += ("RunName == \""+runs[i]+"\" || ");
  }
  if (cutstring != "") cutstring = cutstring.substr(0,cutstring.length()-3); // remove trailing ||
  cutstring += ")";
  return cutstring;
}
