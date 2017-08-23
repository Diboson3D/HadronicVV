
// Local include(s):
#include "../include/VVanalysis.h"
#include "../include/VVAnalysisTools.h"

// External include(s):
// #include "../GoodRunsLists/include/TGoodRunsListReader.h"


#include <TMath.h>
#include "TGraph.h"
#include <iostream>
#include <fstream>

ClassImp( VVanalysis );

VVanalysis::VVanalysis()
  : SCycleBase()
   , m_jetAK4               ( this )
   , m_jetAK8               ( this )
   , m_jetAK8Puppi          ( this )
   , m_eventInfo            ( this )
   , m_electron             ( this )
   , m_muon                 ( this )
   , m_missingEt            ( this )
   , m_genParticle          ( this )
   , m_pileupReweightingTool( this, "pileupReweighting" )
   , m_bTaggingScaleTool    ( this )
   , m_xSec                 ()
   , m_allEvents( "allEvents", this )
   , m_passedLoose( "passedLoose", this )
   , m_passedPuppi( "passedPuppi", this )
   , m_passedDEta( "passedDeta", this )
   , m_passedMjj( "passedMjj", this )
   , m_passedEvents( "passedEvents", this )
   , m_test( "test", this ) 
   , m_genjetAK8( this )
 {
   m_logger << INFO << "Initializing" << SLogger::endmsg;
   SetLogName( GetName() );
   
   DeclareProperty( "JetAK8Name"      ,         m_jetAK8Name              = "jetAK8"        );
   DeclareProperty( "GenJetAK8Name"   ,         m_genjetAK8Name           = "genJetAK8"     );
   DeclareProperty( "JetAK8PuppiName" ,         m_jetAK8PuppiName         = "jetAK8_puppi"  );
   DeclareProperty( "GenParticleName" ,         m_genParticleName         = "genParticle"   );
   DeclareProperty( "ElectronName"    ,         m_electronName            = "el"            );
   DeclareProperty( "MuonName"        ,         m_muonName                = "mu"            );
   
   DeclareProperty( "RecoTreeName"    ,         m_recoTreeName            = "physics" );
   DeclareProperty( "PUPPIJEC"        ,         m_PUPPIJEC                = "weights/puppiCorr.root" );
   // DeclareProperty( "JSONName"        ,         m_jsonName                = std::string (std::getenv("SFRAME_DIR")) + "/../GoodRunsLists/JSON/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt" );
   DeclareProperty( "IsData"          ,         m_isData                  = false );
   DeclareProperty( "IsSignal"        ,         m_isSignal                = true );
   DeclareProperty( "Channel"         ,         m_Channel                 = "qV" );
   
   nSumGenWeights           = 0;
   
}

VVanalysis::~VVanalysis() {
  
  m_logger << INFO << "Done!" << SLogger::endmsg;

}

void VVanalysis::BeginCycle() throw( SError ) { //Block used to perform an initial configuration of the cycle (ead some local file likegood data ranges)
  
  // // Load the GRL:
  // if (m_isData) {
  //   m_logger << INFO << "Loading GoodRunsList from file: " << m_jsonName << SLogger::endmsg;
  //   Root::TGoodRunsListReader reader( TString( m_jsonName.c_str() ) );
  //   if( ! reader.Interpret() ) {
  //     m_logger << FATAL << "Couldn't read in the GRL!" << SLogger::endmsg;
  //     throw SError( ( "Couldn't read in file: " + m_jsonName ).c_str(), SError::SkipCycle );
  //   }
  //   m_grl = reader.GetMergedGoodRunsList();
  //   m_grl.Summary();
  //   m_grl.SetName( "MyGoodRunsList" );
  //
  //   // Add it as a configuration object, so that the worker nodes will receive it:
  //   AddConfigObject( &m_grl );
  // }

   return;

}

void VVanalysis::EndCycle() throw( SError ) { //Any finalisation steps should be done here. (Closure of some helper files opened by the user code for instance.)

   return;

}

