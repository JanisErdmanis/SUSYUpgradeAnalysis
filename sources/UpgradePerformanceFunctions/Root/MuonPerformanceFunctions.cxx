#ifndef MUONPERFORMANCEFUNCTIONS_CXX
#define MUONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setMuonWorkingPoint(MuonCutLevel cutLevel) {
  m_muonCutLevel = cutLevel;
}

float UpgradePerformanceFunctions::muonEtaTriggerEfficiency(float eta) {

    // rpc L0 efficiency data  22 bins for 0<eta<1.1
    const float eta_bin = 0.05;
    const float eff_gold[22] = {0.790656, 0.930483, 0.98033, 0.992508, 0.974555, 0.981241, 0.985142, 0.947444, 0.960144, 0.98223, 0.983938, 0.984972, 0.972907, 0.982902, 0.919753, 0.899409, 0.970952, 0.960322, 0.946016, 0.868755, 0.619748,0};
    const float eff_silver[22] = {0.466976, 0.640636, 0.78031, 0.875155, 0.786891, 0.817936, 0.857857, 0.735092, 0.614015, 0.732087, 0.820762, 0.799978, 0.911093, 0.971162, 0.911653, 0.899409, 0.970952, 0.960322, 0.946016, 0.868755, 0.619748, 0 };
    const float eff_bronze[22] = {0.466303, 0.638691, 0.776874, 0.874595, 0.785156, 0.816267, 0.856739, 0.73399, 0.610921, 0.730391, 0.820209, 0.798315, 0.82698, 0.716125, 0.565287, 0.486861, 0.529895, 0.562725, 0.48137, 0.238961, 0.116867, 0};
    //=======
    
    float eff = 0.98*0.98; //TGC*MDT efficiency
    if (fabs(eta)>2.4) return 0.;
    if (fabs(eta)>1.05) {
        if ( m_layout == LoI || m_layout == LoIVF) eff=0.86; //obsolete numbers
        if ( m_layout == bronze) eff*=0.98; //additional 2% loss from L1Track
    } else {
        int ibin=fabs(eta)/eta_bin;
        float eff_rpc= eff=0.70; //obsolete numbers
        if ( m_layout == gold ) {
            eff_rpc=eff_gold[ibin];
        } else if   ( m_layout == silver ) {
            eff_rpc=eff_silver[ibin];
        } else if (m_layout == bronze) {
            eff_rpc=eff_bronze[ibin];
        } 
        
        if ( m_layout == gold )   eff=eff_rpc*0.98; //RPC recovery with BI RPC chambers
        if ( m_layout == silver ) eff=eff_rpc*0.98; //only partial recovery with BI RPC chambers
        if ( m_layout == bronze ) eff=eff_rpc*0.98*0.98; //inefficient RPC, and additional 2% L1Track inefficiency
    }
    return eff;
}

float UpgradePerformanceFunctions::getSingleMuonTriggerEfficiency(float etMeV, float eta) {
  //single-mu trigger efficiency w.r.t. reconstruction efficiency (tight=true)
  //using 2012 values from K. Nagano
  float minPt=25000.;
  if ( m_layout == gold ) minPt=20000.;
  if (etMeV > minPt) return muonEtaTriggerEfficiency(eta);
  return 0.;
}

float UpgradePerformanceFunctions::getDiMuonTriggerEfficiency(float et1MeV, float et2MeV, float eta1, float eta2) {
  float eff=muonEtaTriggerEfficiency(eta1)*muonEtaTriggerEfficiency(eta2);
  float minPt=15000.;
  if (m_layout == gold) minPt=11000.;
  
  // Result is product of individual muon efficiencies 
  if (et1MeV > minPt && et2MeV > minPt) return eff;
  return 0.;
}

float UpgradePerformanceFunctions::getElectronMuonTriggerEfficiency(float elecEtMeV, float muonEtMeV, float elecEta, float muonEta) {
  float minPt = 15000.;
  float muonEff = muonEtaTriggerEfficiency(muonEta);
  float elecEff = 0.95;
  if (m_layout == bronze) elecEff = 0.91;
  if (fabs(elecEta) > 2.5) elecEff = 0.;
  // Result is product of individual efficiencies 
  if (elecEtMeV > minPt && muonEtMeV > minPt) return muonEff*elecEff;
  return 0.;
}


