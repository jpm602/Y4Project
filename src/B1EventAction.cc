//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "B1EventAction.hh"
#include "B1RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4RootAnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::B1EventAction(B1RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction)
{
  // Pass variables over to run action
  hitPosX = &runAction->hitPosX;
  hitPosY = &runAction->hitPosY;
  hitPosZ = &runAction->hitPosZ;
  edep = &runAction->edep;
  time = &runAction->time;
  deltaEnergy = &runAction->deltaEnergy;
  particleID = &runAction->particleID;
  trackID = &runAction->trackID;
  parentID = &runAction->parentID;

  energyLoss = &runAction->energyLoss;
  
  avalancheSize = &runAction->avalancheSize;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event*)
{
  // Clear all your vectors!!
  hitPosX->clear();
  hitPosY->clear();
  hitPosZ->clear();
  edep->clear();
  time->clear();
  deltaEnergy->clear();
  particleID->clear();
  trackID->clear();
  parentID->clear();

  energyLoss->clear();

  avalancheSize->clear();
  avalancheSize->push_back(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event*)
{
  auto *analysisManager = G4RootAnalysisManager::Instance();
  analysisManager->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::HitPos(G4double x, G4double y, G4double z)
{
  hitPosX->push_back(x);
  hitPosY->push_back(y);
  hitPosZ->push_back(z);
}

void B1EventAction::IDNumbers(G4int pID, G4int tID, G4int prntID)
{
  particleID->push_back(pID);
  trackID->push_back(tID);
  parentID->push_back(prntID);
}

void B1EventAction::FinalEnergy(G4double value)
{
  if(initEnergy!=0)
    {
      G4double energyDiff = initEnergy - value;
      if(energyDiff!=0)
        energyLoss->push_back(energyDiff);
      initEnergy = 0;
    }
}