void VVanalysis::BeginInputFile( const SInputData& ) throw( SError ) { //For each new input file the user has to connect his input variables. This has to be performed in this function.
  
  m_logger << INFO << "Connecting input variables" << SLogger::endmsg;
  if (m_isData) {
    
    m_jetAK8Puppi .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::JetBasic|Ntuple::JetSubstructure, (m_jetAK8PuppiName + "_").c_str() );
    m_jetAK8      .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::JetBasic|Ntuple::JetAnalysis|Ntuple::JetSubstructure|Ntuple::JetSoftdropSubjets, (m_jetAK8Name + "_").c_str() );
    m_eventInfo   .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::EventInfoBasic|Ntuple::EventInfoTrigger|Ntuple::EventInfoMETFilters, "" );
    m_electron    .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::ElectronBasic|Ntuple::ElectronID, (m_electronName + "_").c_str() );
    m_muon        .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::MuonBasic|Ntuple::MuonID|Ntuple::MuonIsolation, (m_muonName + "_").c_str() );
  }
  else {
    m_jetAK8Puppi .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::JetBasic|Ntuple::JetSubstructure, (m_jetAK8PuppiName + "_").c_str() );
    m_jetAK8      .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::JetBasic|Ntuple::JetAnalysis|Ntuple::JetSubstructure|Ntuple::JetTruth|Ntuple::JetSoftdropSubjets|Ntuple::JetSoftdropSubjetsTruth, (m_jetAK8Name + "_").c_str() );
    m_eventInfo   .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::EventInfoBasic|Ntuple::EventInfoTrigger|Ntuple::EventInfoMETFilters|Ntuple::EventInfoTruth, "" );
    m_genParticle .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::GenParticleBasic, (m_genParticleName + "_").c_str() );
    m_genjetAK8   .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::GenJet, (m_genjetAK8Name + "_").c_str() );
    m_electron    .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::ElectronBasic|Ntuple::ElectronID|Ntuple::ElectronSuperCluster, (m_electronName + "_").c_str() );
    m_muon        .ConnectVariables(  m_recoTreeName.c_str(), Ntuple::MuonBasic|Ntuple::MuonID|Ntuple::MuonIsolation, (m_muonName + "_").c_str() );
  }
  // Unused collections
  // m_jetAK4.ConnectVariables(       m_recoTreeName.c_str(), Ntuple::JetBasic|Ntuple::JetAnalysis|Ntuple::JetTruth, (m_jetAK4Name + "_").c_str() );
  // m_missingEt.ConnectVariables(    m_recoTreeName.c_str(), Ntuple::MissingEtBasic, (m_missingEtName + "_").c_str() );
  
  m_logger << INFO << "Connecting input variables completed" << SLogger::endmsg;
  
  m_logger << INFO << "Initializing inputs for puppi softdop mass corrections" << SLogger::endmsg; 
  TFile* file = TFile::Open( m_PUPPIJEC.c_str(),"READ");
  if(file->IsZombie()) throw SError(SError::StopExecution);
  m_puppisd_corr.push_back( dynamic_cast<TF1*>(file->Get("puppiJECcorr_gen")));
  m_puppisd_corr.push_back( dynamic_cast<TF1*>(file->Get("puppiJECcorr_reco_0eta1v3")));
  m_puppisd_corr.push_back( dynamic_cast<TF1*>(file->Get("puppiJECcorr_reco_1v3eta2v5")));
  
  // Get cross section
  b_xSec                        = 1.;
  TString infile = TString(this->GetHistInputFile()->GetName());
  b_xSec  = m_xSec.getLumiWeight( infile ); 
  m_logger << INFO << "Cross section set to " << b_xSec << " for file " << infile <<  SLogger::endmsg;
  if( b_xSec < 0.) throw SError( SError::SkipFile );

  
  return;

}