float UpgradePerformanceFunctions::getMuonEfficiency(float ptMeV, float eta) {
  // Returns the muon efficiency for the specified eta region. 
  // For 'tight=true' this corresponds to 0.97 everywhere, except the acceptance gap at eta~0
  // For 'tight=false' (i.e. loose muons) is 0.99 everywhere, except the acceptance gap at eta~0*/
  //M.Corradi: NEW VERSION FROM 30/7/2013
  // using 2012 values from M. Goblirsch

    if (ptMeV < 4000.|| fabs(eta)>4.0) return 0;

    double max_eta =2.7;
    if  (m_layout == gold || m_layout == LoIVF ) max_eta=4.0;
    
    // large eta extension
    if (fabs(eta)>2.7){
        if (m_layout == gold || m_layout == LoIVF )
        {
            if (m_muonCutLevel == looseMuon){ // large-eta extension
                return 0.99;
            } else {
                return 0.95;
            }
        }
        return 0;
    }
    
    if (m_muonCutLevel == looseMuon) {  // loose selection : CB + ST + calo tagged for |eta|<0.1 + SA for 2.5<|eta|<2.7
      return 0.99; 
    } else if (m_muonCutLevel == tightMuon) { // CB "tight" selection
        if (fabs(eta)<0.1) {
            return 0.54;
        } else if (fabs(eta)<max_eta) {
            return 0.97;
        } else {
            return 0;
        }
    }
    std::cout << "getMuonEfficiency: Invalid working point." << std::endl;
    return 0.;
}

float UpgradePerformanceFunctions::getMuonPtResolution(float ptMeV, float eta) {
  // All of the coefficients expect pT in GeV, so we convert the input and output
  float ptGeV = ptMeV/1000.;

  // ID resolution parameters, for use only if ITK resolution not available //
  // double a1_barrel(0.01607);
  // double a2_barrel(0.000307); // unit: 1/GeV
  
  // double a1_endcap(0.030);
  // double a2_endcap(0.000387); // unit: 1/GeV
  
  // double a1, a2;
  
  // MS resolution parameters //

  double b0_barrel(0.24); // unit: GeV
  double b1_barrel(0.02676);
  double b2_barrel(0.00012); // unit: 1/GeV
  
  double b0_endcap(0.0); // unit: GeV
  double b1_endcap(0.0388);
  double b2_endcap(0.00016); // unit: 1/GeV
  
  double b0, b1, b2;

  // resolution components //
  double sigma_ID, sigma_MS, sigma_CB;
    
  ///////////////////////////////////////
  // CALCULATE THE MOMENTUM RESOLUTION //
  ///////////////////////////////////////

  
  sigma_ID = getTrackPtResolution(ptMeV, eta) / 1000.;
  //sigma_ID = 10.;  


  if (fabs(eta)>2.7) { // large eta tagger momentum measurement comes from ITK
      return sigma_ID*1000;
  }
  
  
  // a parameters are for ID, b parameters are for MS
  if (fabs(eta)<1.05) {
    //    a1 = a1_barrel;
    //    a2 = a2_barrel;
    
    b0 = b0_barrel;
    b1 = b1_barrel;
    b2 = b2_barrel;
  } else {
    // a1 = a1_endcap;
    // a2 = a2_endcap;
    
    b0 = b0_endcap;
    b1 = b1_endcap;
    b2 = b2_endcap;
  }

  sigma_MS = ptGeV*sqrt((b0/ptGeV)*(b0/ptGeV)+
			b1*b1+
			(b2*ptGeV)*(b2*ptGeV));

  
  // Combine ID and MS to get combined resolution CB
  
  if (sigma_ID > 0.) {
    sigma_CB = sigma_ID*sigma_MS/sqrt(sigma_ID*sigma_ID+sigma_MS*sigma_MS);
  } else {
    sigma_CB = sigma_MS;
  }
  return sigma_CB*1000.; // back to MeV
}

float  UpgradePerformanceFunctions::getMuonQOverPtResolution(float ptMeV, float eta){
    // This is resolution of q/p_T :
    return getMuonPtResolution(ptMeV, eta)/ptMeV/ptMeV;
}



#endif
