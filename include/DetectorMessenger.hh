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
// $Id: DetectorMessenger.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction* );
   ~DetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    DetectorConstruction* Detector;
    
  private:
    G4UIdirectory*             detDir;
    G4UIdirectory*             appDir;
    G4UIdirectory*             worldDir;
    G4UIdirectory*             DetSysDir;
    G4UIcmdWithAString*        WorldMaterialCmd; 
    G4UIcmdWith3VectorAndUnit* WorldDimensionsCmd; 
    G4UIcmdWithABool*          WorldVisCmd;
    G4UIcmdWith3VectorAndUnit* WorldMagneticFieldCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;

    // Generic Target Apparatus
    G4UIcmdWithAString*        GenericTargetCmd;
    G4UIcmdWith3VectorAndUnit* GenericTargetDimensionsCmd;
    G4UIcmdWith3VectorAndUnit* GenericTargetPositionCmd;

    G4UIcmdWithAString*        FieldBoxMaterialCmd;
    G4UIcmdWith3VectorAndUnit* FieldBoxDimensionsCmd;
    G4UIcmdWith3VectorAndUnit* FieldBoxPositionCmd;
    G4UIcmdWith3VectorAndUnit* FieldBoxMagneticFieldCmd;

    G4UIcmdWithoutParameter*   AddApparatusSpiceTargetChamberCmd;
    G4UIcmdWithoutParameter*   AddApparatus8piVacuumChamberCmd;
    G4UIcmdWithAnInteger*      AddApparatus8piVacuumChamberDelrinShellCmd;

    // Detection Systems
    G4UIcmdWithAnInteger*   AddDetectionSystemGammaTrackingCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemBrillance380V1Cmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemSodiumIodideCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystem8piCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystem8piDetectorCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemSceptarCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemGriffinForwardCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemGriffinForwardDetectorCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemGriffinBackCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemGriffinBackDetectorCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemSpiceCmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemSpiceV02Cmd;
    G4UIcmdWithAnInteger*   AddDetectionSystemPacesCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
