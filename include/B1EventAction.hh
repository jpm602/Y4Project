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
/// \file B1EventAction.hh
/// \brief Definition of the B1EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>

class B1RunAction;

/// Event action class
///

class B1EventAction : public G4UserEventAction
{
  public:
    B1EventAction(B1RunAction *runAction);
    virtual ~B1EventAction();

    virtual void BeginOfEventAction(const G4Event *event);
    virtual void EndOfEventAction(const G4Event *event);

    virtual void HitPos(G4double x, G4double y, G4double z);
    virtual void EnergyDep(G4double value){edep->push_back(value);};
    virtual void Time(G4double value){time->push_back(value);};
    virtual void DeltaEnergy(G4double value){deltaEnergy->push_back(value);};
    virtual void IDNumbers(G4int pID, G4int tID, G4int prntID);

    virtual void InitialEnergy(G4double value){initEnergy=value;};
    virtual void FinalEnergy(G4double value);
  

  private:
    B1RunAction *fRunAction;

    std::vector<double> *hitPosX;
    std::vector<double> *hitPosY;
    std::vector<double> *hitPosZ;
    std::vector<double> *edep;
    std::vector<double> *time;
    std::vector<double> *deltaEnergy;
    std::vector<int> *particleID;
    std::vector<int> *trackID;
    std::vector<int> *parentID;

    G4double initEnergy = 0;
    std::vector<double> *energyLoss;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


