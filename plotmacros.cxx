//#include "globaldeffs.h"

TString cutname = "NoCuts";
TString cutfolder = "plots/NoCuts"; 
TString xlabel;
//TString hist;
TString HIST = "h_MET_NoCuts";
TCanvas*cvs;
TLegend *leg;

void SetAtlasStyle();

//TString samples[] = {"Wjets_rapidityOrderOff","ttbarJets","TauTauJets","m100_DeltaM9_N2C1","m100_DeltaM9_C1C1","WWJets"};

TString samples[] = {"WJets.v1.0","ttbarJets.v1.0","TauTauJets.v2.0","m100_DeltaM9_N2C1.v2.1","m100_DeltaM9_C1C1.v2.1","WWJets.v1.0","TauTauLowMllJetsFilter.v3.0","TauTauHighMllJetsFilter.v3.0"};

auto tfile1 = TFile::Open("outdir/outdir_"+samples[0]+"/hist-sample.root");
auto tfile2 = TFile::Open("outdir/outdir_"+samples[1]+"/hist-sample.root");
//auto tfile3 = TFile::Open("outdir/outdir_"+samples[2]+"/hist-sample.root");
auto tfile4 = TFile::Open("outdir/outdir_"+samples[3]+"/hist-sample.root");
auto tfile5 = TFile::Open("outdir/outdir_"+samples[4]+"/hist-sample.root");
auto tfile6 = TFile::Open("outdir/outdir_"+samples[5]+"/hist-sample.root");
auto tfile7 = TFile::Open("outdir/outdir_"+samples[6]+"/hist-sample.root");
auto tfile8 = TFile::Open("outdir/outdir_"+samples[7]+"/hist-sample.root");

auto N1 = ((TH1F*)tfile1->Get("h_NEvents_NoCuts"))->GetEntries();
auto N2 = ((TH1F*)tfile2->Get("h_NEvents_NoCuts"))->GetEntries();
//auto N3 = ((TH1F*)tfile3->Get("h_NEvents_NoCuts"))->GetEntries();
auto N4 = ((TH1F*)tfile4->Get("h_NEvents_NoCuts"))->GetEntries();
auto N5 = ((TH1F*)tfile5->Get("h_NEvents_NoCuts"))->GetEntries();
auto N6 = ((TH1F*)tfile6->Get("h_NEvents_NoCuts"))->GetEntries();
auto N7 = ((TH1F*)tfile7->Get("h_NEvents_NoCuts"))->GetEntries();
auto N8 = ((TH1F*)tfile8->Get("h_NEvents_NoCuts"))->GetEntries();

/// Adding comments and fix
// Cross sections in pb
auto sigma1 = 162; // WJets
auto sigma2 = 8.9; // ttbarJets
//auto sigma3 = 8500 * 0.154; // TauTauJets
//auto sigma3 = 0;
auto sigma4 = 5; // N2C1
auto sigma5 = 2.8; // C1C1
auto sigma6 = 1.34; // WWJets
auto sigma7 = 0.40; // TauTauLow
auto sigma8 = 0.93; // TauTauHigh

auto L = 3000*1000; // pb^-1  //000; 3000 fb

auto scale1 = sigma1/N1*L;
auto scale2 = sigma2/N2*L;
//auto scale3 = sigma3/N3*L;
auto scale4 = sigma4/N4*L;
auto scale5 = sigma5/N5*L;
auto scale6 = sigma6/N6*L;
auto scale7 = sigma7/N7*L;
auto scale8 = sigma8/N8*L;

