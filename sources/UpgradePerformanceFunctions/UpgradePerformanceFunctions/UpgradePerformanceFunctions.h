#ifndef UPGRADEPERFORMANCEFUNCTIONS_H
#define UPGRADEPERFORMANCEFUNCTIONS_H

#include <iostream>
#include <string>
#include <math.h>

#include "TF1.h"
#include "TRandom3.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TTree.h"

class UpgradePerformanceFunctions {

 public:

  //  typedef enum {LoI, LoIVF} UpgradeLayout;
  enum UpgradeLayout {LoI, LoIVF, gold, silver, bronze};

  enum ElectronCutLevel {looseElectron, tightElectron}; 
  enum PhotonCutLevel{loosePhoton, tightPhoton}; 
  enum MuonCutLevel {looseMuon, tightMuon}; 
  enum METSyst {nominal, process, thresholdUp, thresholdDown, resoUp, resoDown}; 
  enum PileupEff{HS,PU};

  UpgradePerformanceFunctions();
  UpgradePerformanceFunctions(UpgradeLayout layout, double avgMu);
  ~UpgradePerformanceFunctions() {};

  void setLayout(UpgradeLayout layout);
  void setAvgMu(double avgMu);
  UpgradeLayout getLayout();
  double getAvgMu();

  // Electron performance functions
  void setElectronWorkingPoint(ElectronCutLevel cutLevel);
  void setElectronRandomSeed(unsigned seed);
  float getSingleElectronTriggerEfficiency(float ptMeV, float eta);
  float getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2);
  float getElectronEfficiency(float ptMeV, float eta);
  float getElectronFakeRate(float ptMeV, float eta);
  float getElectronEnergyResolution(float eMeV, float eta);
  float getElectronSmearedEnergy(float eMeV, float eta);
  float getElectronFakeRescaledEnergy(float eMeV, float eta); 
  float interpolateElectronIDMap(float *ptEtaMap,float pT,float eta);
  float interpolateElectronResolutionMap(float *ptEtaMap,float pT,float eta);

  // Photon performance functions
  void setPhotonWorkingPoint(PhotonCutLevel cutLevel);
  void setPhotonRandomSeed(unsigned seed);
  float getSinglePhotonTriggerEfficiency(float etMeV, float eta);
  float getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2);
  float getPhotonEfficiency(float ptMeV);
  float getPhotonFakeRate(float ptMeV);
  float getPhotonEnergyResolution(float eMeV, float eta);
  float getPhotonSmearedEnergy(float eMeV, float eta);
  TLorentzVector getPhotonSmearedVector(const TLorentzVector* init);
  float getPhotonFakeRescaledEnergy(float eMeV);

  // Muon performance functions
  void setMuonWorkingPoint(MuonCutLevel cutLevel);
  float getSingleMuonTriggerEfficiency(float etMeV, float eta);
  float getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2);
  float getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta);
  float getMuonEfficiency(float ptMeV, float eta);
  float getMuonPtResolution(float ptMeV, float eta);
  float getMuonQOverPtResolution(float ptMeV, float eta);

  // Tracking performance functions
  float getTrackPtResolution(float ptMeV, float eta);
  float getTrackInvPtResolution(float invptMeV, float eta);
  float getTrackPtRelativeResolution(float ptMeV, float eta);

  // Tau performance functions
  void setTauRandomSeed(unsigned seed);
  float getSingleTauTriggerEfficiency(float etMeV, float eta, short prong);
  float getDiTauTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2, short prong1, short prong2);
  float getTauEfficiency(float etMeV, float eta, short prong);
  float getTauFakeRate(float etMeV, float eta, short prong);
  float getTauEnergyResolution(float eMeV, float eta, short prong);
  float getTauSmearedEnergy(float eMeV, float eta, short prong);

  // Jet performance functions
  void setJetRandomSeed(unsigned seed);
  float getJetSmearedEnergy(float ptMeV, float eta, bool useJetArea);
  float getJetEnergyResolution(float ptMeV, float eta, bool useJetArea);
  float getJetPtThresholdForFakeRate(float eta, float fakeRate, bool useTrackConfirmation);

  // Missing ET performance functions
  void setMETRandomSeed(unsigned seed);
  void loadMETHistograms(TString filename);
  typedef std::pair<double,double> MET;
  MET getMETSmeared(float sumEtMeV, float METxMeV, float METyMeV, METSyst systValue=nominal);
  float getMETResolution(float sumEtMeV, METSyst systValue=nominal);  

  // Flavour-tagging performance functions
  void setFlavourTaggingCalibrationFilename(TString flavourTaggingCalibrationFilename);
  float getFlavourTagEfficiency(float ptMeV, float eta, char flavour, TString tagger = "mv1", int operating_point = 70);

  // Pile-up overlay functions
  void setPileupRandomSeed(unsigned seed);
  void setPileupEfficiencyScheme(PileupEff puscheme);
  void setPileupEff(float hseff = 0.9); // set efficiency either for HS or PU depending on your scheme 
  // void setPileupHSForwardEff(float hseff = 0.9); --- obsolete function
  void setPileupUseTrackConf(bool usetr = false); // TC yes/no - user's choice
  void setPileupJetPtThresholdMeV(float ptThresholdMeV=30000.); // default approved minimum of 30 GeV
  void setPileupTemplatesPath(TString pileupTemplateFilenamePath); // user provides folder with PULibraries

  bool getPileupTrackConfSetting();
  float getPileupJetPtThresholdMeV();
  std::vector<TLorentzVector> getPileupJets();
  float getTrackJetConfirmEff(float ptMeV, float eta, TString jetType);

 private:

  // Member variables
  UpgradeLayout m_layout;
  double m_avgMu;
  ElectronCutLevel m_eleCutLevel;
  PhotonCutLevel m_photonCutLevel;
  MuonCutLevel m_muonCutLevel;
  PileupEff m_pueff;

  float m_BronzeEtaMax = 2.4;
  float m_SilverEtaMax = 3.2;
  float m_GoldEtaMax = 3.8;
  static const int m_nPUPtBins = 4;

  float m_fEff;
  bool m_bUseTrackConf;
  float fEffNominalArray[m_nPUPtBins];
  float fEffMediumArray[m_nPUPtBins];
  float fEffExtendedArray[m_nPUPtBins];

  //  float m_fHSEffForw; obsolete
  /* bool m_bHS70; obsolete*/
  /* bool m_bHS80; obsolete*/
  /* bool m_bHS90; obsolete*/
  // bool m_bUseTrackConf; obsolete
  // bool m_bUseLetf; obsolete
  // bool m_bUseMedTC; obsolete
  
  float m_fPileupJetThresholdMeV;
  float m_fPileupTCMaxPtMeV;
  
  // Default TRandom3 seed is 4357
  TRandom3 m_electronRandom;
  TRandom3 m_photonRandom;
  TRandom3 m_tauRandom;
  TRandom3 m_jetRandom;
  TRandom3 m_METRandom;
  TRandom3 m_pileupRandom;

  // Missing ET templates
  TH1F  *m_SumEtH[4][6];
  TGraphErrors *m_Ress[4];

  // Flavour tagging filename
  TString m_flavourTaggingCalibrationFilename;

  // Pileup overlay templates
  TTree *m_pileupTree30;
  std::vector<float> *m_pileupPt30,*m_pileupEta30,*m_pileupPhi30,*m_pileupE30;

  float muonEtaTriggerEfficiency(float eta);

  //  void setPileupLETFTrackConf(bool bletf = false); // obsolete, defined by the Layout type
  // void setPileupUseMediumTracker(bool bmedtc = false); // obsolete, defined by the Layout type
  void initializePileupTemplates(TString pileupTemplateFilename); // needs to be called by "setAvgMu" - PU Libraries only for mu 140 and 200 so far



};

#endif
