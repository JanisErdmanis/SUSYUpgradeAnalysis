// lxplus cleansimulation; lxplus push; lxplus compile; lxplus runAna m100_DeltaM9_N2C1; lxplus pull

#define Smearing

#include <SUSYUpgradeExample/UpgradeAnalysis.h>
#include <EventLoop/Job.h> // It calls execute method
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include <AsgTools/MessageCheck.h>
#include "xAODEventInfo/EventInfo.h"
#include <SUSYUpgradeExample/MT2_ROOT.h>
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCore/AuxContainerBase.h"

// this is needed to distribute the algorithm to the workers
ClassImp(UpgradeAnalysis)

//#include "MCTruthClassifier/MCTruthClassifier.h"

//#include <
//

UpgradeAnalysis :: UpgradeAnalysis ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().

  layout = UpgradePerformanceFunctions::gold;
  averageMuValue = 0.;
}

EL::StatusCode UpgradeAnalysis :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  // let's initialize the algorithm to use the xAODRootAccess package
  job.useXAOD ();

  ANA_CHECK_SET_TYPE (EL::StatusCode); // set type of return code you are expecting (add to top of each function once)

  ANA_CHECK(xAOD::Init());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode UpgradeAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  averageMuValue = 200;

//  #if ANA==1
  m_cuts.push_back("NoCuts");

  m_cuts.push_back("MET>100 GeV");
  m_cuts.push_back("1stJet Pt>100 GeV");
  m_cuts.push_back("2 leading leptons Pt > 7 GeV");
  m_cuts.push_back("mTauTau>150 GeV");
  m_cuts.push_back("M(1st l + 2nd l)<12 GeV");
  m_cuts.push_back("2 leptons");
  m_cuts.push_back("3 leptons");

  for( unsigned int i = 0; i < m_cuts.size(); i++) {

    h_NEvents[m_cuts[i]] = new TH1F("h_NEvents_"+ m_cuts[i], "", 1, -0.5, 0.5);
    wk()->addOutput (h_NEvents[m_cuts[i]]);

    // The missing energy is here
    h_MET[m_cuts[i]] = new TH1F("h_MET_"+ m_cuts[i], "", 15, 0, 1000); 
    h_MET[m_cuts[i]]->SetXTitle("#it{E}_{T}^{miss} [GeV]");      h_MET[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_MET[m_cuts[i]]);
    
    h_MSFOS[m_cuts[i]] = new TH1F("h_MSFOS_"+ m_cuts[i], "", 50, 0, 1000); 
    h_MSFOS[m_cuts[i]]->SetXTitle("m_{SFOS} [GeV]");      h_MSFOS[m_cuts[i]]->SetYTitle("Events / 10 GeV");
    wk()->addOutput (h_MSFOS[m_cuts[i]]);

    h_Meff[m_cuts[i]] = new TH1F("h_Meff_"+ m_cuts[i], "", 75, 0, 1500); 
    h_Meff[m_cuts[i]]->SetXTitle("m_{eff} [GeV]");      h_Meff[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_Meff[m_cuts[i]]);

    h_MT[m_cuts[i]] = new TH1F("h_MT_"+ m_cuts[i], "", 50, 0, 1000); 
    h_MT[m_cuts[i]]->SetXTitle("m_{T} [GeV]");      h_MT[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_MT[m_cuts[i]]);

    h_MT2[m_cuts[i]] = new TH1F("h_MT2_"+ m_cuts[i], "", 50, 0, 1000); 
    h_MT2[m_cuts[i]]->SetXTitle("m_{T2} [GeV]");      h_MT2[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_MT2[m_cuts[i]]);

    //

    h_NPho[m_cuts[i]] = new TH1F("h_NPho_"+ m_cuts[i], "", 6, -0.5, 5.5); 
    h_NPho[m_cuts[i]]->SetXTitle("Photon multiplicity");      h_NPho[m_cuts[i]]->SetYTitle("Events");
    // wk()->addOutput (h_NPho[m_cuts[i]]);

    h_NElec[m_cuts[i]] = new TH1F("h_NElec_"+ m_cuts[i], "", 6, -0.5, 5.5); 
    h_NElec[m_cuts[i]]->SetXTitle("Electron multiplicity");      h_NElec[m_cuts[i]]->SetYTitle("Events");
    wk()->addOutput (h_NElec[m_cuts[i]]);

    h_NMuon[m_cuts[i]] = new TH1F("h_NMuon_"+ m_cuts[i], "", 6, -0.5, 5.5); 
    h_NMuon[m_cuts[i]]->SetXTitle("Muon multiplicity");      h_NMuon[m_cuts[i]]->SetYTitle("Events");
    wk()->addOutput (h_NMuon[m_cuts[i]]);

    h_NTau[m_cuts[i]] = new TH1F("h_NTau_"+ m_cuts[i], "", 6, -0.5, 5.5); 
    h_NTau[m_cuts[i]]->SetXTitle("Tau multiplicity");      h_NTau[m_cuts[i]]->SetYTitle("Events");
    //wk()->addOutput (h_NTau[m_cuts[i]]);

    h_NJet[m_cuts[i]] = new TH1F("h_NJet_"+ m_cuts[i], "", 20, -0.5, 19.5); 
    h_NJet[m_cuts[i]]->SetXTitle("Jet multiplicity");      h_NJet[m_cuts[i]]->SetYTitle("Events");
    wk()->addOutput (h_NJet[m_cuts[i]]);

    h_NBJet[m_cuts[i]] = new TH1F("h_NBJet_"+ m_cuts[i], "", 20, -0.5, 19.5); 
    h_NBJet[m_cuts[i]]->SetXTitle("B Jet multiplicity");      h_NBJet[m_cuts[i]]->SetYTitle("Events");
    wk()->addOutput (h_NBJet[m_cuts[i]]);

    h_PtPhos[m_cuts[i]] = new TH1F("h_PtPhos_"+ m_cuts[i], "", 50, 0, 1000); 
    h_PtPhos[m_cuts[i]]->SetXTitle("Photon p_{T} [GeV]");      h_PtPhos[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_PtPhos[m_cuts[i]]);


    h_PtElecs[m_cuts[i]] = new TH1F("h_PtElecs_"+ m_cuts[i], "", 50, 0, 1000); 
    h_PtElecs[m_cuts[i]]->SetXTitle("Electron p_{T} [GeV]");      h_PtElecs[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_PtElecs[m_cuts[i]]);

    h_PtMuons[m_cuts[i]] = new TH1F("h_PtMuons_"+ m_cuts[i], "", 10, 0, 50); 
    h_PtMuons[m_cuts[i]]->SetXTitle("Muon p_{T} [GeV]");      h_PtMuons[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_PtMuons[m_cuts[i]]);

    h_PtTaus[m_cuts[i]] = new TH1F("h_PtTaus_"+ m_cuts[i], "", 50, 0, 1000); 
    h_PtTaus[m_cuts[i]]->SetXTitle("Tau p_{T} [GeV]");      h_PtTaus[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_PtTaus[m_cuts[i]]);

    h_PtJets[m_cuts[i]] = new TH1F("h_PtJets_"+ m_cuts[i], "", 10, 0, 1000); 
    h_PtJets[m_cuts[i]]->SetXTitle("Jet p_{T} [GeV]");      h_PtJets[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    //wk()->addOutput (h_PtJets[m_cuts[i]]);

    // My histograms

    h_PtMuons1st[m_cuts[i]] = new TH1F("h_PtMuons1st_"+ m_cuts[i], "", 20, 0, 25); 
    h_PtMuons1st[m_cuts[i]]->SetXTitle("Muon p_{T} [GeV]");      h_PtMuons1st[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_PtMuons1st[m_cuts[i]]);

    h_PtMuons2nd[m_cuts[i]] = new TH1F("h_PtMuons2nd_"+ m_cuts[i], "", 20, 0, 25); 
    h_PtMuons2nd[m_cuts[i]]->SetXTitle("Muon p_{T} [GeV]");      h_PtMuons2nd[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_PtMuons2nd[m_cuts[i]]);

    h_PtJets1st[m_cuts[i]] = new TH1F("h_PtJets1st_"+ m_cuts[i], "", 50, 0, 300); 
    h_PtJets1st[m_cuts[i]]->SetXTitle("Jet p_{T} [GeV]");      h_PtJets1st[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_PtJets1st[m_cuts[i]]);

    h_PtJets2nd[m_cuts[i]] = new TH1F("h_PtJets2nd_"+ m_cuts[i], "", 50, 0, 300); 
    h_PtJets2nd[m_cuts[i]]->SetXTitle("Jet p_{T} [GeV]");      h_PtJets2nd[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_PtJets2nd[m_cuts[i]]);

    // Does this make segfault
    myhisto[m_cuts[i]] = new TH2F("myhisto" + m_cuts[i],"",30,0,25,30,0,200);
    myhisto[m_cuts[i]]->SetTitle(";1st muon Pt; 1st Jet Pt");
    wk()->addOutput(myhisto[m_cuts[i]]);

    // A new set

    h_angleMuons1st[m_cuts[i]] = new TH1F("h_angleMuons1st_"+ m_cuts[i], "", 15, -4, +4); 
    h_angleMuons1st[m_cuts[i]]->SetXTitle("1st Muon pseidorapidity");      h_angleMuons1st[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_angleMuons1st[m_cuts[i]]);

    h_angleJets1st[m_cuts[i]] = new TH1F("h_angleJets1st_"+ m_cuts[i], "", 15, -4, +4); 
    h_angleJets1st[m_cuts[i]]->SetXTitle("1st Jet pseidorapidity");      h_angleJets1st[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_angleJets1st[m_cuts[i]]);

    h_llmass[m_cuts[i]] = new TH1F("h_llmass_"+ m_cuts[i], "", 25, 0, +100); 
    h_llmass[m_cuts[i]]->SetXTitle("M_ll [GeV]"); h_llmass[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_llmass[m_cuts[i]]);

    h_Phi1stJetMet[m_cuts[i]] = new TH1F("h_Phi1stJetMet"+ m_cuts[i], "", 25, -4, +4); 
    h_Phi1stJetMet[m_cuts[i]]->SetXTitle("DPhi between 1st Jet and MET [rad]"); h_Phi1stJetMet[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_Phi1stJetMet[m_cuts[i]]);


    h_mtautau1[m_cuts[i]] = new TH1F("h_mtautau1_"+ m_cuts[i], "", 25, 0, 500); 
    h_mtautau1[m_cuts[i]]->SetXTitle("Mtautau"); h_mtautau1[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_mtautau1[m_cuts[i]]);

    h_mtautau2[m_cuts[i]] = new TH1F("h_mtautau2_"+ m_cuts[i], "", 25, 0, 500); 
    h_mtautau2[m_cuts[i]]->SetXTitle("Mtautau"); h_mtautau2[m_cuts[i]]->SetYTitle("Events / 20 GeV");
    wk()->addOutput (h_mtautau2[m_cuts[i]]);

  }

  m_Stages.push_back("Generator");
  m_Stages.push_back("Smearing");
  m_Stages.push_back("Fakes");
  m_Stages.push_back("OverlapRemoval");
  m_Stages.push_back("ApplyIsolation");
  m_Stages.push_back("RemoveLowMassPairs");
  m_Stages.push_back("PtEtaThresholds");

  for( unsigned int i = 0; i < m_Stages.size(); i++) {

  h_PtJets1stStages[m_Stages[i]] = new TH1F("h_PtJets1stStages_"+ m_Stages[i], "", 200, 0, 300); 
  h_PtJets1stStages[m_Stages[i]]->SetXTitle("Jet p_{T} [GeV]");      h_PtJets1stStages[m_Stages[i]]->SetYTitle("Events / 20 GeV");
  wk()->addOutput (h_PtJets1stStages[m_Stages[i]]);

  h_PtEleMuo1stStages[m_Stages[i]] = new TH1F("h_PtEleMuo1stStages_"+ m_Stages[i], "", 200, 0, 300); 
  h_PtEleMuo1stStages[m_Stages[i]]->SetXTitle("Lepton p_{T} [GeV]");      h_PtEleMuo1stStages[m_Stages[i]]->SetYTitle("Events / 20 GeV");
  wk()->addOutput (h_PtEleMuo1stStages[m_Stages[i]]);

  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode UpgradeAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode UpgradeAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode UpgradeAnalysis :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.
  xAOD::TEvent* event = wk()->xaodEvent();
  ANA_CHECK_SET_TYPE (EL::StatusCode);

  // as a check, let's see the number of events in our xAOD
  Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int
  m_eventCounter = 0;

  // initialise UpgradePerformanceFunctions
  m_random3.SetSeed(1); // for local smearing
  Info("initialize()", "Setting up UpgradePerformanceFunctions with mu=%f", averageMuValue); 

  // Initializations specific to UpgradePerformanceFunctions
  m_upgrade = new UpgradePerformanceFunctions();
  m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
  m_upgrade->setAvgMu(averageMuValue);
  m_upgrade->setPhotonWorkingPoint(UpgradePerformanceFunctions::tightPhoton);
  m_upgrade->setPhotonRandomSeed(1);
  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::looseElectron);
  m_upgrade->setElectronRandomSeed(1);
  m_upgrade->setMuonWorkingPoint(UpgradePerformanceFunctions::tightMuon);
  m_upgrade->setTauRandomSeed(1);
  m_upgrade->setJetRandomSeed(1);
  m_upgrade->setFlavourTaggingCalibrationFilename("UpgradePerformanceFunctions/flavor_tags_v0.7.root");
  m_upgrade->setMETRandomSeed(1);
  m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");

  m_upgrade->setPileupRandomSeed(1);
  m_upgrade->setPileupRandomSeed(1);
  m_upgrade->setPileupJetPtThresholdMeV(30000.);
  m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PileupEff::HS);
  m_upgrade->setPileupEff(0.9);
  m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/groups/UpgradePhysics/PerformanceFunctions/");

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode UpgradeAnalysis :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  xAOD::TEvent* event = wk()->xaodEvent();
  ANA_CHECK_SET_TYPE (EL::StatusCode);

  #ifndef Smearing
  
  GenPho.clear();
  GenEleMuo.clear();
  GenHadTau.clear();
  GenJet.clear();
  GenBJet.clear();
  
  getTruthPhotons(); 
  getTruthElectrons();
  getTruthMuons();
  getTruthHadronicTaus();
  getTruthJets();
  getTruthMET();

  SmearedPho.clear();
  SmearedEleMuo.clear();
  SmearedHadTau.clear();
  SmearedJet.clear();
  
  for( unsigned int i=0; i<GenJet.size(); i++) {
    SmearedJet.push_back(GenJet[i]);
  }

  for( unsigned int i=0; i<GenPho.size(); i++) {
    SmearedPho.push_back(GenPho[i]);
  }

  for( unsigned int i=0; i<GenEleMuo.size(); i++) {
    if( fabs(GenEleMuo[i].pdgid)!=11 ) continue;
    SmearedEleMuo.push_back(GenEleMuo[i]);
  }

  for( unsigned int i=0; i<GenEleMuo.size(); i++ ){
    if( fabs(GenEleMuo[i].pdgid)!=13 ) continue;
    SmearedEleMuo.push_back(GenEleMuo[i]);
  }

  for( unsigned int i=0; i<GenHadTau.size(); i++) {
    SmearedHadTau.push_back(GenHadTau[i]);
  }

  m_SmearedMETTLV.SetPxPyPzE(m_GenMETTLV.Px(),m_GenMETTLV.Py(),0.,TMath::Sqrt(m_GenMETTLV.Px()*m_GenMETTLV.Px() + m_GenMETTLV.Py()*m_GenMETTLV.Py() ));
  
  sort(SmearedPho.begin(), SmearedPho.end(), compare_pt());   
  sort(SmearedEleMuo.begin(), SmearedEleMuo.end(), compare_pt());   
  sort(SmearedHadTau.begin(), SmearedHadTau.end(), compare_pt());   
  sort(SmearedJet.begin(), SmearedJet.end(), compare_pt());   

  m_trigEff = 1.;

  #else

  GenPho.clear();
  GenEleMuo.clear();
  GenHadTau.clear();
  GenJet.clear();
  GenBJet.clear();
  
  getTruthPhotons(); 
  getTruthElectrons();
  getTruthMuons();
  getTruthHadronicTaus();
  getTruthJets();
  getTruthMET();

  // sort(SmearedPho.begin(), SmearedPho.end(), compare_pt());   
  // sort(SmearedEleMuo.begin(), SmearedEleMuo.end(), compare_pt());   
  // sort(SmearedHadTau.begin(), SmearedHadTau.end(), compare_pt());   
  sort(GenJet.begin(), GenJet.end(), compare_pt());   

  if (GenJet.size()>=1)
    h_PtJets1stStages["Generator"]->Fill( GenJet[0].Pt()*GeV , 1.);

  if (GenEleMuo.size()>=1)
    h_PtEleMuo1stStages["Generator"]->Fill( GenEleMuo[0].Pt()*GeV , 1.);
  
  SmearedPho.clear();
  SmearedEleMuo.clear();
  SmearedHadTau.clear();
  SmearedJet.clear();

  SmearPhotons();
  SmearElectrons();
  SmearMuons();
  SmearHadTaus();
  SmearJets(); 
  SmearMET();

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["Smearing"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["Smearing"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);

  ApplyPhotonFakes();
  ApplyElectronFakes();
  ApplyTauFakes();
  ApplyBtagging();

  sort(SmearedPho.begin(), SmearedPho.end(), compare_pt());   
  sort(SmearedEleMuo.begin(), SmearedEleMuo.end(), compare_pt());   
  sort(SmearedHadTau.begin(), SmearedHadTau.end(), compare_pt());   
  sort(SmearedJet.begin(), SmearedJet.end(), compare_pt());

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["Fakes"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["Fakes"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);

  ApplyPtEtaThresholds(); // Selects events

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["PtEtaThresholds"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["PtEtaThresholds"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);
  
// // //   /* ===============================================================
// // //      ===== Perform basic overlap removal ===========================
// // //      =============================================================== */
// // //   // When electron is not recognized as jet because of threshold of jet algorithm
  OverlapRemoval(); 
// // //   /* ===============================================================
// // //      ===== Signal leptons: isolation, remove low mass pairs ========
// // //      =============================================================== */

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["OverlapRemoval"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["OverlapRemoval"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);

// // // //   // Energy around electron should be small
  ApplyIsolation();

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["ApplyIsolation"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["ApplyIsolation"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);

// // //   // What this thing do?
  RemoveLowMassPairs(); 

// // //   /* ===============================================================
// // //      ===== Trigger effiency.    Analysis dependent, so function ====
// // //      ===== sets m_trigEff to 1 if not edited =======================
// // //      =============================================================== */

  if (SmearedJet.size()>=1)
    h_PtJets1stStages["RemoveLowMassPairs"]->Fill( SmearedJet[0].Pt()*GeV , 1.);

  if (SmearedEleMuo.size()>=1)
    h_PtEleMuo1stStages["RemoveLowMassPairs"]->Fill( SmearedEleMuo[0].Pt()*GeV , 1.);

  ApplyTriggerEfficiency();

// //   /* ===============================================================
// //      ===== Event variables =========================================
// //      =============================================================== */
// //   // Like transverse energy
// // #endif

  #endif

  SmearedBJet.clear();
  SmearedNotBJet.clear();
  for( unsigned int i=0; i<SmearedJet.size(); i++){
    //    if( fabs(SmearedJet[i].pdgid)==4) SmearedNotBJet.push_back(SmearedJet[i]);
    if( fabs(SmearedJet[i].pdgid)==5)
      SmearedBJet.push_back(SmearedJet[i]);
    else
      SmearedNotBJet.push_back(SmearedJet[i]);
  }

  calculateEventVariables();

// //   SmearedEle.clear();
// //   SmearedMuo.clear();
// //   for( unsigned int i=0; i<SmearedEleMuo.size(); i++){
// //     if( fabs(SmearedEleMuo[i].pdgid)==11 ) SmearedEle.push_back(SmearedEleMuo[i]);
// //     if( fabs(SmearedEleMuo[i].pdgid)==13 ) SmearedMuo.push_back(SmearedEleMuo[i]);
// //   }

// // //   /// Making a histograms

  // Printing out parameters for tautau cut ( can be copied inside cutoff )
  // if (SmearedEleMuo.size()>=2) {
  //   cout << "----------------------------------" << endl;
  //   cout <<  m_SmearedMETTLV.Et()*GeV << endl;
  //   cout <<  m_SmearedMETTLV.Pt()*GeV << endl;
  //   cout <<  m_SmearedMETTLV.Px()*GeV << endl;
  //   cout <<  m_SmearedMETTLV.Py()*GeV << endl;

  //   cout <<  SmearedEleMuo[0].Et()*GeV << endl;
  //   cout <<  SmearedEleMuo[0].Pt()*GeV << endl;
  //   cout <<  SmearedEleMuo[0].Px()*GeV << endl;
  //   cout <<  SmearedEleMuo[0].Py()*GeV << endl;
  //   cout <<  SmearedEleMuo[0].M()*GeV << endl;
  //   cout <<  SmearedEleMuo[0].Phi() << endl;

  //   cout <<  SmearedEleMuo[1].Et()*GeV << endl;
  //   cout <<  SmearedEleMuo[1].Pt()*GeV << endl;
  //   cout <<  SmearedEleMuo[1].Px()*GeV << endl;
  //   cout <<  SmearedEleMuo[1].Py()*GeV << endl;
  //   cout <<  SmearedEleMuo[1].M()*GeV << endl;
  //   cout <<  SmearedEleMuo[1].Phi() << endl;
  //   cout << "----------------------------------" << endl;

  //   // double D = SmearedEleMuo[0].Px()*SmearedEleMuo[1].Py() - SmearedEleMuo[0].Py()*SmearedEleMuo[1].Px();
  //   // double D1 = m_SmearedMETTLV.Px()*SmearedEleMuo[1].Py() - m_SmearedMETTLV.Py()*SmearedEleMuo[1].Px();
  //   // double D2 = SmearedEleMuo[0].Px()*m_SmearedMETTLV.Py() - m_SmearedMETTLV.Px()*SmearedEleMuo[0].Py();
  //   // double xi1 = D1/D;
  //   // double xi2 = D2/D;
  //   // cout << xi1 << endl;
  //   // cout << xi2 << endl;
  //   // double px = SmearedEleMuo[0].Px()*(1 + xi1) + SmearedEleMuo[1].Px()*(1 + xi2);
  //   // double py = SmearedEleMuo[0].Py()*(1 + xi1) + SmearedEleMuo[1].Py()*(1 + xi2);
  //   // double E = SmearedEleMuo[0].Py()*(1 + TMath::Abs(xi1)) + SmearedEleMuo[1].Py()*(1 + TMath::Abs(xi2));
  //   // double mtautau = TMath::Sqrt(E*E - px*px - py*py);
  //   // cout << mtautau*GeV << endl;
  //   // double mtautau1 = (SmearedEleMuo[0] + SmearedEleMuo[1] + m_SmearedMETTLV).M();
  //   // double mll = (SmearedEleMuo[0] + SmearedEleMuo[1]).M();
  //   // double x1 = SmearedEleMuo[0].Pt()/(SmearedEleMuo[0].Pt() + m_SmearedMETTLV.Pt());
  //   // double x2 = SmearedEleMuo[1].Pt()/(SmearedEleMuo[1].Pt() + m_SmearedMETTLV.Pt());
  //   // double mtautau2 = mll/TMath::Sqrt(x1*x2);
  //   cout << m_mtautau1*GeV << endl;
  //   cout << m_mtautau2*GeV << endl;
  //   cout << "----------------------------------" << endl;
  // }

  // As we are concerned only about muons
  for (int j=0;j<SmearedEleMuo.size();j++) {
    if (fabs(SmearedEleMuo[j].pdgid)==11)
      SmearedEleMuo[j].Good=false;
  }
  RemoveBad(SmearedEleMuo);

  FillHistos("NoCuts");
  if ( m_SmearedMETTLV.Et()*GeV > 100. ) {
    FillHistos("MET>100 GeV");
    if (SmearedNotBJet.size()==1 && SmearedBJet.size()==0) {
      if ( SmearedNotBJet[0].Pt()*GeV > 100. ) {
        FillHistos("1stJet Pt>100 GeV");
        int nmuob = 0;
        for (int j=0;j<SmearedEleMuo.size();j++) {
          if ( SmearedEleMuo[j].Pt()*GeV>7. && SmearedEleMuo[j].Eta()<2.5 )
            nmuob += 1;
        }
        if ( nmuob>=2 ) {
          FillHistos("2 leading leptons Pt > 7 GeV");
        }
        if (m_mtautau2*GeV>150.) {
          FillHistos("mTauTau>150 GeV");
          if( (SmearedEleMuo[0] + SmearedEleMuo[1]).M()*GeV < 12.) {
            FillHistos("M(1st l + 2nd l)<12 GeV");

            if (SmearedEleMuo.size()==2)
              FillHistos("2 leptons");

            if (SmearedEleMuo.size()==3)
              FillHistos("3 leptons");
          }
        }
      }
    }      
  }
  
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode UpgradeAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode UpgradeAnalysis :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  xAOD::TEvent* event = wk()->xaodEvent();
  ANA_CHECK_SET_TYPE (EL::StatusCode);

  cout << " Analysed " << m_eventCounter << " events" << endl;

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode UpgradeAnalysis :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  return EL::StatusCode::SUCCESS;
}

void UpgradeAnalysis::FillHistos(TString cutname)
{

  h_NEvents[cutname]->Fill( 0.0 , m_trigEff);
  // event variables
  // What is this variable doing
  // This should be a missing energy
  h_MET[cutname]->Fill( m_met*GeV , m_trigEff);
  h_MSFOS[cutname]->Fill( m_mll*GeV , m_trigEff);
  //h_Meff[cutname]->Fill( m_meff*GeV , m_trigEff); // BUGGY
  h_MT[cutname]->Fill( m_mT*GeV , m_trigEff);
  h_MT2[cutname]->Fill( m_mT2*GeV , m_trigEff);

  int sjet = 0;
  for (int j=0;j<SmearedNotBJet.size();j++)
    if (SmearedNotBJet[j].Pt()*GeV>30)
      sjet += 1;
  h_NJet[cutname]->Fill( sjet, m_trigEff);

  int sbjet = 0;
  for (int j=0;j<SmearedBJet.size();j++)
    if (SmearedBJet[j].Pt()*GeV>30)
      sbjet += 1;
  h_NBJet[cutname]->Fill( sbjet , m_trigEff);

  // CHECK HERE
  int nel = 0;
  int nmuon = 0;
  for (int j=0;j<SmearedEleMuo.size();j++) {
    if (fabs(SmearedEleMuo[j].pdgid)==11)
      nel += 1;
    if (fabs(SmearedEleMuo[j].pdgid)==13)
      nmuon += 1;
  }
  
  h_NElec[cutname]->Fill(nel,m_trigEff);
  h_NMuon[cutname]->Fill(nmuon,m_trigEff);

  // // NEED TO CHANGE
  if (SmearedEleMuo.size()>=1) // Smeared elelctron
    h_PtMuons1st[cutname]->Fill( SmearedEleMuo[0].Pt()*GeV , m_trigEff);

  if (SmearedEleMuo.size()>=2) 
    h_PtMuons2nd[cutname]->Fill( SmearedEleMuo[1].Pt()*GeV , m_trigEff);

  if (SmearedNotBJet.size()>=1)
    h_PtJets1st[cutname]->Fill( SmearedNotBJet[0].Pt()*GeV , m_trigEff);

  if (SmearedNotBJet.size()>=2)
    h_PtJets2nd[cutname]->Fill( SmearedNotBJet[1].Pt()*GeV , m_trigEff);

  if (SmearedEleMuo.size()>=1) // Smeared elelctron
    h_angleMuons1st[cutname]->Fill( SmearedEleMuo[0].Eta() , m_trigEff);

  if (SmearedJet.size()>=1) // Smeared elelctron
    h_angleJets1st[cutname]->Fill( SmearedJet[0].Eta() , m_trigEff);

  if (SmearedNotBJet.size()>=1 && SmearedEleMuo.size()>=1) {
    myhisto[cutname]->Fill(SmearedEleMuo[0].Pt()*GeV,SmearedNotBJet[0].Pt()*GeV,m_trigEff);
  }

  // Skipping tauons for a now
  if (SmearedEleMuo.size()>=2) {
    auto mass = (SmearedEleMuo[0] + SmearedEleMuo[1]).M()*GeV;
    h_llmass[cutname]->Fill(mass,m_trigEff);
  }

  if (SmearedNotBJet.size()>=1) {
    auto DPhi = SmearedNotBJet[0].DeltaPhi(m_SmearedMETTLV);
    h_Phi1stJetMet[cutname]->Fill(DPhi,m_trigEff);
  }

  if (SmearedEleMuo.size()>=2) {
    h_mtautau1[cutname]->Fill(m_mtautau1*GeV,m_trigEff);
    h_mtautau2[cutname]->Fill(m_mtautau2*GeV,m_trigEff);
  }
  
  return;
}

EL::StatusCode UpgradeAnalysis::getTruthPhotons(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::TruthParticleContainer* xTruthPhotons = NULL;
  ANA_CHECK( event->retrieve( xTruthPhotons, "TruthPhotons"));
  xAOD::TruthParticleContainer::const_iterator itr;
  for( itr = xTruthPhotons->begin(); itr != xTruthPhotons->end(); itr++){
    if((*itr)->status()!=1 ) continue;
    Particle thisPart;
    thisPart.SetPxPyPzE( (*itr)->px(), (*itr)->py(), (*itr)->pz(), (*itr)->e());
    thisPart.pdgid = (*itr)->pdgId();
    thisPart.etcone20 = (*itr)->auxdata<float>("etcone20");
    thisPart.ptcone30 = (*itr)->auxdata<float>("ptcone30");
    thisPart.Good = true;
    
    // THIS Is very interesting!!!
    // GenEleMuo.push_back(thisPart);
    GenPho.push_back(thisPart);
  }
  return EL::StatusCode::SUCCESS;;
}

EL::StatusCode UpgradeAnalysis::getTruthElectrons(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::TruthParticleContainer* xTruthElectrons = NULL;
  ANA_CHECK( event->retrieve( xTruthElectrons, "TruthElectrons"));
  xAOD::TruthParticleContainer::const_iterator itr;
  for( itr = xTruthElectrons->begin(); itr != xTruthElectrons->end(); itr++){
    if((*itr)->status()!=1 ) continue;
    Particle thisPart;
    thisPart.SetPxPyPzE( (*itr)->px(), (*itr)->py(), (*itr)->pz(), (*itr)->e());
    thisPart.pdgid = (*itr)->pdgId();
    thisPart.etcone20 = (*itr)->auxdata<float>("etcone20");
    thisPart.ptcone30 = (*itr)->auxdata<float>("ptcone30");
    thisPart.Good = true;
    GenEleMuo.push_back(thisPart);
  }
  return EL::StatusCode::SUCCESS;;
}

EL::StatusCode UpgradeAnalysis::getTruthMuons(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::TruthParticleContainer* xTruthMuons = NULL;
  ANA_CHECK( event->retrieve( xTruthMuons, "TruthMuons"));
  xAOD::TruthParticleContainer::const_iterator itr;
  for( itr = xTruthMuons->begin(); itr != xTruthMuons->end(); itr++){
    if((*itr)->status()!=1 ) continue;
    Particle thisPart;
    thisPart.SetPxPyPzE( (*itr)->px(), (*itr)->py(), (*itr)->pz(), (*itr)->e());
    thisPart.pdgid = (*itr)->pdgId();
    thisPart.etcone20 = (*itr)->auxdata<float>("etcone20");
    thisPart.ptcone30 = (*itr)->auxdata<float>("ptcone30");
    thisPart.Good = true;
    GenEleMuo.push_back(thisPart);
  }
  return EL::StatusCode::SUCCESS;;
}

EL::StatusCode UpgradeAnalysis::getTruthHadronicTaus(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::TruthParticleContainer* xTruthTaus = NULL;
  ANA_CHECK( event->retrieve( xTruthTaus, "TruthTaus"));
  xAOD::TruthParticleContainer::const_iterator itr;
  for( itr = xTruthTaus->begin(); itr != xTruthTaus->end(); itr++){
    if( (*itr)->auxdata<char>("IsHadronicTau") != 1 ) continue;
    int n_prong = (*itr)->auxdata<unsigned long>("numChargedPion");
    if( n_prong!=1 && n_prong!=3 ) continue;
    Particle thisPart;
    thisPart.SetPtEtaPhiM( (*itr)->auxdata<double>("pt_vis"), (*itr)->auxdata<double>("eta_vis"), (*itr)->auxdata<double>("phi_vis"), (*itr)->auxdata<double>("m_vis"));
    thisPart.pdgid = (*itr)->pdgId();
    thisPart.nprong = n_prong;
    thisPart.Good = true;
    GenHadTau.push_back(thisPart);
  }
  return EL::StatusCode::SUCCESS;;
}

EL::StatusCode UpgradeAnalysis::getTruthJets(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::JetContainer* xTruthJets = 0;
  ANA_CHECK(event->retrieve( xTruthJets, "AntiKt4TruthJets" ));
  xAOD::JetContainer::const_iterator jet_itr = xTruthJets->begin();
  xAOD::JetContainer::const_iterator jet_end = xTruthJets->end();
  for( ; jet_itr != jet_end; ++jet_itr ) {
    if( (*jet_itr)->pt()*GeV < 10 ) continue;
    // is bjet?
    bool isbjet=false;
    if(  fabs((*jet_itr)->auxdata<int>("ConeTruthLabelID"))==5 ) isbjet=true;

    Particle thisPart;
    thisPart.SetPtEtaPhiM( (*jet_itr)->pt(), (*jet_itr)->eta(), (*jet_itr)->phi(), (*jet_itr)->m());
    thisPart.pdgid = fabs((*jet_itr)->auxdata<int>("ConeTruthLabelID"));
    thisPart.Good = true;
    GenJet.push_back(thisPart);
    
    if( isbjet ) GenBJet.push_back(thisPart);
  }
  return EL::StatusCode::SUCCESS;;
}

EL::StatusCode UpgradeAnalysis::getTruthMET(){
  xAOD::TEvent* event = wk()->xaodEvent();
  const xAOD::MissingETContainer* xTruthMET = NULL;
  ANA_CHECK( event->retrieve( xTruthMET, "MET_Truth"));
  m_GenMETTLV.SetXYZM( (*xTruthMET)["NonInt"]->mpx(), (*xTruthMET)["NonInt"]->mpy(), 0,0); 
  m_GenMETSumet = (*xTruthMET)["NonInt"]->sumet();
  return EL::StatusCode::SUCCESS;;
}

void UpgradeAnalysis::SmearPhotons(){
  SmearedPho.clear();

  for( unsigned int i=0; i<GenPho.size(); i++) {
    //cout << "photon " << GenPho[i].Pt()*GeV << "\t" << GenPho[i].Eta() << endl;
    // smear energy
    double fac = m_upgrade->getPhotonSmearedEnergy(GenPho[i].E(),GenPho[i].Eta())/GenPho[i].E();
    Particle newPhoton = GenPho[i];
    newPhoton.SetPtEtaPhiM(GenPho[i].Pt()*fac,GenPho[i].Eta(),GenPho[i].Phi(),GenPho[i].M());

    // Apply efficiency
    float eff = m_upgrade->getPhotonEfficiency(newPhoton.Et() );
    m_random3.SetSeed( (int)(1e+5*fabs( newPhoton.Phi() ) ) );	
    double x = m_random3.Rndm();
    // if passes eff, keep electron
    if (x < eff) { 
      SmearedPho.push_back(newPhoton);
    }
  }
  return;
}

void UpgradeAnalysis::SmearElectrons(){
  SmearedEleMuo.clear();

  for( unsigned int i=0; i<GenEleMuo.size(); i++) {
    if( fabs(GenEleMuo[i].pdgid)!=11 ) continue;
    //cout << "electron " << GenEleMuo[i].Pt()*GeV << "\t" << GenEleMuo[i].Eta() << endl;
    // smear energy
    double fac = m_upgrade->getElectronSmearedEnergy(GenEleMuo[i].E(),GenEleMuo[i].Eta())/GenEleMuo[i].E();
    Particle newElectron = GenEleMuo[i];
    newElectron.SetPtEtaPhiM(GenEleMuo[i].Pt()*fac,GenEleMuo[i].Eta(),GenEleMuo[i].Phi(),GenEleMuo[i].M());

    // Apply efficiency
    float eff = m_upgrade->getElectronEfficiency(newElectron.Et(), newElectron.Eta() );
    m_random3.SetSeed( (int)(1e+5*fabs( newElectron.Phi() ) ) );	
    double x = m_random3.Rndm();
    // if passes eff, keep electron
    if (x < eff) { 
      SmearedEleMuo.push_back(newElectron);
    }
  }
  return;
}

void UpgradeAnalysis::SmearMuons(){

  for( unsigned int i=0; i<GenEleMuo.size(); i++ ){
    if( fabs(GenEleMuo[i].pdgid)!=13 ) continue;
    //cout << "muon " << GenEleMuo[i].Pt()*GeV << "\t" << GenEleMuo[i].Eta() << endl;
    // smear q/pT to allow for muon charge flip
    double charge =-1;
    if( GenEleMuo[i].pdgid < 0 ) charge=+1; 
    float qoverpt = charge / GenEleMuo[i].Pt();
    float muonQOverPtResolution = m_upgrade->getMuonQOverPtResolution( GenEleMuo[i].Pt(),  GenEleMuo[i].Eta());
    m_random3.SetSeed( (int)(1e+5*fabs( GenEleMuo[i].Phi() ) ) );	
    qoverpt += m_random3.Gaus(0., muonQOverPtResolution);
    float muonSmearedPt = fabs(1./qoverpt);
    Particle newMuon = GenEleMuo[i];
    newMuon.SetPtEtaPhiM( muonSmearedPt, GenEleMuo[i].Eta(), GenEleMuo[i].Phi(), GenEleMuo[i].M() );
    newMuon.pdgid = 13;
    if( qoverpt > 0 ) newMuon.pdgid=-13;
    // Apply efficiency
    float eff = m_upgrade->getMuonEfficiency(newMuon.Pt(), newMuon.Eta() );
    m_random3.SetSeed( (int)(1e+5*fabs( newMuon.Phi() ) ) );	
    double x = m_random3.Rndm();
    // if passes eff, keep muon
    if (x < eff) { 
      SmearedEleMuo.push_back(newMuon);
    }
  }
  return;
}

void UpgradeAnalysis::SmearHadTaus(){
  SmearedHadTau.clear();
  // smear and eff
  for( unsigned int i=0; i<GenHadTau.size(); i++) {
    //cout << "tau " << GenHadTau[i].Pt()*GeV << "\t" << GenHadTau[i].Eta() << endl;
    // smear energy
    double fac = m_upgrade->getTauSmearedEnergy(GenHadTau[i].E(),GenHadTau[i].Eta(), GenHadTau[i].nprong)/GenHadTau[i].E();
    Particle newTau = GenHadTau[i];
    newTau.SetPtEtaPhiM(GenHadTau[i].Pt()*fac,GenHadTau[i].Eta(),GenHadTau[i].Phi(),GenHadTau[i].M());

    if( newTau.Pt()*GeV<20 || fabs(newTau.Eta())>2.5 ) continue;
    // Apply efficiency
    float eff = m_upgrade->getTauEfficiency(newTau.Et(), newTau.Eta(), GenHadTau[i].nprong );
    m_random3.SetSeed( (int)(1e+5*fabs( newTau.Phi() ) ) );	
    double x = m_random3.Rndm();
    // if passes eff, keep electron
    if (x < eff) { 
      SmearedHadTau.push_back(newTau);
    }
  }  
  return;
}

void UpgradeAnalysis::SmearJets(){
  SmearedJet.clear();
  SmearedBJet.clear();

  
  // cout << "+++++++++++++++++++++++++" << endl;
  
  // //Add in pileup jets
  // std::vector<TLorentzVector> pujets;
  // pujets = m_upgrade->getPileupJets();
  // int npiljets = 0;
  // for( unsigned int i = 0; i < pujets.size(); i++) {
  //   Particle pujet;
  //   pujet.SetPtEtaPhiM(pujets[i].Pt(), pujets[i].Eta(), pujets[i].Phi(), pujets[i].M());
  //   if (pujet.Pt() < m_upgrade->getPileupJetPtThresholdMeV() || fabs(pujet.Eta()) > 3) continue;
  //   float trackEff = m_upgrade->getTrackJetConfirmEff(pujet.Pt(), pujet.Eta(), "PU");
  //   m_random3.SetSeed( (int)(1e+5*fabs( pujet.Phi() ) ) );	
  //   float puProb = m_random3.Uniform(1.0);
  //   if (puProb < trackEff){
  //     pujet.Good=true;
  //     pujet.pdgid=-1; //identify PU jets with -1
  //     //SmearedJet.push_back(pujet);
  //     npiljets+=1;
  //   }
  
  //   cout << trackEff << "\t" << pujet.Eta() << endl;
  // }

  // cout << "+++++++++++++++++++++++++" << endl;
  // cout << pujets.size() << endl;
  // cout << npiljets << endl;
  // cout << "+++++++++++++++++++++++++" << endl;

  // smear truth jets
  for( unsigned int i=0; i<GenJet.size(); i++) {
    //cout << "jet " << GenJet[i].Pt()*GeV << "\t" << GenJet[i].Eta() << endl;
    // smear energy
    double smeared_pt = m_upgrade->getJetSmearedEnergy(GenJet[i].Pt(),GenJet[i].Eta(), true);
    Particle newJet = GenJet[i];
    newJet.SetPtEtaPhiM(smeared_pt,GenJet[i].Eta(),GenJet[i].Phi(),GenJet[i].M());

    //cout << "GenPt=" << GenJet[i].Pt()*GeV << " SmearedPt=" << smeared_pt*GeV << endl;
    
    // Apply efficiency
    if (newJet.Pt()*GeV>30.01) {
      float trackEff = m_upgrade->getTrackJetConfirmEff(newJet.Pt(), newJet.Eta(), "HS");
      m_random3.SetSeed( (int)(1e+5*fabs( newJet.Phi() ) ) );	
      float hsProb = m_random3.Uniform(1.0);
      if (hsProb < trackEff){
        SmearedJet.push_back(newJet);
      }
    }
  }

  return;
}

void UpgradeAnalysis::ApplyPhotonFakes(){
  // Apply photon fakes
  for( unsigned int i=0; i<SmearedJet.size(); i++ ){
    float fakeeff = m_upgrade->getPhotonFakeRate(SmearedJet[i].Et());
    m_random3.SetSeed( (int)(1e+5*fabs( SmearedJet[i].Phi() ) ) );	
    float fakeProb = m_random3.Rndm();
    if( fakeProb < fakeeff ){ // add jet to SmearedPho as an photon
      double fac = m_upgrade->getPhotonFakeRescaledEnergy(SmearedJet[i].E()) / SmearedJet[i].E();
      if (fac < 0.) fac = 0.; // protection
      Particle fakePho; 
      fakePho.SetPtEtaPhiM(SmearedJet[i].Pt()*fac, SmearedJet[i].Eta(), SmearedJet[i].Phi(), SmearedJet[i].M() );
      fakePho.pdgid=22;
      fakePho.Good = true;
      SmearedPho.push_back(fakePho);
    }
  }
  return;
}
void UpgradeAnalysis::ApplyElectronFakes(){
  // Apply electron fakes
  for( unsigned int i=0; i<SmearedJet.size(); i++ ){
    float fakeeff = m_upgrade->getElectronFakeRate(SmearedJet[i].Et(), SmearedJet[i].Eta());
    m_random3.SetSeed( (int)(1e+5*fabs( SmearedJet[i].Phi() ) ) );	
    float fakeProb = m_random3.Rndm();
    double fakeCharge = m_random3.Rndm();
    if( fakeProb < fakeeff ){ // add jet to SmearedEleMuo as an electron
      double fac = m_upgrade->getElectronFakeRescaledEnergy(SmearedJet[i].E(), SmearedJet[i].Eta()) / SmearedJet[i].E();
      if (fac < 0.) fac = 0.; // protection
      Particle fakeEle; 
      fakeEle.SetPtEtaPhiM(SmearedJet[i].Pt()*fac, SmearedJet[i].Eta(), SmearedJet[i].Phi(), SmearedJet[i].M() );
      fakeEle.pdgid=-11;
      if( fakeCharge>0.5 ) fakeEle.pdgid=+11;
      fakeEle.Good = true;
      SmearedEleMuo.push_back(fakeEle);
    }
  }
  return;
}
void UpgradeAnalysis::ApplyTauFakes(){
  // Apply tau fake rates
  for( unsigned int i=0; i<SmearedJet.size(); i++ ){
    if( SmearedJet[i].Pt()*GeV < 20 || fabs(SmearedJet[i].Eta())>2.5 ) continue; // pT and eta thresholds for FakeRate
    // decide on nprong for the jet 
    m_random3.SetSeed( (int)(1e+5*fabs( SmearedJet[i].Phi() ) ) );
    double x = m_random3.Rndm();
    int fake_nprong = 3;
    if( x < 0.77 ) fake_nprong=1; 
    float fakeeff = m_upgrade->getTauFakeRate(SmearedJet[i].Et(), SmearedJet[i].Eta(), fake_nprong);
    m_random3.SetSeed( (int)(1e+5*fabs( SmearedJet[i].Phi() ) ) );
    float fakeProb = m_random3.Rndm();
    double fakeCharge = m_random3.Rndm();
    if( fakeProb < fakeeff ){ // add jet to SmearedHadTau
      double fac = 1.;// no function right now to rescale the energy
      if (fac < 0.) fac = 0.; // protection
      Particle fakeTau; 
      fakeTau.SetPtEtaPhiM(SmearedJet[i].Pt()*fac, SmearedJet[i].Eta(), SmearedJet[i].Phi(), SmearedJet[i].M() );
      fakeTau.pdgid=-15;
      if( fakeCharge>0.5 ) fakeTau.pdgid=+15;
      fakeTau.nprong = fake_nprong;
      fakeTau.Good = true;
      SmearedHadTau.push_back(fakeTau);
    }
  }
  return;
}

void UpgradeAnalysis::ApplyBtagging(){
  // emulate the b-tagging efficiency
  for( unsigned int i=0; i<SmearedJet.size(); i++ ){
    double btag_eff;
    if( SmearedJet[i].pdgid == 5 ) btag_eff = m_upgrade->getFlavourTagEfficiency(SmearedJet[i].Pt(), SmearedJet[i].Eta(), 'B', "mv1");
    else if( SmearedJet[i].pdgid == 4 ) btag_eff = m_upgrade->getFlavourTagEfficiency(SmearedJet[i].Pt(), SmearedJet[i].Eta(), 'C', "mv1");
    else if( SmearedJet[i].pdgid == -1 ) btag_eff = m_upgrade->getFlavourTagEfficiency(SmearedJet[i].Pt(), SmearedJet[i].Eta(), 'P', "mv1");
    else btag_eff = m_upgrade->getFlavourTagEfficiency(SmearedJet[i].Pt(), SmearedJet[i].Eta(), 'L', "mv1");
    m_random3.SetSeed( (int)(1e+5*fabs( SmearedJet[i].Phi() ) ) );	
    double x = m_random3.Rndm();
    if (x < btag_eff) SmearedJet[i].pdgid=5;
    else SmearedJet[i].pdgid=0;
  }
  return;
}

void UpgradeAnalysis::SmearMET(){

  UpgradePerformanceFunctions::MET smearMET = m_upgrade->getMETSmeared( m_GenMETSumet, m_GenMETTLV.Px(), m_GenMETTLV.Py());
  m_SmearedMETTLV.SetPxPyPzE(smearMET.first,smearMET.second,0.,TMath::Sqrt(smearMET.first*smearMET.first + smearMET.second*smearMET.second)); // 

  return;
}

void UpgradeAnalysis::ApplyPtEtaThresholds(){

  // photons pT>20 and eta < 2.47
  // for( unsigned int i=0; i<SmearedPho.size(); i++ ){
  //   if( SmearedPho[i].Pt()*GeV < 50 || fabs(SmearedPho[i].Eta())>2.47 ) SmearedPho[i].Good=false;
  // }
  // electrons and muons pT>10 and eta < 2.47/2.5
  // pdgid - ???
  // Segfault problem
  for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
    if( fabs(SmearedEleMuo[i].pdgid) ==11 && (SmearedEleMuo[i].Pt()*GeV < 5. || fabs(SmearedEleMuo[i].Eta())>2.47) ) SmearedEleMuo[i].Good=false;
    if( fabs(SmearedEleMuo[i].pdgid) ==13 && (SmearedEleMuo[i].Pt()*GeV < 5. || fabs(SmearedEleMuo[i].Eta())>2.5) ) SmearedEleMuo[i].Good=false;
  }
  // hadronic taus pT>20 and eta < 2.47
  // for( unsigned int i=0; i<SmearedHadTau.size(); i++ ){
  //   if( SmearedHadTau[i].Pt()*GeV < 20 || fabs(SmearedHadTau[i].Eta())>2.47 ) SmearedHadTau[i].Good=false;
  // }
  // jets pT>20 and eta < 2.5
  for( unsigned int i=0; i<SmearedJet.size(); i++ ){
    if( SmearedJet[i].Pt()*GeV < 50. || fabs(SmearedJet[i].Eta())>2.8 ) SmearedJet[i].Good=false;
  }
  // for( unsigned int i=0; i<SmearedBJet.size(); i++ ){
  //   if( SmearedBJet[i].Pt()*GeV < 20 || fabs(SmearedBJet[i].Eta())>2.5 ) SmearedBJet[i].Good=false;
  // }
//  RemoveBad(SmearedPho);
  RemoveBad(SmearedEleMuo);
//  RemoveBad(SmearedHadTau);
  RemoveBad(SmearedJet);
//  RemoveBad(SmearedBJet);
  return;
}

void UpgradeAnalysis::OverlapRemoval(){

  // apply OR
  // 1. dR(tau, loose ele/muo)<0.2, discard tau
  // for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
  //   for( unsigned int j=0; j<SmearedHadTau.size(); j++ ){
  //     if( SmearedEleMuo[i].DeltaR(SmearedHadTau[j])<0.2) SmearedHadTau[j].Good=false;
  //   }
  // }
  // RemoveBad(SmearedHadTau);
  // // 1. dR(photon, loose ele/muo)<0.4, discard photon
  // for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
  //   for( unsigned int j=0; j<SmearedPho.size(); j++ ){
  //     if( SmearedEleMuo[i].DeltaR(SmearedPho[j])<0.4) SmearedPho[j].Good=false;
  //   }
  // }
  // RemoveBad(SmearedPho);
  // // 3. dR(ele, non b-tagged jet)<0.2, discard non b-tagged jet
  // for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
  //   if( fabs(SmearedEleMuo[i].pdgid) != 11 ) continue;
  //   for( unsigned int j=0; j<SmearedJet.size(); j++ ){
  //     if( SmearedJet[i].pdgid==5 ) continue;
  //     if( SmearedEleMuo[i].DeltaR(SmearedJet[j])<0.2) SmearedJet[j].Good=false;
  //   }
  // }
  //RemoveBad(SmearedJet);
  // 4. dR(ele,jet)<0.4, discard ele
  for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
    if( fabs(SmearedEleMuo[i].pdgid) != 11 ) continue;
    for( unsigned int j=0; j<SmearedJet.size(); j++ ){
      if( SmearedEleMuo[i].DeltaR(SmearedJet[j])<0.4) SmearedEleMuo[i].Good=false;
    }
  }
  RemoveBad(SmearedEleMuo);
  // 5. dR(muon, non b-tagged jet)<0.2, discard non-btagged jet if mu pT/jet pT ratio >0.5
  for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
    if( fabs(SmearedEleMuo[i].pdgid) != 13 ) continue;
    for( unsigned int j=0; j<SmearedJet.size(); j++ ){
      if( SmearedJet[i].pdgid==5 ) continue;
      if( SmearedEleMuo[i].DeltaR(SmearedJet[j])>0.2) continue;
      if( SmearedEleMuo[i].Pt()/SmearedJet[j].Pt()>0.5 ) SmearedJet[j].Good=false;
    }
  }
  RemoveBad(SmearedJet);
  // 6. dR(muon, jet)<0.4, discard muon
  for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
    if( fabs(SmearedEleMuo[i].pdgid) != 13 ) continue;
    for( unsigned int j=0; j<SmearedJet.size(); j++ ){
      if( SmearedEleMuo[i].DeltaR(SmearedJet[j])<0.4) SmearedEleMuo[i].Good=false;
    }
  }
  RemoveBad(SmearedEleMuo);
  // // 7. dR(tau, jet)<0.2, discard jet
  // for( unsigned int i=0; i<SmearedHadTau.size(); i++ ){
  //   for( unsigned int j=0; j<SmearedJet.size(); j++ ){
  //     if( SmearedHadTau[i].DeltaR(SmearedJet[j])<0.2) SmearedJet[j].Good=false;
  //   }
  // }
  // RemoveBad(SmearedJet);
  // // 8. dR(pho, jet)<0.2, discard jet
  // for( unsigned int i=0; i<SmearedPho.size(); i++ ){
  //   for( unsigned int j=0; j<SmearedJet.size(); j++ ){
  //     if( SmearedPho[i].DeltaR(SmearedJet[j])<0.4) SmearedJet[j].Good=false;
  //   }
  // }
  // RemoveBad(SmearedJet);
  return;
}