void VVanalysis::BeginInputData( const SInputData& id ) throw( SError ) { //called once before processing each of the input data types. If you need to initialise output objects (histograms, etc.) before the event-by-event execution, you should do that here. Also the declaration of the output variables has to be done here.
  
  if (!m_isData) m_pileupReweightingTool.BeginInputData( id );
  
  // Declare output variables:
  DeclareVariable( b_lumi                         , "lumi"  );
  DeclareVariable( b_event                        , "event"  );
  DeclareVariable( b_run                          , "run"  );
  DeclareVariable( b_weight                       , "weight"  );
  DeclareVariable( b_weightGen                    , "genWeight");
  DeclareVariable( b_weightPU                     , "puWeight");
  DeclareVariable( b_xSec                         , "xsec");
  DeclareVariable( m_o_njj                        , "njj");
  DeclareVariable( m_o_mjj                        , "jj_LV_mass");
  DeclareVariable( m_o_genmjj                     , "jj_gen_partialMass");
  DeclareVariable( m_o_mpuppisoftdrop_jet1        , "jj_l1_softDrop_mass");
  DeclareVariable( m_o_mpuppisoftdrop_jet2        , "jj_l2_softDrop_mass");
  DeclareVariable( m_o_mgensoftdrop_jet1          , "jj_l1_gen_softDrop_mass");
  DeclareVariable( m_o_mgensoftdrop_jet2          , "jj_l2_gen_softDrop_mass");
  DeclareVariable( m_o_tau1_jet1                  , "jj_l1_tau1");  
  DeclareVariable( m_o_tau1_jet2                  , "jj_l2_tau1");
  DeclareVariable( m_o_tau2_jet1                  , "jj_l1_tau2");  
  DeclareVariable( m_o_tau2_jet2                  , "jj_l2_tau2");
  DeclareVariable( m_o_tau21_jet1                 , "jj_l1_tau21"); 
  DeclareVariable( m_o_tau21_jet2                 , "jj_l2_tau21");
  DeclareVariable( m_o_pt_jet1                    , "jj_l1_pt");
  DeclareVariable( m_o_genpt_jet1                 , "jj_l1_gen_pt");
  DeclareVariable( m_o_pt_jet2                    , "jj_l2_pt");
  DeclareVariable( m_o_genpt_jet2                 , "jj_l2_gen_pt");
  DeclareVariable( Flag_goodVertices              , "Flag_goodVertices");
  DeclareVariable( Flag_globalTightHalo2016Filter , "Flag_CSCTightHaloFilter");
  DeclareVariable( Flag_HBHENoiseFilter           , "Flag_HBHENoiseFilter");
  DeclareVariable( Flag_HBHENoiseIsoFilter        , "Flag_HBHENoiseIsoFilter");
  DeclareVariable( Flag_eeBadScFilter             , "Flag_eeBadScFilter");
  DeclareVariable( Flag_badChargedHadronFilter    , "Flag_badChargedHadronFilter");
  DeclareVariable( Flag_badMuonFilter             , "Flag_badMuonFilter");
  DeclareVariable( Flag_ECALDeadCell              , "Flag_ECALDeadCell");
  DeclareVariable( HLTJet360_TrimMass30           , "HLTJet360_TrimMass30");
  DeclareVariable( HLTHT700_TrimMass50            , "HLTHT700_TrimMass50");
  DeclareVariable( HLTHT650_MJJ950DEtaJJ1p5       , "HLTHT650_MJJ950DEtaJJ1p5");
  DeclareVariable( HLTHT650_MJJ900DEtaJJ1p5       , "HLTHT650_MJJ900DEtaJJ1p5");
  DeclareVariable( HLTHT800                       , "HLTHT800");
  DeclareVariable( HLT_JJ                         , "HLT_JJ");
  DeclareVariable( m_o_jj_nOtherLeptons           , "jj_nOtherLeptons");
  DeclareVariable( jj_mergedVTruth_jet1           , "jj_mergedVTruth_jet1");
  DeclareVariable( jj_mergedVTruth_jet2           , "jj_mergedVTruth_jet2");

  // // Declare the output histograms:
  // Book( TH1F( "Mjj_hist"     , "Mjj", 100, 0.0, 3000. ) );
  
 
  m_test->resize( 6, 0 ); // Reserve six entries in vector for counting entries:
  TH1* genEvents = Book(TH1F("genEvents", "number of generated Events",2,0,2));
              
   return;
}

