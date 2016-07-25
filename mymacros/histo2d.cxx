void histo2d() {

  // This part has to be changed on next calculation
  auto h = (TH2F*)_file0->Get("myhisto");
  h->SetTitle(";1st muon Pt; 1st Jet Pt");
  
  gStyle->SetPalette(kBird);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  auto c=new TCanvas("Canvas","Canvas",800,800);
  h->DrawClone("Colz");
  
}
