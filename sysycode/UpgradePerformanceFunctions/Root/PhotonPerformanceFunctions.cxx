#ifndef PHOTONPERFORMANCEFUNCTIONS_CXX
#define PHOTONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setPhotonWorkingPoint(PhotonCutLevel cutLevel) {
  m_photonCutLevel = cutLevel;
}

void UpgradePerformanceFunctions::setPhotonRandomSeed(unsigned seed) {
  m_photonRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getSinglePhotonTriggerEfficiency(float etMeV, float eta) {
  if (etMeV<120000. || fabs(eta)>2.4 ) {
    std::cout << "WARNING: no trigger efficiency is returned for PT<120 GeV or eta>2.4 since it is assumed this will be the lowest photon trigger threshold !" << std::endl;
    return 0.;
  }
  return 1.0;
}

float UpgradePerformanceFunctions::getDiPhotonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) {
  if (et1MeV<25000. || et2MeV<25000. || fabs(eta1)>2.4 || fabs(eta2)>2.4) {
    std::cout << "WARNING: no trigger efficiency is returned for PT<25 GeV or eta >2.4 since it is assumed this will be the lowest diphoton trigger threshold !" << std::endl;
    return 0.;
  }
  return 1.00;
}

float UpgradePerformanceFunctions::getPhotonEfficiency(float ptMeV) {
  if (m_photonCutLevel == loosePhoton) {
    std::cout << "getPhotonEfficiency: No efficiency for loosePhotons" << std::endl;
    return 0.0;
  } else if (m_photonCutLevel == tightPhoton) {
    return 0.76 - 1.98*exp(-1.0*ptMeV/16100.);
  }
  std::cout << "getPhotonEfficiency: Invalid working point." << std::endl;
  return 0.0;
}

float UpgradePerformanceFunctions::getPhotonFakeRate(float ptMeV) {
  if (m_photonCutLevel == loosePhoton) {
    return 0.99;
  } else if (m_photonCutLevel == tightPhoton) {
    return 9.3E-3*exp(-1.0*ptMeV/27500.);
  }
  std::cout << "getPhotonFakeRate: Invalid working point." << std::endl;
  return 0.0;
}

float UpgradePerformanceFunctions::getPhotonEnergyResolution(float eMeV, float eta) {
//   const double ET_pu=0.6; //0.6 GeV in ET for mu=80,in a trigger tower, after pu suppression; supposed identical for a cluster
//   float S=0;
//   float C=0;
//   if (fabs(eta)<1.4) {
//     S = 0.10;
//     C = 0.010;
//   }
//   else {
//     S = 0.15;
//     C = 0.015;
//   }
//   
//   float eGeV = eMeV/1000.;
//   double sigma2 = 0.3*0.3 + ET_pu*ET_pu*cosh(eta)*cosh(eta)*m_avgMu/80. + S*S*eGeV + C*C*eGeV*eGeV;
//   return sqrt(sigma2)*1000.;
  
  //--- use the same energy resolution as for electrons
  float energyResolution = getElectronEnergyResolution(eMeV, eta);
  return energyResolution;
  
  
}







float UpgradePerformanceFunctions::getPhotonSmearedEnergy(float eMeV, float eta) {
  double deltaE = m_photonRandom.Gaus(0, getPhotonEnergyResolution(eMeV, eta));
  return (eMeV+deltaE);
}

TLorentzVector UpgradePerformanceFunctions::getPhotonSmearedVector(const TLorentzVector* init) {
  double eta = init->Eta();
  double phi = init->Phi();
  double EMeV = init->E();
  double EGeV = init->E()/1000.; // needed for angular smearing

  const double sigma_theta_cst=0.080;//conservatively,from Fig 12 page 51  in  http://arxiv.org/abs/0901.0512 
  double sigma_eta = cosh(eta)*sigma_theta_cst/sqrt(EGeV);
  double eta_sm = eta + m_photonRandom.Gaus(0.,sigma_eta);

  const double sigma_phi_cst = 0.010; //(from Fig 10 page 51 of http://arxiv.org/abs/0901.0512)
  double sigma_phi = sigma_phi_cst/sqrt(EGeV);
  double phi_sm = phi + m_photonRandom.Gaus(0.,sigma_phi);

  float E_sm = getPhotonSmearedEnergy(EMeV, eta_sm);
  double pt_sm = E_sm/cosh(fabs(eta_sm));

  TLorentzVector return_vec;
  return_vec.SetPtEtaPhiE(pt_sm, eta_sm, phi_sm, E_sm);
  return return_vec;
}

float UpgradePerformanceFunctions::getPhotonFakeRescaledEnergy(float eMeV) {
  return (eMeV*(m_photonRandom.Gaus(0.74,0.126)));
}

#endif
