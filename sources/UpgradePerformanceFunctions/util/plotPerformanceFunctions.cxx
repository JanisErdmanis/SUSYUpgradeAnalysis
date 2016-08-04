#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include "TStyle.h"
#include "TString.h"
#include "TColor.h"
#include "TFile.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"


void plotElectronEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotElectronTriggerEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotMuonEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotMuonTriggerEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotElectronEnergyResolution(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt);
void plotFlavourTagEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt, char flavour, TString Tagger, int operating_point);
void plotMETResolution(UpgradePerformanceFunctions *m_upgrade, float sumEtGeV, float METxGeV, float METyGeV);
void plotTrackJetConfirmEff(UpgradePerformanceFunctions *m_upgrade, float plot_pt);

int main() {
  // This hack is needed to force dictionary loading in ROOT 5
  gROOT->ProcessLine("#include <vector>");

  // Style choices for axis on plots
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");

  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  UpgradePerformanceFunctions *m_upgrade = new UpgradePerformanceFunctions();

  plotElectronEfficiency(m_upgrade,3.3,80.0);
  plotElectronTriggerEfficiency(m_upgrade,0.5,45.0);
  plotElectronEnergyResolution(m_upgrade,3.3,100.0);
  plotMuonEfficiency(m_upgrade,2,80.0);
  plotMuonTriggerEfficiency(m_upgrade,0.61,45.0);
  plotFlavourTagEfficiency(m_upgrade,1.5,45,'B',"mv1",70);
  plotMETResolution(m_upgrade, 100,200,0);
  plotTrackJetConfirmEff(m_upgrade,35);

  return 0;
}


void plotElectronEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Efficiency Plots" << std::endl;

  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::tightElectron);
  m_upgrade->setElectronRandomSeed(1);

  TH2D *gold_ElectronEfficiency = new TH2D("gold_ElectronEfficiency","gold_ElectronEfficiency",1000,0,120,1000,0,5);
  TH2D *silver_ElectronEfficiency = new TH2D("silverElectronEfficiency","silver_ElectronEfficiency",1000,0,120,1000,0,5);
  TH2D *bronze_ElectronEfficiency = new TH2D("bronze_ElectronEfficiency","bronze_ElectronEfficiency",1000,0,120,1000,0,5);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
      gold_ElectronEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
      silver_ElectronEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
      bronze_ElectronEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getElectronEfficiency(pt_f*1000,eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0/1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  silver_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);
  bronze_graph_eta->GetXaxis()->SetTitle("Eta");

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D
  gStyle->SetNumberContours(100);
  TCanvas *can = new TCanvas("can","Electron Efficiency",1000,1000);
  can->Divide(2,3);

  can->cd(1);
  gold_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  gold_ElectronEfficiency->GetYaxis()->SetTitle("Eta"); 
  gold_ElectronEfficiency->SetTitle("Tight Gold ElectronEfficiency"); 
  gold_ElectronEfficiency->SetTitle("Tight Gold ElectronEfficiency"); 
  gold_ElectronEfficiency->SetStats(0);
  gold_ElectronEfficiency->Draw("colz");

  can->cd(2);
  silver_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  silver_ElectronEfficiency->GetYaxis()->SetTitle("Eta"); 
  silver_ElectronEfficiency->SetTitle("Tight Silver ElectronEfficiency"); 
  silver_ElectronEfficiency->SetStats(0);
  silver_ElectronEfficiency->Draw("colz");

  can->cd(3);
  bronze_ElectronEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  bronze_ElectronEfficiency->GetYaxis()->SetTitle("Eta"); 
  bronze_ElectronEfficiency->SetTitle("Tight Bronze ElectronEfficiency"); 
  bronze_ElectronEfficiency->SetStats(0);
  bronze_ElectronEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Electron Efficiency in eta for pt = " 
		   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Electron Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Electron Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Electron Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("ElectronEfficiency.png");
  can->Close();

  return;
}

void plotElectronTriggerEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Trigger Efficiency Plots" << std::endl;

  //  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::tightElectron);
  // m_upgrade->setElectronRandomSeed(1);

  TH2D *gold_ElectronTriggerEfficiency = new TH2D("gold_ElectronTriggerEfficiency","gold_ElectronTriggerEfficiency",1000,0,120,1000,-5,5);
  TH2D *silver_ElectronTriggerEfficiency = new TH2D("silverElectronTriggerEfficiency","silver_ElectronTriggerEfficiency",1000,0,120,1000,-5,5);
  TH2D *bronze_ElectronTriggerEfficiency = new TH2D("bronze_ElectronTriggerEfficiency","bronze_ElectronTriggerEfficiency",1000,0,120,1000,-5,5);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = -5.;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
      gold_ElectronTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
      silver_ElectronTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
      bronze_ElectronTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getSingleElectronTriggerEfficiency(pt_f*1000,eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 10.0/1000.0; //iterate eta from -5 to 5 in .01 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can","ElectronTrigger Efficiency",1000,1000);
  can->Divide(2,3);

  can->cd(1);
  gold_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  gold_ElectronTriggerEfficiency->SetTitle("Tight Gold ElectronTriggerEfficiency"); 
  gold_ElectronTriggerEfficiency->SetStats(0);
  gold_ElectronTriggerEfficiency->Draw("colz");

  can->cd(2);
  silver_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  silver_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  silver_ElectronTriggerEfficiency->SetTitle("Tight Silver ElectronTriggerEfficiency"); 
  silver_ElectronTriggerEfficiency->SetStats(0);
  silver_ElectronTriggerEfficiency->Draw("colz");

  can->cd(3);
  bronze_ElectronTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_ElectronTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  bronze_ElectronTriggerEfficiency->SetTitle("Tight Bronze ElectronTriggerEfficiency"); 
  bronze_ElectronTriggerEfficiency->SetStats(0);
  bronze_ElectronTriggerEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Electron Trigger Efficiency in eta for pt = " 
		   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Electron Trigger Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Electron Trigger Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Electron Trigger Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("ElectronTriggerEfficiency.png");
  can->Close();

  return;
}


void plotMuonEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight muon efficiency for pt and eta
  std::cout << "Generating Muon Efficiency Plots" << std::endl;

  m_upgrade->setMuonWorkingPoint(UpgradePerformanceFunctions::tightMuon);

  TH2D *gold_MuonEfficiency = new TH2D("gold_MuonEfficiency","gold_MuonEfficiency",1000,0,120,1000,0,5);
  TH2D *silver_MuonEfficiency = new TH2D("silverMuonEfficiency","silver_MuonEfficiency",1000,0,120,1000,0,5);
  TH2D *bronze_MuonEfficiency = new TH2D("bronze_MuonEfficiency","bronze_MuonEfficiency",1000,0,120,1000,0,5);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
      gold_MuonEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
      silver_MuonEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
      bronze_MuonEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getMuonEfficiency(pt_f*1000,eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0/1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  silver_graph_eta->GetXaxis()->SetTitle("Eta");
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);
  bronze_graph_eta->GetXaxis()->SetTitle("Eta");

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D
  gStyle->SetNumberContours(100);
  TCanvas *can = new TCanvas("can","Muon Efficiency",1000,1000);
  can->Divide(2,3);

  can->cd(1);
  gold_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  gold_MuonEfficiency->GetYaxis()->SetTitle("Eta"); 
  gold_MuonEfficiency->SetTitle("Tight Gold MuonEfficiency"); 
  gold_MuonEfficiency->SetTitle("Tight Gold MuonEfficiency"); 
  gold_MuonEfficiency->SetStats(0);
  gold_MuonEfficiency->Draw("colz");

  can->cd(2);
  silver_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  silver_MuonEfficiency->GetYaxis()->SetTitle("Eta"); 
  silver_MuonEfficiency->SetTitle("Tight Silver MuonEfficiency"); 
  silver_MuonEfficiency->SetStats(0);
  silver_MuonEfficiency->Draw("colz");

  can->cd(3);
  bronze_MuonEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  bronze_MuonEfficiency->GetYaxis()->SetTitle("Eta"); 
  bronze_MuonEfficiency->SetTitle("Tight Bronze MuonEfficiency"); 
  bronze_MuonEfficiency->SetStats(0);
  bronze_MuonEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Muon Efficiency in eta for pt = " 
		   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Muon Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Muon Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Muon Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("MuonEfficiency.png");
  can->Close();

  return;
}


void plotMuonTriggerEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight muon efficiency for pt and eta
  std::cout << "Generating Muon Trigger Efficiency Plots" << std::endl;

  TH2D *gold_MuonTriggerEfficiency = new TH2D("gold_MuonTriggerEfficiency","gold_MuonTriggerEfficiency",1000,0,120,1000,-3,3);
  TH2D *silver_MuonTriggerEfficiency = new TH2D("silverMuonTriggerEfficiency","silver_MuonTriggerEfficiency",1000,0,120,1000,-3,3);
  TH2D *bronze_MuonTriggerEfficiency = new TH2D("bronze_MuonTriggerEfficiency","bronze_MuonTriggerEfficiency",1000,0,120,1000,-3,3);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = -3.;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
      gold_MuonTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
      silver_MuonTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
      bronze_MuonTriggerEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getSingleMuonTriggerEfficiency(pt_f*1000,eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 6.0/1000.0; //iterate eta from -3 to 3 in .006 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can","MuonTrigger Efficiency",1000,1000);
  can->Divide(2,3);

  can->cd(1);
  gold_MuonTriggerEfficiency->SetTitle("Tight Gold MuonTriggerEfficiency"); 
  gold_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  gold_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  gold_MuonTriggerEfficiency->SetStats(0);
  gold_MuonTriggerEfficiency->Draw("colz");

  can->cd(2);
  silver_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  silver_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  silver_MuonTriggerEfficiency->SetTitle("Tight Silver MuonTriggerEfficiency"); 
  silver_MuonTriggerEfficiency->SetStats(0);
  silver_MuonTriggerEfficiency->Draw("colz");

  can->cd(3);
  bronze_MuonTriggerEfficiency->GetXaxis()->SetTitle("Pt (GeV)");
  bronze_MuonTriggerEfficiency->GetYaxis()->SetTitle("Eta");
  bronze_MuonTriggerEfficiency->SetTitle("Tight Bronze MuonTriggerEfficiency"); 
  bronze_MuonTriggerEfficiency->SetStats(0);
  bronze_MuonTriggerEfficiency->Draw("colz");

  can->cd(4);
  std::stringstream eta_stringstream;
  eta_stringstream << "Sweep of Muon Trigger Efficiency in eta for pt = " 
		   << plot_pt << "GeV";
  std::string eta_string(eta_stringstream.str());
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Muon Trigger Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::stringstream pt_stringstream;
  pt_stringstream << "Sweep of Muon Trigger Efficiency in pt for eta = " << plot_eta;
  std::string pt_string(pt_stringstream.str());
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Muon Trigger Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("MuonTriggerEfficiency.png");
  can->Close();

  return;
}


void plotElectronEnergyResolution(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt) {
  // Make plot of Tight electron efficiency for pt and eta
  std::cout << "Generating Electron Energy Resolution Plots" << std::endl;

  m_upgrade->setElectronWorkingPoint(UpgradePerformanceFunctions::tightElectron);
  m_upgrade->setElectronRandomSeed(1);

  TH2D *gold_ElectronEnergyResolution = new TH2D("gold_ElectronEnergyResolution","gold_ElectronEnergyResolution",1000,0,120,1000,0,5);
  TH2D *silver_ElectronEnergyResolution = new TH2D("silverElectronEnergyResolution","silver_ElectronEnergyResolution",1000,0,120,1000,0,5);
  TH2D *bronze_ElectronEnergyResolution = new TH2D("bronze_ElectronEnergyResolution","bronze_ElectronEnergyResolution",1000,0,120,1000,0,5);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
      gold_ElectronEnergyResolution->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
      silver_ElectronEnergyResolution->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
      bronze_ElectronEnergyResolution->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getElectronEnergyResolution(pt_f*1000,eta_f);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0/1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can","Electron EnergyResolution",1000,1000);
  can->Divide(2,3);

  can->cd(1);
  gold_ElectronEnergyResolution->SetTitle("Tight Gold Electron Energy Resolution"); 
  gold_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)"); 
  gold_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta"); 
  gold_ElectronEnergyResolution->SetStats(0);
  gold_ElectronEnergyResolution->Draw("colz");

  can->cd(2);
  silver_ElectronEnergyResolution->SetTitle("Tight Silver Electron Energy Resolution"); 
  silver_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)"); 
  silver_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta"); 
  silver_ElectronEnergyResolution->SetStats(0);
  silver_ElectronEnergyResolution->Draw("colz");

  can->cd(3);
  bronze_ElectronEnergyResolution->SetTitle("Tight Bronze Electron Energy Resolution"); 
  bronze_ElectronEnergyResolution->GetXaxis()->SetTitle("Pt (GeV)"); 
  bronze_ElectronEnergyResolution->GetYaxis()->SetTitle("Eta"); 
  bronze_ElectronEnergyResolution->SetStats(0);
  bronze_ElectronEnergyResolution->Draw("colz");

  can->cd(4);
  std::string eta_string = "Sweep of Electron Energy Resolution in eta for pt = " + std::to_string(plot_pt) + "GeV";
  const char *eta_title = eta_string.c_str();
  gold_graph_eta->SetTitle(eta_title);
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Abolute Electron Energy Resolution");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::string pt_string = "Sweep of Electron Energy Resolution in pt for eta = " + std::to_string(plot_eta);
  const char *pt_title = pt_string.c_str();
  gold_graph_pt->SetTitle(pt_title);
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Abolute Electron Energy Resolution");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  can->Update();
  can->Print("ElectronEnergyResolution.png");
  can->Close();

  return;
}

