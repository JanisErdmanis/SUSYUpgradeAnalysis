#ifndef ELECTRONPERFORMANCEFUNCTIONS_CXX
#define ELECTRONPERFORMANCEFUNCTIONS_CXX

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

void UpgradePerformanceFunctions::setElectronWorkingPoint(ElectronCutLevel cutLevel) {
  m_eleCutLevel = cutLevel;
}

void UpgradePerformanceFunctions::setElectronRandomSeed(unsigned seed) {
  m_electronRandom.SetSeed(seed);
}

float UpgradePerformanceFunctions::getSingleElectronTriggerEfficiency(float ptMeV, float eta) {
  float minPt = 22000.;
  float minPtHighEta = 35000;
  float maxEta = 4.0;
  float eff = 0.95;
  float effHighEta = 0.90;
  if ( m_layout == LoI ) {
    minPt = 25000.;
    maxEta = 2.5;
    eff = 0.88;
  } else if ( m_layout == LoIVF ) {
    minPt = 25000.;
    eff = 0.88;
    effHighEta = 0.88;
  } else if ( m_layout == silver ) {
    minPt = 28000.;
    minPtHighEta = 40000.;
    maxEta = 3.2;
  } else if ( m_layout == bronze ) {
    minPt = 28000.;
    maxEta = 2.5;
    eff = 0.91;
  }
  if ( ptMeV > 50000. && fabs(eta) < 2.5 ) return 1.0;
  if ( ptMeV > 35000. && fabs(eta) < 2.5 && m_layout == gold) return 1.0;
  if ( ptMeV > minPt && fabs(eta) < 2.5 )
    return eff;
  if ( ptMeV > minPtHighEta && fabs(eta) < maxEta )
    return effHighEta;
  //  std::cout << "WARNING: no trigger efficiency is returned for pT "<<ptMeV/1000.<<" GeV, eta="<<eta<<", since it is assumed this will be the first unprescaled single electron trigger threshold !" << std::endl;
  return 0.0;
}

float UpgradePerformanceFunctions::getDiElectronTriggerEfficiency(float pt1MeV, float pt2MeV, float eta1, float eta2) {
  if (pt1MeV<15000. || pt2MeV<15000. || fabs(eta1)>2.5 || fabs(eta2)>2.5) {
    //std::cout << "WARNING: no trigger efficiency is returned for pT<15 GeV or eta>2.5, since it is assumed this will be the lowest unprescaled di-lepton trigger threshold !" << std::endl;
    return 0.0;
  }
  float eff=0.95; //single lepton efficiency
  if ( m_layout == LoI || m_layout == LoIVF) {
    eff = 0.88;
  } else if (m_layout == bronze ) {
    eff = 0.91;
  }
  return eff*eff;
}


