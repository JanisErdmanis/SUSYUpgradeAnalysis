#include <iostream>
#include "TROOT.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

int main() {
  std::cout << "This is a test." << std::endl;
  // This hack is needed to force dictionary loading in ROOT 5
  gROOT->ProcessLine("#include <vector>");

  UpgradePerformanceFunctions *m_upgrade = new UpgradePerformanceFunctions();
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  m_upgrade->setAvgMu(140.);
  std::cout << "Layout is " << m_upgrade->getLayout() 
	    << ", and mu value is " << m_upgrade->getAvgMu() << std::endl;
  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::looseElectron);
  m_upgrade->setElectronRandomSeed(1);
  m_upgrade->setMuonWorkingPoint(UpgradePerformanceFunctions::tightMuon);
  //  m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_histograms.root");
  m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");
  m_upgrade->setJetRandomSeed(1);
  m_upgrade->setPileupRandomSeed(1);
  m_upgrade->setPileupUseTrackConf(true);
  m_upgrade->setPileupJetPtThresholdMeV(30000.);
  m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PU);
  m_upgrade->setPileupEff(0.02);
  m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/groups/UpgradePhysics/PerformanceFunctions/");

  // This is just a test of some methods without realistic input.
  float eventMETreso = m_upgrade->getMETResolution(100000.0);
  std::cout << "MET resolution test result is " << eventMETreso << std::endl;

  std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.0 is "
	    << m_upgrade->getSingleMuonTriggerEfficiency(200000.,1.0)
	    << std::endl;
  
  std::cout << "Muon trigger efficiency at pT=200 GeV, eta=1.1 is "
	    << m_upgrade->getSingleMuonTriggerEfficiency(200000.,1.1)
	    << std::endl;

  float muonEfficiency = m_upgrade->getMuonEfficiency(2000000., 1.5);
  std::cout << "Muon efficiency at pT=2000 GeV, eta=1.5 is "
	    << muonEfficiency << std::endl;

  float muonPtResolution = m_upgrade->getMuonPtResolution(500000., 2.0);
  std::cout << "Muon pT resolution at pT=500 GeV, eta=2.0 is " 
	    << muonPtResolution << " MeV" << std::endl;
  muonPtResolution = m_upgrade->getMuonPtResolution(2000000., 1.5);
  std::cout << "Muon pT resolution at pT=2000 GeV, eta=1.5 is "
	    << muonPtResolution << " MeV" << std::endl;
  muonPtResolution = m_upgrade->getMuonPtResolution(12000., 0.15);
  std::cout << "Muon pT resolution at pT=12 GeV, eta=0.15 is "
	    << muonPtResolution << " MeV" << std::endl;

  float muonQOverPtResolution = m_upgrade->getMuonQOverPtResolution(12000., 0.15);
  std::cout << "Muon qOverPT resolution at pT=12 GeV, eta=0.15 is "
	    << muonQOverPtResolution << "/MeV" << std::endl;

  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  std::cout << "electron efficiency (gold) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (gold) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
  std::cout << "electron efficiency (silver) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (silver) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
  std::cout << "electron efficiency (bronze) = " << m_upgrade->getElectronEfficiency(40000., 1.0) << std::endl;
  std::cout << "muon efficiency (bronze) = " << m_upgrade->getMuonEfficiency(40000., 2.8) << std::endl;
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);

  std::vector<TLorentzVector> pileupJets = m_upgrade->getPileupJets();
  std::cout << "Found " <<  pileupJets.size() << " pileup jets above threshold" << std::endl;
  std::cout << "Track Confirmation Efficiency for PU pT=40 GeV, eta=1.0 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 1.0, "PU") << std::endl;
  std::cout << "Track Confirmation Efficiency for HS pT=40 GeV, eta=1.0 is "
	    << m_upgrade->getTrackJetConfirmEff(40000., 1.0, "HS") << std::endl;

  // Temporarily change layout for flavour tagging
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  m_upgrade->setFlavourTaggingCalibrationFilename("UpgradePerformanceFunctions/flavor_tags_v0.7.root");

  std::cout << "b-tagging efficiency for pT=40 GeV, eta=1.0 is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'B', "mv1", 70) << std::endl;
  std::cout << "c-tagging efficiency for pT=40 GeV, eta=1.0 is " 
	    << m_upgrade->getFlavourTagEfficiency(40000., 1.0, 'C', "mv1", 70) << std::endl;

  return 0;
}
