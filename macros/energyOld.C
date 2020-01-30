#include "Reader.h" // Reads TTree object
#include "TString.h" // For filename string
#include "TCanvas.h" // For graph canvases
#include "TH1F.h" // For 1D histograms

#include <iostream> // std::cout
#include <algorithm> // std::find

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

  std::vector<std::vector<int>> parentsAllEvents; // Vector of vectors containing parent IDs for each event
  std::vector<std::vector<int>> tracksAllEvents; // Vector of vectors containing track IDs for each event
  std::vector<std::vector<double>> edepAllEvents; // Vector of vectors containing summed energy depositions for each track in each event
  std::cout << "vectors created" << std::endl;
  
  // Loop over events to get parent and track IDs for each events
  Long64_t nentries = reader.fChain->GetEntries();
  std::cout << "starting ID loop" << std::endl;
  for(Long64_t ientry=0; ientry<nentries; ientry++)
    {
      reader.GetEntry(ientry); // Get current event
      std::vector<int> parentsSingleEvent; // Vector of parent IDs for this event
      std::vector<int> tracksSingleEvent; // Vector of track IDs for this event

      // Loop over parent IDs in this event
      for(unsigned int i=0; i<reader.ParentID->size(); i++)
	{
	  // Store non duplicate list of parent IDs
	  if(std::find(parentsSingleEvent.begin(), parentsSingleEvent.end(), reader.ParentID->at(i))==parentsSingleEvent.end())
	    parentsSingleEvent.push_back(reader.ParentID->at(i));
	  // Store non duplicate list of track IDs
	  if(std::find(tracksSingleEvent.begin(), tracksSingleEvent.end(), reader.TrackID->at(i))==tracksSingleEvent.end())
	    tracksSingleEvent.push_back(reader.TrackID->at(i));
	}
      
      // Store IDs for a whole event in the vector of vectors
      parentsAllEvents.push_back(parentsSingleEvent);
      tracksAllEvents.push_back(tracksSingleEvent);
    }
  std::cout << "finished ID loop" << std::endl;
  
  // Loop over events to sum energies for each track and store in a vector of vectors that corresponds to the vectors of vectors for ID numbers
  std::cout << "starting energy loop" << std::endl;
  for(Long64_t ientry=0; ientry<nentries; ientry++)
    {
      reader.GetEntry(ientry); // Get current event
      std::vector<double> edepSingleEvent; // Vector of summed energy depositions for each track in this event
      
      // Loop over number of track IDs to set starting energy values to zero
      for(unsigned int i=0; i<tracksAllEvents.at(ientry).size(); i++)
	edepSingleEvent.push_back(0.);

      std::cout << reader.TrackID->size() << " " << reader.EnergyDeposition->size() << std::endl;
      // Loop over track IDs to sum up energy deposition for each
      for(unsigned int i=0; i<reader.TrackID->size(); i++)
      	{
	  // Find element in track ID vector that contains the ID of the current interaction and then save the energy deposition to it
	  std::vector<int>::iterator it = std::find(tracksAllEvents.at(ientry).begin(), tracksAllEvents.at(ientry).end(), reader.TrackID->at(i));
	  if(it!=tracksAllEvents.at(ientry).end())
	    {
	      int element = std::distance(tracksAllEvents.at(ientry).begin(), it); // Uses iterator returned by find to go to correct track ID in event
	      edepSingleEvent.at(element)+=reader.EnergyDeposition->at(i); // Add the energy deposition for this interaction to total for this track
	    }
      	}
      
      // Store energy deposition for a whole event in a vector of vectors
      edepAllEvents.push_back(edepSingleEvent);
    }
  std::cout << "finished energy loop" << std::endl;

  // Make histogram for energy per parent
  TH1F *hEnergyPerParent = new TH1F("energyPerParent", "Energy Deposited Per Primary Particle", 100, 0, 0);
  return 0;
}
// Need to do total energy and total energy per particle