void plothisto() {

  THStack *background = new THStack("stacked","");
  
  auto h1 = (TH1F*)tfile1->Get(HIST);
  h1->Scale(sigma1/N1*L);
  h1->SetFillColor(38);
  //h1->Draw();
  
  auto h2 = (TH1F*)tfile2->Get(HIST);
  h2->Scale(sigma2/N2*L);
  h2->SetFillColor(46);
//  h2->Draw("Same");

// //  auto h3 = (TH1F*)tfile3->Get(HIST);
//   h3->Scale(sigma3/N3*L);
//   h3->SetFillColor(40);

  auto h6 = (TH1F*)tfile6->Get(HIST);
  h6->Scale(sigma6/N6*L);
  h6->SetFillColor(49);

  auto h7 = (TH1F*)tfile7->Get(HIST);
  h7->Scale(sigma7/N7*L);
  h7->SetFillColor(70);

  auto h8 = (TH1F*)tfile8->Get(HIST);
  h8->Scale(sigma8/N8*L);
  h8->SetFillColor(70);

  
//   auto sumh = new TH1F(*h2);
// //  sumh->Add(h1,1.);
//   sumh->Add(h3,1.);
//   sumh->Add(h6,1.);
//   sumh->SetLineWidth(7);
//   sumh->SetLineColor(1);
//   sumh->SetAxisRange(1,sumh->GetMaximum()*5,"Y");
//   sumh->Draw("Samehist");

  background->Add(h7);
  background->Add(h8);
  background->Add(h2);
  background->Add(h1);
  background->Add(h6);
//  background->Add(h3);
  background->Draw("SameHist");

  auto h4 = (TH1F*)tfile4->Get(HIST);
  h4->Scale(sigma4/N4*L);
  h4->SetLineColor(60);
  h4->SetLineWidth(3);
  //h4->SetFillColor(41);
  h4->Draw("SameHist");

  auto h5 = (TH1F*)tfile5->Get(HIST);
  h5->Scale(sigma5/N5*L);
  h5->SetLineColor(44);
  h5->SetLineWidth(3);
//  h5->SetFillColor(42);
  h5->Draw("SameHist");

  leg = new TLegend(.8,.8,1.,1.,"samples");
  leg->SetFillColor(0);
  // leg->AddEntry(h1,samples[0]);
  // leg->AddEntry(h2,samples[1]);
  // leg->AddEntry(h3,samples[2]);
  // leg->AddEntry(h4,samples[3]);
  // leg->AddEntry(h5,samples[4]);

  leg->AddEntry(h1,"Wjets","f");
  leg->AddEntry(h2,"ttbarJets","f");
  //leg->AddEntry(h3,"TauTauJets","f");
  leg->AddEntry(h6,"WWJets","f");
  leg->AddEntry(h4,"N2C1","l");
  leg->AddEntry(h5,"C1C1","l");
  leg->AddEntry(h7,"TauTauJetsLow","f");
  leg->AddEntry(h8,"TauTauJetsHigh","f");
  leg->DrawClone("Same");

  cout << "---------------------------" << endl;
  Double_t error;
  double integral;
  double x = 20;
  integral = h1->IntegralAndError(h1->FindBin(0),h1->FindBin(x),error);
  cout << "Wjets=" << integral << "\t" << error << endl;
  integral = h2->IntegralAndError(h2->FindBin(0),h2->FindBin(x),error);
  cout << "ttbarJets=" << integral << "\t" << error << endl;
  integral = h6->IntegralAndError(h6->FindBin(0),h6->FindBin(x),error);
  cout << "WWJets=" << integral << "\t" << error << endl;
  integral = h7->IntegralAndError(h7->FindBin(0),h7->FindBin(x),error);
  cout << "TauTauJetsLow=" << integral << "\t" << error << endl;
  integral = h8->IntegralAndError(h8->FindBin(0),h8->FindBin(x),error);
  cout << "TauTauJetsHigh=" << integral << "\t" << error << endl;
  integral = h4->IntegralAndError(h4->FindBin(0),h4->FindBin(x),error);
  cout << "N2C1=" << integral << "\t" << error << endl;
  integral = h5->IntegralAndError(h5->FindBin(0),h5->FindBin(x),error);
  cout << "C1C1=" << integral << "\t" << error << endl;
  cout << "---------------------------" << endl;
  
}