void plotFlavourTagEfficiency(UpgradePerformanceFunctions *m_upgrade, float plot_eta, float plot_pt, char flavour, TString Tagger, int operating_point) {
  // Make plot of choosen Flavour tagging efficiency for pt and eta
  std::cout << "Generating " << flavour << "Tagging Efficiency plots" << std::endl;

  //Choose file containing histograms for Flavour tagging
  m_upgrade->setFlavourTaggingCalibrationFilename("UpgradePerformanceFunctions/flavor_tags_v0.7.root");

  TH2D *gold_FlavourTagEfficiency = new TH2D("gold_FlavourTagEfficiency","gold_FlavourTagEfficiency",1000,0,120,1000,0,5);
  TH2D *silver_FlavourTagEfficiency = new TH2D("silverFlavourTagEfficiency","silver_FlavourTagEfficiency",1000,0,120,1000,0,5);
  TH2D *bronze_FlavourTagEfficiency = new TH2D("bronze_FlavourTagEfficiency","bronze_FlavourTagEfficiency",1000,0,120,1000,0,5);
  
  std::vector<float> eta; //Store x values for eta scan
  std::vector<float> pt; //Store x values for pt scan
  std::vector<float> gold_eta; //Store gold eta scan
  std::vector<float> silver_eta; //Store silver eta scan
  std::vector<float> bronze_eta; //Store bronze eta scan
  std::vector<float> gold_pt; //Store gold pt scan
  std::vector<float> silver_pt; //Store silver pt scan
  std::vector<float> bronze_pt; //Store bronze pt scan
  
  float pt_f = 0;
  float eta_f = 0;
  float result_f = 0;

  for(int j=0;j<1000;j++){ //Begin eta loop
    
    for(int i=0;i<1000;i++){ //Begin pt loop
      m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
      result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
      gold_FlavourTagEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        plot_eta = eta_f;
        gold_pt.push_back(result_f);
        pt.push_back(pt_f);
      }

      m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
      result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
      silver_FlavourTagEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        silver_pt.push_back(result_f);
      }
  
      m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
      result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
      bronze_FlavourTagEfficiency->SetBinContent(i,j,result_f);
      if(eta_f < (plot_eta + .004) && eta_f > (plot_eta - .004)) {
        bronze_pt.push_back(result_f);
      }

      if(pt_f < (plot_pt + .11) && pt_f > (plot_pt - .11)){
        plot_pt = pt_f;
        eta.push_back(eta_f);
        
        m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
        result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
        gold_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
        result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
        silver_eta.push_back(result_f);

        m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
        result_f = m_upgrade->getFlavourTagEfficiency(pt_f*1000,eta_f,flavour,Tagger,operating_point);
        bronze_eta.push_back(result_f);
      }

      pt_f = pt_f + 120.0/1000.0; //iterate pt from 0 to 120 in .12(GeV) steps
    } // End pt loop

    eta_f = eta_f + 5.0/1000.0; //iterate eta from 0 to 5 in .005 steps
    pt_f = 0;
  } //End eta loop

  // Create eta scan TGraph objects and edit for plotting
  TGraph *gold_graph_eta = new TGraph(eta.size(),&eta[0],&gold_eta[0]);
  gold_graph_eta->SetLineColor(2);
  gold_graph_eta->SetMarkerColor(2);
  TGraph *silver_graph_eta = new TGraph(eta.size(),&eta[0],&silver_eta[0]);
  silver_graph_eta->SetLineColor(4);
  silver_graph_eta->SetMarkerColor(4);
  TGraph *bronze_graph_eta = new TGraph(eta.size(),&eta[0],&bronze_eta[0]);
  bronze_graph_eta->SetLineColor(1);
  bronze_graph_eta->SetMarkerColor(1);

  // Create pt scan TGraph objects and edit for plotting
  TGraph *gold_graph_pt = new TGraph(pt.size(),&pt[0],&gold_pt[0]);
  gold_graph_pt->SetLineColor(2);
  gold_graph_pt->SetMarkerColor(2);
  TGraph *silver_graph_pt = new TGraph(pt.size(),&pt[0],&silver_pt[0]);
  silver_graph_pt->SetLineColor(4);
  silver_graph_pt->SetMarkerColor(4);
  TGraph *bronze_graph_pt = new TGraph(pt.size(),&pt[0],&bronze_pt[0]);
  bronze_graph_pt->SetLineColor(1);
  bronze_graph_pt->SetMarkerColor(1);

  gStyle->SetPalette(55);// Rainbow palette for TH2D

  TCanvas *can = new TCanvas("can","Flavour Tagging Efficiency",1000,1000);
  can->Divide(2,3);

  std::string flavour_string(1,flavour); 

  can->cd(1);
  std::string gold_histo_name = "Gold " + flavour_string + "-Tagging Efficiency";
  gold_FlavourTagEfficiency->SetTitle(gold_histo_name.c_str()); 
  gold_FlavourTagEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  gold_FlavourTagEfficiency->GetYaxis()->SetTitle("Eta"); 
  gold_FlavourTagEfficiency->SetStats(0);
  gold_FlavourTagEfficiency->Draw("colz");

  can->cd(2);
  std::string silver_histo_name = "Silver " + flavour_string + "-Tagging Efficiency";
  silver_FlavourTagEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  silver_FlavourTagEfficiency->GetYaxis()->SetTitle("Eta"); 
  silver_FlavourTagEfficiency->SetTitle(silver_histo_name.c_str()); 
  silver_FlavourTagEfficiency->SetStats(0);
  silver_FlavourTagEfficiency->Draw("colz");

  can->cd(3);
  std::string bronze_histo_name = "Bronze " + flavour_string + "-Tagging Efficiency";
  bronze_FlavourTagEfficiency->GetXaxis()->SetTitle("Pt (GeV)"); 
  bronze_FlavourTagEfficiency->GetYaxis()->SetTitle("Eta"); 
  bronze_FlavourTagEfficiency->SetTitle(bronze_histo_name.c_str()); 
  bronze_FlavourTagEfficiency->SetStats(0);
  bronze_FlavourTagEfficiency->Draw("colz");

  can->cd(4);
  std::string eta_string = "Sweep of " + flavour_string + "-Tagging efficiency in eta for pt = " + std::to_string(plot_pt) + "GeV";
  gold_graph_eta->SetTitle(eta_string.c_str());
  gold_graph_eta->GetXaxis()->SetTitle("Eta");
  gold_graph_eta->GetYaxis()->SetTitle("Flavour Tagging Efficiency");
  gold_graph_eta->Draw("ALP");
  silver_graph_eta->Draw("LP");
  bronze_graph_eta->Draw("LP");

  can->cd(5);
  std::string pt_string = "Sweep of " + flavour_string + "-Tagging efficiency in pt for eta = " + std::to_string(plot_eta);
  gold_graph_pt->SetTitle(pt_string.c_str());
  gold_graph_pt->GetXaxis()->SetTitle("Pt (GeV)");
  gold_graph_pt->GetYaxis()->SetTitle("Flavour Tagging Efficiency");
  gold_graph_pt->Draw("ALP");
  silver_graph_pt->Draw("LP");
  bronze_graph_pt->Draw("LP");

  std::string filename = flavour_string + "-TagEfficiency.png";

  can->Update();
  can->Print(filename.c_str());
  can->Close();

  return;
}