void VVanalysis::EndInputData( const SInputData& ) throw( SError ) {
  
  // static const Int_t n = 5;
 //  Float_t x_array[ n ] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
 //  Float_t y_array[ n ] = { 0.0, 2.0, 4.0, 6.0, 8.0 };
 //  TGraph mygraph( n, x_array, y_array );
 //  mygraph.SetName( "MyGraph" );
 //  WriteObj( mygraph, "graph_dir" );
    
//   std::ofstream myfile;
//   myfile.open ("debug.txt");
//   std::sort (debugEvent.begin(), debugEvent.end());
//   for(int i=0;i< debugEvent.size();i++){
//     myfile << debugEvent.at(i) << "\n";
//   }
//   myfile.close();
   return;

}



void VVanalysis::BeginMasterInputData( const SInputData& ) throw( SError ){

   return;

}

void VVanalysis::EndMasterInputData( const SInputData& ) throw( SError ){ //this is a good place to print some summaries, do some final calculations on the created histograms (for instance fitting them), etc
    
  m_logger << INFO << "Number of all processed events      :  "<< *m_allEvents   << "   " << ( m_test->size() > 0 ? ( *m_test )[ 0 ] : 0 )<< SLogger::endmsg;
  m_logger << INFO << "Number of events passing pt, eta    :  "<< *m_passedLoose << "   " << ( m_test->size() > 1 ? ( *m_test )[ 1 ] : 0 )<< SLogger::endmsg;
  m_logger << INFO << "Found two PUPPI jets matched to AK8 :  "<< *m_passedPuppi << "   " << ( m_test->size() > 2 ? ( *m_test )[ 2 ] : 0 )<< SLogger::endmsg;
  m_logger << INFO << "Number of events passing dETa       :  "<< *m_passedDEta  << "   " << ( m_test->size() > 4 ? ( *m_test )[ 4 ] : 0 )<< SLogger::endmsg;
  m_logger << INFO << "Number of events passing Mjj        :  "<< *m_passedMjj   << "   " << ( m_test->size() > 3 ? ( *m_test )[ 3 ] : 0 )<< SLogger::endmsg;
  m_logger << INFO << "Number of events passing selection  :  "<< *m_passedEvents<< "   " << ( m_test->size() > 5 ? ( *m_test )[ 5 ] : 0 )<< SLogger::endmsg;
  
  m_logger << INFO << "Number of generated Events (weighted) : "<< nSumGenWeights << SLogger::endmsg;
  
   return;

}
  
  


