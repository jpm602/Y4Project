//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar  2 12:24:20 2020 by ROOT version 6.18/02
// from TTree output/output
// found on file: output.root
//////////////////////////////////////////////////////////

#ifndef Reader_h
#define Reader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class Reader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   std::vector<double>  *EnergyDeposition;
   std::vector<double>  *GasDeltaEnergy;
   std::vector<int>     *ParticleID;
   std::vector<int>     *TrackID;
   std::vector<int>     *ParentID;
   std::vector<double>  *HitPosX;
   std::vector<double>  *HitPosY;
   std::vector<double>  *HitPosZ;
   std::vector<double>  *Time;
   std::vector<double>  *EnergyLoss;
   std::vector<int>     *AvalancheSize;
   std::vector<double>  *AvalancheEnergy;

   // List of branches
   TBranch        *b_EnergyDeposition;   //!
   TBranch        *b_GasDeltaEnergy;   //!
   TBranch        *b_ParticleID;   //!
   TBranch        *b_TrackID;   //!
   TBranch        *b_ParentID;   //!
   TBranch        *b_HitPosX;   //!
   TBranch        *b_HitPosY;   //!
   TBranch        *b_HitPosZ;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_EnergyLoss;   //!
   TBranch        *b_AvalancheSize;   //!
   TBranch        *b_AvalancheEnergy;   //!

   Reader(TTree *tree=0);
   virtual ~Reader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Reader_cxx
Reader::Reader(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("output.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("output.root");
      }
      f->GetObject("output",tree);

   }
   Init(tree);
}

Reader::~Reader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Reader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Reader::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Reader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   EnergyDeposition = 0;
   GasDeltaEnergy = 0;
   ParticleID = 0;
   TrackID = 0;
   ParentID = 0;
   HitPosX = 0;
   HitPosY = 0;
   HitPosZ = 0;
   Time = 0;
   EnergyLoss = 0;
   AvalancheSize = 0;
   AvalancheEnergy = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EnergyDeposition", &EnergyDeposition, &b_EnergyDeposition);
   fChain->SetBranchAddress("GasDeltaEnergy", &GasDeltaEnergy, &b_GasDeltaEnergy);
   fChain->SetBranchAddress("ParticleID", &ParticleID, &b_ParticleID);
   fChain->SetBranchAddress("TrackID", &TrackID, &b_TrackID);
   fChain->SetBranchAddress("ParentID", &ParentID, &b_ParentID);
   fChain->SetBranchAddress("HitPosX", &HitPosX, &b_HitPosX);
   fChain->SetBranchAddress("HitPosY", &HitPosY, &b_HitPosY);
   fChain->SetBranchAddress("HitPosZ", &HitPosZ, &b_HitPosZ);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("EnergyLoss", &EnergyLoss, &b_EnergyLoss);
   fChain->SetBranchAddress("AvalancheSize", &AvalancheSize, &b_AvalancheSize);
   fChain->SetBranchAddress("AvalancheEnergy", &AvalancheEnergy, &b_AvalancheEnergy);
   Notify();
}

Bool_t Reader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Reader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Reader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Reader_cxx