float UpgradePerformanceFunctions::getElectronSmearedEnergy(float eMeV, float eta) {
  float deltaE = m_electronRandom.Gaus(0., getElectronEnergyResolution(eMeV, eta));
  return (eMeV+deltaE);
}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronEfficiency(float ptMeV, float eta) 
//*********************************************************************
//---- Get the electron ID efficiency
{

//---- old code 
//   if (m_eleCutLevel==looseElectron) {
//     return 0.97 - 0.103*exp(1.-ptGeV/15.0);
//   } else if (m_eleCutLevel==tightElectron) {
//     return 0.85 - 0.191*exp(1.-ptGeV/20.0);
//   }
//   std::cout << "getElectronEfficiency: Invalid working point." << std::endl;
//   return 0.0;

float ptGeV = ptMeV / 1000.;

//---- define all the different efficiency maps
const int nEtBins=6;
const int netaBins=8;

//---- Loose efficiencies
const float ElEff_Gold_mu200_Loose[nEtBins][netaBins] = { 
 { 0.568, 0.507, 0.633, 0.683, 0.673, 0.496, 0.549, 0.593 }, // Et[  7 -  15]
 { 0.589, 0.578, 0.654, 0.660, 0.736, 0.447, 0.622, 0.650 }, // Et[ 15 -  20]
 { 0.653, 0.670, 0.707, 0.676, 0.783, 0.475, 0.704, 0.714 }, // Et[ 20 -  30]
 { 0.767, 0.764, 0.817, 0.624, 0.831, 0.482, 0.766, 0.766 }, // Et[ 30 -  50]
 { 0.864, 0.841, 0.895, 0.705, 0.851, 0.537, 0.808, 0.812 }, // Et[ 50 -  80]
 { 0.934, 0.908, 0.932, 0.649, 0.855, 0.702, 0.890, 0.800 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu200_Loose[nEtBins][netaBins] = { 
 { 0.573, 0.509, 0.590, 0.622, 0.703, 0.471, 0.564, 0.546 }, // Et[  7 -  15]
 { 0.554, 0.610, 0.621, 0.660, 0.737, 0.438, 0.617, 0.614 }, // Et[ 15 -  20]
 { 0.638, 0.646, 0.664, 0.695, 0.781, 0.479, 0.696, 0.685 }, // Et[ 20 -  30]
 { 0.751, 0.744, 0.795, 0.610, 0.822, 0.499, 0.759, 0.760 }, // Et[ 30 -  50]
 { 0.839, 0.850, 0.863, 0.694, 0.846, 0.505, 0.827, 0.872 }, // Et[ 50 -  80]
 { 0.922, 0.854, 0.915, 0.737, 0.824, 0.655, 0.833, 0.800 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu200_Loose[nEtBins][netaBins] = { 
 { 0.565, 0.518, 0.589, 0.622, 0.684, 0.528, 0.551, 0.552 }, // Et[  7 -  15]
 { 0.543, 0.597, 0.620, 0.660, 0.725, 0.465, 0.670, 0.650 }, // Et[ 15 -  20]
 { 0.626, 0.633, 0.663, 0.695, 0.765, 0.464, 0.665, 0.733 }, // Et[ 20 -  30]
 { 0.721, 0.719, 0.795, 0.610, 0.820, 0.462, 0.763, 0.794 }, // Et[ 30 -  50]
 { 0.803, 0.833, 0.860, 0.694, 0.840, 0.510, 0.823, 0.846 }, // Et[ 50 -  80]
 { 0.888, 0.866, 0.910, 0.737, 0.859, 0.606, 0.843, 0.800 }, // Et[ 80 - 120]
}; 


//---- Tight efficiencies
const float ElEff_Gold_mu200_Tight[nEtBins][netaBins] = { 
 { 0.406, 0.374, 0.455, 0.390, 0.494, 0.311, 0.343, 0.394 }, // Et[  7 -  15]
 { 0.459, 0.468, 0.495, 0.476, 0.538, 0.298, 0.395, 0.470 }, // Et[ 15 -  20]
 { 0.523, 0.565, 0.551, 0.497, 0.598, 0.301, 0.467, 0.495 }, // Et[ 20 -  30]
 { 0.653, 0.685, 0.693, 0.478, 0.652, 0.311, 0.538, 0.556 }, // Et[ 30 -  50]
 { 0.791, 0.764, 0.809, 0.527, 0.689, 0.340, 0.594, 0.595 }, // Et[ 50 -  80]
 { 0.881, 0.838, 0.869, 0.514, 0.671, 0.421, 0.589, 0.600 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu200_Tight[nEtBins][netaBins] = { 
 { 0.393, 0.358, 0.414, 0.486, 0.495, 0.328, 0.371, 0.362 }, // Et[  7 -  15]
 { 0.410, 0.494, 0.453, 0.500, 0.562, 0.340, 0.415, 0.446 }, // Et[ 15 -  20]
 { 0.498, 0.537, 0.498, 0.519, 0.597, 0.310, 0.494, 0.483 }, // Et[ 20 -  30]
 { 0.629, 0.670, 0.663, 0.437, 0.646, 0.313, 0.546, 0.545 }, // Et[ 30 -  50]
 { 0.766, 0.744, 0.771, 0.495, 0.671, 0.311, 0.616, 0.655 }, // Et[ 50 -  80]
 { 0.872, 0.768, 0.824, 0.632, 0.673, 0.414, 0.667, 0.600 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu200_Tight[nEtBins][netaBins] = { 
 { 0.388, 0.345, 0.411, 0.486, 0.522, 0.377, 0.381, 0.363 }, // Et[  7 -  15]
 { 0.397, 0.484, 0.450, 0.500, 0.566, 0.312, 0.464, 0.446 }, // Et[ 15 -  20]
 { 0.489, 0.517, 0.498, 0.519, 0.597, 0.329, 0.487, 0.524 }, // Et[ 20 -  30]
 { 0.604, 0.645, 0.662, 0.437, 0.655, 0.328, 0.553, 0.567 }, // Et[ 30 -  50]
 { 0.734, 0.740, 0.768, 0.495, 0.682, 0.333, 0.595, 0.639 }, // Et[ 50 -  80]
 { 0.847, 0.768, 0.819, 0.632, 0.742, 0.394, 0.686, 0.600 }, // Et[ 80 - 120]
}; 


const float ElEff_Gold_mu140_Loose[nEtBins][netaBins] = { 
 { 0.64888, 0.59179, 0.68378, 0.77778, 0.72768, 0.47909, 0.60592, 0.60104 }, // Et[  7 -  15]
 { 0.65505, 0.67002, 0.71214, 0.68571, 0.77507, 0.45230, 0.65912, 0.68595 }, // Et[ 15 -  20]
 { 0.72583, 0.73963, 0.77069, 0.69775, 0.80911, 0.48731, 0.72476, 0.73642 }, // Et[ 20 -  30]
 { 0.82927, 0.81829, 0.86348, 0.66314, 0.84781, 0.49139, 0.79418, 0.79987 }, // Et[ 30 -  50]
 { 0.90454, 0.90789, 0.92335, 0.73423, 0.87739, 0.51794, 0.84135, 0.85649 }, // Et[ 50 -  80]
 { 0.94940, 0.94286, 0.95023, 0.78125, 0.88271, 0.62400, 0.88462, 0.90000 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu140_Loose[nEtBins][netaBins] = { 
 { 0.64918, 0.55814, 0.68182, 0.64103, 0.72709, 0.47909, 0.60709, 0.60052 }, // Et[  7 -  15]
 { 0.66218, 0.67782, 0.68586, 0.72549, 0.77618, 0.45289, 0.65910, 0.68549 }, // Et[ 15 -  20]
 { 0.71641, 0.71429, 0.73228, 0.67901, 0.80905, 0.48845, 0.72494, 0.73635 }, // Et[ 20 -  30]
 { 0.81752, 0.79669, 0.83867, 0.65136, 0.84806, 0.49180, 0.79433, 0.80011 }, // Et[ 30 -  50]
 { 0.88651, 0.87162, 0.90609, 0.68182, 0.87738, 0.51832, 0.84045, 0.85600 }, // Et[ 50 -  80]
 { 0.94745, 0.88750, 0.94767, 0.70000, 0.88182, 0.62400, 0.88462, 0.89474 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu140_Loose[nEtBins][netaBins] = { 
 { 0.64589, 0.56190, 0.68029, 0.64103, 0.72713, 0.43697, 0.55835, 0.64494 }, // Et[  7 -  15]
 { 0.66180, 0.67521, 0.68544, 0.72549, 0.77215, 0.41611, 0.67623, 0.72323 }, // Et[ 15 -  20]
 { 0.71540, 0.71845, 0.73200, 0.68354, 0.80791, 0.41586, 0.71669, 0.74120 }, // Et[ 20 -  30]
 { 0.81735, 0.79801, 0.83947, 0.65584, 0.84986, 0.44112, 0.79050, 0.80648 }, // Et[ 30 -  50]
 { 0.88586, 0.87037, 0.90572, 0.67290, 0.86407, 0.46608, 0.84195, 0.88500 }, // Et[ 50 -  80]
 { 0.94632, 0.89189, 0.94529, 0.70000, 0.85366, 0.42857, 0.90196, 1.00000 }, // Et[ 80 - 120]
}; 


const float ElEff_Gold_mu80_Loose[nEtBins][netaBins] = { 
 { 0.73020, 0.64734, 0.76449, 0.76829, 0.75343, 0.46359, 0.63066, 0.65059 }, // Et[  7 -  15]
 { 0.74932, 0.74280, 0.78627, 0.70093, 0.81195, 0.43899, 0.71203, 0.73442 }, // Et[ 15 -  20]
 { 0.80550, 0.79795, 0.81712, 0.75796, 0.83968, 0.46710, 0.77110, 0.77814 }, // Et[ 20 -  30]
 { 0.88647, 0.86736, 0.90207, 0.67419, 0.87942, 0.49020, 0.83804, 0.83553 }, // Et[ 30 -  50]
 { 0.93713, 0.92645, 0.93928, 0.69955, 0.90142, 0.55007, 0.87700, 0.89628 }, // Et[ 50 -  80]
 { 0.96299, 0.94286, 0.95745, 0.70588, 0.91704, 0.63710, 0.94595, 1.00000 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu80_Loose[nEtBins][netaBins] = { 
 { 0.75100, 0.63981, 0.76594, 0.75610, 0.75343, 0.46359, 0.63066, 0.65059 }, // Et[  7 -  15]
 { 0.74804, 0.71020, 0.77153, 0.66667, 0.81195, 0.43899, 0.71203, 0.73442 }, // Et[ 15 -  20]
 { 0.80819, 0.79273, 0.82137, 0.77987, 0.83968, 0.46710, 0.77110, 0.77814 }, // Et[ 20 -  30]
 { 0.87087, 0.85051, 0.89835, 0.63223, 0.87942, 0.49020, 0.83804, 0.83553 }, // Et[ 30 -  50]
 { 0.91414, 0.90868, 0.93114, 0.69725, 0.90142, 0.55007, 0.87700, 0.89628 }, // Et[ 50 -  80]
 { 0.94537, 0.94805, 0.94220, 0.70000, 0.91704, 0.63710, 0.94595, 1.00000 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu80_Loose[nEtBins][netaBins] = { 
 { 0.75100, 0.63981, 0.76594, 0.75610, 0.77589, 0.47967, 0.64668, 0.67045 }, // Et[  7 -  15]
 { 0.74804, 0.71020, 0.77153, 0.66667, 0.81630, 0.44595, 0.68986, 0.75048 }, // Et[ 15 -  20]
 { 0.80819, 0.79273, 0.82137, 0.77987, 0.83648, 0.45402, 0.75204, 0.78651 }, // Et[ 20 -  30]
 { 0.87087, 0.85051, 0.89835, 0.63223, 0.87343, 0.44906, 0.82649, 0.84851 }, // Et[ 30 -  50]
 { 0.91414, 0.90868, 0.93114, 0.69725, 0.90650, 0.45143, 0.86752, 0.92381 }, // Et[ 50 -  80]
 { 0.94537, 0.94805, 0.94220, 0.70000, 0.89697, 0.42857, 0.88235, 1.00000 }, // Et[ 80 - 120]
}; 





const float ElEff_Gold_mu140_Tight[nEtBins][netaBins] = { 
 { 0.47760, 0.46135, 0.51126, 0.58025, 0.53517, 0.33080, 0.36708, 0.41347 }, // Et[  7 -  15]
 { 0.53184, 0.54326, 0.54682, 0.49524, 0.57772, 0.28618, 0.44024, 0.49149 }, // Et[ 15 -  20]
 { 0.59920, 0.63630, 0.62115, 0.53698, 0.62205, 0.31642, 0.49146, 0.53822 }, // Et[ 20 -  30]
 { 0.72719, 0.73694, 0.74927, 0.51492, 0.67234, 0.31728, 0.56263, 0.58779 }, // Et[ 30 -  50]
 { 0.84258, 0.82237, 0.84830, 0.53604, 0.71909, 0.34604, 0.63566, 0.66287 }, // Et[ 50 -  80]
 { 0.91390, 0.87143, 0.89442, 0.62500, 0.74887, 0.45600, 0.69231, 0.75000 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu140_Tight[nEtBins][netaBins] = { 
 { 0.47290, 0.38605, 0.49643, 0.53846, 0.53468, 0.33460, 0.40907, 0.41250 }, // Et[  7 -  15]
 { 0.52101, 0.55649, 0.51883, 0.45098, 0.57899, 0.29256, 0.47015, 0.49172 }, // Et[ 15 -  20]
 { 0.58315, 0.62803, 0.58165, 0.56790, 0.62184, 0.32435, 0.52328, 0.53917 }, // Et[ 20 -  30]
 { 0.70908, 0.72355, 0.72840, 0.50104, 0.67286, 0.32223, 0.58556, 0.59002 }, // Et[ 30 -  50]
 { 0.82274, 0.77703, 0.83379, 0.52727, 0.71896, 0.34929, 0.64984, 0.66971 }, // Et[ 50 -  80]
 { 0.91141, 0.81250, 0.89826, 0.60000, 0.75152, 0.45600, 0.71978, 0.73684 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu140_Tight[nEtBins][netaBins] = { 
 { 0.47245, 0.38571, 0.49591, 0.53846, 0.54575, 0.32773, 0.37071, 0.43596 }, // Et[  7 -  15]
 { 0.52127, 0.55128, 0.51646, 0.45098, 0.60056, 0.29530, 0.47541, 0.52121 }, // Et[ 15 -  20]
 { 0.58172, 0.63107, 0.58082, 0.56962, 0.62725, 0.30174, 0.51753, 0.53519 }, // Et[ 20 -  30]
 { 0.70890, 0.72499, 0.72924, 0.50216, 0.69516, 0.32237, 0.57508, 0.59467 }, // Et[ 30 -  50]
 { 0.82228, 0.77546, 0.83320, 0.51402, 0.71585, 0.35103, 0.64549, 0.65500 }, // Et[ 50 -  80]
 { 0.90951, 0.81081, 0.89362, 0.60000, 0.69512, 0.31429, 0.68627, 0.80000 }, // Et[ 80 - 120]
}; 



const float ElEff_Gold_mu80_Tight[nEtBins][netaBins] = { 
 { 0.55713, 0.51449, 0.59836, 0.60976, 0.56618, 0.29307, 0.40681, 0.46099 }, // Et[  7 -  15]
 { 0.62230, 0.62551, 0.63194, 0.52336, 0.62849, 0.28209, 0.48203, 0.52387 }, // Et[ 15 -  20]
 { 0.68624, 0.68562, 0.68091, 0.62102, 0.66201, 0.30145, 0.52804, 0.57099 }, // Et[ 20 -  30]
 { 0.79713, 0.79146, 0.81059, 0.52601, 0.71603, 0.31095, 0.61311, 0.62736 }, // Et[ 30 -  50]
 { 0.88624, 0.84632, 0.88078, 0.55157, 0.74576, 0.33527, 0.67443, 0.70462 }, // Et[ 50 -  80]
 { 0.92749, 0.88571, 0.90122, 0.61765, 0.79259, 0.39516, 0.72973, 0.76190 }, // Et[ 80 - 120]
}; 
const float ElEff_Silver_mu80_Tight[nEtBins][netaBins] = { 
 { 0.56894, 0.51659, 0.59381, 0.58537, 0.56618, 0.30373, 0.44672, 0.46099 }, // Et[  7 -  15]
 { 0.62123, 0.57551, 0.62117, 0.49020, 0.62849, 0.28843, 0.50839, 0.52524 }, // Et[ 15 -  20]
 { 0.68384, 0.68102, 0.68474, 0.60377, 0.66201, 0.30425, 0.55819, 0.57208 }, // Et[ 20 -  30]
 { 0.78286, 0.76880, 0.80782, 0.49380, 0.71603, 0.31426, 0.63367, 0.62956 }, // Et[ 30 -  50]
 { 0.86747, 0.82877, 0.87060, 0.56881, 0.74576, 0.33962, 0.68694, 0.70913 }, // Et[ 50 -  80]
 { 0.91502, 0.85714, 0.88728, 0.60000, 0.79259, 0.39516, 0.74054, 0.76190 }, // Et[ 80 - 120]
}; 
const float ElEff_Bronze_mu80_Tight[nEtBins][netaBins] = { 
 { 0.56894, 0.51659, 0.59381, 0.58537, 0.61515, 0.33333, 0.43255, 0.49811 }, // Et[  7 -  15]
 { 0.62123, 0.57551, 0.62117, 0.49020, 0.65884, 0.33108, 0.50298, 0.55810 }, // Et[ 15 -  20]
 { 0.68384, 0.68102, 0.68474, 0.60377, 0.67940, 0.31418, 0.54632, 0.58832 }, // Et[ 20 -  30]
 { 0.78286, 0.76880, 0.80782, 0.49380, 0.72041, 0.31745, 0.62068, 0.64578 }, // Et[ 30 -  50]
 { 0.86747, 0.82877, 0.87060, 0.56881, 0.78184, 0.31429, 0.65670, 0.70476 }, // Et[ 50 -  80]
 { 0.91502, 0.85714, 0.88728, 0.60000, 0.78788, 0.31429, 0.68627, 0.60000 }, // Et[ 80 - 120]
}; 



float *mapToUse = (float *) ElEff_Gold_mu200_Loose;

if(m_layout == UpgradePerformanceFunctions::gold){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Gold_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Gold_mu200_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Gold_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Gold_mu140_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Gold_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Gold_mu80_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	} else {
		std::cout << "getElectronEfficiency: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::silver){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Silver_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Silver_mu200_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Silver_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Silver_mu140_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Silver_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Silver_mu80_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	} else {
		std::cout << "getElectronEfficiency: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::bronze){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Bronze_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Bronze_mu200_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Bronze_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Bronze_mu140_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElEff_Bronze_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElEff_Bronze_mu80_Tight;
		else std::cout << "getElectronEfficiency: working point." << std::endl;
	} else {
		std::cout << "getElectronEfficiency: Invalid <mu> value." << std::endl;
	}

} else{
	std::cout << "getElectronEfficiency: Invalid layout." << std::endl;
}


float efficiency = 1.;
efficiency = interpolateElectronIDMap(mapToUse, ptGeV, eta);
if(efficiency>1.0) efficiency = 1.0;

return efficiency;

}


//*********************************************************************
float UpgradePerformanceFunctions::interpolateElectronIDMap(float *ptEtaMap,float pT,float eta)
//*********************************************************************
{
//---- get efficiency value from the 2D maps: linear interpolation in pT and no interpolation in eta

  const int netaBins=8+1;
  const float etaBins[9]={0.00,1.37,1.52,2.47,2.50,3.16,3.35,4.00,4.90};
  const int nEtBins=6+1;
  const float EtBins[7]={7.0,15.0,20.0,30.0,50.0,80.0,120.00};

//---- if pt is out of range, use values estimated at the boundaries
  if(pT < 7) pT = 7.1;
  if(pT >=120) pT =119.; 
  if( fabs(eta)>4.9 ) return 0.;	//---- no electrons after |eta|=4.9


//--- find associated bin eta
  int etaBin =0;
  for( int ietaBin = 0 ; ietaBin < netaBins ; ietaBin++ ){
    if( fabs(eta) < etaBins[ietaBin] ) {etaBin = ietaBin-1; break;}
  }


//--- linear interpolation in pT
  int EtBin=0.;
  for(int ipT=0;ipT<nEtBins-1;ipT++){
  	float binCenter=(EtBins[ipT+1]-EtBins[ipT])/2.+EtBins[ipT];
//	printf("bin center =%f \n",binCenter);
	if(pT<binCenter) {EtBin=ipT; break;}
  }
  if(EtBin==0) EtBin=1;
  if(pT>(EtBins[nEtBins-1]-EtBins[nEtBins -2])/2.+EtBins[nEtBins -2]) EtBin = nEtBins -2;

  float y0=ptEtaMap[etaBin+(EtBin-1)*(netaBins-1)];
  float y1=ptEtaMap[etaBin+EtBin*(netaBins-1)];
  float x0=(EtBins[EtBin]-EtBins[EtBin-1])/2.+EtBins[EtBin-1];
  float x1=(EtBins[EtBin+1]-EtBins[EtBin])/2.+EtBins[EtBin];
  
  float value = y0 +(y1-y0)/(x1-x0) * (pT-x0);
//   printf("ID Interpolation: pt=%f eta=%f ptBin=%d etaBin=%d y0=%f y1=%f x0=%f x1=%f eff_int=%f \n",
//   		pT, eta, EtBin,etaBin,y0,y1,x0,x1,value);

  return value;
}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronFakeRate(float ptMeV, float eta) 
//*********************************************************************
{
//---- returns the probability to find a fake electron in a true jet
//----  pt and eta passed as arguments are the one of the true jet.


//----- old code
//   if (m_eleCutLevel==looseElectron) {
//     return 0.11*exp(-0.033*ptGeV);
//   } else if (m_eleCutLevel==tightElectron) {
//     return 0.0048*exp(-0.035*ptGeV);
//   }
//   std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
//   return 0.0;

float ptGeV = ptMeV / 1000.;

//---- define all the different rejection maps
const int nEtBins=6;
const int netaBins=8;

//---- Loose rejections
const float ElRej_Gold_mu200_Loose[nEtBins][netaBins] = { 
 { 0.02241, 0.06048, 0.01898, 0.03387, 0.21455, 0.03159, 0.05765, 0.10045 }, // Et[  7 -  15]
 { 0.00365, 0.02357, 0.00594, 0.03352, 0.07307, 0.00990, 0.02623, 0.05692 }, // Et[ 15 -  20]
 { 0.00506, 0.02606, 0.00820, 0.01399, 0.07198, 0.01366, 0.03694, 0.05564 }, // Et[ 20 -  30]
 { 0.00325, 0.01133, 0.00703, 0.01000, 0.07853, 0.00737, 0.04065, 0.08466 }, // Et[ 30 -  50]
 { 0.00300, 0.02632, 0.00201, 0.00200, 0.06102, 0.00377, 0.02259, 0.08377 }, // Et[ 50 -  80]
 { 0.00464, 0.02247, 0.00220, 0.00200, 0.02148, 0.00400, 0.01124, 0.08000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu200_Loose[nEtBins][netaBins] = { 
 { 0.02715, 0.09098, 0.02754, 0.02434, 0.23392, 0.03256, 0.06121, 0.09820 }, // Et[  7 -  15]
 { 0.00493, 0.02495, 0.00770, 0.02667, 0.08111, 0.01053, 0.02721, 0.04630 }, // Et[ 15 -  20]
 { 0.00642, 0.02094, 0.00711, 0.00820, 0.07381, 0.01012, 0.03132, 0.05747 }, // Et[ 20 -  30]
 { 0.00269, 0.02664, 0.00887, 0.01370, 0.07939, 0.00328, 0.04993, 0.08934 }, // Et[ 30 -  50]
 { 0.00284, 0.02794, 0.00315, 0.00200, 0.05800, 0.00974, 0.01657, 0.08263 }, // Et[ 50 -  80]
 { 0.00226, 0.02200, 0.00242, 0.00200, 0.02113, 0.00900, 0.01500, 0.08000 }, // Et[ 80 - 120]
}; 
const float ElRej_Bronze_mu200_Loose[nEtBins][netaBins] = { 
 { 0.03320, 0.10485, 0.02885, 0.02455, 0.22860, 0.03659, 0.06077, 0.10258 }, // Et[  7 -  15]
 { 0.00588, 0.02814, 0.00791, 0.02721, 0.07574, 0.00737, 0.03217, 0.06021 }, // Et[ 15 -  20]
 { 0.00845, 0.02222, 0.00776, 0.00820, 0.08372, 0.00575, 0.02519, 0.05595 }, // Et[ 20 -  30]
 { 0.00203, 0.02321, 0.00986, 0.01389, 0.08162, 0.02194, 0.04154, 0.06897 }, // Et[ 30 -  50]
 { 0.00244, 0.02398, 0.00396, 0.00800, 0.03673, 0.03000, 0.03684, 0.09091 }, // Et[ 50 -  80]
 { 0.00228, 0.02200, 0.00342, 0.00800, 0.04819, 0.03000, 0.04000, 0.07000 }, // Et[ 80 - 120]
}; 

//--- mu = 140
const float ElRej_Gold_mu140_Loose[nEtBins][netaBins] = { 
 { 0.02344, 0.05377, 0.02160, 0.02808, 0.18515, 0.02522, 0.05917, 0.10233 }, // Et[  7 -  15]
 { 0.00385, 0.02789, 0.00661, 0.05000, 0.06747, 0.00665, 0.02688, 0.04677 }, // Et[ 15 -  20]
 { 0.00503, 0.02095, 0.00615, 0.00893, 0.07132, 0.00856, 0.04162, 0.05820 }, // Et[ 20 -  30]
 { 0.00372, 0.01246, 0.00511, 0.01562, 0.08379, 0.01190, 0.04603, 0.05852 }, // Et[ 30 -  50]
 { 0.00442, 0.01784, 0.00160, 0.00800, 0.05263, 0.01042, 0.03081, 0.04494 }, // Et[ 50 -  80]
 { 0.00669, 0.01316, 0.00150, 0.00800, 0.03729, 0.01000, 0.03000, 0.04000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu140_Loose[nEtBins][netaBins] = { 
 { 0.02506, 0.06055, 0.02255, 0.01929, 0.18660, 0.02332, 0.05840, 0.10049 }, // Et[  7 -  15]
 { 0.00432, 0.03048, 0.00811, 0.01250, 0.07340, 0.00949, 0.02606, 0.04852 }, // Et[ 15 -  20]
 { 0.00603, 0.02094, 0.00656, 0.01235, 0.07219, 0.00920, 0.04151, 0.05619 }, // Et[ 20 -  30]
 { 0.00457, 0.01823, 0.00752, 0.01200, 0.08210, 0.01525, 0.04572, 0.06627 }, // Et[ 30 -  50]
 { 0.00559, 0.01892, 0.00454, 0.01200, 0.07047, 0.01560, 0.03778, 0.09091 }, // Et[ 50 -  80]
 { 0.00622, 0.01800, 0.00400, 0.01200, 0.05632, 0.01500, 0.03000, 0.09000 }, // Et[ 80 - 120]
}; 

const float ElRej_Bronze_mu140_Loose[nEtBins][netaBins] = { 
 { 0.02490, 0.06042, 0.02297, 0.01706, 0.20359, 0.01844, 0.05405, 0.10844 }, // Et[  7 -  15]
 { 0.00966, 0.03231, 0.00826, 0.01335, 0.06392, 0.00879, 0.03492, 0.05036 }, // Et[ 15 -  20]
 { 0.00873, 0.02190, 0.00632, 0.01382, 0.07129, 0.00557, 0.04448, 0.06203 }, // Et[ 20 -  30]
 { 0.00757, 0.01882, 0.00850, 0.01300, 0.07825, 0.01535, 0.04106, 0.09231 }, // Et[ 30 -  50]
 { 0.00783, 0.01825, 0.00577, 0.01300, 0.09459, 0.01500, 0.04540, 0.09850 }, // Et[ 50 -  80]
 { 0.00740, 0.01800, 0.00500, 0.01300, 0.05455, 0.01500, 0.06250, 0.09830 }, // Et[ 80 - 120]
}; 



//--- mu = 80
const float ElRej_Gold_mu80_Loose[nEtBins][netaBins] = { 
 { 0.02141, 0.05499, 0.01842, 0.02386, 0.15912, 0.02006, 0.05638, 0.09546 }, // Et[  7 -  15]
 { 0.00397, 0.01551, 0.00592, 0.02857, 0.06135, 0.00806, 0.03054, 0.04571 }, // Et[ 15 -  20]
 { 0.00457, 0.01746, 0.00718, 0.01802, 0.06989, 0.01226, 0.04184, 0.04963 }, // Et[ 20 -  30]
 { 0.00305, 0.01619, 0.00798, 0.01500, 0.08288, 0.01336, 0.04403, 0.07980 }, // Et[ 30 -  50]
 { 0.00281, 0.02181, 0.00240, 0.00800, 0.06381, 0.01000, 0.03297, 0.03371 }, // Et[ 50 -  80]
 { 0.00347, 0.01316, 0.00200, 0.00800, 0.02951, 0.01000, 0.03000, 0.03000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu80_Loose[nEtBins][netaBins] = { 
 { 0.02151, 0.07021, 0.02302, 0.02000, 0.15834, 0.02154, 0.05453, 0.09400 }, // Et[  7 -  15]
 { 0.00356, 0.02553, 0.00649, 0.04082, 0.06533, 0.00684, 0.02850, 0.05771 }, // Et[ 15 -  20]
 { 0.00422, 0.01808, 0.00768, 0.02469, 0.06869, 0.01030, 0.04007, 0.06185 }, // Et[ 20 -  30]
 { 0.00272, 0.01813, 0.00804, 0.02000, 0.08048, 0.01311, 0.04571, 0.08416 }, // Et[ 30 -  50]
 { 0.00278, 0.02260, 0.00333, 0.01000, 0.05906, 0.01387, 0.03698, 0.08206 }, // Et[ 50 -  80]
 { 0.00441, 0.02000, 0.00300, 0.01000, 0.02960, 0.01200, 0.03174, 0.08000 }, // Et[ 80 - 120]
}; 
const float ElRej_Bronze_mu80_Loose[nEtBins][netaBins] = { 
 { 0.02148, 0.06943, 0.02319, 0.01974, 0.15878, 0.02103, 0.05256, 0.09402 }, // Et[  7 -  15]
 { 0.00378, 0.02525, 0.00540, 0.04082, 0.06937, 0.00819, 0.04533, 0.05431 }, // Et[ 15 -  20]
 { 0.00416, 0.01375, 0.00558, 0.02439, 0.07121, 0.01372, 0.04308, 0.06228 }, // Et[ 20 -  30]
 { 0.00292, 0.01781, 0.00693, 0.02000, 0.08302, 0.01509, 0.05294, 0.06542 }, // Et[ 30 -  50]
 { 0.00174, 0.02210, 0.00325, 0.01000, 0.04665, 0.01500, 0.07419, 0.10345 }, // Et[ 50 -  80]
 { 0.00433, 0.02000, 0.00300, 0.01000, 0.03226, 0.01562, 0.05000, 0.10000 }, // Et[ 80 - 120]
}; 



//---- Tight rejections
const float ElRej_Gold_mu200_Tight[nEtBins][netaBins] = { 
 { 0.00178, 0.00911, 0.00234, 0.00178, 0.04728, 0.00776, 0.01083, 0.05098 }, // Et[  7 -  15]
 { 0.00085, 0.00294, 0.00059, 0.00400, 0.01879, 0.00275, 0.00351, 0.02338 }, // Et[ 15 -  20]
 { 0.00019, 0.00293, 0.00100, 0.00200, 0.02239, 0.00216, 0.00693, 0.02708 }, // Et[ 20 -  30]
 { 0.00028, 0.00300, 0.00125, 0.00200, 0.02764, 0.00246, 0.00976, 0.03101 }, // Et[ 30 -  50]
 { 0.00067, 0.00329, 0.00200, 0.00200, 0.01569, 0.00300, 0.00821, 0.03074 }, // Et[ 50 -  80]
 { 0.00060, 0.00347, 0.00200, 0.00200, 0.00983, 0.00400, 0.00900, 0.03000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu200_Tight[nEtBins][netaBins] = { 
 { 0.00186, 0.00941, 0.00247, 0.00221, 0.05096, 0.01395, 0.02191, 0.04978 }, // Et[  7 -  15]
 { 0.00090, 0.00399, 0.00088, 0.00667, 0.01931, 0.00301, 0.01222, 0.02420 }, // Et[ 15 -  20]
 { 0.00023, 0.00493, 0.00102, 0.00300, 0.02362, 0.00607, 0.02215, 0.03065 }, // Et[ 20 -  30]
 { 0.00037, 0.00510, 0.00127, 0.00300, 0.02994, 0.00300, 0.02853, 0.03185 }, // Et[ 30 -  50]
 { 0.00071, 0.00327, 0.00200, 0.00300, 0.01600, 0.00380, 0.00852, 0.03100 }, // Et[ 50 -  80]
 { 0.00060, 0.00400, 0.00242, 0.00300, 0.01004, 0.00400, 0.01000, 0.03000 }, // Et[ 80 - 120]
}; 
const float ElRej_Bronze_mu200_Tight[nEtBins][netaBins] = { 
 { 0.00190, 0.00949, 0.00248, 0.00223, 0.10333, 0.01360, 0.02620, 0.05025 }, // Et[  7 -  15]
 { 0.00100, 0.00402, 0.00088, 0.00680, 0.04114, 0.00295, 0.012716, 0.02278 }, // Et[ 15 -  20]
 { 0.00023, 0.00494, 0.00102, 0.00300, 0.04547, 0.00192, 0.02145, 0.02381 }, // Et[ 20 -  30]
 { 0.00037, 0.00614, 0.00127, 0.00300, 0.04977, 0.00627, 0.02929, 0.03103 }, // Et[ 30 -  50]
 { 0.00071, 0.00700, 0.00200, 0.00300, 0.01837, 0.00500, 0.01579, 0.03173 }, // Et[ 50 -  80]
 { 0.00060, 0.00700, 0.00242, 0.00300, 0.01205, 0.00600, 0.01900, 0.03000 }, // Et[ 80 - 120]
}; 

//--- mu = 140
const float ElRej_Gold_mu140_Tight[nEtBins][netaBins] = { 
 { 0.00220, 0.00510, 0.00260, 0.00432, 0.03896, 0.00524, 0.00901, 0.05181 }, // Et[  7 -  15]
 { 0.00041, 0.00517, 0.00035, 0.00400, 0.01728, 0.00200, 0.00457, 0.01774 }, // Et[ 15 -  20]
 { 0.00012, 0.00349, 0.00123, 0.00300, 0.01968, 0.00155, 0.00946, 0.02663 }, // Et[ 20 -  30]
 { 0.00034, 0.00338, 0.00128, 0.00300, 0.02941, 0.00170, 0.00354, 0.03787 }, // Et[ 30 -  50]
 { 0.00080, 0.00300, 0.00100, 0.00300, 0.02024, 0.00521, 0.00560, 0.03448 }, // Et[ 50 -  80]
 { 0.00050, 0.00516, 0.00100, 0.00300, 0.01017, 0.00500, 0.00500, 0.02000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu140_Tight[nEtBins][netaBins] = { 
 { 0.00224, 0.00692, 0.00261, 0.00643, 0.03819, 0.00916, 0.02428, 0.05013 }, // Et[  7 -  15]
 { 0.00045, 0.00535, 0.00051, 0.00600, 0.01933, 0.00150, 0.01330, 0.02123 }, // Et[ 15 -  20]
 { 0.00050, 0.00349, 0.00089, 0.00600, 0.02116, 0.00524, 0.02024, 0.02662 }, // Et[ 20 -  30]
 { 0.00048, 0.00521, 0.00188, 0.00500, 0.02845, 0.00780, 0.02502, 0.04024 }, // Et[ 30 -  50]
 { 0.00080, 0.00400, 0.00114, 0.00500, 0.01884, 0.00707, 0.00926, 0.03573 }, // Et[ 50 -  80]
 { 0.00050, 0.00600, 0.00100, 0.00500, 0.01439, 0.00700, 0.00900, 0.03500 }, // Et[ 80 - 120]
}; 
const float ElRej_Bronze_mu140_Tight[nEtBins][netaBins] = { 
 { 0.00235, 0.00678, 0.00256, 0.00683, 0.08623, 0.01064, 0.02340, 0.04991 }, // Et[  7 -  15]
 { 0.00048, 0.00462, 0.00053, 0.00700, 0.02973, 0.00879, 0.01163, 0.02398 }, // Et[ 15 -  20]
 { 0.00036, 0.00365, 0.00095, 0.00700, 0.03630, 0.00879, 0.02021, 0.02632 }, // Et[ 20 -  30]
 { 0.00051, 0.00538, 0.00250, 0.00700, 0.04980, 0.00835, 0.02174, 0.03590 }, // Et[ 30 -  50]
 { 0.00090, 0.00500, 0.00119, 0.00700, 0.05068, 0.00800, 0.02000, 0.06000 }, // Et[ 50 -  80]
 { 0.00050, 0.00600, 0.00100, 0.00700, 0.01509, 0.00800, 0.02000, 0.06000 }, // Et[ 80 - 120]
}; 



//--- mu = 80
const float ElRej_Gold_mu80_Tight[nEtBins][netaBins] = { 
 { 0.00290, 0.00943, 0.00366, 0.00317, 0.03284, 0.00322, 0.00897, 0.04652 }, // Et[  7 -  15]
 { 0.00041, 0.00414, 0.00070, 0.02030, 0.01250, 0.00147, 0.00398, 0.02385 }, // Et[ 15 -  20]
 { 0.00023, 0.00582, 0.00103, 0.01200, 0.01586, 0.00094, 0.00747, 0.02635 }, // Et[ 20 -  30]
 { 0.00030, 0.00609, 0.00096, 0.00500, 0.02233, 0.00100, 0.00210, 0.04244 }, // Et[ 30 -  50]
 { 0.00020, 0.00697, 0.00080, 0.00500, 0.01097, 0.00100, 0.00275, 0.02247 }, // Et[ 50 -  80]
 { 0.00020, 0.00616, 0.00070, 0.00500, 0.00328, 0.00100, 0.00200, 0.02000 }, // Et[ 80 - 120]
}; 
const float ElRej_Silver_mu80_Tight[nEtBins][netaBins] = { 
 { 0.00294, 0.01142, 0.00464, 0.00333, 0.03193, 0.00637, 0.02537, 0.04535 }, // Et[  7 -  15]
 { 0.00059, 0.00993, 0.00075, 0.02041, 0.01427, 0.00244, 0.01371, 0.02291 }, // Et[ 15 -  20]
 { 0.00034, 0.00626, 0.00119, 0.01235, 0.01647, 0.00322, 0.02454, 0.02779 }, // Et[ 20 -  30]
 { 0.00035, 0.01036, 0.00094, 0.00600, 0.02142, 0.00765, 0.02686, 0.03940 }, // Et[ 30 -  50]
 { 0.00022, 0.01000, 0.00111, 0.00600, 0.01247, 0.00500, 0.01439, 0.02471 }, // Et[ 50 -  80]
 { 0.00022, 0.01000, 0.00100, 0.00600, 0.00634, 0.00500, 0.00800, 0.02000 }, // Et[ 80 - 120]
}; 
const float ElRej_Bronze_mu80_Tight[nEtBins][netaBins] = { 
 { 0.00291, 0.01140, 0.00466, 0.00329, 0.07491, 0.00882, 0.02319, 0.04466 }, // Et[  7 -  15]
 { 0.00059, 0.00995, 0.00074, 0.02041, 0.03834, 0.00419, 0.01688, 0.03306 }, // Et[ 15 -  20]
 { 0.00033, 0.00872, 0.00118, 0.01220, 0.03956, 0.00804, 0.02315, 0.02491 }, // Et[ 20 -  30]
 { 0.00034, 0.01108, 0.00092, 0.00600, 0.04623, 0.00455, 0.02249, 0.02804 }, // Et[ 30 -  50]
 { 0.00022, 0.01000, 0.00108, 0.00600, 0.02041, 0.00500, 0.02564, 0.06897 }, // Et[ 50 -  80]
 { 0.00022, 0.01000, 0.00090, 0.00600, 0.01613, 0.00562, 0.01000, 0.04000 }, // Et[ 80 - 120]
}; 


float *mapToUse = (float *) ElRej_Gold_mu200_Loose;

if(m_layout == UpgradePerformanceFunctions::gold){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Gold_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Gold_mu200_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Gold_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Gold_mu140_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Gold_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Gold_mu80_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	} else {
		std::cout << "getElectronFakeRate: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::silver){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Silver_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Silver_mu200_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Silver_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Silver_mu140_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Silver_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Silver_mu80_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	} else {
		std::cout << "getElectronFakeRate: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::bronze){
	if(m_avgMu == 200){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Bronze_mu200_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Bronze_mu200_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 140){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Bronze_mu140_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Bronze_mu140_Tight;
		else std::cout << "getElectronFakeRate: Invalid working point." << std::endl;
	}else if(m_avgMu == 80){
		if(m_eleCutLevel==UpgradePerformanceFunctions::looseElectron) mapToUse = (float *) ElRej_Bronze_mu80_Loose;
		else if(m_eleCutLevel==UpgradePerformanceFunctions::tightElectron) mapToUse = (float *) ElRej_Bronze_mu80_Tight;
		else std::cout << "getElectronFakeRate: working point." << std::endl;
	} else {
		std::cout << "getElectronFakeRate: Invalid <mu> value." << std::endl;
	}

} else{
	std::cout << "getElectronFakeRate: Invalid layout." << std::endl;
}



float efficiency = 1.;
efficiency = interpolateElectronIDMap(mapToUse, ptGeV, eta);

if(efficiency>1.0) efficiency = 1.0;
if(efficiency<=0.0) efficiency = 0.0001;

return efficiency;



}


//*********************************************************************
float UpgradePerformanceFunctions::getElectronEnergyResolution(float eMeV, float eta) 
//*********************************************************************
{
//---- Returns the absolute energy resolution in MeV

//---- old code 
//   const double ET_pu=0.6; //0.6 GeV in ET for mu=80,in a trigger tower, after pu suppression; supposed identical for a cluster; taken from ATLAS-TDR-022-2013 page 25
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
//   double mu = m_avgMu;
//   double sigma2 = 0.3*0.3 + ET_pu*ET_pu*cosh(eta)*cosh(eta)*mu/80. + S*S*eGeV + C*C*eGeV*eGeV;
//   return sqrt(sigma2);


float eGeV = eMeV / 1000.;


//---- define all the different resolution maps
const int nEtBins=6;
const int netaBins=14;
  
const float ElERes_Gold_mu200[nEtBins][netaBins] = { 
 {  9.752, 11.455, 16.650, 10.993,  9.148, 23.463, 31.330, 30.128, 30.949, 31.443, 31.591, 66.794, 50.884, 41.477 }, // Et[  7 -  15]
 {  6.964,  7.609, 13.622,  7.524,  5.864, 20.472, 22.108, 26.614, 34.214, 21.778, 36.023, 41.312, 26.732, 38.689 }, // Et[ 15 -  20]
 {  5.041,  5.482, 10.200,  5.718,  4.045,  4.708, 15.902, 18.687, 25.653, 18.251, 21.885, 29.429, 29.209, 27.694 }, // Et[ 20 -  30]
 {  3.378,  3.729,  7.830,  3.877,  3.005,  4.059, 10.854, 12.880, 22.009, 13.843, 17.826, 21.827, 18.746, 23.942 }, // Et[ 30 -  50]
 {  2.538,  2.618,  6.535,  2.804,  2.281,  2.645,  7.612,  9.939, 21.029, 10.993, 12.557, 19.503, 11.262, 19.107 }, // Et[ 50 -  80]
 {  2.732,  2.023,  4.237,  2.359,  6.044,  3.379,  7.322, 12.745, 25.013, 12.438, 11.777, 13.288, 15.000, 18.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Silver_mu200[nEtBins][netaBins] = { 
 { 11.198, 12.322, 19.247, 11.535,  8.788, 23.024, 30.545, 43.626, 39.052, 42.384, 54.825, 64.918, 61.679, 53.803 }, // Et[  7 -  15]
 {  7.479,  7.787, 13.258,  7.365,  6.416, 23.324, 20.957, 27.441, 34.538, 29.140, 40.017, 52.205, 49.461, 37.269 }, // Et[ 15 -  20]
 {  5.384,  5.523, 10.705,  5.836,  4.677,  4.206, 16.002, 19.620, 24.841, 17.756, 24.385, 33.824, 26.310, 25.942 }, // Et[ 20 -  30]
 {  3.495,  3.790,  8.246,  3.861,  3.026,  3.791, 11.177, 13.015, 23.238, 15.518, 17.425, 23.438, 21.750, 16.261 }, // Et[ 30 -  50]
 {  2.652,  2.850,  6.970,  2.924,  5.536,  2.670,  8.633, 11.082, 18.114, 10.877, 12.014, 21.847, 19.805, 23.652 }, // Et[ 50 -  80]
 {  2.458,  2.170,  6.883,  2.089,  5.187,  3.834,  5.387, 20.041, 20.811, 23.706, 12.650, 15.974, 19.000, 18.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Bronze_mu200[nEtBins][netaBins] = { 
 { 11.003, 12.630, 17.823, 11.900,  8.788, 23.024, 25.512, 41.838, 41.678, 25.058, 74.963, 86.762, 58.027, 44.380 }, // Et[  7 -  15]
 {  7.625,  7.893, 12.873,  7.366,  6.454, 23.324, 20.115, 27.700, 30.836, 19.783, 38.723, 50.876, 29.909, 44.220 }, // Et[ 15 -  20]
 {  5.306,  5.500, 10.709,  5.838,  4.677,  4.206, 15.024, 18.933, 26.512, 18.767, 24.520, 25.726, 18.707, 29.960 }, // Et[ 20 -  30]
 {  3.501,  3.789,  8.240,  3.853,  3.029,  3.791, 11.817, 12.838, 26.572, 11.781, 16.542, 21.662, 20.311, 20.061 }, // Et[ 30 -  50]
 {  2.646,  2.845,  6.998,  2.919,  5.538,  2.670,  7.897,  9.477, 26.146,  9.714, 11.673, 21.637, 23.637, 18.243 }, // Et[ 50 -  80]
 {  2.128,  2.170,  4.826,  2.089,  6.187,  3.834,  9.404,  9.390, 27.346, 10.397, 10.589, 15.670, 19.000, 18.000 }, // Et[ 80 - 120]
}; 

const float ElERes_Gold_mu140[nEtBins][netaBins] = { 
 {  8.715,  9.821, 16.441, 10.133,  7.569, 20.975, 26.328, 29.104, 27.220, 25.732, 34.116, 44.157, 31.224, 34.566 }, // Et[  7 -  15]
 {  5.637,  6.322, 11.383,  7.049,  5.715, 20.056, 20.310, 22.259, 30.438, 20.626, 30.015, 35.323, 29.428, 30.391 }, // Et[ 15 -  20]
 {  4.431,  4.677,  8.900,  4.982,  3.907,  5.653, 14.028, 16.055, 25.555, 17.112, 20.932, 26.912, 23.448, 24.677 }, // Et[ 20 -  30]
 {  3.007,  3.293,  6.977,  3.301,  2.812,  3.534,  9.810, 11.559, 20.712, 11.964, 15.774, 20.431, 18.725, 20.317 }, // Et[ 30 -  50]
 {  2.357,  2.434,  6.180,  2.498,  5.079,  2.538,  6.966,  8.733, 18.760,  9.809, 12.389, 13.301, 15.719, 15.945 }, // Et[ 50 -  80]
 {  1.574,  1.897,  4.179,  2.824,  6.066,  2.608,  5.016,  6.772, 20.334,  9.065, 10.174, 14.448, 14.848, 15.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Silver_mu140[nEtBins][netaBins] = { 
 {  8.870,  9.550, 20.065, 10.862,  7.252, 17.516, 26.349, 29.089, 27.220, 26.079, 34.124, 44.081, 31.844, 34.566 }, // Et[  7 -  15]
 {  5.974,  6.572, 10.830,  6.775,  5.997, 23.023, 20.237, 22.175, 30.506, 20.634, 30.035, 35.365, 29.438, 30.420 }, // Et[ 15 -  20]
 {  4.707,  4.700,  9.534,  5.155,  3.834,  3.993, 14.022, 16.072, 25.206, 17.105, 20.916, 26.922, 23.517, 24.724 }, // Et[ 20 -  30]
 {  3.078,  3.407,  7.636,  3.645,  2.780,  3.255,  9.794, 11.568, 20.699, 11.962, 15.760, 20.119, 18.740, 20.307 }, // Et[ 30 -  50]
 {  2.464,  2.612,  6.342,  2.659,  3.690,  2.946,  6.974,  8.787, 18.702,  9.866, 12.391, 13.340, 15.713, 15.945 }, // Et[ 50 -  80]
 {  1.578,  1.961,  4.040,  2.636,  3.333,  2.288,  4.990,  6.742, 20.334,  9.065, 10.174, 14.055, 14.848, 15.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Bronze_mu140[nEtBins][netaBins] = { 
 {  8.915,  9.534, 20.358, 10.866,  7.186, 17.516, 23.114, 25.068, 31.477, 41.280, 69.584, 60.161, 52.175, 50.009 }, // Et[  7 -  15]
 {  5.969,  6.551, 10.898,  7.081,  5.940, 23.023, 16.912, 17.419, 28.828, 22.017, 28.071, 39.257, 30.272, 35.688 }, // Et[ 15 -  20]
 {  4.722,  4.679,  9.517,  5.121,  3.854,  4.032, 14.600, 15.700, 27.616, 16.679, 20.819, 23.771, 18.615, 30.073 }, // Et[ 20 -  30]
 {  3.075,  3.416,  7.703,  3.645,  2.775,  3.220,  9.836, 11.026, 24.508, 11.174, 14.029, 20.278, 17.042, 16.470 }, // Et[ 30 -  50]
 {  2.462,  2.589,  6.423,  2.666,  3.716,  2.911,  6.849,  6.848, 21.022,  9.925,  8.579,  9.389, 16.129, 24.291 }, // Et[ 50 -  80]
 {  1.583,  1.953,  4.324,  2.719, 11.613, 13.288,  4.801,  6.424, 21.246,  9.260, 10.904,  7.680, 12.000, 15.000 }, // Et[ 80 - 120]
}; 


const float ElERes_Gold_mu80[nEtBins][netaBins] = { 
 {  7.466,  8.236, 15.705,  8.614,  5.544, 20.967, 19.876, 25.377, 26.360, 23.354, 25.344, 34.268, 27.814, 32.618 }, // Et[  7 -  15]
 {  5.173,  5.682, 10.966,  5.730,  3.709, 21.881, 15.517, 16.843, 30.446, 16.105, 22.451, 27.811, 24.954, 25.660 }, // Et[ 15 -  20]
 {  3.845,  4.017,  8.398,  4.148,  3.174,  3.921, 10.881, 12.870, 22.908, 13.829, 17.308, 21.306, 18.918, 20.227 }, // Et[ 20 -  30]
 {  2.757,  2.774,  6.500,  2.962,  2.480,  2.865,  7.990,  9.057, 20.028, 10.701, 12.897, 16.029, 14.993, 17.162 }, // Et[ 30 -  50]
 {  9.475,  2.265,  5.520,  2.427,  3.940,  2.562,  6.104,  7.311, 17.646,  8.416,  9.757, 11.998, 15.127, 17.759 }, // Et[ 50 -  80]
 {  1.586,  2.892,  4.341,  2.214,  2.025,  2.132,  4.071,  4.614, 12.000,  9.525,  9.740, 10.001, 12.407, 13.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Silver_mu80[nEtBins][netaBins] = { 
 {  7.992,  7.374, 13.025,  8.383,  5.640, 32.583, 19.876, 25.377, 26.360, 23.354, 25.344, 34.268, 27.814, 32.618 }, // Et[  7 -  15]
 {  5.061,  5.793,  8.884,  6.430,  4.335, 19.316, 15.517, 16.843, 30.446, 16.105, 22.451, 27.811, 24.954, 25.660 }, // Et[ 15 -  20]
 {  3.876,  4.170,  9.148,  4.465,  3.168,  3.329, 10.881, 12.870, 22.908, 13.829, 17.308, 21.306, 18.918, 20.227 }, // Et[ 20 -  30]
 {  2.673,  2.795,  6.776,  3.001,  2.481,  3.178,  7.990,  9.057, 20.028, 10.701, 12.897, 16.029, 14.993, 17.162 }, // Et[ 30 -  50]
 {  2.269,  2.203,  5.820,  2.322,  4.404,  3.278,  6.104,  7.311, 17.646,  9.416, 10.757, 11.998, 15.127, 17.759 }, // Et[ 50 -  80]
 {  1.586,  1.814,  4.818,  2.571,  2.975,  3.457,  4.071,  4.614, 17.000,  9.525,  9.740, 10.001, 13.407, 13.000 }, // Et[ 80 - 120]
}; 
const float ElERes_Bronze_mu80[nEtBins][netaBins] = { 
 {  7.992,  7.374, 13.025,  8.383,  5.640, 32.583, 18.206, 18.348, 25.728, 20.929, 47.897, 29.518, 60.808, 37.969 }, // Et[  7 -  15]
 {  5.061,  5.793,  8.884,  6.430,  4.335, 19.316, 15.206, 13.934, 27.632, 18.011, 28.959, 34.868, 28.852, 17.633 }, // Et[ 15 -  20]
 {  3.876,  4.170,  9.148,  4.465,  3.168,  3.329, 10.020, 12.812, 23.815, 11.204, 14.731, 20.695, 16.423, 12.565 }, // Et[ 20 -  30]
 {  2.673,  2.795,  6.776,  3.001,  2.481,  3.178,  7.808,  9.190, 25.497,  8.762, 11.265, 13.863, 15.170, 17.676 }, // Et[ 30 -  50]
 {  9.269,  2.203,  5.820,  2.322,  2.404,  2.278,  5.998,  6.791, 19.046,  8.500,  9.934, 12.749, 11.929, 17.450 }, // Et[ 50 -  80]
 {  1.586,  1.814,  4.818,  2.571,  1.975,  2.457,  4.473,  5.633, 18.635,  8.415,  10.458, 7.237, 11.000, 13.000 }, // Et[ 80 - 120]
}; 





float *mapToUse = (float *) ElERes_Gold_mu200;

if(m_layout == UpgradePerformanceFunctions::gold){
	if(m_avgMu == 200){
		mapToUse = (float *) ElERes_Gold_mu200;
	}else if(m_avgMu == 140){
		mapToUse = (float *) ElERes_Gold_mu140;
	}else if(m_avgMu == 80){
		mapToUse = (float *) ElERes_Gold_mu80;
	} else {
		std::cout << "getElectronEnergyResolution: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::silver){
	if(m_avgMu == 200){
		mapToUse = (float *) ElERes_Silver_mu200;
	}else if(m_avgMu == 140){
		mapToUse = (float *) ElERes_Silver_mu140;
	}else if(m_avgMu == 80){
		mapToUse = (float *) ElERes_Silver_mu80;
	} else {
		std::cout << "getElectronEnergyResolution: Invalid <mu> value." << std::endl;
	}

}else if(m_layout == UpgradePerformanceFunctions::bronze){
	if(m_avgMu == 200){
		mapToUse = (float *) ElERes_Bronze_mu200;
	}else if(m_avgMu == 140){
		mapToUse = (float *) ElERes_Bronze_mu140;
	}else if(m_avgMu == 80){
		mapToUse = (float *) ElERes_Bronze_mu80;
	} else {
		std::cout << "getElectronEnergyResolution: Invalid <mu> value." << std::endl;
	}

} else{
	std::cout << "getElectronEnergyResolution: Invalid layout." << std::endl;
}



float resolution = 1.;
float Et = eGeV/cosh(eta);
resolution = interpolateElectronResolutionMap(mapToUse, Et, eta);
resolution = resolution/100. * eMeV; 

return resolution;
  
}


//*********************************************************************
float UpgradePerformanceFunctions::interpolateElectronResolutionMap(float *ptEtaMap,float pT,float eta)
//*********************************************************************
{
//---- get resolution value from the 2D maps: linear interpolation in pT and no interpolation in eta


  const int nEtBins=6+1;
  const float EtBins[7]={7.0,15.0,20.0,30.0,50.0,80.0,120.00};
  const int netaBins=14+1;
  const float etaBins[15]={0.00,0.80,1.37,1.52,2.01,2.47,2.50,2.80,3.16,3.35,3.70,4.00,4.30,4.60,4.90};



//---- if pt or eta is out of range, use values estimated at the boundaries
  if(pT < 7) pT = 7.1;
  if(pT >=120) pT =119.; 
  if( fabs(eta)>4.9 ) eta = 4.8;	


//--- find associated bin in eta
  int etaBin =0;
  for( int ietaBin = 0 ; ietaBin < netaBins ; ietaBin++ ){
    if( fabs(eta) < etaBins[ietaBin] ) {etaBin = ietaBin-1; break;}
  }


//--- linear interpolation in pT
  int EtBin=0.;
  for(int ipT=0;ipT<nEtBins-1;ipT++){
  	float binCenter=(EtBins[ipT+1]-EtBins[ipT])/2.+EtBins[ipT];
//	printf("bin center =%f \n",binCenter);
	if(pT<binCenter) {EtBin=ipT; break;}
  }
  if(EtBin==0) {EtBin=1;}
  if(pT>(EtBins[nEtBins-1]-EtBins[nEtBins -2])/2.+EtBins[nEtBins -2]) {EtBin = nEtBins -2;}
  
  float y0=ptEtaMap[etaBin+(EtBin-1)*(netaBins-1)];
  float y1=ptEtaMap[etaBin+EtBin*(netaBins-1)];
  float x0=(EtBins[EtBin]-EtBins[EtBin-1])/2.+EtBins[EtBin-1];
  float x1=(EtBins[EtBin+1]-EtBins[EtBin])/2.+EtBins[EtBin];
  
  float value = y0 +(y1-y0)/(x1-x0) * (pT-x0);
 
//  printf("Resol Interpolation: pt=%f eta=%f ptBin=%d etaBin=%d y0=%f y1=%f x0=%f x1=%f eff_int=%f \n",
//   		pT, eta, EtBin,etaBin,y0,y1,x0,x1,value);

  return value;
}



float UpgradePerformanceFunctions::getElectronFakeRescaledEnergy(float eMeV, float eta) 
{
// 
//   if (m_eleCutLevel == looseElectron) {
//     float binning[20] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 200};
//     float scales[20] = {-0.60, -0.636549, -0.677891, -0.701189, -0.707942, -0.703792, -0.701427, -0.700264, -0.70114, -0.701068, -0.698009, -0.692705, -0.681838, -0.678387, -0.676639, -0.668481, -0.663736, -0.668471, -0.666521, -0.665515};
//     float sigmas[20] = {0.13, 0.132724, 0.143584, 0.145539, 0.149064, 0.153829, 0.156095, 0.157976, 0.160515, 0.162841, 0.169783, 0.174657, 0.183426, 0.185493, 0.189531, 0.194585, 0.201598, 0.195728, 0.202998, 0.206324};
//     
//     //Checking which bin we are in
//     int i=0;
//     while (i<19 && eMeV/1000.>binning[i+1]) i++;
//     
//     return eMeV * (1 + m_electronRandom.Gaus(scales[i],sigmas[i]) );
//   } else if (m_eleCutLevel == tightElectron) {
//     float binning[20] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 200};
//     float scales[20] = {-0.5, -0.541424, -0.601461, -0.621471, -0.621278, -0.608145, -0.622162, -0.624346, -0.622605, -0.637172, -0.620544, -0.637961, -0.637561, -0.610907, -0.630854, -0.60472, -0.622769, -0.64527, -0.610933, -0.612421};
//     float sigmas[20] = {0.17, 0.174439, 0.183794, 0.191502, 0.196909, 0.195764, 0.185789, 0.182931, 0.184728, 0.185637, 0.201431, 0.198198, 0.204317, 0.215402, 0.220042, 0.228437, 0.224068, 0.196602, 0.215806, 0.22049};
//     
//     //Checking which bin we are in
//     int i=0;
//     while (i<19 && eMeV/1000.>binning[i+1]) i++;
//     
//     return eMeV * (1 + m_electronRandom.Gaus(scales[i],sigmas[i]) );
//   }
//   std::cout << "getElectronFakeRescaledEnergy: Invalid working point." << std::endl;
//   return 0.;

float eGeV = eMeV / 1000.;


//---- define all the different resolution maps
const int nEtBins=6;
const int netaBins=14;
  

const float FakeElERes_Gold_mu200[nEtBins][netaBins] = { 
 { 16.032, 18.698, 19.596, 20.103, 25.825, 26.483, 29.914, 25.624, 19.891, 26.264, 40.642, 37.070, 25.230, 27.114 }, // Et[  7 -  15]
 { 14.685, 15.906, 18.967, 17.460, 25.606, 19.351, 19.388, 21.214, 21.687, 22.560, 24.960, 28.403, 19.745, 18.065 }, // Et[ 15 -  20]
 { 17.546, 16.783, 20.201, 23.625, 22.719, 17.189, 26.337, 26.394, 27.880, 28.668, 28.830, 19.176, 20.914, 21.044 }, // Et[ 20 -  30]
 { 18.503, 17.358, 27.525, 27.498, 26.161, 28.864, 26.548, 28.442, 21.397, 31.808, 33.471, 32.299, 34.021, 13.374 }, // Et[ 30 -  50]
 { 20.402, 14.065, 25.068, 21.330, 17.032, 17.310, 25.298, 30.297, 22.810, 24.702, 40.313, 25.989, 13.648,  13.000 }, // Et[ 50 -  80]
 { 19.668, 27.135, 37.334, 20.873, 24.542, 20.000, 29.422, 26.174, 18.309, 27.016, 30.000, 25.000, 13.000,  13.000 }, // Et[ 80 - 120]
}; 

const float FakeElEMean_Gold_mu200[nEtBins][netaBins] = { 
 { -62.716, -58.085, -56.238, -58.586, -51.105, -41.114, -37.021, -35.317, -42.786, -38.173, -27.559, -21.585, -35.194, -35.063 }, // Et[  7 -  15]
 { -72.165, -68.788, -63.518, -67.612, -60.119, -59.498, -55.379, -51.320, -54.775, -50.341, -44.135, -41.046, -45.722, -52.332 }, // Et[ 15 -  20]
 { -75.904, -73.967, -67.273, -69.162, -66.921, -62.387, -57.335, -56.578, -57.478, -54.888, -47.901, -53.553, -53.412, -49.805 }, // Et[ 20 -  30]
 { -80.221, -79.889, -68.150, -72.992, -71.276, -56.299, -64.534, -63.959, -70.303, -52.127, -42.937, -42.434, -43.946, -61.434 }, // Et[ 30 -  50]
 { -85.372, -86.887, -74.129, -84.470, -83.668, -88.036, -68.803, -62.959, -70.482, -44.580, -64.827, -47.059, -70.419, -66.659 }, // Et[ 50 -  80]
 { -95.714, -81.470, -59.230, -95.502, -83.083, -83.808, -68.931, -67.335, -83.441, -73.737, -42.267, -44.499, -30.000, -50.000 }, // Et[ 80 - 120]
}; 




float resolution = 1.;
float mean = 1.;
float Et = eGeV/cosh(eta);
resolution = interpolateElectronResolutionMap((float *) FakeElERes_Gold_mu200, Et, eta);
resolution = resolution/100.; 

mean = interpolateElectronResolutionMap((float *) FakeElEMean_Gold_mu200, Et, eta);
mean = mean/100.; 

float fakeElEnergy = eMeV * (1 + m_electronRandom.Gaus(mean,resolution) );


//printf("Fake el. energy = %f [ mean=%f res=%f ] Et=%f eta=%f\n",fakeElEnergy/1000.,mean/1000.,resolution/1000., Et, eta); 

return fakeElEnergy;


}




#endif