void UpgradeAnalysis::ApplyIsolation(){

  // Basic isolation of etcone20 and ptcone30 to be < 15% of the lepton pT
  // probably needs review for each analysis

  for( unsigned int i = 0; i<SmearedEleMuo.size(); i++){
    if( SmearedEleMuo[i].etcone20/SmearedEleMuo[i].Pt() > 0.15 ) SmearedEleMuo[i].Good=false;
    if( SmearedEleMuo[i].ptcone30/SmearedEleMuo[i].Pt() > 0.15 ) SmearedEleMuo[i].Good=false;
  }
  RemoveBad(SmearedEleMuo);
  for( unsigned int i = 0; i<SmearedPho.size(); i++){
    if( SmearedPho[i].etcone20/SmearedPho[i].Pt() > 0.15 ) SmearedPho[i].Good=false;
    if( SmearedPho[i].ptcone30/SmearedPho[i].Pt() > 0.15 ) SmearedPho[i].Good=false;
  }
  RemoveBad(SmearedPho);
  return;
}

void UpgradeAnalysis::RemoveLowMassPairs(){

  // discard SFOS leptons with mll<12 GeV
  for( unsigned int i=0; i<SmearedEleMuo.size(); i++ ){
    for( unsigned int j=0; j<SmearedEleMuo.size(); j++ ){
      if( i==j ) continue;
      if( SmearedEleMuo[i].pdgid*SmearedEleMuo[j].pdgid!=-121 && SmearedEleMuo[i].pdgid*SmearedEleMuo[j].pdgid!=-169 ) continue;
      TLorentzVector LL; LL = SmearedEleMuo[i]+SmearedEleMuo[j];
      if( LL.M()*GeV>12 ) continue;
      SmearedEleMuo[i].Good=false; SmearedEleMuo[j].Good=false;
    }
  }
  RemoveBad(SmearedEleMuo);
  return;
}

