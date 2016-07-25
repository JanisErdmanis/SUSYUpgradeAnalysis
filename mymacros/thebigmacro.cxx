#include "globaldeffs.h"

void thebigmacro() {

  auto h1 = (TH1F*)tfile0->Get("h_PtMuons1st_JetCut Pt>0 GeV");
  h1->SetFillColor(38);
  h1->Draw();
  // stacked->Add(h1);
  
  auto h2 = (TH1F*)tfile0->Get("h_PtMuons1st_JetCut Pt>100 GeV");
  h2->SetFillColor(46);
  h2->Draw("Same");
  //  stacked->Add(h2);
  
  auto h3 = (TH1F*)tfile0->Get("h_PtMuons1st_JetCut Pt>200 GeV");
  h3->SetFillColor(42);
  h3->Draw("Same");

  TLegend leg(.1,.7,.3,.9,"Jet Cutoffs");
  leg.SetFillColor(0);
  leg.AddEntry(h1,"Pt>0");
  leg.AddEntry(h2,"Pt>100");
  leg.AddEntry(h3,"Pt>200");
  leg.DrawClone("Same");

  
//   smallmacro();

//   // auto tfile0 = TFile::Open("outdir_"+samples[0]+"/hist-susyexamples.root");
//   // auto tfile1 = TFile::Open("outdir_"+samples[1]+"/hist-susyexamples.root");

  
// //  TFile *_file0 = TFile::Open("outdir_m100_DeltaM9_N2C1/hist-susyexamples.root");

//   // A comparssion plot
// //   new TCanvas();
//   // auto h_PtMuons1st_0 = (TH1F*)tfile0->Get("h_PtMuons1st_NoCut GeV");
//   // h_PtMuons1st_0->SetFillColor(38);
//   // h_PtMuons1st_0->Draw();
//   //h1->Draw();
  
// //   auto h_PtMuons1st_1 = (TH1F*)tfile1->Get("h_PtMuons1st_NoCut GeV");
//   // h_PtMuons1st_1->SetFillColor(42);
//   // h_PtMuons1st_1->Draw("Same");
      
 }

  