void plotoverlaping() {
  
  auto h1 = (TH1F*)tfile1->Get(HIST);
//  h1->Scale(sigma1/N1*L);
  h1->SetLineColor(38);
  h1->SetLineWidth(3);
  h1->Draw();
  
  auto h2 = (TH1F*)tfile2->Get(HIST);
//  h2->Scale(sigma2/N2*L);
  h2->SetLineColor(46);
  h2->SetLineWidth(3);
  h2->Draw("Same");

//   auto h3 = (TH1F*)tfile3->Get(HIST);
// //  h3->Scale(sigma3/N3*L);
//   h3->SetLineColor(40);
//   h3->SetLineWidth(3);
//   h3->Draw("Same");
  
  auto h6 = (TH1F*)tfile6->Get(HIST);
//  h6->Scale(sigma6/N6*L);
  h6->SetLineColor(49);
  h6->SetLineWidth(3);
  h6->Draw("Same");

//   background->Add(h1);
//   background->Add(h2);
//   background->Add(h3);
// //  background->Add(h6);
    
//   background->Draw("hist");

  auto h4 = (TH1F*)tfile4->Get(HIST);
//  h4->Scale(sigma4/N4*L);
  h4->SetLineColor(60);
  h4->SetLineWidth(3);
  h4->Draw("Same");
  //h4->SetFillColor(41);

  auto h5 = (TH1F*)tfile5->Get(HIST);
//  h5->Scale(sigma5/N5*L);
  h5->SetLineColor(2);
  h5->SetLineWidth(3);
//  h5->SetFillColor(42);
  h5->Draw("Same");

  TLegend leg(.7,.7,.9,0.8,"samples");
  leg.SetFillColor(0);
  leg.AddEntry(h1,samples[0]);
  leg.AddEntry(h2,samples[1]);
//  leg.AddEntry(h3,samples[2]);
  leg.AddEntry(h4,samples[3]);
  leg.AddEntry(h5,samples[4]);
  leg.AddEntry(h6,samples[5]);
  leg.DrawClone("Same");

}

// double significance(double *x,double *par) {
//   int s = ((TH1F*)tfile1->Get("h_MET_NoCuts"))->Integral(0,*x);
//   return s;
// }

// void plotintegral() {

//   // double x = 3;
//   // int s = ((TH1F*)tfile1->Get("h_MET_NoCuts"))->Integral(0,x);
//   double x = 3.;
//   cout << integr(&x,&x) << endl;
// }

void plotsmearingjets() {
  
  auto tfile = tfile5;

  SetAtlasStyle();

  cvs = new TCanvas();

  botframe = cvs->DrawFrame(0,1e3,150,1e5);
  botframe->SetTitle("Title;Pt of leading jet;Events");
  // botframe = cvs->DrawFrame(0,1,800,1e8);
  // xlabel = "MSFOS";
  // HIST = "h_MSFOS_"+cutname;
  // plothisto();
  cvs->SetLogy();
  // botframe->SetTitle("The title;MSFOS [GeV];Events");
  // cvs->SaveAs(cutfolder+"/"+"h_MSFOS"+".png");

  
  auto h1 = (TH1F*)tfile->Get("h_PtJets1stStages_Generator");
  h1->SetLineColor(60);
  h1->SetLineWidth(3);
  //h4->SetFillColor(41);
  h1->Draw("SameHist");

  auto h2 = (TH1F*)tfile->Get("h_PtJets1stStages_Smearing");
  h2->SetLineColor(50);
  h2->SetLineWidth(3);
//  h5->SetFillColor(42);
  h2->Draw("SameHist");

  auto h3 = (TH1F*)tfile->Get("h_PtJets1stStages_PtEtaThresholds");
  h3->SetLineColor(40);
  h3->SetLineWidth(3);
  //h5->SetFillColor(42);
  h3->Draw("SameHist");

  auto h4 = (TH1F*)tfile->Get("h_PtJets1stStages_OverlapRemoval");
  h4->SetLineColor(30);
  h4->SetLineWidth(3);
  //  h5->SetFillColor(42);
  h4->Draw("SameHist");


  auto leg = new TLegend(.8,.8,1.,1.,"Smearing Stages");
  leg->SetFillColor(0);
  leg->AddEntry(h1,"Generator");
  leg->AddEntry(h2,"Smearing");
  leg->AddEntry(h3,"Fakes");
  leg->AddEntry(h4,"OverlapRemoval");
  leg->DrawClone("Same");

  cvs->SetLogy();
  cvs->SaveAs("plots/h_PtJets1stStages.png");
  
}

