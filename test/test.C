#include "Reader.h" // Reads TTree object
#include "TString.h" // For filename string
#include "TCanvas.h" // For graph canvases
#include "TH1F.h" // For 1D histograms

#include <iostream> // std::cout

int main(int argc, char** argv)
  {
    TFile *file = new TFile("output.root");

    TTree *tree = (TTree*)file->Get("output");
    Reader reader(tree);
    std::cout << "tree opened" << std::endl;

    TCanvas *c1 = new TCanvas();
    TH1F *hDeltaE = new TH1F("deltaE", "Total Delta Energy of Primary Particle in Gas Regions;Delta Energy (MeV);Number of Events", 100, 0, 0);

    Long64_t nentries = reader.fChain->GetEntries();
    for(Long64_t ientry=0; ientry<nentries; ientry++)
      {
	if(ientry%100==0) // Progress meter
	  std::cout << "on entry " << ientry << " out of " << nentries << std::endl;
	reader.GetEntry(ientry); // Get current event

	// Total delta E variable
	double deltaE = 0;

	// Loop over delta energies to sum them for this event
	for(unsigned int i=0; i<reader.EnergyLoss->size(); i++)
	  deltaE+=reader.EnergyLoss->at(i);
	hDeltaE->Fill(deltaE);
      }
    hDeltaE->Draw();
    c1->Print("test.png");
    delete hDeltaE;

    return 0;
  }
