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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(B1EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
  const B1DetectorConstruction* detectorConstruction
    = static_cast<const B1DetectorConstruction*>
    (G4RunManager::GetRunManager()->GetUserDetectorConstruction());   

  // get volume of the current step
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();

  // Tracking criteria - done like this for easier analysis - need to store a blank value when the other interaction happens so that numbers correspond
  if((volume->GetName() == "Gas" && step->GetDeltaEnergy()!=0) || (volume->GetName() == "Plate" && edepStep!=0))
    {
      // Save particle ID, track ID, and parent ID
      G4Track *track = step->GetTrack();
      fEventAction->IDNumbers(track->GetDynamicParticle()->GetPDGcode(), track->GetTrackID(), track->GetParentID());
      // Save position of hit
      G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
      fEventAction->HitPos(pos.x()/mm, pos.y()/mm, pos.z()/mm);
      // Save time since start of event of hit
      fEventAction->Time(step->GetPostStepPoint()->GetGlobalTime()/ns);
      // Gas tracking for initial particle interactions
      if(volume->GetName() == "Gas")
	{
	  // Save delta energy of particle in gas
	  fEventAction->DeltaEnergy(step->GetDeltaEnergy()/MeV);
	  // Saving a blank number for edep to make analysis work
	  fEventAction->EnergyDep(0);
	}
      // Plate tracking for final particles that are read out
      else if(volume->GetName() == "Plate")
	{
	  // Save energy deposition of particles hitting the plate
	  fEventAction->EnergyDep(edepStep/MeV);
	  // Saving a blank number for deltaE to make analysis work
	  fEventAction->DeltaEnergy(0);
	}
      else
	{
	  // Saving a blank number for edep to make analysis work
	  fEventAction->EnergyDep(0);
	  // Saving a blank number for deltaE to make analysis work
	  fEventAction->DeltaEnergy(0);
	}
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

