#include "KAmpKounselor.h"
#include "KBackyardKampSite.h"

int main(int /*argc*/, char* argv[])
{
  //argv[1] is the input raw kdata file
  //argv[2] is the output amp kdata file
  
  KAmpKounselor mKampKounselor;
  
  
  //add KAmpSites HERE!
  KBackyardKampSite *mBackYard = new KBackyardKampSite();
  
  mKampKounselor.AddKAmpSite(mBackYard);
  
  
  
  //Tell the KampKounselor to RunKamp, which analyzes the data according
  //to your KAmpSites.
  int theRet = mKampKounselor.RunKamp(argv[1], argv[2]);
  
  
  
  //be environmentally friendly. clean up the garbage from your kampsite.
  delete mBackYard;
  
  
  return theRet;
}