void plotMETResolution(UpgradePerformanceFunctions *m_upgrade, float sumEtGeV, float METxGeV, float METyGeV){
  // Make plot of Missing Transverse Energy performance
  std::cout << "Generating Missing Transverse Energy performance plots" << std::endl;

  //Choose file containing histograms for Missing Transverse Energy
  m_upgrade->loadMETHistograms("UpgradePerformanceFunctions/sumetPU_mu200_ttbar_gold.root");

  // Set random number generator seed for MET smearing function
  m_upgrade->setMETRandomSeed(1);

  // Set average pileup for analysis (only mu = 200 exists currently)
  m_upgrade->setAvgMu(200.);

  // Storage container for MET result defined by UpgradePerformaceFunctions
  UpgradePerformanceFunctions::MET met_result; // Definition: typedef std::pair<double,double> MET
  met_result.first = 0; // initialize METxMeV to 0
  met_result.second = 0; // initialize METyMeV to 0

  TH1D *SmearedMETx = new TH1D("SmearedMETx","SmearedMETx",100,-200,200);
  TH1D *SmearedMETy = new TH1D("SmearedMETy","SmearedMETy",100,-200,200);
  TH1D *SmearedMETphi = new TH1D("SmearedMETphi","SmearedMETphiSmeared",100,-4,4);

  std::vector<float> gold_resol; //Store gold MET resolution
  std::vector<float> silver_resol; //Store silver MET resolution
  std::vector<float> bronze_resol; //Store bronze MET resolution
  std::vector<float> sumEt; // Store sum Et for plotting  

  float result_f = 0;
  float temp_sumEt = 0;
  float temp_METx = 0;
  float temp_METy = 0;
  float temp_phi = 0;
  float truth_phi = atan2(METyGeV,METxGeV);

  for(int i=0;i<1000000;i++){ //Create 100000 smeared events
    m_upgrade->setLayout(UpgradePerformanceFunctions::gold); // Only gold implemented
    met_result = m_upgrade->getMETSmeared(sumEtGeV*1000,METxGeV*1000,METyGeV*1000);

    temp_METx = met_result.first/1000; // Smeared METx in GeV
    temp_METy = met_result.second/1000; // Smeared METy in GeV
    temp_phi = atan2(temp_METy,temp_METx);

    SmearedMETx->Fill(temp_METx - METxGeV); // Center Distribution at 0 
    SmearedMETy->Fill(temp_METy - METyGeV); // Center distribution at 0

    if((temp_phi-truth_phi) <= -M_PI) {
        temp_phi = temp_phi + 2*M_PI - truth_phi;
    }
    else {
        temp_phi = temp_phi - truth_phi;
    }
    SmearedMETphi->Fill(temp_phi);

  } //End METSmeared loop

  for(int i=0;i<70000;i++) { // Generate points for MET Resolution plot
    temp_sumEt = i * 100; 
    sumEt.push_back(temp_sumEt/1000);
    m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
    result_f = m_upgrade->getMETResolution(temp_sumEt);
    gold_resol.push_back(result_f/1000.0);

    m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
    result_f = m_upgrade->getMETResolution(temp_sumEt);
    silver_resol.push_back(result_f/1000.0);

    m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
    result_f = m_upgrade->getMETResolution(temp_sumEt);
    bronze_resol.push_back(result_f/1000.0);
  } //End METResolution loop

  // Create sumEt scan TGraph objects and edit for plotting
  TGraph *bronze_graph_resol = new TGraph(sumEt.size(),&sumEt[0],&bronze_resol[0]);
  bronze_graph_resol->SetLineColor(1);
  bronze_graph_resol->SetMarkerColor(1);
  TGraph *silver_graph_resol = new TGraph(sumEt.size(),&sumEt[0],&silver_resol[0]);
  silver_graph_resol->SetLineColor(4);
  silver_graph_resol->SetMarkerColor(4);
  TGraph *gold_graph_resol = new TGraph(sumEt.size(),&sumEt[0],&gold_resol[0]);
  gold_graph_resol->SetLineColor(2);
  gold_graph_resol->SetMarkerColor(2);

  TCanvas *can = new TCanvas("can","Missing Transverse Energy Performance",1000,1000);
  can->Divide(2,2);

  can->cd(1);
  SmearedMETx->SetTitle("Smeared MET on X-axis"); 
  SmearedMETx->GetXaxis()->SetTitle("[Smeared METx - True METx] (GeV)"); 
  SmearedMETx->SetStats(0);
  SmearedMETx->Draw();

  can->cd(2);
  SmearedMETy->SetTitle("Smeared MET on Y-axis"); 
  SmearedMETy->GetXaxis()->SetTitle("[Smeared METy - True METy] (GeV)"); 
  SmearedMETy->SetStats(0);
  SmearedMETy->Draw();

  can->cd(3);
  SmearedMETphi->SetTitle("Smeared MET in phi"); 
  SmearedMETphi->GetXaxis()->SetTitle("[Smeared Phi - True Phi] (radians)"); 
  SmearedMETphi->SetStats(0);
  SmearedMETphi->Draw();

  can->cd(4);
  bronze_graph_resol->SetTitle("MET resolution vs raw sumEt");
  bronze_graph_resol->GetXaxis()->SetTitle("raw sumEt (GeV)");
  bronze_graph_resol->GetYaxis()->SetTitle("Absolute MET resolution (GeV)");
  bronze_graph_resol->Draw("ALP");
  silver_graph_resol->Draw("LP");
  gold_graph_resol->Draw("LP");

  can->Update();
  can->Print("METPerformance.png");
  can->Close();

  return;
}