void VVanalysis::ExecuteEvent( const SInputData&, Double_t weight) throw( SError ) { //This is the main analysis function that is called for each event. It receives the weight of the event, as it is calculated by the framework from the luminosities and generator cuts defined in the XML configuration.
  
  float gW = (m_eventInfo.genEventWeight < 0) ? -1 : 1; 
  nSumGenWeights += gW;
  Hist( "genEvents" )->Fill(gW);
  
  clearBranches();
  
  if (!m_isData) {
    b_weight= getEventWeight();
  }
  // for inclusive signal samples only take generated hadronic events

  // if( m_isSignal && !SignalIsHad( m_genParticle, m_Channel)) throw SError( SError::SkipEvent);

  ++m_allEvents;
  ( *m_test )[ 0 ]++;
      
   if( m_jetAK8.N < 2 ) throw SError( SError::SkipEvent );
  //-------------Select two fat jets-------------//
  std::vector<UZH::Jet> goodFatJets;
  std::vector<UZH::Jet> goodGenJets;
  std::vector<UZH::GenParticle> GenQuarks  = FindGeneratedQuarks(m_genParticle, m_isData);
  std::vector<UZH::Electron> goodElectrons = FindGoodLeptons(m_electron);
  std::vector<UZH::Muon>     goodMuons     = FindGoodLeptons(m_muon);

  std::vector<int> puppiMatch;
  for ( int i = 0; i < (m_jetAK8.N); ++i ) {
   
    UZH::Jet myjet( &m_jetAK8, i );
    if ( i == 0 && !myjet.IDTight()) break;
    if (! (myjet.pt() > 200       )) continue;
    if (! (fabs(myjet.eta()) < 2.5)) continue;
    if (! (myjet.IDTight()        )) continue;
  
    //Match to puppi jet
    float dRmin = 99.;
    if(m_jetAK8Puppi.pt->size() < 2) throw SError( SError::SkipEvent);
    for ( int ii = 0; ii < abs((m_jetAK8Puppi.pt->size())); ++ii ) {
      UZH::Jet mypuppijet( &m_jetAK8Puppi, ii );
      float dR = myjet.DeltaR(mypuppijet);
      if ( dR > dRmin ) continue;
      bool samePuppiJet =0;

      for(int m=0;m<abs(puppiMatch.size());m++)
      {
        if(ii == puppiMatch.at(m)) samePuppiJet=1;
      }
      if (samePuppiJet) continue;
      puppiMatch.push_back(ii);
      dRmin = dR;
      myjet.puppi_softdropmass= ApplyPuppiSoftdropMassCorrections(mypuppijet,m_puppisd_corr,m_isData);//mypuppijet.softdrop_mass();
      myjet.puppi_tau1        = mypuppijet.tau1();
      myjet.puppi_tau2        = mypuppijet.tau2();
       
    }
    
    //Match to gen jet
    if(!m_isData){
      dRmin = 99.;
      int jetIdx = 99;
      for ( int j = 0; j < (m_genjetAK8.N); ++j ) {
        UZH::Jet genJet( &m_genjetAK8, j );
        if ( genJet.pt() < 1. ) continue;
        float dR = genJet.DeltaR(myjet);
        if ( dR > dRmin ) continue;
        dRmin = dR;
        jetIdx = j;
      }
      UZH::Jet selectedJet( &m_genjetAK8, jetIdx );
      goodGenJets.push_back(selectedJet);
    }
    
    
    if(! FoundNoLeptonOverlap(goodElectrons,goodMuons,myjet.tlv()) ) continue;
    goodFatJets.push_back(myjet);
  }
  




  //-------------Select two fat jets-------------//
  if( goodFatJets.size() < 2 ) throw SError( SError::SkipEvent );
  ++m_passedLoose;
  ( *m_test )[ 1 ]++;
     
   //-------------Make sure we have a PUPPI match-------------// 
  if( goodFatJets[0].puppi_softdropmass == -99  || goodFatJets[1].puppi_softdropmass == -99
    || goodFatJets[0].puppi_tau1 == -99         || goodFatJets[1].puppi_tau1 == -99
    || goodFatJets[0].puppi_tau2 == -99         || goodFatJets[1].puppi_tau2 == -99
     ) throw SError( SError::SkipEvent );
  ++m_passedPuppi;
  ( *m_test )[ 2 ]++;


     
  std::vector<UZH::Jet> goodFatJets_sorted = SortAfterPuppiSDMass(goodFatJets);
  
  // dEta cut
  if( fabs( (goodFatJets_sorted[0].tlv()).Eta() - (goodFatJets_sorted[1].tlv()).Eta() )  > 1.3 ) throw SError( SError::SkipEvent );
  
  
  // // FOR SYNCH
//   if (m_eventInfo.lumiBlock  == 94 && m_eventInfo.eventNumber== 15904 && m_eventInfo.runNumber  == 1) {
//     std::cout<< "jj mass =" << (goodFatJets_sorted[0].tlv() + goodFatJets_sorted[1].tlv()).M() << std::endl;
//     std::cout<< "jj deta = " << fabs( (goodFatJets_sorted[0].tlv()).Eta() - (goodFatJets_sorted[1].tlv()).Eta() ) << std::endl;
//     for( int i=0; i < goodFatJets.size(); ++i){
//          std::cout<< i << " goodFatJets_sorted mass = " << goodFatJets_sorted[i].puppi_softdropmass << std::endl;
//          std::cout<< i << " goodFatJets_sorted pt   = " << goodFatJets_sorted[i].tlv().Pt()  << std::endl;
//           std::cout<< i << " goodFatJets_sorted tau21   = " << goodFatJets_sorted[i].puppi_tau2/goodFatJets_sorted[i].puppi_tau1  << std::endl;
//          std::cout<<""<<std::endl;
//     }
//   }

  
  ++m_passedDEta;
  ( *m_test )[ 3 ]++;
  //debugEvent.push_back(m_eventInfo.eventNumber);

  // Loose Mjj cut to slim down samples
  TLorentzVector dijet = goodFatJets_sorted[0].tlv() + goodFatJets_sorted[1].tlv();
  if( dijet.M() < 1000. ) throw SError( SError::SkipEvent );
  
  ++m_passedMjj;
  ( *m_test )[ 4 ]++;
  
 
  //debugEvent.push_back(m_eventInfo.eventNumber);
  
  ++m_passedEvents;
  ( *m_test )[ 5 ]++;
  

  // Fill tree
  m_o_jj_nOtherLeptons     = goodElectrons.size()+goodMuons.size();
  m_o_njj                  = goodFatJets_sorted.size();
  m_o_mjj                  = dijet.M();
  m_o_mpuppisoftdrop_jet1  = goodFatJets_sorted[0].puppi_softdropmass;
  m_o_mpuppisoftdrop_jet2  = goodFatJets_sorted[1].puppi_softdropmass;
  m_o_tau1_jet1            = goodFatJets_sorted[0].puppi_tau1;
  m_o_tau1_jet2            = goodFatJets_sorted[1].puppi_tau2;
  m_o_tau2_jet1            = goodFatJets_sorted[0].puppi_tau1;
  m_o_tau2_jet2            = goodFatJets_sorted[1].puppi_tau2;
  m_o_tau21_jet1           = goodFatJets_sorted[0].puppi_tau2/goodFatJets_sorted[0].puppi_tau1;
  m_o_tau21_jet2           = goodFatJets_sorted[1].puppi_tau2/goodFatJets_sorted[1].puppi_tau1;
  m_o_pt_jet1              = goodFatJets_sorted[0].tlv().Pt();
  m_o_pt_jet2              = goodFatJets_sorted[1].tlv().Pt();
  
  
  bool isfired = false;
  for( std::map<std::string,bool>::iterator it = (m_eventInfo.trigDecision)->begin(); it != (m_eventInfo.trigDecision)->end(); ++it){
    if( (it->first).find("AK8PFJet360_TrimMass30")          != std::string::npos) HLTJet360_TrimMass30      = it->second;
    if( (it->first).find("AK8PFHT700_TrimR0p1PT0p03Mass50") != std::string::npos) HLTHT700_TrimMass50       = it->second;
    if( (it->first).find("PFHT650_WideJetMJJ950DEtaJJ1p5")  != std::string::npos) HLTHT650_MJJ950DEtaJJ1p5  = it->second;
    if( (it->first).find("PFHT650_WideJetMJJ900DEtaJJ1p5")  != std::string::npos) HLTHT650_MJJ900DEtaJJ1p5  = it->second;
    if( (it->first).find("PFHT800_v")                       != std::string::npos) HLTHT800                  = it->second;
    if (HLTJet360_TrimMass30 or HLTHT700_TrimMass50 or HLTHT650_MJJ950DEtaJJ1p5 or HLTHT650_MJJ900DEtaJJ1p5 or HLTHT800) isfired = true;
  }
  
  b_lumi                          = m_eventInfo.lumiBlock;
  b_event                         = m_eventInfo.eventNumber;
  b_run                           = m_eventInfo.runNumber;
  Flag_goodVertices               = m_eventInfo.PV_filter;
  Flag_globalTightHalo2016Filter  = m_eventInfo.passFilter_CSCHalo;
  Flag_HBHENoiseFilter            = m_eventInfo.passFilter_HBHELoose;
  Flag_HBHENoiseIsoFilter         = m_eventInfo.passFilter_HBHEIso;
  Flag_eeBadScFilter              = m_eventInfo.passFilter_EEBadSc;
  Flag_badChargedHadronFilter     = m_eventInfo.passFilter_chargedHadronTrackResolution;
  Flag_badMuonFilter              = m_eventInfo.passFilter_muonBadTrack;
  Flag_ECALDeadCell               = m_eventInfo.passFilter_ECALDeadCell;
  HLT_JJ                          = isfired;
  

  if(!m_isData){
    m_o_genmjj                      = (goodGenJets[0].tlv() + goodGenJets[1].tlv()).M();
    m_o_mgensoftdrop_jet1           = goodGenJets[0].softdropmass();
    m_o_mgensoftdrop_jet2           = goodGenJets[1].softdropmass();
    m_o_genpt_jet1                  = goodGenJets[0].tlv().Pt();
    m_o_genpt_jet2                  = goodGenJets[1].tlv().Pt();
  }
  
  jj_mergedVTruth_jet1                =  isMergedVJet(goodFatJets_sorted[0].tlv(),GenQuarks) ;
  jj_mergedVTruth_jet2                =  isMergedVJet(goodFatJets_sorted[1].tlv(),GenQuarks) ;
 
   
  
  // }
  // TString StatusString = "Before_Cuts_";
  // FillValidationHists( GENERAL, StatusString );
  // FillValidationHists( ELECTRON, StatusString );
  // FillValidationHists( MUON, StatusString );
  // FillValidationHists( JETS, StatusString );

  return;

}

 

