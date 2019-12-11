#include "Reader.h" // Reads TTree object

#include <algorithm> // std::find

int energy()
{
  // Open file, get tree, give it to reader
  TFile *file = new TFile("output.root");
  TTree *tree = (TTree*)file->Get("output");
  Reader reader(tree);
  std::cout << "file opened and tree given to reader" << std::endl;

  std::vector<std::vector<int>> parentsAllEvents; // Vector of vectors containing parent IDs for each event
  std::vector<std::vector<int>> tracksAllEvents; // Vector of vectors containing track IDs for each event
  std::vector<std::vector<double>> edepAllEvents; // Vector of vectors containing summed energy depositions per parent for each event
  std::cout << "vectors of vectors created" << std::endl;
  
  // Loop over events to get parent and track IDs for each events
  Long64_t nentries = reader.fChain->GetEntries(); // Crashes here for some reason - segfault in GetEntries()??
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
  
  // Loop over events to sum energies for each parent and store in a vector of vectors that corresponds to the previous ones
  std::cout << "starting energy loop" << std::endl;
  for(Long64_t ientry=0; ientry<nentries; ientry++)
    {
      reader.GetEntry(ientry); // Get current event
      std::vector<double> edepSingleEvent; // Vector of summed energy depositions per parent for this event
      
      // Loop over number of parent IDs to set starting energy values to zero
      for(unsigned int i=0; i<parentsAllEvents.at(ientry).size(); i++)
	edepSingleEvent.push_back(0.);
      // Loop over parent IDs to sum up energy deposition for each
      for(unsigned int i=0; i<reader.ParentID->size(); i++)
      	{
	  // Find element in parent ID vector that contains the ID of the current interaction and then save the energy deposition to it
	  std::vector<int>::iterator it = std::find(parentsAllEvents.at(ientry).begin(), parentsAllEvents.at(ientry).end(), reader.ParentID->at(i));
	  int element = std::distance(parentsAllEvents.at(ientry).begin(), it); // Uses iterator returned by find to get element of vector
	  edepSingleEvent.at(element)+=reader.EnergyDeposition->at(i);
      	}
      
      // Store energy deposition for a whole event in a vector of vectors
      edepAllEvents.push_back(edepSingleEvent);
    }
  std::cout << "finished energy loop" << std::endl;
  
  return 0;
}
// Need to do total energy per parent and total energy per parent per particle