void plotsmearingleptons() {
  
  auto tfile = tfile5;

  SetAtlasStyle();

  cvs = new TCanvas();

  botframe = cvs->DrawFrame(0,1e3,20,1e5);
  botframe->SetTitle("Title;Pt of leding lepton;Events");
  // botframe = cvs->DrawFrame(0,1,800,1e8);
  // xlabel = "MSFOS";
  // HIST = "h_MSFOS_"+cutname;
  // plothisto();
  cvs->SetLogy();
  // botframe->SetTitle("The title;MSFOS [GeV];Events");
  // cvs->SaveAs(cutfolder+"/"+"h_MSFOS"+".png");

  
  auto h1 = (TH1F*)tfile->Get("h_PtEleMuo1stStages_Generator");
  h1->SetLineColor(60);
  h1->SetLineWidth(3);
  //h4->SetFillColor(41);
  h1->Draw("SameHist");

  auto h2 = (TH1F*)tfile->Get("h_PtEleMuo1stStages_Smearing");
  h2->SetLineColor(50);
  h2->SetLineWidth(3);
//  h5->SetFillColor(42);
  h2->Draw("SameHist");

  auto h3 = (TH1F*)tfile->Get("h_PtEleMuo1stStages_PtEtaThresholds");
  h3->SetLineColor(40);
  h3->SetLineWidth(3);
  //  h5->SetFillColor(42);
  h3->Draw("SameHist");

  auto h4 = (TH1F*)tfile->Get("h_PtEleMuo1stStages_OverlapRemoval");
  h4->SetLineColor(30);
  h4->SetLineWidth(3);
  //  h5->SetFillColor(42);
  h4->Draw("SameHist");


  auto leg = new TLegend(.8,.8,1.,1.,"Smearing Stages");
  leg->SetFillColor(0);
  leg->AddEntry(h1,"Generator");
  leg->AddEntry(h2,"Smearing");
  leg->AddEntry(h3,"Fakes");
  leg->AddEntry(h4,"OverlapRemoval");
  leg->DrawClone("Same");

  cvs->SetLogy();
  cvs->SaveAs("plots/h_PtEleMuo1stStages.png");
  
}


void plotmacros() {

  // auto tfile = TFile::Open("outdir/outdir_ttbarJets/hist-sample.root");
  // auto h = (TH1F*)tfile->Get("h_NEvents_NoCuts");
  // auto entries = h->GetEntries();
}

