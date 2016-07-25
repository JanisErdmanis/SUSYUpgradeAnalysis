#include "globaldeffs.h"

void plothisto() {

  THStack *background = new THStack("stacked",cutname + ";" + xlabel + ";");
  
  auto h1 = (TH1F*)tfile1->Get(HIST);
  h1->SetFillColor(38);
  //h1->Draw();
  
  auto h2 = (TH1F*)tfile2->Get(HIST);
  h2->SetFillColor(46);
//  h2->Draw("Same");

  auto h3 = (TH1F*)tfile3->Get(HIST);
  h3->SetFillColor(40);

  background->Add(h1);
  background->Add(h2);
  background->Add(h3);
  
  background->Draw();

  auto h4 = (TH1F*)tfile4->Get(HIST);
  h4->SetFillColor(41);
  h4->Draw("Same");

  auto h5 = (TH1F*)tfile5->Get(HIST);
  h5->SetFillColor(42);
  h5->Draw("Same");

    
  TLegend leg(.1,.7,.3,.9,"samples");
  leg.SetFillColor(0);
  leg.AddEntry(h1,samples[0]);
  leg.AddEntry(h2,samples[1]);
  leg.AddEntry(h3,samples[2]);
  leg.AddEntry(h4,samples[3]);
  leg.AddEntry(h5,samples[4]);
  leg.DrawClone("Same");

}
  
