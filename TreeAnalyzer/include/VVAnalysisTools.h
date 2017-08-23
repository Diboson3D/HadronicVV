#ifndef VVANALYSISTOOLS_H
#define VVANALYSISTOOLS_H

#include "../include/VVanalysis.h"
#include <TF1.h>

bool isMergedVJet(TLorentzVector goodFatJet, std::vector<UZH::GenParticle> VdecayProducts);

std::vector<UZH::GenParticle> FindGeneratedQuarks(Ntuple::GenParticleNtupleObject m_genParticle, bool m_isData);

float ApplyPuppiSoftdropMassCorrections(UZH::Jet puppiJet,std::vector<TF1*> m_puppisd_corr, bool m_isData);

bool FoundNoLeptonOverlap(std::vector<UZH::Electron> goodEle, std::vector<UZH::Muon> goodMu, TLorentzVector Jet);

std::vector<UZH::Electron> FindGoodLeptons(Ntuple::ElectronNtupleObject m_electrons);
std::vector<UZH::Muon> FindGoodLeptons(Ntuple::MuonNtupleObject m_muons);

bool SignalIsHad( Ntuple::GenParticleNtupleObject data_ , std::string m_Channel);

std::vector<UZH::Jet> SortAfterPuppiSDMass(std::vector <UZH::Jet> jets);

void PrintEvent(std::vector<UZH::Jet> );

#endif