void VVanalysis::FillValidationHists( ValHistsType ht, const TString& status ) {  

   if( ht == GENERAL ){
      TString suffix = status + "General_";
   } 
   else if( ht == ELECTRON ){
      TString suffix = status + "Electron_";
   } 
   else if( ht == MUON ){
      TString suffix = status + "Muon_";
   } 
   else if( ht == JETS ){
      TString suffix = status + "Jets_";
      Book( TH1F( suffix + "mjj", "mjj", 300, 0.0, 3000.0 ) )->Fill( m_o_mjj );
   } 
   else {
      SError error( SError::StopExecution );
      error << GetName() << ": Validation histogram type"  << ht << " not supported";
      throw error;
   }

   return;
}

double VVanalysis::getEventWeight() {
  
  double weight = 1.;
  for( unsigned int v = 0; v < (m_eventInfo.actualIntPerXing)->size(); ++v ){
    if ( (*m_eventInfo.bunchCrossing)[v] == 0 ) {
      b_weightPU = m_pileupReweightingTool.getPileUpweight( (*m_eventInfo.actualIntPerXing)[v] );
      m_logger << VERBOSE << "Weight: " << b_weightPU << " for true: " << (*m_eventInfo.actualIntPerXing)[v] << SLogger::endmsg;
      break;
    }
  }
  b_weightGen = (m_eventInfo.genEventWeight < 0) ? -1 : 1; 
  weight *= b_weightPU*b_weightGen;
  
  return weight;
  
}


