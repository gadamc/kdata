{
  //
  /*
   This code was writtine by Benjamin Schmidt at KIT and it was used to calculate rates. It was also
   the script that determined when the Muon Veto system was taking "good" data. The condition for 'good'
   data is that the system was ON, the stamps were valid and there was no more that 5 minutes between
   any two events in the muon veto system.
   This code is left here unaltered from Oct 13, 2010. 
   */
  
  
  gSystem->Load("/kalinka/home/schmidt-b/develSvn/lib/libEdwDS.so");
  TString inEDSFile ("/kalinka/home/edelweiss/Bolo/Run12/Eds/Final/Bckgd/Eds_v1b_AllBolosVeto.root");
  //gSystem->Load("/kalinka/home/schmidt-b/develSvnOld/lib/libEdwDS.so"); //momentarily using old version of the repository
  //String inEDSFile ( "/kalinka/home/edelweiss/Bolo/Run12/Eds/Final/Old/Eds_AllBolosVeto.root");
  cout << "Input file " << inEDSFile.Data() << endl;
  EdwDSReader fEdsFile(inEDSFile.Data());
  EdwHLAEvent *e = dynamic_cast<EdwHLAEvent*>(fEdsFile.GetEvent());
  if(e == 0) return -1;
  
  Long64_t muonStart=1235646943, muonStop=1274769069, boloStart=1238228687, boloStop=1272894205;
  Int_t days=0, boloDays=0;
  Float_t fLiveDays=0, fBoloLiveDays=0;
  Int_t noNeutron=0;
  Long64_t lastMuonTime=0, muonTime=0, muonTimeTemp=0, muonTimeTempTemp=0, firstMuonOn=0, secondMuonOn=0;
  Long64_t lastMuonStamp=0, muonStamp=0, muonStampTemp=0, firstMuonStamp=0;
  Long64_t lastMuonTimeW=0,  firstMuonOnW=0;
  Long64_t lastBoloTime=0, boloTime=0, boloTimeTemp=0, firstBoloOn=0, secondBoloOn=0;
  Long64_t lastBoloTimeW=0, firstBoloOnW=0;
  Long64_t soft2MuonEvents=0;
  Long64_t allMuonEvents=0;
  Long64_t allBoloEvents=0, noNoiseBoloEvents=0, noHeatBoloEvents=0, fiducialBoloEvents=0; // events in the complete lifetime of the bolos 
                                                                                           // number of events in the combined lifetime will only be a rough estimate, due to different lifetimes of different bolos...
  Long64_t first=0, last=0;
  
  Float_t muonDaysOn=0, muonDaysOff=0, boloDaysOn=0, boloDaysOff=0;
  Float_t muonDaysOnW=0, muonDaysOffW=0, boloDaysOnW=0, boloDaysOffW=0;
  const Int_t daySec=3600*24;
  
  //const Long64_t timeOffSet=// Unixtime 1970-1995 in secons;
  //get these start and stop values, if they are not already known and set:
  Int_t entries;
  entries=fEdsFile.GetEntries();
  if(muonStart==0 || boloStart==0 ){
    fEdsFile.GetEntry(0);
    Int_t i=1;
    while(e->GetNumMuonModules()==0){
      fEdsFile.GetEntry(i);
      i++;
    }
    muonStart=e->GetEventTriggerTime();
    cout << "muonStart found"<< endl;
    
    i=0;
    fEdsFile.GetEntry(i);
    while(e->GetNumBolos()==0){
      i++;
      fEdsFile.GetEntry(i);
    }
    boloStart=e->GetEventTriggerTime();
    cout << "boloStart found "<< endl;
    
    i=entries-1;
    fEdsFile.GetEntry(i);
    while(e->GetNumMuonModules()==0){
      i--;
      fEdsFile.GetEntry(i);
    }
    muonStop=e->GetEventTriggerTime();
    cout<< "muonStop found"<< endl; 
    
    i=entries-1;
    fEdsFile.GetEntry(i);
    while(e->GetNumBolos()==0){
      i--;
      fEdsFile.GetEntry(i);
    }
    boloStop=e->GetEventTriggerTime();
    cout<< "boloStop found "<< endl;
    
    
  }
  days=(muonStop-muonStart)/(3600*2); //binning of 2 hours!
  boloDays=(boloStop-boloStart)/(3600*2);
  fLiveDays=(muonStop-muonStart)/Float_t(3600*24);// not the real purpose
  fBoloLiveDays=(boloStop-boloStart)/Float_t(3600*24);
  cout<<"Input values found: muonStart : muonStop : days : boloStart : boloStop : boloDays" << endl;
  cout <<"                 " << muonStart << " : " << muonStop << " : " << fLiveDays << " : "<< boloStart << " : " << boloStop << " : " << fBoloLiveDays << endl;
  cout << "Entries: "<< entries << endl;
  first=TMath::Min(muonStart, boloStart);
  last=TMath::Max(muonStop, boloStop);
  
  Int_t startValuesFound=0;
  Int_t currentEntry=0;
  const Int_t numBolosRun12=13;
  string bolometers[numBolosRun12]={"FID401", "FID402", "ID2", "ID3", "ID4", "ID5", "ID6", "ID401", "ID402", "ID403", "ID404", "ID405", "GGA4"};
  Long64_t singleBolos[numBolosRun12*2];
  Long64_t singleBolosW[numBolosRun12*2];
  Long64_t singleBoloTime[numBolosRun12];
  Long64_t singleBoloTimeTemp[numBolosRun12];
  for(Int_t i=0; i< numBolosRun12; i++){
    singleBolos[i*2]=-99; singleBolos[i*2+1]=-99;
    singleBoloTime[i]=-99; singleBoloTimeTemp[i]=-99;
    singleBolosW[i*2]=-99; singleBolosW[i*2+1]=-99;
  }
  
  //Filling the starting values for all 13 bolometers!
  while(startValuesFound<numBolosRun12){
    fEdsFile.GetNextBoloEntry(currentEntry);
    currentEntry=fEdsFile.GetCurrentEntryNumber();
    for(Int_t i=0; i< e->GetNumBolos() ;i++){
      for(Int_t j=0; j< numBolosRun12; j++){
        if(e->GetBolo(i)->GetDetectorName()==bolometers[j]){
          if(singleBolos[j*2]==-99){ //check if it is not yet initialised
            singleBolos[j*2]=e->GetEventTriggerTime();
            singleBolos[j*2+1]=e->GetEventTriggerTime();
            singleBoloTime[j]=e->GetEventTriggerTime();
            singleBoloTimeTemp[j]=e->GetEventTriggerTime();
            startValuesFound++;
            cout << "Starting value "<< startValuesFound << " :"<< singleBolos[j*2] << endl; 
          }
        }
      }
    }
  }
  
  cout << "Starting values of every single bolometer found! "<< endl;
  //define Lifetime as at least every minute there has to be a muonEvent with cloesd Chariot, laser measurement, but this applies only for Run60 and afterwards I think
  //define the 4 diagramms I want to build:
  Bool_t muonB=false, noise=true, muonOn=false, boloOn=false;
  string outFile="countRates.root";
  TFile fOut(outFile.c_str(), "RECREATE");
  TH1F noCutsMuonRate("noCutsMuonRate", "muon event rate - no cuts; Time; count rate (2 hours average)", days, muonStart, muonStop);
  TH1F timeQualMuonRate("timeQualMuonRate", "muon event rate", days, muonStart, muonStop);
  TH1F closedMuonRate("closedMuonRate", "muon event rate", days, muonStart, muonStop);
  TH1F mod5MuonRate("mod5MuonRate", "Rate of 5modules hit events in the Muon-Veto system; Time; Entries per 2h", days, muonStart, muonStop);
  TH1F totMuonRate("totMuonRate", "Rate of events in the Muon-Veto system; Time; Entries per day", days, muonStart, muonStop);
  TH1F mod2MuonRate("mod2MuonRate", "Rate of 2modules hit events in the Muon-Veto system; Time; Entries per 2h", days, muonStart, muonStop);
  TH1F noNoiseBoloRate("noNoiseBoloRate", "Rate of events in the bolometer system; Time; Entries per 2h", boloDays, boloStart, boloStop);
  TH1F totBoloRate("totBoloRate", "Rate of events in the bolometer system; Time; Entries per 2h", boloDays, boloStart, boloStop);
  TH1F dist("distance", "Dist Est- Dist Nemo", 2000, -10, 10); 
  
  
  
  TTree t("treeOfLife", "Lieftime uVeto -(Bolo)");
  t.Branch("muonStart", &firstMuonOnW, "muonStart/L" );
  t.Branch("muonStop", &lastMuonTimeW, "muonStop/L");
  t.Branch("muonDaysOn", &muonDaysOnW, "muonDaysOn/F");
  t.Branch("muonDaysOff", &muonDaysOffW, "muonDaysOff/F");
  t.Branch("boloStart", &firstBoloOnW, "boloStart/L");
  t.Branch("boloStop", &lastBoloTimeW, "boloStop/L");
  t.Branch("boloDaysOn", &boloDaysOnW, "boloDaysOn/F");
  t.Branch("boloDaysOff", &boloDaysOffW, "boloDaysOff/F");
  t.Branch("singleBolos", &singleBolosW, "singleBolo[26]/L"); // this has to be an array
  
  
  
  for(Int_t j=0; j< entries; j++){
    //common information isNeutron?
    if(j%100000==0)cout <<"Iteration j: "<< j << endl;
    fEdsFile.GetEntry(j);
    muonB=false;
    noise=true;
    noNeutron=0;
    for(Int_t k = 0; k < e->GetNumMuonModules(); k++){
      EdwHLAMuonModuleSubRecord* muon = e->GetMuonModule(k);
      if(!(muon->IsNeutronCounter())){ //implementation in muonVetoSubRecordBase: if events contains neutrounCounterData this returns true! 
        noNeutron++;
      }
    }
    
    
    
    // finding the lifetime of the uVeto!!! alive= closed, taking data + good stamps + see above no neutron counter, but I keep events wich have neutron counter + muon module
    if( muonB && e->GetNumMuonModules()>0 &&  e->GetMuonVetoSystemRecord()->TestEventQualityBit(0) && !e->GetMuonVetoSystemRecord()->TestEventQualityBit(1) && (((e->GetMuonVetoSystemRecord()->GetDistanceEst())-(e->GetMuonVetoSystemRecord()->GetDistanceNemo())) < 4.0) && (((e->GetMuonVetoSystemRecord()->GetDistanceEst())-(e->GetMuonVetoSystemRecord()->GetDistanceNemo())) > 3.8)  ){ 
      //if(e->GetMuonVetoSystemRecord()->GetRunNumber()==59 && e->GetMuonVetoSystemRecord()->GetFileNumber()==89)muonTime=e->GetEventTriggerTime()+1252571041-1252484641; //nice try but you forgot about the shift in events!
      if(e->GetMuonVetoSystemRecord()->GetRunNumber()==59 && e->GetMuonVetoSystemRecord()->GetFileNumber()==89  || j==12908917)muonTime=e->GetEventTriggerTime()+1252571041-1252484641; //thanks I don't have to care about the last event bechause this is the last File of Run59
      else muonTime=e->GetEventTriggerTime(); 
      muonStamp=e->GetStamp();
      allMuonEvents++;
      if(e->GetNumMuonModules()>1)soft2MuonEvents++;
      if(muonTime==muonStart){
        firstMuonOn=muonTime;
        firstMuonStamp=muonStamp;
      }
      else{
        if(muonStamp > muonStampTemp){ //if there is no restart in stamps rely on the stamp else use the less reliable unix time information at least less reliable for the first runs after run 60 it should be fine also
          if(muonStamp-muonStampTemp < 300.0*1e5 || j==38744288 || j==37119432 || j==37501243 || j==23046115 || j==19113341); //system is on j==badStamp except for the last one
          else{
            lastMuonTime=muonTimeTemp;
            // if(e->GetMuonVetoSystemRecord()->GetRunNumber() <63)lastMuonTime=muonTimeTemp; //is beeing corrected in the uVetoFiles
            //  else lastMuonTime=muonTimeTempTemp;
            muonDaysOn=Float_t(lastMuonTime-firstMuonOn)/(Float_t)daySec;
            muonDaysOff=Float_t(muonTime-lastMuonTime)/(Float_t)daySec;
            if(muonDaysOn==0);//one single good event in a long offline period does not lead to any lifetime
            else{
              muonDaysOnW=muonDaysOn; muonDaysOffW=muonDaysOff; lastMuonTimeW=lastMuonTime; firstMuonOnW=firstMuonOn;
              boloDaysOnW=-99; boloDaysOffW=-99; lastBoloTimeW=-99; firstBoloOnW=-99;
              for(Int_t x=0; x<numBolosRun12*2; x++){
                singleBolosW[x]=-99;
              }
              t.Fill();
              firstMuonOn=muonTime; 
            }
          }
        }
        else{
          if((muonTime-muonTimeTemp) < 300);//if muon event in 5 min system is still on (changed value to 600s=10min probability calculation seems to be totally flawed 600 results in a nice plot, but is it really trustworth 600-> another change back) 
          else{
            lastMuonTime=muonTimeTemp;
            muonDaysOn=Float_t(lastMuonTime-firstMuonOn)/(Float_t)daySec;
            muonDaysOff=Float_t(muonTime-lastMuonTime)/(Float_t)daySec;
            if(muonDaysOn==0);//one single good event in a long offline period does not lead to any lifetime
            else{
              muonDaysOnW=muonDaysOn; muonDaysOffW=muonDaysOff; lastMuonTimeW=lastMuonTime; firstMuonOnW=firstMuonOn;
              boloDaysOnW=-99; boloDaysOffW=-99; lastBoloTimeW=-99; firstBoloOnW=-99;
              for(Int_t x=0; x<numBolosRun12*2; x++){
                singleBolosW[x]=-99;
              }
              t.Fill();
              firstMuonOn=muonTime;
            }
          }
        }
      }
      muonTimeTempTemp=muonTimeTemp;
      muonTimeTemp=muonTime;
      muonStampTemp=muonStamp;
    }//end lifetime uVeto
    
    //begin lifetime boloSystem
    if(e->GetNumBolos()>0 ){ 
      boloTime=e->GetEventTriggerTime(); 
      for(Int_t m=0; m< e->GetNumBolos(); m++){
        allBoloEvents++;//what do I define as an event, what about multiple hits in bolos -> define an event as each hit in a bolometr -> multiple hits = multiple event
        if(e->GetBolo(m)->GetEventFlag()!=0){
          noNoiseBoloEvents++;
          if(e->GetBolo(m)->GetEventFlag()!=1){
            noHeatBoloEvents++;
            if(e->GetBolo(m)->GetEventFlag()==2)fiducialBoloEvents++;
          }
        }
        for(Int_t x=0; x<numBolosRun12; x++){ // handling each bolometer separatly, I still have to do the beginning!
          if(e->GetBolo(m)->GetDetectorName()==bolometers[x]){
            singleBoloTime[x]=e->GetEventTriggerTime();
            if(singleBoloTime[x]-singleBoloTimeTemp[x] < 300){ 
            }
            else {
              muonDaysOnW=-99; muonDaysOffW=-99; lastMuonTimeW=-99; firstMuonOnW=-99;
              boloDaysOnW=-99; boloDaysOffW=-99; lastBoloTimeW=-99; firstBoloOnW=-99;
              for(Int_t y=0; y<(numBolosRun12*2); y++){
                singleBolosW[y]=-99;
              }
              singleBolosW[x*2+1]=singleBoloTimeTemp[x];
              singleBolosW[x*2]=singleBolos[x*2];
              if(singleBolosW[x*2+1]==-99){
                cout << "Error: Writing default value instead of lifetime; \n x(13 bolos) \t singleBolos[x*2]\t m(hit) \t j(event)\n " << x << "\t" << singleBolos[x*2] << "\t" <<m << "\t" << j << endl;
              }
              t.Fill();
              singleBolos[x*2]=singleBoloTime[x];
            }
            singleBoloTimeTemp[x]=singleBoloTime[x];
          }
        }
      }
      if(boloTime==boloStart){
        firstBoloOn=boloTime;
      }
      else{
        
        if((boloTime-boloTimeTemp) < 300);//if muon event in 5 min system is still on
        else{
          lastBoloTime=boloTimeTemp;
          boloDaysOn=Float_t(lastBoloTime-firstBoloOn)/(Float_t)daySec;
          boloDaysOff=Float_t(boloTime-lastBoloTime)/(Float_t)daySec;
          boloDaysOnW=boloDaysOn; boloDaysOffW=boloDaysOff; lastBoloTimeW=lastBoloTime; firstBoloOnW=firstBoloOn;
          muonDaysOnW=-99; muonDaysOffW=-99; lastMuonTimeW=-99; firstMuonOnW=-99;
          for(Int_t y=0; y<(numBolosRun12*2); y++){
            singleBolosW[y]=-99;
          }
          t.Fill();
          firstBoloOn=boloTime;
          if(boloDaysOnW==-99){
            cout << "Error: Writing default value instead of lifetime; \n x(13 bolos) \t singleBolos[x*2]\t m(hit) \t j(event)\n " << x << "\t" << singleBolos[x*2] << "\t" <<m << "\t" << j << endl;
          }
        }
      }
      boloTimeTemp=boloTime;
      
      
    }
    //cout << "handling the last entry" << endl;
    if(j==entries-1){
      firstMuonOnW=firstMuonOn;
      lastMuonTime=muonTime;
      lastMuonTimeW=lastMuonTime;
      muonDaysOn=(lastMuonTime-firstMuonOn)/(Float_t)daySec;
      muonDaysOnW=muonDaysOn;
      //muonDaysOff=(secondMuonOn-lastMuonTime)/(Float_t)daySec;
      muonDaysOffW=-99;
      firstBoloOnW=firstBoloOn;
      lastBoloTime=boloTime;
      lastBoloTimeW=lastBoloTime;
      boloDaysOn=(lastBoloTime-firstBoloOn)/(Float_t)daySec;
      boloDaysOnW=boloDaysOn;
      //boloDaysOff=(secondBoloOn-lastBoloTime)/(Float_t)daySec;
      boloDaysOffW=-99;
      for(Int_t x=0; x < numBolosRun12; x++){
        singleBolosW[x*2]=singleBolos[x*2];
        singleBolosW[x*2+1]=singleBoloTime[x];
      }
      cout << "filling!" << endl;
      t.Fill();
    }
    //end of lifetime finding
    //cout << "lifetime filled!" << endl;
    
    //finding the muon and bolo count rates essential for errors of the lifetime
    if(muonB){
      dist.Fill((e->GetMuonVetoSystemRecord()->GetDistanceEst())-(e->GetMuonVetoSystemRecord()->GetDistanceNemo())); 
      //cout << "DistEst: "<<(e->GetMuonVetoSystemRecord()->GetDistanceEst())<< " : DistNemo: " <<(e->GetMuonVetoSystemRecord()->GetDistanceNemo()) << " num modules " << e->GetNumMuonModules() << " num bolos " << e->GetNumBolos() << endl;
      noCutsMuonRate.Fill(e->GetEventTriggerTime());
      if( (((e->GetMuonVetoSystemRecord()->GetDistanceEst())-(e->GetMuonVetoSystemRecord()->GetDistanceNemo())) < 4.0) && (((e->GetMuonVetoSystemRecord()->GetDistanceEst())-(e->GetMuonVetoSystemRecord()->GetDistanceNemo())) > 3.8) ) {
        closedMuonRate.Fill(e->GetEventTriggerTime());
        if( ((e->GetMuonVetoSystemRecord()->TestEventQualityBit(0)) && (!e->GetMuonVetoSystemRecord()->TestEventQualityBit(1))) || ((e->GetMuonVetoSystemRecord()->TestEventQualityBit(0)) &&e->GetMuonVetoSystemRecord()->GetRunNumber()>61) ){ 
          totMuonRate.Fill(e->GetEventTriggerTime());
          if(noNeutron>1)mod2MuonRate.Fill(e->GetEventTriggerTime());
          if(noNeutron>4)mod5MuonRate.Fill(e->GetEventTriggerTime());
        }
      }
      if(e->GetMuonVetoSystemRecord()->TestEventQualityBit(0) && !e->GetMuonVetoSystemRecord()->TestEventQualityBit(1)){
        timeQualMuonRate.Fill(e->GetEventTriggerTime());
      }
    }
    
    if(e->GetNumBolos()>0){
      totBoloRate->Fill(e->GetEventTriggerTime());
      for(Int_t k=0; k< e->GetNumBolos(); k++){
        if(e->GetBolo(k)->GetEventFlag()!=0){
          noise=false;
        }
      }
      if(!noise)noNoiseBoloRate.Fill(e->GetEventTriggerTime()); //trigger rate rather than event rate because of multiple hit events!
    }
  }
  
  /*TCanvas c1;
   c1.Divide(2,2);
   c1.cd(1);
   totMuonRate.Draw();
   c1.cd(2);
   mod2MuonRate.Draw();
   c1.cd(3);
   totBoloRate.Draw();
   c1.cd(4);
   noNoiseBoloRate.Draw();*/
  
  cout << "Finished working! Burying the result on the hard drive!" << endl;
  TParameter<Long64_t> p1("num2ModuleEvents",soft2MuonEvents);
  TParameter<Long64_t> p2("numAllMuonEvents",allMuonEvents);
  TParameter<Long64_t> p3("firstEventP",first );
  TParameter<Long64_t> p4("lastEventP", last);
  TParameter<Long64_t> p5("firstMuonP", muonStart );
  TParameter<Long64_t> p6("lastMuonP", muonStop);
  TParameter<Long64_t> p7("allBoloEventsP", allBoloEvents);
  TParameter<Long64_t> p8("noNoiseBoloEventsP", noNoiseBoloEvents);
  TParameter<Long64_t> p9("noHeatBoloEventsP", noHeatBoloEvents);
  TParameter<Long64_t> p10("fiducialBoloEventsP", fiducialBoloEvents);
  TParameter<Int_t> p11("numBolosRun12", numBolosRun12);
  
  p1.Write();  p3.Write(); p4.Write(); p5.Write(); p6.Write(); p2.Write(); p7.Write(); p8.Write(); p9.Write(); p10.Write(); p11.Write();
  dist.Write();
  timeQualMuonRate.Write();
  closedMuonRate.Write();
  noCutsMuonRate.Write();
  totMuonRate.Write();
  mod2MuonRate.Write(); mod5MuonRate.Write();
  totBoloRate.Write();
  noNoiseBoloRate.Write();
  t.Write();
  
  fOut.Close();
  
  cout << "In total there are " << allMuonEvents << " and " << soft2MuonEvents << " 2 muon Module events (soft condition) in the lifetime of the uVeto system.";
  cout << "Finished ;-), Output written to "<< outFile.c_str() <<" Have a nice day on the beach ;-)"<< endl;
}
