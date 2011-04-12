
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "EventClass.C"

int main(int argc, char* argv[]) {

  int nbvoies = atoi(argv[1]);
  ifstream infile(argv[2],ios::in);
  ofstream outfile(argv[3],ios::out);

  int toffset=0, flag=0; float eion=0; int i_entry;
  int p=0;
  if (nbvoies == 4) {
    float energies[4], ldbs[4]; int bins[4], ionflags[4];
    float esync[2];
    while (infile >> i_entry >> energies[0] >> energies[1] >> energies[2] >> energies[3]
	   >> ldbs[0] >> ldbs[1] >> ldbs[2] >> ldbs[3] >> bins[0] >> bins[1]
	   >> bins[2] >> bins[3] >> esync[0] >> esync[1] ) {
      flag=EventClass(energies,ldbs,bins,esync,ionflags,nbvoies,toffset,eion);
      outfile << i_entry <<" "<<flag<<" "<<toffset<<" "<<eion<<" "<<ionflags[0]<<" "<<ionflags[1]<<" "
	      <<ionflags[2]<<" "<<ionflags[3]<<" 0 0"<<endl;
    }
  } else if (nbvoies == 6) {
    float energies[6], ldbs[6]; int bins[6], ionflags[6];
    float esync[4];
    while (infile >> i_entry >> energies[0] >> energies[1] >> energies[2] >> energies[3] >> energies[4]>>energies[5]
	   >> ldbs[0] >> ldbs[1] >> ldbs[2] >> ldbs[3] >> ldbs[4]>>ldbs[5]
	   >>bins[0] >> bins[1] >> bins[2] >> bins[3]>>bins[4]>>bins[5]
	   >> esync[0] >> esync[1] >> esync[2] >> esync[3]) {
      flag=EventClass(energies,ldbs,bins,esync,ionflags,nbvoies,toffset,eion);
      outfile << i_entry <<" "<<flag<<" "<<toffset<<" "<<eion<<" "<<ionflags[0]<<" "<<ionflags[1]<<" "
	      <<ionflags[2]<<" "<<ionflags[3]<<" "<<ionflags[4]<<" "<<ionflags[5]<<endl;
      //      if (i_entry > p) { cout << i_entry << endl; p+=100000; }
    }
  } else cout <<"error nbvoies evtclass compile"<<endl;

  infile.close();
  outfile.close();

  return 0;
}
