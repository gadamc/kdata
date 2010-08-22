
string BoloName(string channel,string delim="_") {
  string strcut=channel.substr(channel.find_last_of(delim)+1);
  if (channel.find("ID") != string::npos)
    strcut=strcut.substr(0,strcut.length()-2);
  return strcut;
}

Bool_t IsID(string bolo) {
  if (bolo.find("ID") != string::npos) return 1;
  else return 0;
}

Bool_t IsFID(string bolo) {
  if (bolo.find("FID") != string::npos) return 1;
  else return 0;
}

string ChannelName(string bolo, string channeltype, string delim="_") {

  // Tableau:
  // 3 categories de bolos interdigit, les fid, les ID40* et les ID*(+ID201)
  // col1 = g_FIDAB / c_ID40AB / g_IDAB
  // col2 = g_FIDCD / c_ID40CD / g_IDCD
  // vet1 = c_FIDAB / g_ID40AB / c_IDAB
  // vet2 = c_FIDCD / g_ID40AB / g_IDCD
  // gar1 =         / c_ID40GH / c_IDGH
  // gar2 =         / g_ID40GH / g_IDGH
  // heat [chaleurs]
  // ++ Le cas des GeNTD classiques.

  Bool_t isheat=0;
  if (channeltype.find("heat") != string::npos) isheat=1;
  string channelname="";
  if (isheat) {
    if (bolo.find("ID") != string::npos) channelname="chaleur_"+bolo+"AB";
    else channelname="chaleur_"+bolo;
  }
  else {
    if (bolo.find("ID40") != string::npos && bolo.find("FID") == string::npos) { // ID40*
      if (channeltype.find("col1") != string::npos) {channelname="centre_"+bolo+"AB";}
      if (channeltype.find("col2") != string::npos) {channelname="centre_"+bolo+"CD";}
      if (channeltype.find("vet1") != string::npos) {channelname="garde_"+bolo+"AB";}
      if (channeltype.find("vet2") != string::npos) {channelname="garde_"+bolo+"CD";}
      if (channeltype.find("gar1") != string::npos) {channelname="centre_"+bolo+"GH";}
      if (channeltype.find("gar2") != string::npos) {channelname="garde_"+bolo+"GH";}
    }
    else if (bolo.find("ID") != string::npos) { // ID* (incl 201) ou FID
      if (channeltype.find("col1") != string::npos) {channelname="garde_"+bolo+"AB";}
      if (channeltype.find("col2") != string::npos) {channelname="garde_"+bolo+"CD";}
      if (channeltype.find("vet1") != string::npos) {channelname="centre_"+bolo+"AB";}
      if (channeltype.find("vet2") != string::npos) {channelname="centre_"+bolo+"CD";}
      if (channeltype.find("gar1") != string::npos) {channelname="centre_"+bolo+"GH";}
      if (channeltype.find("gar2") != string::npos) {channelname="garde_"+bolo+"GH";}
    }
    else { // Classic GeNTD
      if (channeltype.find("col1") != string::npos) {channelname="centre_"+bolo;}
      if (channeltype.find("gar1") != string::npos) {channelname="garde_"+bolo;}
    }
  }

  // Underscore en option
  if (delim == " " && channelname != "")
    channelname.replace(channelname.find_first_of("_"),1," ");

  // Les FID n'ont pas de garde!
  if (bolo.find("FID") != string::npos && channeltype.find("gar") != string::npos)
    channelname="";

  return channelname;
}
