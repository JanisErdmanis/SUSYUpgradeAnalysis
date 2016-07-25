void stackedhisto() {
  //TH2F *h = (TH2F*)_file0->Get("myhisto");
//  THStack *stacked = new THStack("stacked","stacked hist; muon Pt; events");
  // This part makes a segfault
  auto h1 = (TH1F*)_file0->Get("h_PtMuons1st_JetCut Pt>0 GeV");
  h1->SetFillColor(38);
  h1->Draw();
  // stacked->Add(h1);
  
  auto h2 = (TH1F*)_file0->Get("h_PtMuons1st_JetCut Pt>100 GeV");
  h2->SetFillColor(46);
  h2->Draw("Same");
//  stacked->Add(h2);
  
  auto h3 = (TH1F*)_file0->Get("h_PtMuons1st_JetCut Pt>200 GeV");
  h3->SetFillColor(42);
  h3->Draw("Same");

  TLegend leg(.1,.7,.3,.9,"Jet Cutoffs");
  leg.SetFillColor(0);
  leg.AddEntry(h1,"Pt>0");
  leg.AddEntry(h2,"Pt>100");
  leg.AddEntry(h3,"Pt>200");
  leg.DrawClone("Same");
  
//  stacked->Add(h3);
  
//  stacked->Draw();

}

