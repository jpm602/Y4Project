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
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "B1ElectricFieldSetup.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4PVReplica.hh"
#include "G4UnitsTable.hh"
#include "G4UserLimits.hh"

#include <string>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction()
  : G4VUserDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Convert between atomic mass units and kg, set Boltzmann constant in SI units
  static const G4double amu = 1.66054e-27*kg;
  G4double kBoltzmann = 1.38064582e-23*joule/kelvin;
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  // Visualisation settings
  G4VisAttributes *envelopeVisAttributes = new G4VisAttributes(); // Needed to make envelopes invisible
  envelopeVisAttributes->SetVisibility(false);
  G4VisAttributes *gasVisAttributes = new G4VisAttributes(); // Make gas volume green
  gasVisAttributes->SetColour(0., 1., 0.);

  //     
  // World
  //
  G4double worldSizeXY = 20*m;
  G4double worldSizeZ  = 20*m;
  G4Material *worldMat = nist->FindOrBuildMaterial("G4_Galactic");
  
  G4Box *solidWorld =    
    new G4Box("World",                       // its name
	      0.5*worldSizeXY, 0.5*worldSizeXY, 0.5*worldSizeZ);     // its size
      
  G4LogicalVolume *logicWorld =                         
    new G4LogicalVolume(solidWorld,          // its solid
                        worldMat,           // its material
                        "World");            // its name
                                   
  G4VPhysicalVolume *physWorld = 
    new G4PVPlacement(0,                     // no rotation
                      G4ThreeVector(),       // at (0,0,0)
                      logicWorld,            // its logical volume
                      "World",               // its name
                      0,                     // its mother  volume
                      false,                 // no boolean operation
                      0,                     // copy number
                      checkOverlaps);        // overlaps checking

  // Full model of RPCs
  if(!fSiliconModel)
    {
      // Gas parameters
      G4double gasThick = 1*mm;
      G4double gasPressure = 100000*hep_pascal;
      
      // Gas components
      G4Material *tetra = new G4Material("Tetrafluroethane", 4.25*kg/m3, 3, kStateGas);
      tetra->AddElement(nist->FindOrBuildElement(6), 2);
      tetra->AddElement(nist->FindOrBuildElement(1), 2);
      tetra->AddElement(nist->FindOrBuildElement(9), 4);
      G4double tetraMass = (2*12.0107 + 2*1.00784 + 4*18.998403)*amu; // Mass of tetrafluoroethane in kg
      G4double tetraPercent = 94.7; // Percentage of mixture
	
      G4Material *butane = nist->FindOrBuildMaterial("G4_BUTANE"); // assuming butane and iso-butane have same properties - density matches
      G4double butaneMass = (4*12.0107 + 10*1.00784)*amu; // Mass of butane in kg
      G4double butanePercent = 5.0; // Percentage of mixture
      
      G4Material *sf6 = new G4Material("Sulphur Hexafluoride", 6.17*kg/m3, 2, kStateGas);
      sf6->AddElement(nist->FindOrBuildElement(16), 1);
      sf6->AddElement(nist->FindOrBuildElement(9), 6);
      G4double sf6Mass = (32.065 + 6*18.998403)*amu; // Mass of SF6 in kg
      G4double sf6Percent = 0.3; // Percentage of mixture

      G4double averageMass = (tetraMass*tetraPercent + butaneMass*butanePercent + sf6Mass*sf6Percent)/100;
      G4double gasDensity = (gasPressure*averageMass)/(kBoltzmann*CLHEP::STP_Temperature);
      G4Material *gasMat = new G4Material("rpcGas", gasDensity, 3, kStateGas, CLHEP::STP_Temperature, gasPressure);
      gasMat->AddMaterial(tetra, tetraPercent*perCent);
      gasMat->AddMaterial(butane, butanePercent*perCent);
      gasMat->AddMaterial(sf6, sf6Percent*perCent);
      std::cout << gasDensity/(kg/m3) << std::endl;
      
      // Plate Parameters
      G4double plateThick = 1.2*mm;
      G4double plateSizeX = 2*m;
      G4double plateSizeY = 1*m;
      G4Material *plateMat = nist->FindOrBuildMaterial("G4_BAKELITE");

      // Electric field
      B1ElectricFieldSetup *emFieldSetup = new B1ElectricFieldSetup();

      // Overall detector parameters
      if(fCodexb) // Full size detector
	{
	  detSizeXY = 10*m;
	  layerSep = 4*cm; // from surface of one layer to surface of the next
	  nFaceLayers = 6;
	  nPerCentreGroup = 3;
	  nCentreLayers = 5;
	}
      else // Demonstrator module
      	{
      	  detSizeXY = 2*m;
      	  layerSep = 4*cm;
      	  nFaceLayers = 3;
      	  nPerCentreGroup = 3;
      	  nCentreLayers = 1;
      	}
      G4double rpcThick = 2*plateThick + gasThick;
      G4double faceThick = nFaceLayers*rpcThick + (nFaceLayers - 1)*layerSep;
      G4double groupThick = nPerCentreGroup*rpcThick + (nPerCentreGroup - 1)*layerSep;

      // Centre Envelope
      G4Box *solidCentreEnv =
	new G4Box("Centre Volume",                                       // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*detSizeXY);          // its size

      G4LogicalVolume *logicCentreEnv =
	new G4LogicalVolume(solidCentreEnv,       // its solid
			    worldMat,             // its material
			    "Centre Volume");     // its name
      logicCentreEnv->SetVisAttributes(envelopeVisAttributes);

      G4VPhysicalVolume *physCentreEnv = 
	new G4PVPlacement(0,                     // no rotation
			  G4ThreeVector(),       // at (0,0,0)
			  logicCentreEnv,        // its logical volume
			  "Centre Volume",       // its name
			  logicWorld,            // its mother  volume
			  false,                 // no boolean operation
			  0,                     // copy number
			  checkOverlaps);        // overlaps checking
      
      // Face envelopes
      std::vector<G4ThreeVector> facePos; // Positions of faces
      facePos.push_back(G4ThreeVector(0, 0, -0.5*detSizeXY - 0.5*faceThick));
      facePos.push_back(G4ThreeVector(0, 0, 0.5*detSizeXY + 0.5*faceThick));
      facePos.push_back(G4ThreeVector(-0.5*detSizeXY - 0.5*faceThick, 0, 0));
      facePos.push_back(G4ThreeVector(0.5*detSizeXY + 0.5*faceThick, 0, 0));
      facePos.push_back(G4ThreeVector(0, -0.5*detSizeXY - 0.5*faceThick, 0));
      facePos.push_back(G4ThreeVector(0, 0.5*detSizeXY + 0.5*faceThick, 0));
      
      G4Box *solidFaceEnv =
	new G4Box("Face",                                                // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*faceThick);          // its size

      G4LogicalVolume *logicFaceEnv =
	new G4LogicalVolume(solidFaceEnv,         // its solid
			    worldMat,             // its material
			    "Face");              // its name
      logicFaceEnv->SetVisAttributes(envelopeVisAttributes);
      
      for(unsigned int nFace=0; nFace<facePos.size(); nFace++) // Place the six cube faces
	{
	  // Rotation matrix
	  G4RotationMatrix *rotFace = new G4RotationMatrix();
	  if(facePos.at(nFace).y()!=0) // Rotate for top/bottom face
	    rotFace->rotateX(90*deg);
	  if(facePos.at(nFace).x()!=0) // Rotate for side faces
	    rotFace->rotateY(90*deg);
	  G4VPhysicalVolume *physFaceEnv = 
	    new G4PVPlacement(rotFace,               // rotation
			      facePos.at(nFace),     // at the relevant face position
			      logicFaceEnv,          // its logical volume
			      "Face",                // its name
			      logicWorld,            // its mother  volume
			      false,                 // no boolean operation
			      nFace,                 // copy number
			      checkOverlaps);        // overlaps checking
	}

      // Central RPC groups envelopes
      G4Box *solidCentreGroup =
	new G4Box("Group",                                       // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*groupThick); // its size

      G4LogicalVolume *logicCentreGroup =
	new G4LogicalVolume(solidCentreGroup,     // its solid
			    worldMat,             // its material
			    "Group");             // its name
      logicCentreGroup->SetVisAttributes(envelopeVisAttributes);
      
      for(unsigned int nGroup=0; nGroup<nCentreLayers; nGroup++) // Places the centre RPC groups
	{
	  G4double groupSep = (detSizeXY - nCentreLayers*groupThick)/(nCentreLayers + 1); // Equal spacing along the length of the detector
	  G4double groupZPos = -0.5*detSizeXY + groupSep + 0.5*groupThick + nGroup*(groupThick + groupSep);

	  G4VPhysicalVolume *physGroupEnv = 
	    new G4PVPlacement(0,                             // no rotation
			      G4ThreeVector(0, 0, groupZPos),// at the relevant group position
			      logicCentreGroup,              // its logical volume
			      "Group",                       // its name
			      logicCentreEnv,                // its mother  volume
			      false,                         // no boolean operation
			      nGroup,                        // copy number
			      checkOverlaps);                // overlaps checking
	}

      // Sub envelopes for RPC layers
      G4Box *solidRPCEnv =
	new G4Box("RPC",                                              // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*rpcThick);        // its size

      G4LogicalVolume *logicRPCEnv =
	new G4LogicalVolume(solidRPCEnv,          // its solid
			    worldMat,             // its material
			    "RPC");               // its name
      logicRPCEnv->SetVisAttributes(envelopeVisAttributes);
      
      // Placing RPCs in face layers
      for(unsigned int nLayers=0; nLayers<nFaceLayers; nLayers++)
	{
	  G4double rpcZPos = -0.5*faceThick + 0.5*rpcThick + nLayers*(rpcThick + layerSep);
	  G4VPhysicalVolume *physRPCEnv = 
	    new G4PVPlacement(0,                           // no rotation
			      G4ThreeVector(0, 0, rpcZPos),// at each layer position in the face
			      logicRPCEnv,                 // its logical volume
			      "RPC",                       // its name
			      logicFaceEnv,                // its mother  volume
			      false,                       // no boolean operation
			      nLayers,                     // copy number
			      checkOverlaps);              // overlaps checking
	}
      // Placing RPCs inside groups in central volume
      for(unsigned int nLayers=0; nLayers<nPerCentreGroup; nLayers++)
	{
	  G4double rpcZPos = -0.5*groupThick + 0.5*rpcThick + nLayers*(rpcThick + layerSep);
	  G4VPhysicalVolume *physRPCCentreEnv = 
	    new G4PVPlacement(0,                           // no rotation
			      G4ThreeVector(0, 0, rpcZPos),// at each layer position in the group
			      logicRPCEnv,                 // its logical volume
			      "RPC",                       // its name
			      logicCentreGroup,            // its mother  volume
			      false,                       // no boolean operation
			      nLayers,                     // copy number
			      checkOverlaps);              // overlaps checking
	}
      
      // Sub envelopes for plates and gas inside RPC
      G4Box *solidPlateEnv =
	new G4Box("Plate",                                              // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*plateThick);        // its size

      G4LogicalVolume *logicPlateEnv =                         
	new G4LogicalVolume(solidPlateEnv,      // its solid
			    worldMat,           // its material
			    "Plate");           // its name
      logicPlateEnv->SetVisAttributes(envelopeVisAttributes);

      // Loop places plates at both ends of the RPC
      for(unsigned int nPlate=0; nPlate<2; nPlate++)
	{
	  G4ThreeVector platePos = G4ThreeVector(0, 0, -0.5*rpcThick + 0.5*plateThick);
	  if(nPlate%2==0)
	    platePos*=-1; // flip sign to place at front/back of RPC
	  G4VPhysicalVolume *physPlateEnv = 
	    new G4PVPlacement(0,               // no rotation
			      platePos,        // at the front/back of the RPC layer
			      logicPlateEnv,   // its logical volume
			      "Plate",         // its name
			      logicRPCEnv,     // its mother  volume
			      false,           // no boolean operation
			      nPlate,          // copy number
			      checkOverlaps);  // overlaps checking
	}
      G4Box *solidGasEnv =
	new G4Box("Gas",                                                // its name
		  0.5*detSizeXY, 0.5*detSizeXY, 0.5*gasThick);          // its size

      G4LogicalVolume *logicGasEnv =                         
	new G4LogicalVolume(solidGasEnv,        // its solid
			    worldMat,           // its material
			    "Gas");             // its name
      logicGasEnv->SetVisAttributes(envelopeVisAttributes);

      G4VPhysicalVolume *physGasEnv = 
	new G4PVPlacement(0,                     // no rotation
			  G4ThreeVector(0, 0, 0),// middle of the RPC layer
			  logicGasEnv,           // its logical volume
			  "Gas",                 // its name
			  logicRPCEnv,           // its mother  volume
			  false,                 // no boolean operation
			  0,                     // copy number
			  checkOverlaps);        // overlaps checking
      
      // Plate replicas solid and logical volumes
      G4Box *solidPlateStrip =
	new G4Box("Plate",                                              // its name
		  0.5*plateSizeX, 0.5*detSizeXY, 0.5*plateThick);       // its size

      G4LogicalVolume *logicPlateStrip =                         
	new G4LogicalVolume(solidPlateStrip,    // its solid
			    plateMat,           // its material
			    "Plate");           // its name
      logicPlateStrip->SetVisAttributes(envelopeVisAttributes);

      G4Box *solidPlate =
	new G4Box("Plate",                                              // its name
		  0.5*plateSizeX, 0.5*plateSizeY, 0.5*plateThick);      // its size

      G4LogicalVolume *logicPlate =                         
	new G4LogicalVolume(solidPlate,         // its solid
			    plateMat,           // its material
			    "Plate");           // its name

      // Plate placement
      G4int nReplicasX = detSizeXY/plateSizeX;
      G4int nReplicasY = detSizeXY/plateSizeY;

      G4VPhysicalVolume *plateReplicaStrip =
      	new G4PVReplica("Plate",                  // name
			logicPlateStrip,          // logical volume
			logicPlateEnv,            // mother volume
			kXAxis,                   // replication axis
			nReplicasX,               // number of replicas
			plateSizeX);              // width

      G4VPhysicalVolume *plateReplica =
      	new G4PVReplica("Plate",                  // name
			logicPlate,               // logical volume
			plateReplicaStrip,        // mother volume
			kYAxis,                   // replication axis
			nReplicasY,               // number of replicas
			plateSizeY);              // width

      // Gas replicas solid and logical volumes
      G4Box *solidGasStrip =
	new G4Box("Gas",                                              // its name
		  0.5*plateSizeX, 0.5*detSizeXY, 0.5*gasThick);       // its size

      G4LogicalVolume *logicGasStrip =                         
	new G4LogicalVolume(solidGasStrip,    // its solid
			    gasMat,           // its material
			    "Gas");           // its name
      logicGasStrip->SetVisAttributes(envelopeVisAttributes);

      G4Box *solidGas =
	new G4Box("Gas",                                              // its name
		  0.5*plateSizeX, 0.5*plateSizeY, 0.5*gasThick);      // its size

      G4LogicalVolume *logicGas =                         
	new G4LogicalVolume(solidGas,         // its solid
			    gasMat,           // its material
			    "Gas");           // its name
      logicGas->SetVisAttributes(gasVisAttributes);
      logicGas->SetFieldManager(emFieldSetup->GetLocalFieldManager(), false);
      //std::cout << "EM FIELD IS SWITCHED OFF VERY IMPORTANT TO KNOW THIS TURN IT BACK ON SOMETIME" << std::endl;

      // Gas placement
      G4VPhysicalVolume *gasReplicaStrip =
      	new G4PVReplica("Gas",                  // name
			logicGasStrip,          // logical volume
			logicGasEnv,            // mother volume
			kXAxis,                 // replication axis
			nReplicasX,             // number of replicas
			plateSizeX);            // width

      G4VPhysicalVolume *gasReplica =
      	new G4PVReplica("Gas",                  // name
			logicGas,               // logical volume
			gasReplicaStrip,        // mother volume
			kYAxis,                 // replication axis
			nReplicasY,             // number of replicas
			plateSizeY);            // width

      // Step limits - RPCs
      G4double rpcLimit = 0.01*mm;
      G4UserLimits *rpcLimiter = new G4UserLimits();
      rpcLimiter->SetMaxAllowedStep(rpcLimit);
      logicGas->SetUserLimits(rpcLimiter);
      logicPlate->SetUserLimits(rpcLimiter);

      // // Test volume
      // G4Box *testBox = new G4Box("Gas", 5*m, 5*m, 27*mm);
      // G4LogicalVolume *logicTest = new G4LogicalVolume(testBox, gasMat, "Gas");
      // G4VPhysicalVolume *physTes =
      // 	new G4PVPlacement(0,                     // no rotation
      // 			  G4ThreeVector(0, 0, -10*m),// placement
      // 			  logicTest,           // its logical volume
      // 			  "Gas",                 // its name
      // 			  logicWorld,           // its mother  volume
      // 			  false,                 // no boolean operation
      // 			  0,                     // copy number
      // 			  checkOverlaps);        // overlaps checking
      //logicTest->SetUserLimits(rpcLimiter);
      
      // // Step limits - world - needs to be larger for code speed
      // G4double worldLimit = 10*cm;
      // G4UserLimits *worldLimiter = new G4UserLimits();
      // worldLimiter->SetMaxAllowedStep(worldLimit);
      // logicWorld->SetUserLimits(worldLimiter);
    }
  // Silicon model of RPCs
  else
    {
      // RPC layers - sizes, separations, material
      G4double layerSizeXY = 10*m;
      G4double layerSizeZ = 2*cm;
      layerSep = 4*cm;
      G4double tripletSep = 1.55*m;
      G4Material *layerMat = nist->FindOrBuildMaterial("G4_Si");

      // Envelope for RPC layers in box
      G4Box *solidBoxEnvelope =
	new G4Box("boxEnvelope",
		  5*m, 5*m, 5*m);

      G4LogicalVolume *logicBoxEnvelope =
	new G4LogicalVolume(solidBoxEnvelope,
			    worldMat,
			    "Box");
      logicBoxEnvelope->SetVisAttributes(envelopeVisAttributes);
      
      G4VPhysicalVolume *physBoxEnvelope =
	new G4PVPlacement(0,                    // no rotation
			  G4ThreeVector(),       // placed at origin
			  logicBoxEnvelope,      // its logical volume
			  "Box",                 // its name
			  logicWorld,            // its mother  volume
			  false,                 // no boolean operation
			  0,                     // copy number
			  checkOverlaps);        // overlaps checking
      
      // RPC layers - solid and logical volumes
      G4Box *solidLayer =
	new G4Box("layer",
		  0.5*layerSizeXY, 0.5*layerSizeXY, 0.5*layerSizeZ);
      
      G4LogicalVolume *logicLayer =
	new G4LogicalVolume(solidLayer,
			    layerMat,
			    "Layer");
      
      // RPC layers - physical volumes
      // Inside box - place inside box envelope to group in visualisation menu
      
      // Loop to create RPC triplets
      for(unsigned int i=1; i<6; i++)
	{
	  // Loop to create individual layers of RPC triplets inside box
	  for(unsigned int j=0; j<3; j++)
	    {
	      G4ThreeVector layerPos = G4ThreeVector(0, 0, -5*m + i*tripletSep + j*(layerSep + layerSizeZ));
	      std::string num = std::to_string(i) + std::to_string(j);
	      int copyNo = std::stoi(num);
	      
	      G4VPhysicalVolume *physLayer =
		new G4PVPlacement(0,                     // no rotation
				  layerPos,              // its position
				  logicLayer,            // its logical volume
				  "Layer",               // its name
				  logicBoxEnvelope,      // its mother  volume
				  false,                 // no boolean operation
				  copyNo,                // copy number
				  checkOverlaps);        // overlaps checking
	    }
	}
      // RPC layers on each face of the box
      // Make vectors to each face of cube
      std::vector<G4ThreeVector> facePos;
      facePos.push_back(G4ThreeVector(0, 0, 5*m + 18*cm));
      facePos.push_back(G4ThreeVector(0, 0, -5*m - 18*cm));
      facePos.push_back(G4ThreeVector(0, 5*m + 18*cm, 0));
      facePos.push_back(G4ThreeVector(0, -5*m - 18*cm, 0));
      facePos.push_back(G4ThreeVector(5*m + 18*cm, 0, 0));
      facePos.push_back(G4ThreeVector(-5*m - 18*cm, 0, 0));

      // Loop to create RPC triplets on each face
      for(unsigned int i=0; i<facePos.size(); i++)
	{
	  G4RotationMatrix *rotFace = new G4RotationMatrix(); // Needed to rotate to match faces of cube
	  if(facePos.at(i).y()!=0) // Rotate for top/bottom face
	    rotFace->rotateX(90*deg);
	  if(facePos.at(i).x()!=0) // Rotate for side faces
	    rotFace->rotateY(90*deg);
	  
	  // Loop to create RPC layers on each face
	  for(unsigned int j=1; j<7; j++)
	    {
	      // Set layer position based on which face is currently being used
	      G4ThreeVector layerPos = facePos.at(i);
	      if(layerPos.x()>0)
		layerPos.setX(facePos.at(i).x() + j*(layerSep + layerSizeZ));
	      else if(layerPos.x()<0)
		layerPos.setX(facePos.at(i).x() - j*(layerSep + layerSizeZ));
	      else if(layerPos.y()>0)
		layerPos.setY(facePos.at(i).y() + j*(layerSep + layerSizeZ));
	      else if(layerPos.y()<0)
		layerPos.setY(facePos.at(i).y() - j*(layerSep + layerSizeZ));
	      else if(layerPos.z()>0)
		layerPos.setZ(facePos.at(i).z() + j*(layerSep + layerSizeZ));
	      else if(layerPos.z()<0)
		layerPos.setZ(facePos.at(i).z() - j*(layerSep + layerSizeZ));
	      
	      // Set copy number as before
	      std::string num = std::to_string(i+1) + std::to_string(j);
	      int copyNo = std::stoi(num);
	      
	      G4VPhysicalVolume *physLayer =
	      	new G4PVPlacement(rotFace,               // rotation
	      			  layerPos,              // its position
	      			  logicLayer,            // its logical volume
				  "Layer",               // its name
	      			  logicWorld,            // its mother  volume
	      			  false,                 // no boolean operation
	      			  copyNo,                // copy number
	      			  checkOverlaps);        // overlaps checking
	    }
	}
    }
  
  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