void VVanalysis::clearBranches() {
  b_weight                      = 1.;
  b_weightGen                   = 1.;
  b_weightPU                    = 1.;
  b_weightBtag                  = 1.;
  
  
  m_o_mjj                       = -99;
  m_o_genmjj                    = -99;
  m_o_mpuppisoftdrop_jet1       = -99;
  m_o_mpuppisoftdrop_jet2       = -99;
  m_o_mgensoftdrop_jet1         = -99;
  m_o_mgensoftdrop_jet2         = -99;
  m_o_tau1_jet1                 = -99;
  m_o_tau1_jet2                 = -99;
  m_o_tau2_jet1                 = -99;
  m_o_tau2_jet2                 = -99;
  m_o_tau21_jet1                = -99;
  m_o_tau21_jet2                = -99;
  Flag_goodVertices             = -99;
  Flag_globalTightHalo2016Filter= -99;
  Flag_HBHENoiseFilter          = -99;
  Flag_HBHENoiseIsoFilter       = -99;
  Flag_eeBadScFilter            = -99;
  Flag_badChargedHadronFilter   = -99;
  Flag_badMuonFilter            = -99;
  Flag_ECALDeadCell             = -99;
  HLTJet360_TrimMass30          = -99;
  HLTHT700_TrimMass50           = -99;
  HLTHT650_MJJ950DEtaJJ1p5      = -99;
  HLTHT650_MJJ900DEtaJJ1p5      = -99;
  HLTHT800                      = -99;
  nLeptonOverlap                = -99;
  jj_mergedVTruth_jet1          = -99;
  jj_mergedVTruth_jet2          = -99;
}


