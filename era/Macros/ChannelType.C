
string ChannelType(string channel) {
  
  // Tableau:
  // 3 categories de bolos interdigit, les fid, les ID40* et les ID*(+ID201)
  // col1 = g_FIDAB / c_ID40AB / g_IDAB
  // col2 = g_FIDCD / c_ID40CD / g_IDCD
  // vet1 = c_FIDAB / g_ID40AB / c_IDAB
  // vet2 = c_FIDCD / g_ID40AB / g_IDCD
  // gar1 =         / c_ID40GH / c_IDGH
  // gar2 =         / g_ID40GH / g_IDGH
  // heat [ok]
  // ++ Le cas des GeNTD classiques.
  
  Bool_t isheat=0;
  if (channel.find("chaleur") != string::npos) isheat=1;
  string channeltype="";
  if (isheat) channeltype="heat";
  else {
    if (channel.find("ID40") != string::npos && channel.find("FID") == string::npos) {
      if (channel.find("centre") != string::npos) {
        if (channel.find("AB") != string::npos) channeltype = "col1";
        else if (channel.find("CD") != string::npos) channeltype = "col2";
        else channeltype = "gar1";
      } else {
        if (channel.find("AB") != string::npos) channeltype = "vet1";
        else if (channel.find("CD") != string::npos) channeltype = "vet2";
        else channeltype = "gar2";
      }
    } else if (channel.find("ID") != string::npos) { // ID* (incl 201) ou FID
      if (channel.find("centre") != string::npos) {
        if (channel.find("AB") != string::npos) channeltype="vet1";
        else if (channel.find("CD") != string::npos) channeltype="vet2";
        else channeltype="gar1";
      } else {
        if (channel.find("AB") != string::npos) channeltype="col1";
        else if (channel.find("CD") != string::npos) channeltype="col2";
        else channeltype="gar2";
      }
    } else { // Classic GeNTD
      if (channel.find("centre") != string::npos) channeltype="col1";
      else channeltype="gar1";
    }
  }
  
  return channeltype;
}