void plotsignificance() {
  
  auto h1 = (TH1F*)tfile1->Get(HIST);
  h1->Scale(sigma1/N1*L);

  auto h2 = (TH1F*)tfile2->Get(HIST);
  h2->Scale(sigma2/N2*L);

  // auto h3 = (TH1F*)tfile3->Get(HIST);
  // h3->Scale(sigma3/N3*L);

  auto h6 = (TH1F*)tfile6->Get(HIST);
  h6->Scale(sigma6/N6*L);

  auto h4 = (TH1F*)tfile4->Get(HIST);
  h4->Scale(sigma4/N4*L);

  auto h5 = (TH1F*)tfile5->Get(HIST);
  h5->Scale(sigma5/N5*L);

  auto maximum = 400; // Can be read from histo
  auto x = 0;

//  TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);

  gr = new TGraph();

  // int n = 100;
  // for(int i=0;i<n;i++) {
  //   x = 0 + maximum*i/n;
  //   double bkg = h2->Integral(h2->FindBin(x),-1) + h3->Integral(h3->FindBin(x),-1) + h6->Integral(h6->FindBin(x),100);
  //   double sign = h4->Integral(h4->FindBin(x),-1) + h5->Integral(h5->FindBin(x),-1);

  //   //double significance = RooStats::NumberCountingUtils::BinomialExpP(sign+bkg,bkg,0.3);

  //   double significance = 0;
  //   //cout << bkg << " " << sign << " " << significance << endl;

  //   gr->SetPoint(i,x,significance);
    
  // }
  
  gr->Draw("AC*");
  gr->SetTitle("Significance for MET; MET cut; significance");
  //cout << h1->Integral(0,100) << endl;
  //  cout << h2->Integral(0,100) 
}

#include <sys/stat.h>

TH1F * botframe;

void h_allhistos() {

  mkdir(cutfolder,0700);
  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,500,1e9);
  HIST = "h_MET_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Missing Energy [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_MET"+".png");

  // // Here I could do a significance plot!!!
  // cvs = new TCanvas();
  // plotsignificance();
  // cvs->SaveAs(cutfolder+"/"+"h_MET" + "_significance" + ".png");
  
  gROOT->Reset();
  cvs = new TCanvas();
  botframe =  cvs->DrawFrame(0,1,300,1e9);
  xlabel = "1st Jet Pt";
  HIST = "h_PtJets1st_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;1st Jet Pt [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_PtJets1st"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,25,1e8);
  xlabel = "1st Muon Pt";
  HIST = "h_PtMuons1st_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;1st lepton Pt [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_PtMuons1st"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,25,1e8);
  xlabel = "2nd Muon Pt";
  HIST = "h_PtMuons2nd_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;2nd lepton Pt [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_PtMuons2nd"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,25,1e8);
  xlabel = "3rd Muon Pt";
  HIST = "h_PtMuons3rd_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;3rd lepton Pt [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_PtMuons3rd"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(-4,1,+4,1e9);
  xlabel = "Pseidorapidity of 1st Jet";
  HIST = "h_angleJets1st_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Pseidorapidity of 1st Jet;Events");
  cvs->SaveAs(cutfolder+"/"+"h_angleJets1st"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(-4,1,+4,1e9);
  xlabel = "Pseidorapidity of 1st Muon";
  HIST = "h_angleMuons1st_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Pseidorapidity of 1st lepton;Events");
  cvs->SaveAs(cutfolder+"/"+"h_angleMuons1st"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,100,1e7);
  xlabel = "Two leading lepton mass";
  HIST = "h_llmass_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Two leading lepton mass [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_llmass"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,18,1e9);
  xlabel = "Number of Jets in the event";
  HIST = "h_NJet_"+cutname;
