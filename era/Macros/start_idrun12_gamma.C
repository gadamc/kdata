
{

  int nbntps, nbruns;
  if (mac == "S1") {
    nbntps=342; nbruns=37;
  } else if (mac == "S2") {
    nbntps=291; nbruns=42;
  } else if (mac == "S3") {
    nbntps=286; nbruns=38;
  } else if (mac == "S4") {
    nbntps=327; nbruns=37;
  } else cout << "mac error" << endl;

  TChain* chain = new TChain("EdwRootNtp","calib evts");
  string basedir=DATAPATH+"/ID_Run12/Gamma"+mac+"/";
  string ntpdir=basedir+"/ntps/";
  for (int i=1; i<=nbntps; i++) { std::ostringstream truc; truc<<i; chain->AddFile((ntpdir+"ntp_"+truc.str()+".root").c_str()); }

  string chal=ChannelName(bolo,"heat");
  string col1=ChannelName(bolo,"col1");
  string col2=ChannelName(bolo,"col2");
  string vet1=ChannelName(bolo,"vet1");
  string vet2=ChannelName(bolo,"vet2");
  string gar1=ChannelName(bolo,"gar1");
  string gar2=ChannelName(bolo,"gar2");
  string eionntpfile=basedir+"eion_"+bolo+".root";
  string echalntpfile=basedir+"eheat_"+bolo+".root";
  string cutntpfile=basedir+"cuts_"+bolo+".root";
  string qualdir=basedir+"/periods/";
  string periodfile=qualdir+"period_list.txt";
  string heationfile=qualdir+"/heation_"+bolo+".txt";
  string dstfile=basedir+"dst_"+bolo+".root";
  string dst_ascii=basedir+"dstred_"+bolo+".txt";
  string coinfile=basedir+"coinflag_"+bolo+".root";
  string runlist=qualdir+"runlist.txt";
  string runlist_bolo=qualdir+"runlist_"+bolo+".txt";
  string tstnew_dir="/sps/edelweis/torrento/tstnew/";
  string tstfile=basedir+"tstnew.root";
  string dstredfile=basedir+"dstred_"+bolo+".root";

}