void UpgradeAnalysis::ApplyTriggerEfficiency(){

  //set to 100% as default
  m_trigEff = 1.;

  return;
}

void UpgradeAnalysis::calculateEventVariables(){

  // met
  m_met = m_SmearedMETTLV.Et();

  // find the mass of the SFOS lepton pair closest to Z mass
  double mlldZ = 999999; m_mll=9999999999; m_gotSFOS=false; 
  for( unsigned int i = 0; i < SmearedEleMuo.size(); i++) {
    for( unsigned int j = 0; j < SmearedEleMuo.size(); j++) {
      if( i==j ) continue;
      TLorentzVector LL; LL = SmearedEleMuo[i]+SmearedEleMuo[j];
      double Mll = LL.M();
      if(  SmearedEleMuo[i].pdgid*SmearedEleMuo[j].pdgid !=-121 && SmearedEleMuo[i].pdgid*SmearedEleMuo[j].pdgid !=-169 ) continue;
      m_gotSFOS=true;
      if( fabs(Mll - 91200) < mlldZ ){ mlldZ = fabs(Mll - 91200); m_mll=Mll;}
    }
  }

  // transverse mass using leading lepton 
  m_mT = -9999999999;
  if( SmearedEleMuo.size()>0 ) m_mT = M_T(SmearedEleMuo[0], m_SmearedMETTLV);

  // sum of lepton pT
  m_lt=0.;
  for( unsigned int i = 0; i < SmearedEleMuo.size(); i++) m_lt += SmearedEleMuo[i].Pt();
  // sum of tau pT
  double sumtauPt=0.;
  for( unsigned int i = 0; i < SmearedHadTau.size(); i++) sumtauPt += SmearedHadTau[i].Pt();

  // sum jet (40 GeV) pT
  m_ht = 0.;
  for( unsigned int i = 0; i < SmearedJet.size(); i++){
    if( SmearedJet[i].Pt()*GeV<40 ) continue;
    m_ht += SmearedJet[i].Pt();
  }

  // effective mass
  m_meff = m_met + m_lt + sumtauPt + m_ht;

  // stransverse mass using the two leading leptons
  if( SmearedEleMuo.size() >=2 ) m_mT2=getMT2(SmearedEleMuo[0], SmearedEleMuo[1], m_SmearedMETTLV );

  // Mtautau masses
  if( SmearedEleMuo.size() >=2) {
    m_mtautau1 = (SmearedEleMuo[0] + SmearedEleMuo[1] + m_SmearedMETTLV).M();
    double mll = (SmearedEleMuo[0] + SmearedEleMuo[1]).M();
    double x1 = SmearedEleMuo[0].Pt()/(SmearedEleMuo[0].Pt() + m_SmearedMETTLV.Pt());
    double x2 = SmearedEleMuo[1].Pt()/(SmearedEleMuo[1].Pt() + m_SmearedMETTLV.Pt());
    m_mtautau2 = mll/TMath::Sqrt(x1*x2);
  }

  return;
}


/* =========================================================================================================================== */
float getMT2(Particle p1, Particle p2, TLorentzVector met, double invisiblemass_1, double invisiblemass_2)
{
  TLorentzVector pA = p1;
  TLorentzVector pB = p2;
  ComputeMT2 mycalc = ComputeMT2(pA,pB,met, invisiblemass_1, invisiblemass_2);
  
  return mycalc.Compute();  
}


// Transverse mass
double M_T(TLorentzVector vec1, TLorentzVector vec2) {
  float mt = (vec1.Mt() + vec2.Mt())*(vec1.Mt() + vec2.Mt()) - (vec1+vec2).Perp2();
  mt = (mt >= 0.) ? sqrt(mt) : sqrt(-mt);
  return mt;
}


void RemoveBad(vector<Particle> &vec1) {
  vector<Particle> Survivors;
  for(vector<Particle>::iterator it1 = vec1.begin(); it1 != vec1.end(); ++it1) {
    if( it1->Good == false) continue;
    Survivors.push_back( *it1 );
  }
  vec1=Survivors;
  return;
}



