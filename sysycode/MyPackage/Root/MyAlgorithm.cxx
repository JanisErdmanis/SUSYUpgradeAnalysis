#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <MyPackage/MyAlgorithm.h>

#include <TH1F.h>
#include <TLorentzVector.h>
#include "xAODJet/JetContainer.h"
#include "xAODRootAccess/TEvent.h"

using namespace std;

class Particle : public TLorentzVector {
 public:
  bool Good;
  double pdgid;
  int nprong;
  double etcone20;
  double ptcone30;
};

std::vector<TString> m_cuts;
vector< Particle > GenJet; 
std::map<TString, TH1F*> h_PtJets1st; //!
const double GeV = 0.001;

struct compare_pt {
  bool operator()(const Particle &left, const Particle &right) {
    return left.Pt() > right.Pt();
  }
};

// this is needed to distribute the algorithm to the workers
ClassImp(MyAlgorithm)

MyAlgorithm :: MyAlgorithm ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode MyAlgorithm :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  job.useXAOD ();

  // ANA_CHECK_SET_TYPE (EL::StatusCode); 

  // ANA_CHECK(xAOD::Init());

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  m_cuts.push_back("NoCuts");

  int i = 0;
  h_PtJets1st[m_cuts[i]] = new TH1F("h_PtJets1st_"+ m_cuts[i], "", 200, 0, 300); 
  h_PtJets1st[m_cuts[i]]->SetXTitle("Jet p_{T} [GeV]");      h_PtJets1st[m_cuts[i]]->SetYTitle("Events / 20 GeV");
  wk()->addOutput (h_PtJets1st[m_cuts[i]]);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MyAlgorithm :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  // xAOD::TEvent* event = wk()->xaodEvent();
  // ANA_CHECK_SET_TYPE (EL::StatusCode);

  // // as a check, let's see the number of events in our xAOD
  // Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

  // m_eventCounter = 0;
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  xAOD::TEvent* event = wk()->xaodEvent();
  
  GenJet.clear();
  
  const xAOD::JetContainer* xTruthJets = 0;
  event->retrieve( xTruthJets, "AntiKt4TruthJets" );
  xAOD::JetContainer::const_iterator jet_itr = xTruthJets->begin();
  xAOD::JetContainer::const_iterator jet_end = xTruthJets->end();
  for( ; jet_itr != jet_end; ++jet_itr ) {
    if( (*jet_itr)->pt()*GeV < 10 ) continue;

    Particle thisPart;
    thisPart.SetPtEtaPhiM( (*jet_itr)->pt(), (*jet_itr)->eta(), (*jet_itr)->phi(), (*jet_itr)->m());
    thisPart.pdgid = fabs((*jet_itr)->auxdata<int>("ConeTruthLabelID"));
    thisPart.Good = true;
    GenJet.push_back(thisPart);
    
  }

  sort(GenJet.begin(), GenJet.end(), compare_pt());
  if (GenJet.size()>=1)
    h_PtJets1st["NoCuts"]->Fill( GenJet[0].Pt()*GeV , 1.);

  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: finalize ()
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
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyAlgorithm :: histFinalize ()
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