void plotTrackJetConfirmEff(UpgradePerformanceFunctions *m_upgrade, float plot_pt) {
  // Make plot of Missing Transverse Energy performance
  std::cout << "Generating Track-Jet Confirmation Efficiency plots" << std::endl;

  //Initialize jet parameters
  m_upgrade->setJetRandomSeed(1);
  m_upgrade->setPileupUseTrackConf(true); 
  m_upgrade->setPileupRandomSeed(1);
  m_upgrade->setPileupJetPtThresholdMeV(30000.);

  // Set pileup (options currently incude 140 and 200
  m_upgrade->setAvgMu(200.);

  TCanvas *can = new TCanvas("can","Track-jet confirmation efficiency",1000,1000);
  can->Divide(2,2);

  // Vectors for storing Track Jet Confirmation Efficiency for gold, silver, bronze
  std::vector<float> gold_HS;
  std::vector<float> gold_PU;
  std::vector<float> silver_HS;
  std::vector<float> silver_PU; 
  std::vector<float> bronze_HS;
  std::vector<float> bronze_PU;

  std::vector<float> eta; 

  float temp_eta = 0;
  float store_result = 0;

  for(int j=0;j<2;j++) { // First loop Scheme = HS, Second loop Scheme = PU
    if(j == 0){ // First loop Scheme = HS
      std::cout << "First Track-jet confirmation loop Scheme = HS and PileupEff = .9" <<std::endl;
      m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PileupEff::HS); 
      m_upgrade->setPileupEff(.9);
    }
    else { // Second loop Scheme = PU
      std::cout << "Second Track-jet confirmation loop Scheme = PU and PileupEff = .02" <<std::endl;
      m_upgrade->setPileupEfficiencyScheme(UpgradePerformanceFunctions::PileupEff::PU); 
      m_upgrade->setPileupEff(0.02);
    }

    // **** Gold Loop **** //
    temp_eta = 0; // Start eta at 0
    m_upgrade->setLayout(UpgradePerformanceFunctions::gold);
    m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/groups/UpgradePhysics/PerformanceFunctions");
    for(int i=0;i<1000;i++){ // Gold loop to fill vectors
      eta.push_back(temp_eta);
      
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"HS");
      gold_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"PU"); 
      gold_PU.push_back(store_result);

      temp_eta = temp_eta + 4.0/1000.0;
    } // End Gold loop to fill vectors

    // **** Silver Loop **** //
    temp_eta = 0; // Start eta at 0
    m_upgrade->setLayout(UpgradePerformanceFunctions::silver);
    m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/groups/UpgradePhysics/PerformanceFunctions");
    for(int i=0;i<1000;i++){ // Silver loop to fill vectors
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"HS");
      silver_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"PU"); 
      silver_PU.push_back(store_result);

      temp_eta = temp_eta + 4.0/1000.0;
    } // End Silver loop to fill vectors

    // **** Bronze Loop **** //
    temp_eta = 0; // Start eta at 0
    m_upgrade->setLayout(UpgradePerformanceFunctions::bronze);
    m_upgrade->setPileupTemplatesPath("/afs/cern.ch/atlas/groups/UpgradePhysics/PerformanceFunctions");
    for(int i=0;i<1000;i++){ // Bronze loop to fill vectors
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"HS");
      bronze_HS.push_back(store_result);
      store_result = m_upgrade->getTrackJetConfirmEff(plot_pt*1000,temp_eta,"PU"); 
      bronze_PU.push_back(store_result);

      temp_eta = temp_eta + 4.0/1000.0;
    } // End Bronze loop to fill vectors


    if(j == 0) { // Generate and draw plots for Scheme = HS and PileupEff = .9

      // Create Track Jet Confirmation Efficiency for jets from Hard Scatter
      TGraph *bronze_graph1_HS = new TGraph(eta.size(),&eta[0],&bronze_HS[0]);
      bronze_graph1_HS->SetLineColor(1);
      bronze_graph1_HS->SetMarkerColor(1);
      TGraph *silver_graph1_HS = new TGraph(eta.size(),&eta[0],&silver_HS[0]);
      silver_graph1_HS->SetLineColor(4);
      silver_graph1_HS->SetMarkerColor(4);
      TGraph *gold_graph1_HS = new TGraph(eta.size(),&eta[0],&gold_HS[0]);
      gold_graph1_HS->SetLineColor(2);
      gold_graph1_HS->SetMarkerColor(2);

      // Create Track Jet Confirmation Efficiency for jets from Pileup
      TGraph *bronze_graph1_PU = new TGraph(eta.size(),&eta[0],&bronze_PU[0]);
      bronze_graph1_PU->SetLineColor(1);
      bronze_graph1_PU->SetMarkerColor(1);
      TGraph *silver_graph1_PU = new TGraph(eta.size(),&eta[0],&silver_PU[0]);
      silver_graph1_PU->SetLineColor(4);
      silver_graph1_PU->SetMarkerColor(4);
      TGraph *gold_graph1_PU = new TGraph(eta.size(),&eta[0],&gold_PU[0]);
      gold_graph1_PU->SetLineColor(2);
      gold_graph1_PU->SetMarkerColor(2);

      can->cd(1);
      gold_graph1_PU->SetTitle("TrackJetConfirmEff for PU with Scheme = HS and PileupEff = .9");
      gold_graph1_PU->GetXaxis()->SetTitle("Eta");
      gold_graph1_PU->GetYaxis()->SetTitle("TrackJetConfirmEff");
      gold_graph1_PU->Draw("ALP");
      silver_graph1_PU->Draw("LP");
      bronze_graph1_PU->Draw("LP");

      can->cd(2);
      gold_graph1_HS->SetTitle("TrackJetConfirmEff for HS with Scheme = HS and PileupEff = .9");
      gold_graph1_HS->GetXaxis()->SetTitle("Eta");
      gold_graph1_HS->GetYaxis()->SetTitle("TrackJetConfirmEff");
      gold_graph1_HS->Draw("ALP");
      silver_graph1_HS->Draw("LP");
      bronze_graph1_HS->Draw("LP");

      // Clear out vectors for second loop plots
      gold_HS.clear();
      gold_PU.clear();
      silver_HS.clear();
      silver_PU.clear(); 
      bronze_HS.clear();
      bronze_PU.clear();
      eta.clear(); 
    }

    else { // Generate and draw plots for Scheme = PU and PileupEff = .02
      // Create Track Jet Confirmation Efficiency for jets from Hard Scatter
      TGraph *bronze_graph2_HS = new TGraph(eta.size(),&eta[0],&bronze_HS[0]);
      bronze_graph2_HS->SetLineColor(1);
      bronze_graph2_HS->SetMarkerColor(1);
      TGraph *silver_graph2_HS = new TGraph(eta.size(),&eta[0],&silver_HS[0]);
      silver_graph2_HS->SetLineColor(4);
      silver_graph2_HS->SetMarkerColor(4);
      TGraph *gold_graph2_HS = new TGraph(eta.size(),&eta[0],&gold_HS[0]);
      gold_graph2_HS->SetLineColor(2);
      gold_graph2_HS->SetMarkerColor(2);

      // Create Track Jet Confirmation Efficiency for jets from Pileup
      TGraph *bronze_graph2_PU = new TGraph(eta.size(),&eta[0],&bronze_PU[0]);
      bronze_graph2_PU->SetLineColor(1);
      bronze_graph2_PU->SetMarkerColor(1);
      TGraph *silver_graph2_PU = new TGraph(eta.size(),&eta[0],&silver_PU[0]);
      silver_graph2_PU->SetLineColor(4);
      silver_graph2_PU->SetMarkerColor(4);
      TGraph *gold_graph2_PU = new TGraph(eta.size(),&eta[0],&gold_PU[0]);
      gold_graph2_PU->SetLineColor(2);
      gold_graph2_PU->SetMarkerColor(2);

      can->cd(3);
      gold_graph2_PU->SetTitle("TrackJetConfirmEff for PU with Scheme = PU and PileupEff = .02");
      gold_graph2_PU->GetXaxis()->SetTitle("Eta");
      gold_graph2_PU->GetYaxis()->SetTitle("TrackJetConfirmEff");
      gold_graph2_PU->Draw("ALP");
      silver_graph2_PU->Draw("LP");
      bronze_graph2_PU->Draw("LP");

      can->cd(4);
      gold_graph2_HS->SetTitle("TrackJetConfirmEff for HS with Scheme = PU and PileupEff = .02");
      gold_graph2_HS->GetXaxis()->SetTitle("Eta");
      gold_graph2_HS->GetYaxis()->SetTitle("TrackJetConfirmEff");
      gold_graph2_HS->Draw("ALP");
      silver_graph2_HS->Draw("LP");
      bronze_graph2_HS->Draw("LP");
    }
  }

  can->Update();
  can->Print("TrackJetConfirmEff.png");
  can->Close();

  return;
}