//  plotoverlaping();
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Number of Jets in the event;Events");
  cvs->SaveAs(cutfolder+"/"+"h_NJet"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,6,1e9);
  xlabel = "Number of BJets in the event";
  HIST = "h_NBJet_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Number of B Jets in the event;Events");
  cvs->SaveAs(cutfolder+"/"+"h_NBJet"+".png");

  // // First Jet Pt without normalisation
  // gROOT->Reset();
  // cvs = new TCanvas();
  // botframe = cvs->DrawFrame(0,1,300,1e9);
  // xlabel = "1st Jet Pt";
  // HIST = "h_PtJets1st_"+cutname;
  // //plothisto();
  // plotoverlaping();
  // cvs->SetLogy();
  // //SEGFAULT  botframe->SetTitle("The title;1st Jet Pt [GeV];Events");
  // cvs->SaveAs(cutfolder+"/"+"h_PtJets1st_Brian"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,800,1e8);
  xlabel = "MSFOS";
  HIST = "h_MSFOS_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;MSFOS [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_MSFOS"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,800,1e8);
  xlabel = "Mtautau1";
  HIST = "h_mtautau1_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Mtautau1 [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_Mtautau1"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,800,1e8);
  xlabel = "Mtautau2";
  HIST = "h_mtautau2_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Mtautau2 [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_Mtautau2"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,800,1e8);
  xlabel = "MT";
  HIST = "h_MT_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;MT [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_MT"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,6,1e8);
  xlabel = "Electron multiplicity";
  HIST = "h_NElec_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;ELectron multiplicity;Events");
  cvs->SaveAs(cutfolder+"/"+"h_NElec"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,6,1e8);
  xlabel = "Muon multiplicity";
  HIST = "h_NMuon_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;Muon multiplicity;Events");
  cvs->SaveAs(cutfolder+"/"+"h_NMuon"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,200,1e8);
  xlabel = "MT2";
  HIST = "h_MT2_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;MT2 [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_MT2"+".png");

  gROOT->Reset();
  cvs = new TCanvas();
  botframe = cvs->DrawFrame(0,1,200,1e8);
  xlabel = "MT2Jets";
  HIST = "h_MT2Jets_"+cutname;
  plothisto();
  cvs->SetLogy();
  botframe->SetTitle("The title;MT2Jets [GeV];Events");
  cvs->SaveAs(cutfolder+"/"+"h_MT2Jets"+".png");

}

void h_plotwithallcuts() {

  cout << "WJets=" << N1 << endl;
  cout << "ttbarJets=" << N2 << endl;
//  cout << "TauTauJets=" << N3 << endl;
  cout << "m100_DeltaM9_N2C1=" << N4 << endl;
  cout << "m100_DeltaM9_C1C1=" << N5 << endl;
  cout << "WWJets=" << N6 << endl;

  SetAtlasStyle();
    
  cutname = "NoCuts";
  cutfolder = "plots/NoCuts";
  h_allhistos();

  cutname = "MET>200 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "1stJet Pt>100 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "DPhi>0.4";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "2 leading leptons Pt > 5 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "mTauTau>150 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "12<M(1st l + 2nd l)<50 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "Pt 1st and 2nd lepton < 15 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  cutname = "MSFOS < 60 GeV";
  cutfolder += "/" + cutname;
  h_allhistos();

  // auto temp = cutfolder;
  // cutname = "2 leptons";
  // cutfolder = temp + "/" + cutname;
  // h_allhistos();

  // cutname = "3 leptons";
  // cutfolder = temp + "/" + cutname;
  // h_allhistos();

  // // cutname = "1stJet Pt>200 GeV";
  // // cutfolder = "NoCuts/" + cutname; // "1stJet Pt>200 GeV"; //CutName1;
  // // h_allhistos();

  
  // cutname = "MET>200 GeV";
  // cutfolder += "/" + cutname; 
  // h_allhistos();

  // cutname = "Dphi(JET,MET)>0.4";
  // cutfolder += "/" + cutname; 
  // h_allhistos();
  
  // cutname = "2 leading muons Pt > 5 GeV";
  // cutfolder += "/" + cutname; 
  // h_allhistos();

  // cutname = "BJetCut";
  // cutfolder += "/" + cutname; 
  // h_allhistos();

  // cutname = "1stJet Pt>100 GeV";
  // cutfolder = "NoCuts/" + cutname;
  // h_allhistos();

  // cutname = "Big_angle_between_jets";
  // cutfolder += "/" + cutname;
  // h_allhistos();

  // cutname = "Dphi(2nd Jet,MET)>0.5";
  // cutfolder += "/" + cutname;
  // h_allhistos();

  // cutname = "M(1st l + 2nd l)<12 GeV";
  // cutfolder += "/" + cutname;
  // h_allhistos();
  
}



