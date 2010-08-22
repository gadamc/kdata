
bool DetectTrig(int& bittrigger1, int& bittrigger2, int& num_detecteur) {

  // bittrigger1/2 = "bittrigger" indique dans levt samba
  // num_detecteur = numero samba, entre 0 et 32..

  Bool_t b=0;
  // Assumes the bit trigger is 32-bits!
  // Valable uniquement pour "bittrigger1" (sinon il faudra faire +32 :-))

  /* Fct de conversion complete entier->binaire
  for (int i=31; i>=0; i--) {
    int bit = ((bittrigger1 >> i) & 1);
    if (i== num_detecteur && bit == 1) b=1;
  }
  for (int i=63; i>=32; i--) {
    int iprime=i-32;
    int bit = ((bittrigger2 >> iprime) & 1);
    if (i== num_detecteur && bit == 1) b=1;
  }
  */

  // Optimise :
  //int i=num_detecteur;
  int bit=0;
  if (num_detecteur >= 32) {
    int i=num_detecteur-32;
    bit = ((bittrigger2 >> i) & 1);
  } else {
    bit = ((bittrigger1 >> num_detecteur) & 1);
  }
  if (bit == 1) b=1;

  return b;
}
