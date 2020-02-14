#include "Reader.h" // Reads TTree object
#include "TString.h" // For filename string
#include "TCanvas.h" // For graph canvases
#include "TH1F.h" // For 1D histograms
#include "THStack.h" // For plotting multiple histograms
#include "TLegend.h" // Legend for energy per particle type plot

#include <iostream> // std::cout
#include <algorithm> // std::find
#include <string> // std::to_string

int main(int argc, char** argv)
{
  // Open file
  TString filename;
  if(argc>1)
    filename = argv[1];
  else
    filename = "output.root";
  TFile *file = new TFile(filename);

  // Get tree and give it to reader
  TTree *tree = (TTree*)file->Get("output");
  Reader reader(tree);
  std::cout << "file opened and tree given to reader" << std::endl;

  // Vectors of vectors to store particle IDs and relevant energy depositions for each event
  std::vector<std::vector<int>> particleIDsAllEvents;
  std::vector<std::vector<double>> edepPerPIDAllEvents;

  // Histograms and canvas
  // Plotting total energy deposition for each event
  TCanvas *c1 = new TCanvas();
  TH1F *hTotalEdep = new TH1F("edepTotal", "Energy Deposited per Primary Particle;Energy Deposition (MeV);Number of Events", 100, 0, 0);
  // Plotting total delta energy for each event
  TH1F *hTotalDeltaE = new TH1F("deltaETotal", "Total Delta Energy of Primary Particle in Gas Regions;Delta Energy(MeV);Number of Events", 100, 0, 0);

  // Loop over events to add up total energy depositions
  Long64_t nentries = reader.fChain->GetEntries();
  for(Long64_t ientry=0; ientry<nentries; ientry++)
    {
      if(ientry%100==0) // Progress meter
	std::cout << "on entry " << ientry << " out of " << nentries << std::endl;
      reader.GetEntry(ientry); // Get current event
      
      std::vector<int> particleIDs; // Vector of particle IDs for this event
      std::vector<double> edep; // Vector of energy depositions per particle type for this event
      
      // Loop over particle IDs in this event to find all unique ones
      for(unsigned int i=0; i<reader.ParticleID->size(); i++)
	{
	  // Store particle ID if it isn't already
	  if(std::find(particleIDs.begin(), particleIDs.end(), reader.ParticleID->at(i)) == particleIDs.end())
	    particleIDs.push_back(reader.ParticleID->at(i));
	}
      
      // Loop to set all elements of edep to zero ready for summing energy depositions
      for(unsigned int i=0; i<particleIDs.size(); i++)
	  edep.push_back(0.);
      double totalEdep = 0; // Set initial total edep to zero ready for summing energy depositions
      
      // Loop to sum energy depositions for this event for each particle type and also total energy deposition from all types
      for(unsigned int i=0; i<reader.EnergyDeposition->size(); i++)
	{
	  if(reader.EnergyDeposition->at(i)>1e-6) // Check entry isn't blank
	    {
	      if(reader.ParentID->at(i)!=0)
		totalEdep += reader.EnergyDeposition->at(i);
	      
	      // Find element in energy deposition vector that corresponds to this particle ID and add this current value onto it
	      std::vector<int>::iterator it = std::find(particleIDs.begin(), particleIDs.end(), reader.ParticleID->at(i));
	      if(it!=particleIDs.end())
		{
		  int element = std::distance(particleIDs.begin(), it);
		  edep.at(element)+=reader.EnergyDeposition->at(i);
		}
	    }
	}

      double totalDeltaEnergy = 0; // Set initial value of total delta energy to zero
      // Loop to sum delta energies of the primary particle in the gas
      for(unsigned int i=0; i<reader.GasDeltaEnergy->size(); i++)
	{
	  if(reader.GasDeltaEnergy->at(i)<-1e-20) // Check entry isn't blank
	    {
	      if(reader.ParentID->at(i)==0)
		totalDeltaEnergy+=reader.GasDeltaEnergy->at(i);
	    }
	}
      
      // Push back results from this event into vectors for all events
      particleIDsAllEvents.push_back(particleIDs);
      edepPerPIDAllEvents.push_back(edep);
      particleIDs.clear();
      edep.clear();

      // Fill histograms for edep and delta energy
      hTotalEdep->Fill(totalEdep);
      hTotalDeltaE->Fill(totalDeltaEnergy);
    }

  // Draw edep histogram, save it, then delete it
  hTotalEdep->Draw();
  c1->Print("totalEdep.png");
  delete hTotalEdep;
  // Draw deltaE histogram, save it, then delete it
  hTotalDeltaE->Draw();
  c1->Print("totalDeltaE.png");
  delete hTotalDeltaE;
  
  // Plotting total energy deposition for each particle species for each event
  THStack *hs = new THStack("hs", "Energy Deposited per Primary Particle;Energy Deposition (MeV);Number of Events"); // Histogram stack
  TLegend *legend = new TLegend(0.7, 0.7, 0.95, 0.95);
  // Find event with most unique particle species and make a vector of histograms of that length
  int maxLengthID = 0; // Set a starting value
  for(unsigned int i=0; i<particleIDsAllEvents.size(); i++)
    {
      if(particleIDsAllEvents.at(i).size()>particleIDsAllEvents.at(maxLengthID).size()) // Set new max length if it is longer
	maxLengthID = i;
    }
  // Make vector with correct number of histograms
  std::vector<TH1F*> histoVector;
  for(unsigned int i=0; i<particleIDsAllEvents.at(maxLengthID).size(); i++)
    {
      TH1F *hEdepPerPID = new TH1F("edepPerPID", "Energy Deposited per Primary Particle per Particle Species;Energy Deposition (MeV);Number of Events", 100, 0, 0);
      histoVector.push_back(hEdepPerPID);
    }
  // Loop through all events to fill relevant histograms with the correct energy depositions
  for(unsigned int i=0; i<particleIDsAllEvents.size(); i++)
    {
      for(unsigned int j=0; j<particleIDsAllEvents.at(i).size(); j++)
	{
	  // Find histogram for this particle species
	  std::vector<int>::iterator it
	    = std::find(particleIDsAllEvents.at(maxLengthID).begin(), particleIDsAllEvents.at(maxLengthID).end(), particleIDsAllEvents.at(i).at(j));
	  if(it!=particleIDsAllEvents.at(maxLengthID).end())
	    {
	      // Fill correct histogram with the relevant energy deposition value
	      int element = std::distance(particleIDsAllEvents.at(maxLengthID).begin(), it);
	      histoVector.at(element)->Fill(edepPerPIDAllEvents.at(i).at(j));
	    }
	}
    }
  // Loop through vector of histograms to add them all to the stack
  for(unsigned int i=0; i<histoVector.size(); i++)
    {
      std::cout << "number of particle species: " << histoVector.size() << std::endl;
      // Add to histogram stack
      histoVector.at(i)->SetLineColor(i+2); // Set a different colour for each particle type
      hs->Add(histoVector.at(i));
      // Legend entry labelling each particle ID
      TString label = "Particle ID: ";
      label += std::to_string(particleIDsAllEvents.at(maxLengthID).at(i));
      legend->AddEntry(histoVector.at(i), label, "l");
    }
  hs->Draw();
  legend->Draw();
  c1->Print("edepPerParticle.png");
  c1->SetLogx();
  c1->SetLogy();
  c1->Print("edepPerParticleLog.png");
  c1->SetLogx(0);
  c1->SetLogy(0);
  delete hs;
  delete legend;

  file->Close();
  
  return 0;
}

