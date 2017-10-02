#include "../include/VVAnalysisTools.h"
#include "../include/VVanalysis.h"


bool isMergedVJet(TLorentzVector goodFatJet, std::vector<UZH::GenParticle> VdecayProducts) {
    int associatedQuarks=0;
    for(unsigned int i=0;i< VdecayProducts.size();i++)
    {
      TLorentzVector q = VdecayProducts[i].tlv();
      if (goodFatJet.DeltaR(q) < 0.8) associatedQuarks +=1;

    }
    if (associatedQuarks ==2) return 1;
    else return 0;   
}


std::vector<UZH::GenParticle> FindGeneratedQuarks(Ntuple::GenParticleNtupleObject m_genParticle,bool m_isData)
{
  std::vector<UZH::GenParticle> GenQuarks;
  if(m_isData) return GenQuarks;
  for(int i=0;i< m_genParticle.N;i++)
  {
    bool containsV=0;
    if(TMath::Abs(m_genParticle.pdgId->at(i)) > 6 or TMath::Abs(m_genParticle.pdgId->at(i)) <1) continue;
       
    for(int ii=0;ii< m_genParticle.nMoth->at(i);ii++)
    {
           
      if(TMath::Abs(m_genParticle.mother->at(i).at(ii)) == 24 or TMath::Abs(m_genParticle.mother->at(i).at(ii)) == 23) containsV = 1;  

    }
    if (!containsV) continue;
    UZH::GenParticle MyQuark( &m_genParticle, i );
    GenQuarks.push_back(MyQuark);
  }
  return GenQuarks;    
}


float ApplyPuppiSoftdropMassCorrections(UZH::Jet puppiJet,std::vector<TF1*> m_puppisd_corr, bool m_isData){
 float genCorr =1;
 if(!m_isData) genCorr = m_puppisd_corr[0]->Eval(puppiJet.pt());
 float recoCorr = 1;
 if( fabs(puppiJet.eta()) <= 1.3) recoCorr = m_puppisd_corr[1]->Eval(puppiJet.pt());
 else if (fabs(puppiJet.eta()) > 1.3) recoCorr = m_puppisd_corr[2]->Eval(puppiJet.pt());
    
 return puppiJet.softdrop_mass()*genCorr*recoCorr;
}


bool FoundNoLeptonOverlap(std::vector<UZH::Electron> goodEle, std::vector<UZH::Muon> goodMu, TLorentzVector Jet){
 
  for( unsigned int e =0;e< goodEle.size(); e++)
  {
    if(Jet.DeltaR(goodEle.at(e).tlv()) < 0.8) return 0;  
  }
  for( unsigned int m =0;m< goodMu.size(); m++)
  {
    if(Jet.DeltaR(goodMu.at(m).tlv()) < 0.8) return 0;   
  }
  return 1;   
}

std::vector<UZH::Electron> FindGoodLeptons(Ntuple::ElectronNtupleObject m_electrons){
    std::vector<UZH::Electron> goodEle;
    for(int i=0;i< m_electrons.N;i++)
    {
        UZH::Electron ele(&m_electrons,i);
	//        if( fabs( ele.superCluster_eta() ) >= 2.5 ) continue;
        if( ! ele.isHeepElectron()  ) continue;
        goodEle.push_back(ele);
    }
    return goodEle;
}

std::vector<UZH::Muon> FindGoodLeptons(Ntuple::MuonNtupleObject m_muons){
    std::vector<UZH::Muon> goodMu;
    for(int i=0;i< m_muons.N;i++)
    {
        UZH::Muon mu(&m_muons,i);
        if( mu.pt() <= 30.) continue;
        if( fabs( mu.eta() ) >= 2.4 ) continue;
        if( ! mu.isTightMuon()  ) continue; // //isHighPtMuon
        if( mu.trackIso()/mu.pt() >= 0.1 ) continue;
        goodMu.push_back(mu);
    }
    return goodMu;
}



bool SignalIsHad( Ntuple::GenParticleNtupleObject data_ , std::string m_Channel) {
  
  bool isSignal = false;
  int nVs = 0;
  
  for( int p = 0; p < (data_.N); ++p ){
    bool isHad = false;
    if( (*data_.pt).at(p) < 0.01) continue;
    if( fabs((*data_.pdgId).at(p)) != 24 and fabs((*data_.pdgId).at(p)) != 23  and fabs((*data_.pdgId).at(p)) != 25) continue;
    if ((*data_.dau)[p].size() < 2) continue;
    for( unsigned int d = 0; d < (*data_.dau)[p].size(); ++d ) {
      if( fabs((*data_.dau)[p][d]) > 9) continue;
      isHad = true;
    }
    if (!isHad) continue;
    nVs ++;
  }
  
  if (m_Channel.find("qV")!=std::string::npos && nVs>0) isSignal = true;
  if (m_Channel.find("VV")!=std::string::npos && nVs>1) isSignal = true;
  return isSignal;
}


std::vector<UZH::Jet> SortAfterPuppiSDMass(std::vector <UZH::Jet> jets){
    std::vector<float> mass;
    std::map<float,int> map;
    for(unsigned int i=0;i< jets.size(); i++)
    {
      mass.push_back(jets[i].puppi_softdropmass);
      map[jets[i].puppi_softdropmass] = i;
    }
    std::sort(mass.begin(),mass.end());
    std::vector<UZH::Jet> sorted;
    for(unsigned int i=0;i<mass.size();i++)
    {
      sorted.push_back(jets.at(map[mass.at(mass.size()-i-1)]));
    }
    return sorted;
}

std::vector<UZH::Jet> SortAfterTau21(std::vector <UZH::Jet> jets){
    std::vector<float> tau21;
    std::map<float,int> map;
    for(unsigned int i=0;i< jets.size(); i++)
    {
      tau21.push_back(jets[i].puppi_tau2/jets[i].puppi_tau1);
      map[jets[i].puppi_tau2/jets[i].puppi_tau1] = i;
    }
    std::sort(tau21.begin(),tau21.end(),std::greater<float>() );
    std::vector<UZH::Jet> sorted;
    for(unsigned int i=0;i<tau21.size();i++)
    {
      sorted.push_back(jets.at(map[tau21.at(tau21.size()-i-1)]));
    }
    return sorted;
}



void PrintEvent(std::vector<UZH::Jet> jets  )
{
    std::cout << "D_eta " << fabs(jets.at(0).tlv().Eta()-jets.at(1).tlv().Eta()) << std::endl;
    std::cout << "Mjj   " << (jets.at(0).tlv()+jets.at(1).tlv()).M() <<std::endl;
    for(unsigned int i=0;i<jets.size(); i++)
    {
        std::cout << "jet pt " << jets.at(i).tlv().Pt() << " eta " << jets.at(i).tlv().Eta() << " phi " << jets.at(i).tlv().Phi() << " e " << jets.at(i).tlv().E() << " puppi sd mass "<< jets.at(i).puppi_softdropmass << std::endl;
    
    }
}












