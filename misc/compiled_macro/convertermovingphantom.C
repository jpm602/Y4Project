#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TAxis.h>
#include <TString.h>
#include <iostream>
#include <string>

#include "TTreeReader.h"
#include <vector>
#include "TFile.h"
#include "TTree.h"

#include "GetPosition.hh"

int main(int argc, char *argv[])
{
  TApplication app("app", &argc, argv);

  std::vector<double> meanlist;

  GetPosition * gap0 = new GetPosition(0, 0, 1000);
  meanlist.push_back(gap0->GetConverterPositions(0, 0, 1000));
  GetPosition * gap1 = new GetPosition(1, 1000, 1000);
  meanlist.push_back(gap1->GetConverterPositions(1, 1000, 1000));
  GetPosition * gap2 = new GetPosition(2, 2000, 1000);
  meanlist.push_back(gap2->GetConverterPositions(2, 2000, 1000));
  GetPosition * gap3 = new GetPosition(3, 3000, 1000);
  meanlist.push_back(gap3->GetConverterPositions(3, 3000, 1000));
  GetPosition * gap4 = new GetPosition(4, 4000, 1000);
  meanlist.push_back(gap4->GetConverterPositions(4, 4000, 1000));
  GetPosition * gap5 = new GetPosition(5, 5000, 1000);
  meanlist.push_back(gap5->GetConverterPositions(5, 5000, 1000));

  std::vector<double> gaplist = {-10, 0, 10, 20, 30, 40};
  TCanvas*c2 = new TCanvas();
  TH2D *histomean = new TH2D("MeanHisto", "Mean Photon Entry Position at Converter Vs Phantom Position;Phantom Position(mm);Mean Photon Entry Position(mm)", 60, -15, 45, 35, -15, 20);
  for(Long64_t j=0; j<meanlist.size(); j++)
    {
      histomean->Fill(gaplist[j], meanlist[j]);
    }
  histomean->Draw();
  c2->Print("convposmean.pdf");
 
  return 0;
}

