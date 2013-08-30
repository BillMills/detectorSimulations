#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensitiveDetector.hh"

#include "G4Material.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Para.hh"
#include "G4Cons.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"

#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "DetectionSystemGriffin.hh"

///////////////////////////////////////////////////////////////////////
// The ::DetectionSystemGriffin constructor instatiates all the 
// Logical and Physical Volumes used in the detector geometery (found
// in our local files as it contains NDA-protected info), and the
// ::~DetectionSystemGriffin destructor deletes them from the stack 
// when they go out of scope
///////////////////////////////////////////////////////////////////////

DetectionSystemGriffin::~DetectionSystemGriffin()
{
  delete germanium_block_SD;
  delete left_casing_SD;
  delete right_casing_SD;
  delete left_extension_SD;
  delete right_extension_SD;
  delete back_plug_SD;

  // LogicalVolumes in ConstructNewHeavyMet
  delete hevimet_log;

  // LogicalVolumes in ConstructNewSuppressorCasing
  delete left_suppressor_extension_log;
  delete right_suppressor_extension_log;
  delete left_suppressor_log;
  delete right_suppressor_log;
  delete back_quarter_suppressor_log;

  delete shell_for_back_quarter_suppressor_log;
  delete shell_for_right_suppressor_log;
  delete shell_for_left_suppressor_log;
  delete shell_for_right_suppressor_extension_log;
  delete shell_for_left_suppressor_extension_log;
  // LogicalVolumes in ConstructBGOCasing

  delete BGO_casing_log;
  delete back_BGO_log;
  
  // LogicalVolumes in ConstructColdFinger
  delete cooling_side_block_log;
  delete structureMat_cold_finger_log;
  delete cooling_bar_log;
  delete fet_air_hole_log;
  delete triangle_post_log;
  delete extra_cold_block_log;
  delete finger_log;
  delete end_plate_log;

  // LogicalVolumes in ConstructComplexDetectorBlock
  delete germanium_hole_log;
  delete germanium_block1_log;
  delete inner_dead_layer_log;
  delete inter_crystal_electrodeMat_back_log;
  delete inter_crystal_electrodeMat_front_log;
 
  // LogicalVolumes in ConstructBasicDetectorBlock
  delete germanium_block_log;
 
  // LogicalVolumes in ConstructDetector
  delete tank_log;
  delete finger_shell_log;
  delete rear_plate_log;
  delete bottom_side_panel_log;
  delete top_side_panel_log;
  delete left_side_panel_log;
  delete right_side_panel_log;
  delete lower_left_tube_log;
  delete upper_left_tube_log;
  delete lower_right_tube_log;
  delete upper_right_tube_log;
  delete lower_left_cone_log;
  delete upper_left_cone_log;
  delete lower_right_cone_log;
  delete upper_right_cone_log;
  delete bottom_wedge_log;
  delete top_wedge_log;
  delete left_wedge_log;
  delete right_wedge_log;
  delete bottom_bent_piece_log;
  delete top_bent_piece_log;
  delete left_bent_piece_log;
  delete right_bent_piece_log;
  delete front_face_log;
}// end ::~DetectionSystemGriffin

///////////////////////////////////////////////////////////////////////
// ConstructDetectionSystemGriffin builds the DetectionSystemGriffin 
// at the origin
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::Build(G4SDManager* mySDman)
{ 
//  this->exp_hall_log = exp_hall_log;  

  if( !germanium_block_SD ) {
    germanium_block_SD = new SensitiveDetector(sdName0, colNameGe);
    mySDman->AddNewDetector( germanium_block_SD );
  }
  if (this->BGO_selector == 1)
  {
    if( !left_casing_SD && include_side_suppressors) {
      left_casing_SD = new SensitiveDetector(sdName1, colNameLeftCasing);
      mySDman->AddNewDetector( left_casing_SD );
    }
    if( !right_casing_SD && include_side_suppressors) {
      right_casing_SD = new SensitiveDetector(sdName2, colNameRightCasing);
      mySDman->AddNewDetector( right_casing_SD );
    }
    if( !left_extension_SD && include_extension_suppressors ) {
      left_extension_SD = new SensitiveDetector(sdName3, colNameLeftExtension);
      mySDman->AddNewDetector( left_extension_SD );
    }
    if( !right_extension_SD && include_extension_suppressors) {
      right_extension_SD = new SensitiveDetector(sdName4, colNameRightExtension);
      mySDman->AddNewDetector( right_extension_SD );
    }
    if( !back_plug_SD && include_back_suppressors) {
      back_plug_SD = new SensitiveDetector(sdName5, colNameBackPlug);
      mySDman->AddNewDetector( back_plug_SD );
    }
  }
  
  BuildOneDetector();

  germanium_block1_log->SetSensitiveDetector( germanium_block_SD );
  if(include_side_suppressors) {
      left_suppressor_log->SetSensitiveDetector( left_casing_SD );
      right_suppressor_log->SetSensitiveDetector( right_casing_SD );
  }
  if(include_extension_suppressors) {
    left_suppressor_extension_log->SetSensitiveDetector( left_extension_SD );
    right_suppressor_extension_log->SetSensitiveDetector( right_extension_SD );
  }
  if(include_back_suppressors) {
    back_quarter_suppressor_log->SetSensitiveDetector( back_plug_SD );
  }

}//end ::Build

G4int DetectionSystemGriffin::PlaceDetector(G4LogicalVolume* exp_hall_log, G4ThreeVector moveBAH, G4RotationMatrix* rotateBAH, G4int detector_number)
{
  G4double theta 	= this->coords[detector_number][0]*deg;     
  G4double phi 		= this->coords[detector_number][1]*deg;		
  G4double alpha 	= this->coords[detector_number][2]*deg;	// yaw	
  G4double beta 	= this->coords[detector_number][3]*deg;	// pitch	
  G4double gamma 	= this->coords[detector_number][4]*deg;	// roll	
          
  G4double x;
  G4double y;
  G4double z;
  
  G4RotationMatrix* rotate = new G4RotationMatrix; 		// rotation matrix corresponding to direction vector
  rotate->rotateX(M_PI/2.0);  
  rotate->rotateX(alpha);
  rotate->rotateY(beta);
  rotate->rotateZ(gamma); 

  // positioning
  G4double dist_from_origin = this->air_box_back_length/2.0 +this->air_box_front_length + this->new_rhombi_radius;
  
  x = 0;
  y = 0;
  z = dist_from_origin;
    
  G4ThreeVector move((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));
   
  this->assembly->MakeImprint(exp_hall_log, move, rotate, 0);
  this->suppressorShellAssembly->MakeImprint(exp_hall_log, move, rotate, 0);

  /////////////////////////////////////////////////////////////////////
  // now we place all 4 of the 1/4 detectors using the LogicalVolume
  // of the 1st, ensuring that they too will have the hole   
  /////////////////////////////////////////////////////////////////////
  copy_number = germanium_copy_number + detector_number*4;

  // 1st
  G4RotationMatrix* rotate_germanium1 = new G4RotationMatrix;
  rotate_germanium1->rotateY(-M_PI/2.0);
  rotate_germanium1->rotateX(alpha);
  rotate_germanium1->rotateY(beta);
  rotate_germanium1->rotateZ(gamma); 

  x = (this->germanium_width + this->germanium_separation)/2.0;
  y = (this->germanium_width + this->germanium_separation)/2.0;
  z = this->germanium_length/2.0 +this->can_face_thickness/2.0 +this->germanium_dist_from_can_face +this->shift + this->applied_back_shift+ dist_from_origin;

  G4ThreeVector move_germanium1((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->germaniumAssembly->MakeImprint(exp_hall_log, move_germanium1, rotate_germanium1, copy_number++);
  
  // 2nd
  G4RotationMatrix* rotate_germanium2 = new G4RotationMatrix;
  rotate_germanium2->rotateY(-M_PI/2.0);
  rotate_germanium2->rotateX(-M_PI/2.0);
  rotate_germanium2->rotateX(alpha);
  rotate_germanium2->rotateY(beta);
  rotate_germanium2->rotateZ(gamma); 

  x = (this->germanium_width + this->germanium_separation)/2.0;
  y = -(this->germanium_width + this->germanium_separation)/2.0;
  z = this->germanium_length/2.0 +this->can_face_thickness/2.0 +this->germanium_dist_from_can_face +this->shift + this->applied_back_shift+ dist_from_origin;

  G4ThreeVector move_germanium2((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->germaniumAssembly->MakeImprint(exp_hall_log, move_germanium2, rotate_germanium2, copy_number++);
  
  // 3rd
  G4RotationMatrix* rotate_germanium3 = new G4RotationMatrix;
  rotate_germanium3->rotateY(-M_PI/2.0);
  rotate_germanium3->rotateX(-M_PI);  
  rotate_germanium3->rotateX(alpha);
  rotate_germanium3->rotateY(beta);
  rotate_germanium3->rotateZ(gamma);     

  x = -(this->germanium_width + this->germanium_separation)/2.0;
  y = -(this->germanium_width + this->germanium_separation)/2.0;
  z = this->germanium_length/2.0 +this->can_face_thickness/2.0 +this->germanium_dist_from_can_face +this->shift + this->applied_back_shift+ dist_from_origin;
    
  G4ThreeVector move_germanium3((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->germaniumAssembly->MakeImprint(exp_hall_log, move_germanium3, rotate_germanium3, copy_number++);
  
  // 4th
  G4RotationMatrix* rotate_germanium4 = new G4RotationMatrix;
  rotate_germanium4->rotateY(-M_PI/2.0);
  rotate_germanium4->rotateX(M_PI/2.0);
  rotate_germanium4->rotateX(alpha);
  rotate_germanium4->rotateY(beta);
  rotate_germanium4->rotateZ(gamma);     

  x = -(this->germanium_width + this->germanium_separation)/2.0;
  y = (this->germanium_width + this->germanium_separation)/2.0;
  z = this->germanium_length/2.0 +this->can_face_thickness/2.0 +this->germanium_dist_from_can_face +this->shift + this->applied_back_shift+ dist_from_origin;
    
  G4ThreeVector move_germanium4((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->germaniumAssembly->MakeImprint(exp_hall_log, move_germanium4, rotate_germanium4, copy_number++);
    
  /////////////////////////////////////////////////////////////////////
  // end germanium_block1_log
  /////////////////////////////////////////////////////////////////////
  copy_number = back_suppressor_copy_number + detector_number*4;

  if(include_back_suppressors) {
  // Back Suppressors
  // this one goes behind Ge 1
  G4RotationMatrix* rotate_back_quarter_suppressor1 = new G4RotationMatrix;
  rotate_back_quarter_suppressor1->rotateX(alpha);
  rotate_back_quarter_suppressor1->rotateY(beta);
  rotate_back_quarter_suppressor1->rotateZ(gamma);    

//  x = (this->detector_total_width/4.0 + this->suppressor_shell_thickness);
//  y = (this->detector_total_width/4.0 + this->suppressor_shell_thickness);
  x = (this->detector_total_width/4.0);
  y = (this->detector_total_width/4.0);
  z = (this->back_BGO_thickness - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_length +this->BGO_can_seperation + this->shift + this->applied_back_shift + dist_from_origin;

  G4ThreeVector move_back_quarter_suppressor1((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->suppressorBackAssembly->MakeImprint(exp_hall_log, move_back_quarter_suppressor1, rotate_back_quarter_suppressor1, copy_number++);


  // this one goes behind Ge 2
  G4RotationMatrix* rotate_back_quarter_suppressor2 = new G4RotationMatrix;
  rotate_back_quarter_suppressor2->rotateX(-M_PI/2.0);
  rotate_back_quarter_suppressor2->rotateX(alpha);
  rotate_back_quarter_suppressor2->rotateY(beta);
  rotate_back_quarter_suppressor2->rotateZ(gamma);    
    
  x = (this->detector_total_width/4.0);
  y = -(this->detector_total_width/4.0);
  z = (this->back_BGO_thickness - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_length +this->BGO_can_seperation + this->shift + this->applied_back_shift + dist_from_origin;

  G4ThreeVector move_back_quarter_suppressor2((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->suppressorBackAssembly->MakeImprint(exp_hall_log, move_back_quarter_suppressor2, rotate_back_quarter_suppressor2, copy_number++);


  // this one goes behind Ge 3
  G4RotationMatrix* rotate_back_quarter_suppressor3 = new G4RotationMatrix;
  rotate_back_quarter_suppressor3->rotateX(M_PI);
  rotate_back_quarter_suppressor3->rotateX(alpha);
  rotate_back_quarter_suppressor3->rotateY(beta);
  rotate_back_quarter_suppressor3->rotateZ(gamma);    

  x = -(this->detector_total_width/4.0);
  y = -(this->detector_total_width/4.0);
  z = (this->back_BGO_thickness - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_length +this->BGO_can_seperation + this->shift + this->applied_back_shift + dist_from_origin;

  G4ThreeVector move_back_quarter_suppressor3((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->suppressorBackAssembly->MakeImprint(exp_hall_log, move_back_quarter_suppressor3, rotate_back_quarter_suppressor3, copy_number++);

  // this one goes behind Ge 3
  G4RotationMatrix* rotate_back_quarter_suppressor4 = new G4RotationMatrix;
  rotate_back_quarter_suppressor4->rotateX(M_PI/2.0);
  rotate_back_quarter_suppressor4->rotateX(alpha);
  rotate_back_quarter_suppressor4->rotateY(beta);
  rotate_back_quarter_suppressor4->rotateZ(gamma);    
    
  x = -(this->detector_total_width/4.0);
  y = (this->detector_total_width/4.0);
  z = (this->back_BGO_thickness - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_length +this->BGO_can_seperation + this->shift + this->applied_back_shift + dist_from_origin;

  G4ThreeVector move_back_quarter_suppressor4((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));

  this->suppressorBackAssembly->MakeImprint(exp_hall_log, move_back_quarter_suppressor4, rotate_back_quarter_suppressor4, copy_number++);
  }

  // Now Side Suppressors
  /////////////////////////////////////////////////////////////////////
  // Note : Left and Right are read from the BACK of the detector
  // Suppressors 1 and 2 cover germanium 1
  // Suppressors 3 and 4 cover germanium 2
  // Suppressors 5 and 6 cover germanium 3
  // Suppressors 7 and 8 cover germanium 4
  /////////////////////////////////////////////////////////////////////
  copy_number = right_suppressor_side_copy_number + detector_number*4;
  copy_number_two = left_suppressor_side_copy_number + detector_number*4;

  // Replacement for this->side_suppressor_length
  G4double shell_side_suppressor_length = this->side_suppressor_length
    + (this->suppressor_shell_thickness*2.0);

  // Replacement for this->suppressor_extension_length
  G4double shell_suppressor_extension_length = this->suppressor_extension_length
        + (this->suppressor_shell_thickness*2.0)*(1.0/tan(this->bent_end_angle)
    - tan(this->bent_end_angle));

  // Replacement for this->suppressor_extension_angle: must totally recalculate
  G4double shell_suppressor_extension_angle = atan(((this->back_inner_radius
    + this->bent_end_length +(this->BGO_can_seperation
        + this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
        / tan(this->bent_end_angle)
        - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
        * sin(this->bent_end_angle))
        * tan(this->bent_end_angle) -(this->forward_inner_radius +this->hevimet_tip_thickness)
        * sin(this->bent_end_angle))/(shell_suppressor_extension_length));

  // these two parameters are for shifting the extensions back and out when in their BACK position
  G4double extension_back_shift = this->air_box_front_length
    - (this->hevimet_tip_thickness +shell_suppressor_extension_length
    + (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
    * tan(this->bent_end_angle))
    * cos(this->bent_end_angle);

  G4double extension_radial_shift = extension_back_shift*tan(this->bent_end_angle);

  if(include_side_suppressors) {
        
  G4RotationMatrix* rotate_suppressor1 = new G4RotationMatrix;
  rotate_suppressor1->rotateZ(M_PI/2.0);
  rotate_suppressor1->rotateY(M_PI/2.0);
  rotate_suppressor1->rotateX(alpha);
  rotate_suppressor1->rotateY(beta);
  rotate_suppressor1->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  y = (this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  x = this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation;

  G4ThreeVector move_inner_suppressor1((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->rightSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor1, rotate_suppressor1, copy_number++);

  G4RotationMatrix* rotate_suppressor2 = new G4RotationMatrix;
  rotate_suppressor2->rotateY(-M_PI/2.0);    
  rotate_suppressor2->rotateX(-M_PI);
  rotate_suppressor2->rotateX(alpha);
  rotate_suppressor2->rotateY(beta);
  rotate_suppressor2->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  x = (this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  y = this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation;
    
  G4ThreeVector move_inner_suppressor2((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->leftSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor2, rotate_suppressor2, copy_number_two++);

  G4RotationMatrix* rotate_suppressor3 = new G4RotationMatrix;
  rotate_suppressor3->rotateZ(M_PI/2.0);
  rotate_suppressor3->rotateY(M_PI/2.0);
  rotate_suppressor3->rotateX(-M_PI/2.0);
  rotate_suppressor3->rotateX(alpha);
  rotate_suppressor3->rotateY(beta);
  rotate_suppressor3->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  x = (this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  y = -(this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor3((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->rightSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor3, rotate_suppressor3, copy_number++);

  G4RotationMatrix* rotate_suppressor4 = new G4RotationMatrix;
  rotate_suppressor4->rotateY(-M_PI/2.0);
  rotate_suppressor4->rotateX(M_PI/2.0);
  rotate_suppressor4->rotateX(alpha);
  rotate_suppressor4->rotateY(beta);
  rotate_suppressor4->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  y = -(this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  x = (this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor4((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->leftSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor4, rotate_suppressor4, copy_number_two++);

  G4RotationMatrix* rotate_suppressor5 = new G4RotationMatrix;
  rotate_suppressor5->rotateZ(M_PI/2.0);
  rotate_suppressor5->rotateY(M_PI/2.0);
  rotate_suppressor5->rotateX(-M_PI);
  rotate_suppressor5->rotateX(alpha);
  rotate_suppressor5->rotateY(beta);
  rotate_suppressor5->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  y = -(this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  x = -(this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor5((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->rightSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor5, rotate_suppressor5, copy_number++);

  G4RotationMatrix* rotate_suppressor6 = new G4RotationMatrix;
  rotate_suppressor6->rotateY(-M_PI/2.0);
  rotate_suppressor6->rotateX(alpha);
  rotate_suppressor6->rotateY(beta);
  rotate_suppressor6->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  x = -(this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  y = -(this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor6((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->leftSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor6, rotate_suppressor6, copy_number_two++);

  G4RotationMatrix* rotate_suppressor7 = new G4RotationMatrix;
  rotate_suppressor7->rotateZ(M_PI/2.0);
  rotate_suppressor7->rotateY(M_PI/2.0);
  rotate_suppressor7->rotateX(M_PI/2.0);
  rotate_suppressor7->rotateX(alpha);
  rotate_suppressor7->rotateY(beta);
  rotate_suppressor7->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  x = -(this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  y = (this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor7((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->rightSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor7, rotate_suppressor7, copy_number++);

  G4RotationMatrix* rotate_suppressor8 = new G4RotationMatrix;
  rotate_suppressor8->rotateY(-M_PI/2.0);
  rotate_suppressor8->rotateX(-M_PI/2.0);
  rotate_suppressor8->rotateX(alpha);
  rotate_suppressor8->rotateY(beta);
  rotate_suppressor8->rotateZ(gamma);     

  z = (shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
    + this->applied_back_shift - this->suppressor_shell_thickness/2.0 + dist_from_origin);
  y = (this->detector_total_width/2.0 +this->BGO_can_seperation + this->side_BGO_thickness/2.0)/2.0;
  x = -(this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness + this->detector_total_width/2.0 +this->BGO_can_seperation);
    
  G4ThreeVector move_inner_suppressor8((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

  this->leftSuppressorCasingAssembly->MakeImprint(exp_hall_log, move_inner_suppressor8, rotate_suppressor8, copy_number_two++);
  }

  // now we add the side pieces of suppressor that extend out in front of the can when it's in the back position
  copy_number = right_suppressor_extension_copy_number + detector_number*4;
  copy_number_two = left_suppressor_extension_copy_number + detector_number*4;

  if(this->applied_back_shift == 0.0 && include_extension_suppressors)		// If the detectors are forward, put the extensions in the back position
  {
    // the placement of the extensions matches the placement of the side_suppressor pieces
    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(M_PI/2.0);
    rotate_extension1->rotateY(this->bent_end_angle);
    rotate_extension1->rotateX(alpha);
    rotate_extension1->rotateY(beta);
    rotate_extension1->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    y = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	
	
    x = -((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) + extension_radial_shift;
	
    G4ThreeVector move_inner_extension1((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension1, rotate_extension1, copy_number++);

    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(M_PI/2.0);
    rotate_extension2->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension2->rotateX(alpha);
    rotate_extension2->rotateY(beta);
    rotate_extension2->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    x = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    y = -((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) + extension_radial_shift;

    G4ThreeVector move_inner_extension2((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension2, rotate_extension2, copy_number_two++);

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateZ(M_PI/2.0);
    rotate_extension3->rotateY(this->bent_end_angle);
    rotate_extension3->rotateX(-M_PI/2.0);
    rotate_extension3->rotateX(alpha);
    rotate_extension3->rotateY(beta);
    rotate_extension3->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    x = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    y = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) - extension_radial_shift;

    G4ThreeVector move_inner_extension3((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension3, rotate_extension3, copy_number++);

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateY(M_PI/2.0);
    rotate_extension4->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension4->rotateX(-M_PI/2.0); 
    rotate_extension4->rotateX(alpha);
    rotate_extension4->rotateY(beta);
    rotate_extension4->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    y = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    x = -((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) + extension_radial_shift;

    G4ThreeVector move_inner_extension4((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension4, rotate_extension4, copy_number_two++);

    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateY(this->bent_end_angle);
    rotate_extension5->rotateX(-M_PI);
    rotate_extension5->rotateX(alpha);
    rotate_extension5->rotateY(beta);
    rotate_extension5->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    y = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    x = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) - extension_radial_shift;

    G4ThreeVector move_inner_extension5((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension5, rotate_extension5, copy_number++);

    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension6->rotateX(-M_PI); 
    rotate_extension6->rotateX(alpha);
    rotate_extension6->rotateY(beta);
    rotate_extension6->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    x = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    y = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) - extension_radial_shift;

    G4ThreeVector move_inner_extension6((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension6, rotate_extension6, copy_number_two++);

    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateZ(M_PI/2.0);
    rotate_extension7->rotateY(this->bent_end_angle);
    rotate_extension7->rotateX(M_PI/2.0);
    rotate_extension7->rotateX(alpha);
    rotate_extension7->rotateY(beta);
    rotate_extension7->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    x = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    y = -((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) + extension_radial_shift;

    G4ThreeVector move_inner_extension7((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension7, rotate_extension7, copy_number++);

    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI/2.0);
    rotate_extension8->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension8->rotateX(M_PI/2.0); 
    rotate_extension8->rotateX(alpha);
    rotate_extension8->rotateY(beta);
    rotate_extension8->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    y = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    x = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) - extension_radial_shift;

    G4ThreeVector move_inner_extension8((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension8, rotate_extension8, copy_number_two++);

  }//end if(detectors forward) statement

  // Otherwise, put them forward
  else if(this->applied_back_shift == this->back_inner_radius - this->forward_inner_radius && include_extension_suppressors)
  {
    // the placement of the extensions matches the placement of the side_suppressor pieces
    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(M_PI/2.0);   
    rotate_extension1->rotateY(this->bent_end_angle);
    rotate_extension1->rotateX(alpha);
    rotate_extension1->rotateY(beta);
    rotate_extension1->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    y = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    x = - ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));
	
    G4ThreeVector move_inner_extension1((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension1, rotate_extension1, copy_number++);

    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(M_PI/2.0);
    rotate_extension2->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension2->rotateX(alpha);
    rotate_extension2->rotateY(beta);
    rotate_extension2->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;
	
    x = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	
	
    y = - ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension2((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension2, rotate_extension2, copy_number_two++);

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateZ(M_PI/2.0);
    rotate_extension3->rotateY(this->bent_end_angle);
    rotate_extension3->rotateX(-M_PI/2.0);
    rotate_extension3->rotateX(alpha);
    rotate_extension3->rotateY(beta);
    rotate_extension3->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    x = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    y = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension3((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension3, rotate_extension3, copy_number++);

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateY(M_PI/2.0);
    rotate_extension4->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension4->rotateX(-M_PI/2.0); 
    rotate_extension4->rotateX(alpha);
    rotate_extension4->rotateY(beta);
    rotate_extension4->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    y = - (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;	

    x = - ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension4((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension4, rotate_extension4, copy_number_two++);

    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateY(this->bent_end_angle);
    rotate_extension5->rotateX(-M_PI);
    rotate_extension5->rotateX(alpha);
    rotate_extension5->rotateY(beta);
    rotate_extension5->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    y = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    x = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension5((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension5, rotate_extension5, copy_number++);

    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension6->rotateX(-M_PI); 
    rotate_extension6->rotateX(alpha);
    rotate_extension6->rotateY(beta);
    rotate_extension6->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    x = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    y = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension6((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension6, rotate_extension6, copy_number_two++);

    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateZ(M_PI/2.0);
    rotate_extension7->rotateY(this->bent_end_angle);
    rotate_extension7->rotateX(M_PI/2.0);
    rotate_extension7->rotateX(alpha);
    rotate_extension7->rotateY(beta);
    rotate_extension7->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;

    x = -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    y = - ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension7((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->rightSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension7, rotate_extension7, copy_number++);

    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI/2.0);
    rotate_extension8->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension8->rotateX(M_PI/2.0); 
    rotate_extension8->rotateX(alpha);
    rotate_extension8->rotateY(beta);
    rotate_extension8->rotateZ(gamma);      

    z = -this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius
	- this->suppressor_shell_thickness/2.0 + dist_from_origin;	
	
    y = (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0 - this->detectorPlacementCxn)/2.0;		

    x = ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    G4ThreeVector move_inner_extension8((x*cos(theta)+z*sin(theta))*cos(phi)-y*sin(phi),(x*cos(theta)+z*sin(theta))*sin(phi)+y*cos(phi),-x*sin(theta)+z*cos(theta));     

    this->leftSuppressorExtensionAssembly->MakeImprint(exp_hall_log, move_inner_extension8, rotate_extension8, copy_number_two++);
        
  }//end if(detectors back) statement
  return 1;
}


///////////////////////////////////////////////////////////////////////
// BuildOneDetector()
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::BuildOneDetector()
{
  // Build assembly volumes
  G4AssemblyVolume* myAssembly = new G4AssemblyVolume();
  this->assembly = myAssembly;
  G4AssemblyVolume* myGermaniumAssembly = new G4AssemblyVolume();
  this->germaniumAssembly = myGermaniumAssembly;  
  G4AssemblyVolume* myLeftSuppressorCasingAssembly = new G4AssemblyVolume();
  this->leftSuppressorCasingAssembly = myLeftSuppressorCasingAssembly;  
  G4AssemblyVolume* myRightSuppressorCasingAssembly = new G4AssemblyVolume();
  this->rightSuppressorCasingAssembly = myRightSuppressorCasingAssembly;  
  G4AssemblyVolume* myLeftSuppressorExtensionAssembly = new G4AssemblyVolume();
  this->leftSuppressorExtensionAssembly = myLeftSuppressorExtensionAssembly;  
  G4AssemblyVolume* myRightSuppressorExtensionAssembly = new G4AssemblyVolume();
  this->rightSuppressorExtensionAssembly = myRightSuppressorExtensionAssembly;  
  G4AssemblyVolume* mySuppressorBackAssembly = new G4AssemblyVolume();
  this->suppressorBackAssembly = mySuppressorBackAssembly;          
  G4AssemblyVolume* mySuppressorShellAssembly = new G4AssemblyVolume();
  this->suppressorShellAssembly = mySuppressorShellAssembly;

  ConstructComplexDetectorBlockWithDeadLayer();
  BuildelectrodeMatElectrodes();
  
  ConstructDetector();
  
  // Include BGOs?
  if (this->BGO_selector == 1) {
    if (this->suppressor_shells_include_flag == true) {
      ConstructNewSuppressorCasingWithShells();  
    }
    else if (this->suppressor_shells_include_flag == false) {
      ConstructNewSuppressorCasing();   // THis still needs some work!
      G4cout << "**************************************************" << G4endl;
      G4cout << "ConstructNewSuppressorCasing still needs some work!," << G4endl;
      G4cout << "**************************************************" << G4endl;
      exit(1);
    }
  }
  else if (this->BGO_selector == 0) {
    G4cout << "Not building BGO " << G4endl;
  }
  else {
    G4cout << "Error 234235" << G4endl;
    exit(1);
  }

  ConstructColdFinger(); 

  if((this->applied_back_shift == this->back_inner_radius -this->forward_inner_radius) && (this->hevimet_selector == 1)) {
    ConstructNewHeavyMet(); 
  }

}

///////////////////////////////////////////////////////////////////////
// ConstructComplexDetectorBlock builds four quarters of germanium
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructComplexDetectorBlock()
{
  G4Material* materialGe = G4Material::GetMaterial("G4_Ge");
  if( !materialGe ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  G4Material* materialVacuum = G4Material::GetMaterial("Vacuum");
  if( !materialVacuum ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* germanium_block1_vis_att = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  germanium_block1_vis_att->SetVisibility(true);

  G4VisAttributes* dead_layer_vis_att = new G4VisAttributes(G4Colour(0.80, 0.0, 0.0));
  dead_layer_vis_att->SetVisibility(true);
  
  G4VisAttributes* air_vis_att = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  air_vis_att->SetVisibility(true);
  
  G4VisAttributes* electrodeMat_vis_att = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  electrodeMat_vis_att->SetVisibility(true);

  G4SubtractionSolid* detector1 = this->quarterDetector();
				
  germanium_block1_log = new G4LogicalVolume(detector1, materialGe, "germanium_block1_log", 0, 0, 0);
  germanium_block1_log->SetVisAttributes(germanium_block1_vis_att);

  this->germaniumAssembly->AddPlacedVolume(germanium_block1_log, this->move_null, this->rotate_null);  
  
  // now we make the hole that will go in the back of each quarter detector,
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI;
  G4double hole_radius = this->germanium_hole_radius;
  G4double hole_half_length_z = (this->germanium_length -this->germanium_hole_dist_from_face)/2.0;

  G4Tubs* hole_tubs = new G4Tubs("hole_tubs", 0.0, hole_radius, hole_half_length_z, start_angle, final_angle);

  G4ThreeVector move_hole(this->germanium_shift, -(this->germanium_shift),
	-((this->germanium_hole_dist_from_face)));

  germanium_hole_log = new G4LogicalVolume(hole_tubs, materialVacuum, "germanium_hole_log", 0, 0, 0);
  germanium_hole_log->SetVisAttributes(air_vis_att);

  this->germaniumAssembly->AddPlacedVolume(germanium_hole_log, move_hole, this->rotate_null);  

}//end ::ConstructComplexDetectorBlock

///////////////////////////////////////////////////////////////////////
// ConstructComplexDetectorBlockWithDeadLayer builds four quarters of 
// germanium, with dead layers
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructComplexDetectorBlockWithDeadLayer()
{
  G4Material* materialGe = G4Material::GetMaterial("G4_Ge");
  if( !materialGe ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  G4Material* materialVacuum = G4Material::GetMaterial("Vacuum");
  if( !materialVacuum ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* germanium_block1_vis_att = new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  germanium_block1_vis_att->SetVisibility(true);

  G4VisAttributes* dead_layer_vis_att = new G4VisAttributes(G4Colour(0.80, 0.0, 0.0));
  dead_layer_vis_att->SetVisibility(true);
  
  G4VisAttributes* air_vis_att = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  air_vis_att->SetVisibility(true);
  
  G4VisAttributes* electrodeMat_vis_att = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  electrodeMat_vis_att->SetVisibility(true);

  G4SubtractionSolid* detector1 = this->quarterDetector();
				
  germanium_block1_log = new G4LogicalVolume(detector1, materialGe, "germanium_block1_log", 0, 0, 0);
  germanium_block1_log->SetVisAttributes(germanium_block1_vis_att);

  this->germaniumAssembly->AddPlacedVolume(germanium_block1_log, this->move_null, this->rotate_null);  
 
  /////////////////////////////////////////////////////////////////////
  // Since if we are using this method, the inner dead layer must be
  // simulated, make a cylinder of germanium, place the air cylinder
  // inside it, and then place the whole thing in the crystal
  /////////////////////////////////////////////////////////////////////
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI;
  G4double dead_layer_radius = this->germanium_hole_radius + this->inner_dead_layer_thickness;
  G4double dead_layer_half_length_z = (this->germanium_length 
  	- this->germanium_hole_dist_from_face
	+ this->inner_dead_layer_thickness)/2.0;

  G4Tubs* dead_layer_tubs = new G4Tubs("dead_layer_tubs", this->germanium_hole_radius,
	dead_layer_radius, dead_layer_half_length_z, start_angle,final_angle);
  
  G4ThreeVector move_dead_layer(this->germanium_shift, -(this->germanium_shift),
	-((this->germanium_hole_dist_from_face
	- this->inner_dead_layer_thickness)/2.0));   

  inner_dead_layer_log = new G4LogicalVolume(dead_layer_tubs, materialGe, "inner_dead_layer_log", 0, 0, 0);
  inner_dead_layer_log->SetVisAttributes(dead_layer_vis_att);
  
  this->germaniumAssembly->AddPlacedVolume(inner_dead_layer_log, move_dead_layer, this->rotate_null);  

  // dead layer cap
  dead_layer_radius = this->germanium_hole_radius;
  G4double dead_layer_cap_half_length_z = (this->inner_dead_layer_thickness)/2.0;

  G4Tubs* dead_layer_cap = new G4Tubs("dead_layer_cap", 0.0,
	dead_layer_radius, dead_layer_cap_half_length_z, start_angle,final_angle);
  
  G4ThreeVector move_dead_layer_cap(this->germanium_shift, -(this->germanium_shift),
	-((this->germanium_hole_dist_from_face - this->inner_dead_layer_thickness)/2.0 - dead_layer_half_length_z + this->inner_dead_layer_thickness/2.0));   

  inner_dead_layer_cap_log = new G4LogicalVolume(dead_layer_cap, materialGe, "inner_dead_layer_cap_log", 0, 0, 0);
  inner_dead_layer_cap_log->SetVisAttributes(dead_layer_vis_att);
  
  this->germaniumAssembly->AddPlacedVolume(inner_dead_layer_cap_log, move_dead_layer_cap, this->rotate_null);  

  
  // now we make the hole that will go in the back of each quarter detector,
  // and place it inside the dead layer cylinder
  start_angle = 0.0*M_PI;
  final_angle = 2.0*M_PI;
  G4double hole_radius = this->germanium_hole_radius;
  G4double hole_half_length_z = (this->germanium_length -this->germanium_hole_dist_from_face)/2.0;

  G4Tubs* hole_tubs = new G4Tubs("hole_tubs", 0.0, hole_radius, hole_half_length_z, start_angle, final_angle);

  G4ThreeVector move_hole(this->germanium_shift, -(this->germanium_shift),
	-((this->germanium_hole_dist_from_face
	- this->inner_dead_layer_thickness)/2.0) 
	-(this->inner_dead_layer_thickness/2.0)); 

  germanium_hole_log = new G4LogicalVolume(hole_tubs, materialVacuum, "germanium_hole_log", 0, 0, 0);
  germanium_hole_log->SetVisAttributes(air_vis_att);

  this->germaniumAssembly->AddPlacedVolume(germanium_hole_log, move_hole, this->rotate_null);  
	
}//end ::ConstructComplexDetectorBlockWithDeadLayer

///////////////////////////////////////////////////////////////////////
// Builds a layer of electrodeMat between germanium crystals to
// approximate electrodes, etc. that Eurisys won't tell us 
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::BuildelectrodeMatElectrodes()
{
  G4Material* electrodeMaterial = G4Material::GetMaterial(this->electrodeMaterial);
  if( !electrodeMaterial ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* electrodeMat_vis_att = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  electrodeMat_vis_att->SetVisibility(true);

  // electrodeMat layers between crystals - back part
  G4RotationMatrix* rotate_electrodeMat_back = new G4RotationMatrix;
  rotate_electrodeMat_back->rotateZ(M_PI/2.0);
  G4ThreeVector move_inter_crystal_electrodeMat_back(this->germanium_length/2.0 
	+ this->germanium_bent_length/2.0
	+ this->can_face_thickness/2.0
	+ this->germanium_dist_from_can_face + this->shift
	+ this->applied_back_shift, 0,0);
	
  G4UnionSolid* inter_crystal_electrodeMat_back = this->interCrystalelectrodeMatBack(); 
  
  inter_crystal_electrodeMat_back_log = new G4LogicalVolume(inter_crystal_electrodeMat_back,
	electrodeMaterial, "inter_crystal_electrodeMat_back_log", 0, 0, 0);
  inter_crystal_electrodeMat_back_log->SetVisAttributes(electrodeMat_vis_att);

  this->assembly->AddPlacedVolume(inter_crystal_electrodeMat_back_log, move_inter_crystal_electrodeMat_back, rotate_electrodeMat_back);  

  // electrodeMat between crystals - front part
  G4RotationMatrix* rotate_electrodeMat_front = new G4RotationMatrix;
  rotate_electrodeMat_front->rotateY(M_PI/2.0);
  G4UnionSolid* inter_crystal_electrodeMat_front = this->interCrystalelectrodeMatFront(); 
  
  G4ThreeVector move_inter_crystal_electrodeMat_front(this->germanium_bent_length/2.0
	+ this->electrodeMat_starting_depth/2.0 
	+ this->can_face_thickness/2.0
	+ this->germanium_dist_from_can_face + this->shift
	+ this->applied_back_shift, 0,0);
  
  inter_crystal_electrodeMat_front_log = new G4LogicalVolume(inter_crystal_electrodeMat_front,
	electrodeMaterial, "inter_crystal_electrodeMat_front_log", 0, 0, 0);
  inter_crystal_electrodeMat_front_log->SetVisAttributes(electrodeMat_vis_att);
	
  this->assembly->AddPlacedVolume(inter_crystal_electrodeMat_front_log, move_inter_crystal_electrodeMat_front, rotate_electrodeMat_front);  
}//end ::BuildelectrodeMatElectrodes

///////////////////////////////////////////////////////////////////////
// ConstructDetector builds the structureMat can, cold finger shell, 
// and liquid nitrogen tank. Since the can's face is built first, 
// everything else is placed relative to it  
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructDetector() 
{
  G4Material* structureMaterial = G4Material::GetMaterial(this->structureMaterial);
  if( !structureMaterial ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  // first we make the can's front face
  G4Box* front_face = this->squareFrontFace();
  front_face_log = new G4LogicalVolume(front_face, structureMaterial, "front_face_log", 0, 0, 0);

  G4ThreeVector move_front_face(this->shift +this->applied_back_shift, 0, 0);

  this->assembly->AddPlacedVolume(front_face_log, move_front_face, this->rotate_null); 

  // now we put on the four angled side pieces
  G4Para* right_bent_piece = this->bentSidePiece();
  G4RotationMatrix* rotate_right_bent_piece = new G4RotationMatrix;
  rotate_right_bent_piece->rotateY(-this->bent_end_angle);
  G4ThreeVector move_right_bent_piece(((this->bent_end_length)/2.0) +this->shift 
	+ this->applied_back_shift, 0, ((this->can_face_thickness 
	- this->bent_end_length)*tan(this->bent_end_angle) 
	+ this->detector_total_width -this->can_face_thickness)/2.0);

  right_bent_piece_log = new G4LogicalVolume(right_bent_piece, structureMaterial, "right_bent_piece", 0, 0, 0);

  this->assembly->AddPlacedVolume(right_bent_piece_log, move_right_bent_piece, rotate_right_bent_piece); 

  G4Para* left_bent_piece = this->bentSidePiece();
  G4RotationMatrix* rotate_left_bent_piece = new G4RotationMatrix;
  rotate_left_bent_piece->rotateX(M_PI);
  rotate_left_bent_piece->rotateY(this->bent_end_angle);

  G4ThreeVector move_left_bent_piece(((this->bent_end_length)/2.0) +this->shift 
	+ this->applied_back_shift, 0, -((this->can_face_thickness 
	- this->bent_end_length) *tan(this->bent_end_angle) 
	+ this->detector_total_width -this->can_face_thickness)/2.0);

  left_bent_piece_log = new G4LogicalVolume(left_bent_piece, structureMaterial, "left_bent_piece", 0, 0, 0);

  this->assembly->AddPlacedVolume(left_bent_piece_log, move_left_bent_piece, rotate_left_bent_piece);

  G4Para* top_bent_piece = this->bentSidePiece();

  G4RotationMatrix* rotate_top_bent_piece = new G4RotationMatrix;
  rotate_top_bent_piece->rotateY(-this->bent_end_angle);
  rotate_top_bent_piece->rotateX(-M_PI/2.0);

  G4ThreeVector move_top_bent_piece(((this->bent_end_length)/2.0) 
  	+ this->shift +this->applied_back_shift,
	((this->can_face_thickness -this->bent_end_length)
	* tan(this->bent_end_angle) + this->detector_total_width
	- this->can_face_thickness)/2.0, 0);

  top_bent_piece_log = new G4LogicalVolume(top_bent_piece, structureMaterial, "top_bent_piece", 0, 0, 0);

  this->assembly->AddPlacedVolume(top_bent_piece_log, move_top_bent_piece, rotate_top_bent_piece);
  
  G4Para* bottom_bent_piece = this->bentSidePiece();

  G4RotationMatrix* rotate_bottom_bent_piece = new G4RotationMatrix;
  rotate_bottom_bent_piece->rotateY(-this->bent_end_angle);
  rotate_bottom_bent_piece->rotateX(M_PI/2.0);

  G4ThreeVector move_bottom_bent_piece(((this->bent_end_length)/2.0) +this->shift 
	+ this->applied_back_shift, -((this->can_face_thickness 
	- this->bent_end_length)*tan(this->bent_end_angle) 
	+ this->detector_total_width -this->can_face_thickness)/2.0, 0);

  bottom_bent_piece_log = new G4LogicalVolume(bottom_bent_piece, structureMaterial, "bottom_bent_piece", 0, 0, 0);

  this->assembly->AddPlacedVolume(bottom_bent_piece_log, move_bottom_bent_piece, rotate_bottom_bent_piece);

  // now we add the wedges to the edges of the face. These complete the angled side pieces 
  G4Trap* right_wedge = this->cornerWedge();

  G4RotationMatrix* rotate_right_wedge = new G4RotationMatrix;
  rotate_right_wedge->rotateX(M_PI/2.0);
  rotate_right_wedge->rotateY(-M_PI/2.0);

  G4ThreeVector move_right_wedge(this->shift +this->applied_back_shift, 0, 
	(this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) +(this->can_face_thickness/2.0)
	* tan(this->bent_end_angle)/2.0);  	

  right_wedge_log = new G4LogicalVolume(right_wedge, structureMaterial, "right_wedge_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(right_wedge_log, move_right_wedge, rotate_right_wedge);

  G4Trap* left_wedge = this->cornerWedge();

  G4RotationMatrix* rotate_left_wedge = new G4RotationMatrix;
  rotate_left_wedge->rotateX(M_PI/2.0);
  rotate_left_wedge->rotateY(-M_PI/2.0);
  rotate_left_wedge->rotateX(M_PI);

  G4ThreeVector move_left_wedge(this->shift +this->applied_back_shift, 0, 
	-((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) +(this->can_face_thickness/2.0)				  
	* tan(this->bent_end_angle)/2.0));  	

  left_wedge_log = new G4LogicalVolume(left_wedge, structureMaterial, "left_wedge_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(left_wedge_log, move_left_wedge, rotate_left_wedge);

  G4Trap* top_wedge = this->cornerWedge();

  G4RotationMatrix* rotate_top_wedge = new G4RotationMatrix;
  rotate_top_wedge->rotateX(M_PI/2.0);
  rotate_top_wedge->rotateY(-M_PI/2.0);
  rotate_top_wedge->rotateX(-M_PI/2.0);
  
  G4ThreeVector move_top_wedge(this->shift +this->applied_back_shift, 
  	(this->detector_total_width/2.0) -(this->bent_end_length *tan(this->bent_end_angle))  
	+ (this->can_face_thickness/2.0) *tan(this->bent_end_angle)/2.0, 0);  	

  top_wedge_log = new G4LogicalVolume(top_wedge, structureMaterial, "top_wedge_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(top_wedge_log, move_top_wedge, rotate_top_wedge);

  G4Trap* bottom_wedge = this->cornerWedge();

  G4RotationMatrix* rotate_bottom_wedge = new G4RotationMatrix;
  rotate_bottom_wedge->rotateX(M_PI/2.0);
  rotate_bottom_wedge->rotateY(-M_PI/2.0);
  rotate_bottom_wedge->rotateX(M_PI/2.0);  

  G4ThreeVector move_bottom_wedge(this->shift +this->applied_back_shift, 
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) +(this->can_face_thickness/2.0)				  
	* tan(this->bent_end_angle)/2.0), 0);  	

  bottom_wedge_log = new G4LogicalVolume(bottom_wedge, structureMaterial, "bottom_wedge_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(bottom_wedge_log, move_bottom_wedge, rotate_bottom_wedge);

  // now we add the rounded corners, made from a quarter of a cone
  
  // Correction factor used to move the corner cones so as to avoid 
  // holes left in the can caused by the bent side pieces
  G4double hole_eliminator = this->can_face_thickness *(1.0 -tan(this->bent_end_angle));	

  G4Cons* upper_right_cone = this->roundedEndEdge();

  G4RotationMatrix* rotate_upper_right_cone = new G4RotationMatrix;
  rotate_upper_right_cone->rotateY(M_PI/2.0);
  rotate_upper_right_cone->rotateX(M_PI/2.0);

  G4ThreeVector move_upper_right_cone(this->bent_end_length/2.0 +this->shift +this->applied_back_shift,
	(this->detector_total_width/2.0) -(this->bent_end_length 
	*tan(this->bent_end_angle)) -hole_eliminator, 
	(this->detector_total_width/2.0) -(this->bent_end_length 
	*tan(this->bent_end_angle)) - hole_eliminator);

  upper_right_cone_log = new G4LogicalVolume(upper_right_cone, structureMaterial, "upper_right_cone_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(upper_right_cone_log, move_upper_right_cone, rotate_upper_right_cone);

  G4Cons* lower_right_cone = this->roundedEndEdge();

  G4RotationMatrix* rotate_lower_right_cone = new G4RotationMatrix;
  rotate_lower_right_cone->rotateY(M_PI/2.0);
  rotate_lower_right_cone->rotateX(M_PI);

  G4ThreeVector move_lower_right_cone(this->bent_end_length/2.0 +this->shift +this->applied_back_shift,
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator),
	(this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator);

  lower_right_cone_log = new G4LogicalVolume(lower_right_cone, structureMaterial, "lower_right_cone_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(lower_right_cone_log, move_lower_right_cone, rotate_lower_right_cone);

  G4Cons* upper_left_cone = this->roundedEndEdge();

  G4RotationMatrix* rotate_upper_left_cone = new G4RotationMatrix;
  rotate_upper_left_cone->rotateY(M_PI/2.0);

  G4ThreeVector move_upper_left_cone(this->bent_end_length/2.0 +this->shift +this->applied_back_shift,
	(this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator,
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator));

  upper_left_cone_log = new G4LogicalVolume(upper_left_cone, structureMaterial, "upper_left_cone_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(upper_left_cone_log, move_upper_left_cone, rotate_upper_left_cone);

  G4Cons* lower_left_cone = this->roundedEndEdge();

  G4RotationMatrix* rotate_lower_left_cone = new G4RotationMatrix;
  rotate_lower_left_cone->rotateY(M_PI/2.0);
  rotate_lower_left_cone->rotateX(-M_PI/2.0);

  G4ThreeVector move_lower_left_cone(this->bent_end_length/2.0 +this->shift +this->applied_back_shift,
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator),
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)) -hole_eliminator));

  lower_left_cone_log = new G4LogicalVolume(lower_left_cone, structureMaterial, "lower_left_cone_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(lower_left_cone_log, move_lower_left_cone, rotate_lower_left_cone);

  // now we add the corner tubes which extend the rounded corners to the back of the can
  G4Tubs* upper_right_tube = this->cornerTube();

  G4RotationMatrix* rotate_upper_right_tube = new G4RotationMatrix;
  rotate_upper_right_tube->rotateY(M_PI/2.0);
  rotate_upper_right_tube->rotateX(M_PI/2.0);

  G4ThreeVector move_upper_right_tube((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift,
	(this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)), (this->detector_total_width/2.0)
	- (this->bent_end_length *tan(this->bent_end_angle)));

  upper_right_tube_log = new G4LogicalVolume(upper_right_tube, structureMaterial, "upper_right_tube_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(upper_right_tube_log, move_upper_right_tube, rotate_upper_right_tube);

  G4Tubs* lower_right_tube = this->cornerTube();

  G4RotationMatrix* rotate_lower_right_tube = new G4RotationMatrix;
  rotate_lower_right_tube->rotateY(M_PI/2.0);
  rotate_lower_right_tube->rotateX(M_PI);

  G4ThreeVector move_lower_right_tube((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift,
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle))), (this->detector_total_width/2.0)
	- (this->bent_end_length *tan(this->bent_end_angle)));

  lower_right_tube_log = new G4LogicalVolume(lower_right_tube, structureMaterial, "lower_right_tube_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(lower_right_tube_log, move_lower_right_tube, rotate_lower_right_tube);

  G4Tubs* upper_left_tube = this->cornerTube();

  G4RotationMatrix* rotate_upper_left_tube = new G4RotationMatrix;
  rotate_upper_left_tube->rotateY(M_PI/2.0);
  
  G4ThreeVector move_upper_left_tube((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift,
	(this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle)), -((this->detector_total_width/2.0)
	- (this->bent_end_length *tan(this->bent_end_angle))));

  upper_left_tube_log = new G4LogicalVolume(upper_left_tube, structureMaterial, "upper_left_tube_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(upper_left_tube_log, move_upper_left_tube, rotate_upper_left_tube);

  G4Tubs* lower_left_tube = this->cornerTube();

  G4RotationMatrix* rotate_lower_left_tube = new G4RotationMatrix;
  rotate_lower_left_tube->rotateY(M_PI/2.0);
  rotate_lower_left_tube->rotateX(-M_PI/2.0);

  G4ThreeVector move_lower_left_tube((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift,
	- ((this->detector_total_width/2.0) -(this->bent_end_length 
	* tan(this->bent_end_angle))), -((this->detector_total_width/2.0)
	- (this->bent_end_length *tan(this->bent_end_angle))));

  lower_left_tube_log = new G4LogicalVolume(lower_left_tube, structureMaterial, "lower_left_tube_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(lower_left_tube_log, move_lower_left_tube, rotate_lower_left_tube);

  // now we add the side panels that extend from the bent pieces to the back of the can
  G4Box* right_side_panel = this->sidePanel();

  G4RotationMatrix* rotate_right_side_panel = new G4RotationMatrix;
  rotate_right_side_panel->rotateX(M_PI/2.0);

  G4ThreeVector move_right_side_panel((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift, 0,
	(this->detector_total_width -this->can_side_thickness)/2.0);

  right_side_panel_log = new G4LogicalVolume(right_side_panel, structureMaterial, "right_side_panel_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(right_side_panel_log, move_right_side_panel, rotate_right_side_panel);

  G4Box* left_side_panel = this->sidePanel();

  G4RotationMatrix* rotate_left_side_panel = new G4RotationMatrix;
  rotate_left_side_panel->rotateX(-M_PI/2.0);

  G4ThreeVector move_left_side_panel((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift, 0,
	- (this->detector_total_width -this->can_side_thickness)/2.0);

  left_side_panel_log = new G4LogicalVolume(left_side_panel, structureMaterial, "left_side_panel_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(left_side_panel_log, move_left_side_panel, rotate_left_side_panel);

  G4Box* top_side_panel = this->sidePanel();

  G4ThreeVector move_top_side_panel((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift, (this->detector_total_width
	- this->can_side_thickness)/2.0, 0);

  top_side_panel_log = new G4LogicalVolume(top_side_panel, structureMaterial, "top_side_panel_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(top_side_panel_log, move_top_side_panel, this->rotate_null);

  G4Box* bottom_side_panel = this->sidePanel();

  G4RotationMatrix* rotate_bottom_side_panel = new G4RotationMatrix;
  rotate_bottom_side_panel->rotateX(-M_PI);

  G4ThreeVector move_bottom_side_panel((this->detector_total_length +this->bent_end_length 
	- this->rear_plate_thickness -this->can_face_thickness)/2.0 
	+ this->shift +this->applied_back_shift, -(this->detector_total_width
	- this->can_side_thickness)/2.0, 0);

  bottom_side_panel_log = new G4LogicalVolume(bottom_side_panel, structureMaterial, "bottom_side_panel_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(bottom_side_panel_log, move_bottom_side_panel, rotate_bottom_side_panel);

  // now we add the rear plate, which has a hole for the cold finger to pass through
  G4SubtractionSolid* rear_plate = this->rearPlate();

  G4RotationMatrix* rotate_rear_plate = new G4RotationMatrix;
  rotate_rear_plate->rotateY(M_PI/2.0);

  G4ThreeVector move_rear_plate(this->detector_total_length -this->can_face_thickness/2.0
	- this->rear_plate_thickness/2.0 +this->shift 
	+ this->applied_back_shift, 0, 0);

  rear_plate_log = new G4LogicalVolume(rear_plate, structureMaterial, "rear_plate_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(rear_plate_log, move_rear_plate, rotate_rear_plate);

  // we know add the cold finger shell, which extends out the back of the can
  G4Tubs* finger_shell = this->coldFingerShell();
  
  G4RotationMatrix* rotate_finger_shell = new G4RotationMatrix;
  rotate_finger_shell->rotateY(M_PI/2.0);

  G4ThreeVector move_finger_shell(this->cold_finger_shell_length/2.0 -this->can_face_thickness/2.0
	+ this->detector_total_length +this->shift 
	+ this->applied_back_shift, 0, 0);

  finger_shell_log = new G4LogicalVolume(finger_shell, structureMaterial, "finger_shell_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(finger_shell_log, move_finger_shell, rotate_finger_shell);

  // lastly we add the liquid nitrogen tank at the back, past the cold finger shell
  G4Tubs* tank = this->liquidNitrogenTank();

  G4RotationMatrix* rotate_tank = new G4RotationMatrix;
  rotate_tank->rotateY(M_PI/2.0);

  G4ThreeVector move_tank((this->coolant_length -this->can_face_thickness)/2.0 + this->detector_total_length    	
	+ this->cold_finger_shell_length + this->shift +this->applied_back_shift, 0, 0);

  tank_log = new G4LogicalVolume(tank, structureMaterial, "tank_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(tank_log, move_tank, rotate_tank);
 
}// end::ConstructDetector

///////////////////////////////////////////////////////////////////////
// ConstructColdFinger has changed in this version!  It now 
// incorporates all the internal details released in Nov 2004 by 
// Eurisys. ConstructColdFinger builds the cold finger as well as the 
// cold plate. The finger extends to the Liquid Nitrogen tank, while 
// the plate is always the same distance from the back of the germanium  
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructColdFinger()
{
  G4Material* materialAir = G4Material::GetMaterial("Air");
  if( !materialAir ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  G4Material* structureMaterial = G4Material::GetMaterial(this->structureMaterial);
  if( !structureMaterial ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  G4Material* electrodeMaterial = G4Material::GetMaterial(this->electrodeMaterial);
  if( !electrodeMaterial ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* cold_finger_vis_att = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
  cold_finger_vis_att->SetVisibility(true);

  G4VisAttributes* structureMat_vis_att = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  structureMat_vis_att->SetVisibility(true);

  G4VisAttributes* air_vis_att = new G4VisAttributes(G4Colour(0.8,0.8,0.8));
  air_vis_att->SetVisibility(true);

  G4Box* end_plate = this->endPlate();

  // cut out holes
  G4double air_hole_distance = this->germanium_separation/2.0 
	+ (this->germanium_width/2.0 - this->germanium_shift); //centre of the middle hole  
  
  G4RotationMatrix* rotate_fet_air_hole = new G4RotationMatrix;
  rotate_fet_air_hole->rotateY(M_PI/2.0);
  
  G4Tubs* fet_air_hole_cut = this->airHoleCut();   

  G4ThreeVector move_cut1(0 ,-air_hole_distance, air_hole_distance);
  G4ThreeVector move_cut2(0 ,air_hole_distance, air_hole_distance);
  G4ThreeVector move_cut3(0 ,air_hole_distance, -air_hole_distance);  
  G4ThreeVector move_cut4(0 ,-air_hole_distance, -air_hole_distance);   
  
  G4SubtractionSolid* end_plate_cut1 = new G4SubtractionSolid("end_plate_cut1", 
  	end_plate, fet_air_hole_cut, rotate_fet_air_hole, move_cut1);

  G4SubtractionSolid* end_plate_cut2 = new G4SubtractionSolid("end_plate_cut2", 
  	end_plate_cut1, fet_air_hole_cut, rotate_fet_air_hole, move_cut2);

  G4SubtractionSolid* end_plate_cut3 = new G4SubtractionSolid("end_plate_cut3", 
  	end_plate_cut2, fet_air_hole_cut, rotate_fet_air_hole, move_cut3);

  G4SubtractionSolid* end_plate_cut4 = new G4SubtractionSolid("end_plate_cut4", 
  	end_plate_cut3, fet_air_hole_cut, rotate_fet_air_hole, move_cut4);  	  	  
  	
  G4ThreeVector move_end_plate(this->cold_finger_end_plate_thickness/2.0 +this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 0, 0);

  end_plate_log = new G4LogicalVolume(end_plate_cut4, structureMaterial, "end_plate_log", 0, 0, 0);
  end_plate_log->SetVisAttributes(structureMat_vis_att);

  this->assembly->AddPlacedVolume(end_plate_log, move_end_plate, 0);

  // Place holes in the old cold plate
  G4Tubs* fet_air_hole = this->airHole();

  fet_air_hole_log = new G4LogicalVolume(fet_air_hole, materialAir, "fet_air_hole_log", 0, 0, 0);
  fet_air_hole_log->SetVisAttributes(air_vis_att);

  G4ThreeVector move_fet_air_hole1(this->cold_finger_end_plate_thickness/2.0 +this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift,-air_hole_distance, air_hole_distance);

  G4ThreeVector move_fet_air_hole2(this->cold_finger_end_plate_thickness/2.0 +this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, air_hole_distance, air_hole_distance);  

  G4ThreeVector move_fet_air_hole3(this->cold_finger_end_plate_thickness/2.0 +this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, air_hole_distance, -air_hole_distance);

  G4ThreeVector move_fet_air_hole4(this->cold_finger_end_plate_thickness/2.0 +this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, -air_hole_distance, -air_hole_distance);
   
  this->assembly->AddPlacedVolume(fet_air_hole_log, move_fet_air_hole1, rotate_fet_air_hole);
  this->assembly->AddPlacedVolume(fet_air_hole_log, move_fet_air_hole2, rotate_fet_air_hole);
  this->assembly->AddPlacedVolume(fet_air_hole_log, move_fet_air_hole3, rotate_fet_air_hole);
  this->assembly->AddPlacedVolume(fet_air_hole_log, move_fet_air_hole4, rotate_fet_air_hole);    
  
  // Cold Finger
  G4Tubs* finger = this->finger();

  G4RotationMatrix* rotate_finger = new G4RotationMatrix;
  rotate_finger->rotateY(M_PI/2.0);

  G4ThreeVector move_finger((this->cold_finger_length +this->can_face_thickness)/2.0
	+ this->germanium_dist_from_can_face +this->germanium_length
	+ this->cold_finger_space +this->cold_finger_end_plate_thickness
	+ this->shift +this->applied_back_shift
	+ this->structureMat_cold_finger_thickness/2.0 , 0, 0); //changed Jan 2005

  finger_log = new G4LogicalVolume(finger, electrodeMaterial, "finger_log", 0, 0, 0);
  finger_log->SetVisAttributes(cold_finger_vis_att);

  this->assembly->AddPlacedVolume(finger_log, move_finger, rotate_finger);  

  // The extra block of structureMat that's in the diagram "Cut C" 
  G4SubtractionSolid* extra_cold_block = this->extraColdBlock();
  G4RotationMatrix* rotate_extra_cold_block = new G4RotationMatrix;
  rotate_extra_cold_block->rotateY(M_PI/2.0);
  G4ThreeVector move_extra_cold_block(this->detector_total_length -this->can_face_thickness/2.0
	- this->rear_plate_thickness +this->shift 
	+ this->applied_back_shift -this->extra_block_distance_from_back_plate
        - this->extra_block_thickness/2.0, 0, 0);
  extra_cold_block_log = new G4LogicalVolume(extra_cold_block, structureMaterial, "extra_cold_block_log", 0, 0, 0);

  this->assembly->AddPlacedVolume(extra_cold_block_log, move_extra_cold_block, rotate_extra_cold_block);  

  // The structures above the cooling end plate
  G4Tubs* structureMat_cold_finger = this->structureMatColdFinger();
  structureMat_cold_finger_log = new G4LogicalVolume(structureMat_cold_finger, structureMaterial, "structureMat_cold_finger_log", 0, 0, 0);
  structureMat_cold_finger_log->SetVisAttributes(structureMat_vis_att);

  G4RotationMatrix* rotate_structureMat_cold_finger = new G4RotationMatrix;
  rotate_structureMat_cold_finger->rotateY(M_PI/2.0);
  G4ThreeVector move_structureMat_cold_finger(this->structureMat_cold_finger_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 0, 0);

  this->assembly->AddPlacedVolume(structureMat_cold_finger_log, move_structureMat_cold_finger, rotate_structureMat_cold_finger);  
 
  // Cooling Side Block 
  G4Box* cooling_side_block = this->coolingSideBlock();
  cooling_side_block_log = new G4LogicalVolume(cooling_side_block, structureMaterial, "cooling_side_block_log", 0, 0, 0);
  cooling_side_block_log->SetVisAttributes(structureMat_vis_att);

  G4RotationMatrix* rotate_cooling_side_block1 = new G4RotationMatrix;
  rotate_cooling_side_block1->rotateZ(M_PI/2.0);
  G4ThreeVector move_cooling_side_block1(this->cooling_side_block_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	0, this->germanium_width - this->cooling_side_block_horizontal_depth/2.0);
  
  this->assembly->AddPlacedVolume(cooling_side_block_log, move_cooling_side_block1, rotate_cooling_side_block1);    
  
  G4RotationMatrix* rotate_cooling_side_block2 = new G4RotationMatrix;
  rotate_cooling_side_block2->rotateZ(M_PI/2.0);
  rotate_cooling_side_block2->rotateX(-M_PI/2.0);
  G4ThreeVector move_cooling_side_block2(this->cooling_side_block_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	this->germanium_width - this->cooling_side_block_horizontal_depth/2.0, 0);

  this->assembly->AddPlacedVolume(cooling_side_block_log, move_cooling_side_block2, rotate_cooling_side_block2);    
  
  G4RotationMatrix* rotate_cooling_side_block3 = new G4RotationMatrix;
  rotate_cooling_side_block3->rotateZ(M_PI/2.0);
  rotate_cooling_side_block3->rotateX(-M_PI);
  G4ThreeVector move_cooling_side_block3(this->cooling_side_block_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	0, -(this->germanium_width - this->cooling_side_block_horizontal_depth/2.0));
	
  this->assembly->AddPlacedVolume(cooling_side_block_log, move_cooling_side_block3, rotate_cooling_side_block3);
  
  G4RotationMatrix* rotate_cooling_side_block4 = new G4RotationMatrix;
  rotate_cooling_side_block4->rotateZ(M_PI/2.0);
  rotate_cooling_side_block4->rotateX(M_PI/2.0);
  G4ThreeVector move_cooling_side_block4(this->cooling_side_block_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	- (this->germanium_width - this->cooling_side_block_horizontal_depth/2.0), 0);

  this->assembly->AddPlacedVolume(cooling_side_block_log, move_cooling_side_block4, rotate_cooling_side_block4);  
  
  // Cooling Side Bars
  G4Box* cooling_bar = this->coolingBar();
  cooling_bar_log = new G4LogicalVolume(cooling_bar, structureMaterial, "cooling_bar_log", 0, 0, 0);
  cooling_bar_log->SetVisAttributes(structureMat_vis_att);
   
  G4RotationMatrix* rotate_cooling_bar1 = new G4RotationMatrix;
  rotate_cooling_bar1->rotateZ(M_PI/2.0);
  G4ThreeVector move_cooling_bar1(this->cooling_bar_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	0, (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - this->structureMat_cold_finger_radius)/2.0));

  this->assembly->AddPlacedVolume(cooling_bar_log, move_cooling_bar1, rotate_cooling_bar1);  

  G4RotationMatrix* rotate_cooling_bar2 = new G4RotationMatrix;
  rotate_cooling_bar2->rotateZ(M_PI/2.0);  
  rotate_cooling_bar2->rotateX(-M_PI/2.0);
  G4ThreeVector move_cooling_bar2(this->cooling_bar_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	( this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - this->structureMat_cold_finger_radius)/2.0), 0);

  this->assembly->AddPlacedVolume(cooling_bar_log, move_cooling_bar2, rotate_cooling_bar2);  
  
  G4RotationMatrix* rotate_cooling_bar3 = new G4RotationMatrix;
  rotate_cooling_bar3->rotateZ(M_PI/2.0);  
  rotate_cooling_bar3->rotateX(-M_PI);
  G4ThreeVector move_cooling_bar3(this->cooling_bar_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	0, -(this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - this->structureMat_cold_finger_radius)/2.0));

  this->assembly->AddPlacedVolume(cooling_bar_log, move_cooling_bar3, rotate_cooling_bar3);  
  
  G4RotationMatrix* rotate_cooling_bar4 = new G4RotationMatrix;
  rotate_cooling_bar4->rotateZ(M_PI/2.0);  
  rotate_cooling_bar4->rotateX(M_PI/2.0);
  G4ThreeVector move_cooling_bar4(this->cooling_bar_thickness/2.0
	+ this->cold_finger_end_plate_thickness 
	+ this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift, 
	- (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - this->structureMat_cold_finger_radius)/2.0), 0);

  this->assembly->AddPlacedVolume(cooling_bar_log, move_cooling_bar4, rotate_cooling_bar4);     
   
  // Triangle posts from "Cut A"
  // First, find how far from the centre to place the tips of the triangles
  G4double distance_of_the_tip = this->germanium_separation/2.0 
        + (this->germanium_width/2.0 - this->germanium_shift) //centre of the middle hole
	+ sqrt( pow((this->germanium_outer_radius 
	+ this->triangle_posts_distance_from_crystals), 2.0)
	- pow(this->germanium_width/2.0 - this->germanium_shift, 2.0) );
  // The distance of the base from the detector centre
  G4double distance_of_the_base = this->germanium_separation/2.0
        + this->germanium_width;
  G4double triangle_post_length = this->germanium_length - this->triangle_post_starting_depth
        + this->cold_finger_space;

  G4Trd* triangle_post = this->trianglePost();
  triangle_post_log = new G4LogicalVolume(triangle_post, structureMaterial, "triangle_post_log", 0, 0, 0);
   
  G4RotationMatrix* rotate_triangle_post1 = new G4RotationMatrix;
  rotate_triangle_post1->rotateY(0);
  G4ThreeVector move_triangle_post1(this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift
	- triangle_post_length/2.0,
	0, (distance_of_the_base + distance_of_the_tip)/2.0); 

  this->assembly->AddPlacedVolume(triangle_post_log, move_triangle_post1, rotate_triangle_post1);  
   
  G4RotationMatrix* rotate_triangle_post2 = new G4RotationMatrix;
  rotate_triangle_post2->rotateY(0);
  rotate_triangle_post2->rotateX(-M_PI/2.0);
  G4ThreeVector move_triangle_post2(this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift
	- triangle_post_length/2.0,
	(distance_of_the_base + distance_of_the_tip)/2.0, 0);  

  this->assembly->AddPlacedVolume(triangle_post_log, move_triangle_post2, rotate_triangle_post2);  
   
  G4RotationMatrix* rotate_triangle_post3 = new G4RotationMatrix;
  rotate_triangle_post3->rotateY(0);
  rotate_triangle_post3->rotateX(-M_PI);
  G4ThreeVector move_triangle_post3(this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift
	- triangle_post_length/2.0,
	0, -(distance_of_the_base + distance_of_the_tip)/2.0); 

  this->assembly->AddPlacedVolume(triangle_post_log, move_triangle_post3, rotate_triangle_post3);  
   
  G4RotationMatrix* rotate_triangle_post4 = new G4RotationMatrix;
  rotate_triangle_post4->rotateY(0);
  rotate_triangle_post4->rotateX(M_PI/2.0);
  G4ThreeVector move_triangle_post4(this->can_face_thickness/2.0 
	+ this->germanium_dist_from_can_face +this->germanium_length 
	+ this->cold_finger_space +this->shift +this->applied_back_shift
	- triangle_post_length/2.0,
	- (distance_of_the_base + distance_of_the_tip)/2.0, 0); 

  this->assembly->AddPlacedVolume(triangle_post_log, move_triangle_post4, rotate_triangle_post4);  
   
}//end ::ConstructColdFinger

///////////////////////////////////////////////////////////////////////
// methods used in ConstructColdFinger()
///////////////////////////////////////////////////////////////////////
G4Box* DetectionSystemGriffin::endPlate()
{
   G4double half_thickness_x = this->cold_finger_end_plate_thickness/2.0;
   G4double half_length_y = this->germanium_width;  //Since it must cover the back of the detector
   G4double half_length_z = half_length_y;  //Since it is symmetric

   G4Box* end_plate = new G4Box("end_plate", half_thickness_x, half_length_y, half_length_z);

   return end_plate;
}//end ::endPlate


G4Tubs* DetectionSystemGriffin::finger()
{
   G4double inner_radius = 0.0*cm;
   G4double outer_radius = this->cold_finger_radius;
   G4double half_length_z = (this->cold_finger_length 
        - this->structureMat_cold_finger_thickness)/2.0;
   G4double start_angle = 0.0*M_PI;
   G4double final_angle = 2.0*M_PI;

   G4Tubs* finger = new G4Tubs("finger", inner_radius, outer_radius, half_length_z, start_angle, final_angle);
 
   return finger;
}//end ::finger


G4SubtractionSolid* DetectionSystemGriffin::extraColdBlock()
{
   G4double half_width_x = this->germanium_width;
   G4double half_width_y = half_width_x;  
   G4double half_thickness_z = this->extra_block_thickness/2.0;
  
   G4Box* plate = new G4Box("plate", half_width_x, half_width_y, half_thickness_z);

   G4double inner_radius = 0.0;  
   G4double outer_radius = this->extra_block_inner_diameter/2.0;  
   G4double half_height_z = half_thickness_z +1.0*cm;  //+1.0*cm just to make sure the hole goes completely through the plate
   G4double start_angle = 0.0*M_PI;
   G4double final_angle = 2.0*M_PI;

   G4Tubs* hole = new G4Tubs("hole", inner_radius, outer_radius, half_height_z, start_angle, final_angle);     

   G4SubtractionSolid* extra_cold_block = new G4SubtractionSolid("extra_cold_block", plate, hole);

   return extra_cold_block;
}//end ::extraColdBlock


G4Trd* DetectionSystemGriffin::trianglePost()
{
  // Calculations that are also done in ConstructColdFinger for positioning
  // First, find how far from the centre to place the tips of the triangles
  G4double distance_of_the_tip = this->germanium_separation/2.0 
	+ (this->germanium_width/2.0 - this->germanium_shift) //centre of the middle hole
	+ sqrt( pow((this->germanium_outer_radius 
	+ this->triangle_posts_distance_from_crystals), 2.0)
	- pow(this->germanium_width/2.0 - this->germanium_shift, 2.0) );
  // The distance of the base from the detector centre
  G4double distance_of_the_base = this->germanium_separation/2.0
        + this->germanium_width;
  // The distance away from the boundary between crystals of the side points
  G4double distance_of_the_side_points = sqrt( pow((this->germanium_outer_radius 
	+ this->triangle_posts_distance_from_crystals), 2.0)
	- pow(this->germanium_width/2.0 +/*notice*/ this->germanium_shift, 2.0) );

  // Measurements to make the posts with
  G4double length = this->germanium_length - this->triangle_post_starting_depth
        + this->cold_finger_space;
  G4double base_to_tip_height = (distance_of_the_base-distance_of_the_tip);
  G4double half_width_of_base = distance_of_the_side_points;

  G4double half_width_of_top = this->trianglePostDim; //the easiest way to make a triangle
  //G4Trd( const G4String& pName,G4double  dx1, G4double dx2, G4double  dy1, G4double dy2,G4double  dz )
  G4Trd* triangle_post = new G4Trd("triangle_post", length/2.0, length/2.0, 
        half_width_of_top, half_width_of_base, base_to_tip_height/2.0);
	
  return triangle_post;
}//end ::trianglePost


G4Tubs* DetectionSystemGriffin::airHole()
{
   G4double inner_radius = 0.0*cm;
   G4double outer_radius = this->fet_air_hole_radius;
   G4double half_length_z = this->cold_finger_end_plate_thickness/2.0;
   G4double start_angle = 0.0*M_PI;
   G4double final_angle = 2.0*M_PI;

   G4Tubs* fet_air_hole = new G4Tubs("fet_air_hole", inner_radius, 
        outer_radius, half_length_z, start_angle, final_angle);
 
   return fet_air_hole;
}//end ::airHole

G4Tubs* DetectionSystemGriffin::airHoleCut()
{
   G4double inner_radius = 0.0*cm;
   G4double outer_radius = this->fet_air_hole_radius;
   G4double half_length_z = this->cold_finger_end_plate_thickness/2.0 + 1.0*cm;
   G4double start_angle = 0.0*M_PI;
   G4double final_angle = 2.0*M_PI;

   G4Tubs* fet_air_hole = new G4Tubs("fet_air_hole", inner_radius, 
        outer_radius, half_length_z, start_angle, final_angle);
 
   return fet_air_hole;
}//end ::airHoleCut

G4Box* DetectionSystemGriffin::coolingBar()
{
   G4double half_thickness_x = this->cooling_bar_thickness/2.0;
   G4double half_length_y = this->cooling_bar_width/2.0;
   G4double half_length_z = (this->germanium_width 
        - this->cooling_side_block_horizontal_depth
        - this->structureMat_cold_finger_radius)/2.0;

   G4Box* cooling_bar = new G4Box("cooling_bar", half_thickness_x, half_length_y, half_length_z);

   return cooling_bar;
}//end ::coolingBar


G4Box* DetectionSystemGriffin::coolingSideBlock()
{
   G4double half_thickness_x = this->cooling_side_block_width/2.0;
   G4double half_length_y = this->cooling_side_block_thickness/2.0;
   G4double half_length_z = this->cooling_side_block_horizontal_depth/2.0;

   G4Box* cooling_side_block = new G4Box("cooling_side_block", 
        half_thickness_x, half_length_y, half_length_z);

   return cooling_side_block;
}//end ::coolingSideBlock


G4Tubs* DetectionSystemGriffin::structureMatColdFinger()
{
   G4double inner_radius = 0.0*cm;
   G4double outer_radius = this->structureMat_cold_finger_radius;
   G4double half_length_z = this->structureMat_cold_finger_thickness/2.0;
   G4double start_angle = 0.0*M_PI;
   G4double final_angle = 2.0*M_PI;

   G4Tubs* structureMat_cold_finger= new G4Tubs("structureMat_cold_finger", 
        inner_radius, outer_radius, half_length_z, start_angle, final_angle);
 
   return structureMat_cold_finger;
}//end ::structureMatColdFinger


///////////////////////////////////////////////////////////////////////
// ConstructNewSuppressorCasingWithShells builds the suppressor that 
// surrounds the can. It tapers off at the front, and there is a thick 
// piece covering the back of the can, which is divided in the middle,
// as per the design in the specifications. Also, there is a layer
// of structureMat that surrounds the physical pieces.
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructNewSuppressorCasingWithShells()
{
  G4Material* structureMaterial = G4Material::GetMaterial(this->structureMaterial);
  if( !structureMaterial ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  G4Material* materialBGO = G4Material::GetMaterial(this->BGO_material);
  if( !materialBGO ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }
  
  // Change some values to accomodate the shells
  // Replacement for this->side_suppressor_length
  G4double shell_side_suppressor_length = this->side_suppressor_length
	+ (this->suppressor_shell_thickness*2.0);

  // Replacement for this->suppressor_extension_length
  G4double shell_suppressor_extension_length = this->suppressor_extension_length
        + (this->suppressor_shell_thickness*2.0)*(1.0/tan(this->bent_end_angle)
	- tan(this->bent_end_angle));

  // Replacement for this->suppressor_extension_angle: must totally recalculate
  G4double shell_suppressor_extension_angle = atan(((this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation 
        + this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
        / tan(this->bent_end_angle)
        - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
        * sin(this->bent_end_angle))
        * tan(this->bent_end_angle) -(this->forward_inner_radius +this->hevimet_tip_thickness)
        * sin(this->bent_end_angle))/(shell_suppressor_extension_length));

  G4VisAttributes* Suppressor_vis_att = new G4VisAttributes(G4Colour(0.75,0.75,0.75));
  Suppressor_vis_att->SetVisibility(true);
  G4VisAttributes* innards_vis_att = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75));
  innards_vis_att->SetVisibility(true);
  G4VisAttributes* back_innards_vis_att = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  back_innards_vis_att->SetVisibility(true);
   
  // first we add the four pieces of back suppressor, and their shells

  G4SubtractionSolid* back_quarter_suppressor = this->backSuppressorQuarter();
   
  G4SubtractionSolid* shell_for_back_quarter_suppressor = this->shellForBackSuppressorQuarter();

  // Insert the structureMat shell first.  The shells must be given numbers, rather than
  // the suppressor pieces themselves.  As an error checking method, the suppressor
  // pieces are given a copy number value far out of range of any useful copy number.
  if(include_back_suppressors) {
  shell_for_back_quarter_suppressor_log = new G4LogicalVolume(
  	shell_for_back_quarter_suppressor, structureMaterial, 
        "back_quarter_suppressor_log", 0, 0, 0);
   shell_for_back_quarter_suppressor_log->SetVisAttributes(Suppressor_vis_att);
   
  G4Material* back_material = G4Material::GetMaterial(this->back_suppressor_material);
  if( !back_material ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

   back_quarter_suppressor_log = new G4LogicalVolume(back_quarter_suppressor, back_material, 
        "back_quarter_suppressor_log", 0, 0, 0);
   back_quarter_suppressor_log->SetVisAttributes(back_innards_vis_att);

  this->suppressorBackAssembly->AddPlacedVolume(back_quarter_suppressor_log, this->move_null, this->rotate_null);

  G4RotationMatrix* rotate_back_quarter_suppressor1 = new G4RotationMatrix;
  rotate_back_quarter_suppressor1->rotateX(-M_PI/2.0);
  G4ThreeVector move_back_quarter_suppressor1((this->back_BGO_thickness -this->can_face_thickness)/2.0
    + this->suppressor_shell_thickness
        + this->detector_total_length +this->BGO_can_seperation +this->shift
    + this->applied_back_shift, -this->detector_total_width/4.0,
        - (this->detector_total_width/4.0) );
    
  this->suppressorShellAssembly->AddPlacedVolume(shell_for_back_quarter_suppressor_log, move_back_quarter_suppressor1, rotate_back_quarter_suppressor1);

  G4RotationMatrix* rotate_back_quarter_suppressor2 = new G4RotationMatrix;
  rotate_back_quarter_suppressor2->rotateX(M_PI);
  G4ThreeVector move_back_quarter_suppressor2((this->back_BGO_thickness -this->can_face_thickness)/2.0
    + this->suppressor_shell_thickness
        + this->detector_total_length +this->BGO_can_seperation +this->shift
        + this->applied_back_shift, -this->detector_total_width/4.0,
        (this->detector_total_width/4.0) );

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_back_quarter_suppressor_log, move_back_quarter_suppressor2, rotate_back_quarter_suppressor2);

  G4RotationMatrix* rotate_back_quarter_suppressor3 = new G4RotationMatrix;
  rotate_back_quarter_suppressor3->rotateX(M_PI/2.0);
  G4ThreeVector move_back_quarter_suppressor3((this->back_BGO_thickness
    - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
        + this->detector_total_length +this->BGO_can_seperation +this->shift
        + this->applied_back_shift, this->detector_total_width/4.0 ,
        (this->detector_total_width/4.0 ) );
        
  this->suppressorShellAssembly->AddPlacedVolume(shell_for_back_quarter_suppressor_log, move_back_quarter_suppressor3, rotate_back_quarter_suppressor3);

  G4ThreeVector move_back_quarter_suppressor4((this->back_BGO_thickness
    - this->can_face_thickness)/2.0 + this->suppressor_shell_thickness
    + this->detector_total_length +this->BGO_can_seperation
    + this->shift
    + this->applied_back_shift, this->detector_total_width/4.0,
    - (this->detector_total_width/4.0) );

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_back_quarter_suppressor_log, move_back_quarter_suppressor4, this->rotate_null);
  }

  // now we add the side pieces of suppressor that taper off towards the front of the can

  // Define the structureMat shell logical volume
  G4SubtractionSolid* shell_for_right_suppressor = this->shellForFrontRightSlantSuppressor();

  shell_for_right_suppressor_log = new G4LogicalVolume(shell_for_right_suppressor, structureMaterial,
	"shell_for_right_suppressor_log", 0,0,0);
  shell_for_right_suppressor_log->SetVisAttributes(Suppressor_vis_att);

  G4SubtractionSolid* shell_for_left_suppressor = this->shellForFrontLeftSlantSuppressor();

  shell_for_left_suppressor_log = new G4LogicalVolume(shell_for_left_suppressor, structureMaterial,
	"shell_for_left_suppressor_log", 0,0,0);
  shell_for_left_suppressor_log->SetVisAttributes(Suppressor_vis_att);
  
  G4SubtractionSolid* right_suppressor = this->frontRightSlantSuppressor();

  right_suppressor_log = new G4LogicalVolume(right_suppressor, materialBGO, "right_suppressor_log", 0, 0, 0);
  right_suppressor_log->SetVisAttributes(innards_vis_att);

  G4SubtractionSolid* left_suppressor = this->frontLeftSlantSuppressor();

  left_suppressor_log = new G4LogicalVolume(left_suppressor, materialBGO, "left_suppressor_log", 0, 0, 0);
  left_suppressor_log->SetVisAttributes(innards_vis_att);
  
  this->rightSuppressorCasingAssembly->AddPlacedVolume(right_suppressor_log, this->move_null, this->rotate_null);
  this->leftSuppressorCasingAssembly->AddPlacedVolume(left_suppressor_log, this->move_null, this->rotate_null);
  
  /////////////////////////////////////////////////////////////////////
  // Note : Left and Right are read from the BACK of the detector
  // Suppressors 1 and 2 cover germanium 1
  // Suppressors 3 and 4 cover germanium 2
  // Suppressors 5 and 6 cover germanium 3
  // Suppressors 7 and 8 cover germanium 4
  /////////////////////////////////////////////////////////////////////  
  G4RotationMatrix* rotate_suppressor1 = new G4RotationMatrix;
  rotate_suppressor1->rotateZ(M_PI/2.0);
  rotate_suppressor1->rotateY(M_PI/2.0);
  rotate_suppressor1->rotateX(-M_PI/2.0);
  
  G4ThreeVector move_suppressor1(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
        + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->side_BGO_thickness/2.0
        + this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation), 
	- (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_log, move_suppressor1, rotate_suppressor1);
  
  G4RotationMatrix* rotate_suppressor2 = new G4RotationMatrix;
  rotate_suppressor2->rotateY(-M_PI/2.0);
  rotate_suppressor2->rotateX(M_PI/2.0);
    
  G4ThreeVector move_suppressor2(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
        + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
        + this->applied_back_shift, -(this->detector_total_width/2.0 +this->BGO_can_seperation 
        + this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0, 
	- (this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation));

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_log, move_suppressor2, rotate_suppressor2);
    
  G4RotationMatrix* rotate_suppressor3 = new G4RotationMatrix;
  rotate_suppressor3->rotateZ(M_PI/2.0);
  rotate_suppressor3->rotateY(M_PI/2.0);
  rotate_suppressor3->rotateX(M_PI);
  
  G4ThreeVector move_suppressor3(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
        + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
        + this->applied_back_shift, -(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0, 
	this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_log, move_suppressor3, rotate_suppressor3);

  G4RotationMatrix* rotate_suppressor4 = new G4RotationMatrix;
  rotate_suppressor4->rotateY(-M_PI/2.0);
  
  G4ThreeVector move_suppressor4(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->side_BGO_thickness/2.0
        + this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation), 
	(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_log, move_suppressor4, rotate_suppressor4);

  G4RotationMatrix* rotate_suppressor5 = new G4RotationMatrix;
  rotate_suppressor5->rotateZ(M_PI/2.0);
  rotate_suppressor5->rotateY(M_PI/2.0);
  rotate_suppressor5->rotateX(M_PI/2.0);
  
  G4ThreeVector move_suppressor5(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, this->side_BGO_thickness/2.0
	+ this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation, 
	(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_log, move_suppressor5, rotate_suppressor5);
  
  G4RotationMatrix* rotate_suppressor6 = new G4RotationMatrix;
  rotate_suppressor6->rotateY(-M_PI/2.0);
  rotate_suppressor6->rotateX(-M_PI/2.0);
  
  G4ThreeVector move_suppressor6(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0, 
	this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_log, move_suppressor6, rotate_suppressor6);

  G4RotationMatrix* rotate_suppressor7 = new G4RotationMatrix;
  rotate_suppressor7->rotateZ(M_PI/2.0);
  rotate_suppressor7->rotateY(M_PI/2.0);
  
  G4ThreeVector move_suppressor7(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
        + this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
        + this->applied_back_shift, (this->detector_total_width/2.0 
        + this->BGO_can_seperation 
        + this->side_BGO_thickness/2.0
	+ this->suppressor_shell_thickness)/2.0,
	- (this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness 
        + this->detector_total_width/2.0
	+ this->BGO_can_seperation));

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_log, move_suppressor7, rotate_suppressor7);
    
  G4RotationMatrix* rotate_suppressor8 = new G4RotationMatrix;
  rotate_suppressor8->rotateY(-M_PI/2.0);
  rotate_suppressor8->rotateX(M_PI);
  
  G4ThreeVector move_suppressor8(shell_side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, this->side_BGO_thickness/2.0
	+ this->suppressor_shell_thickness
	+ this->detector_total_width/2.0 +this->BGO_can_seperation, 
	- (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness)/2.0);

  this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_log, move_suppressor8, rotate_suppressor8);

  // now we add the side pieces of suppressor that extend out in front of the can when it's in the back position

  // Define the shell right logical volume
  G4SubtractionSolid* shell_for_right_suppressor_extension = this->shellForRightSuppressorExtension();
  
  shell_for_right_suppressor_extension_log = new G4LogicalVolume(shell_for_right_suppressor_extension, 
	materialBGO, "shell_for_right_suppressor_extension_log", 0, 0, 0);
  shell_for_right_suppressor_extension_log->SetVisAttributes(Suppressor_vis_att);
  
  G4SubtractionSolid* right_suppressor_extension = this->rightSuppressorExtension();
  
  right_suppressor_extension_log = new G4LogicalVolume(right_suppressor_extension, materialBGO, 
	"right_suppressor_extension_log", 0, 0, 0);
  right_suppressor_extension_log->SetVisAttributes(innards_vis_att);

  // Define the left shell logical volume
  G4SubtractionSolid* shell_for_left_suppressor_extension = this->shellForLeftSuppressorExtension();
  
  shell_for_left_suppressor_extension_log = new G4LogicalVolume(shell_for_left_suppressor_extension,
	materialBGO, "shell_for_left_suppressor_extension_log", 0, 0, 0);
  shell_for_left_suppressor_extension_log->SetVisAttributes(Suppressor_vis_att);
  
  G4SubtractionSolid* left_suppressor_extension = this->leftSuppressorExtension();
  
  left_suppressor_extension_log = new G4LogicalVolume(left_suppressor_extension, materialBGO, 
	"left_suppressor_extension_log", 0, 0, 0);
  left_suppressor_extension_log->SetVisAttributes(innards_vis_att);
  
  this->rightSuppressorExtensionAssembly->AddPlacedVolume(right_suppressor_extension_log, this->move_null, this->rotate_null);
  this->leftSuppressorExtensionAssembly->AddPlacedVolume(left_suppressor_extension_log, this->move_null, this->rotate_null);

  if(this->applied_back_shift == 0.0)		// If the detectors are forward, put the extensions in the back position
  {
    // these two parameters are for shifting the extensions back and out when in their BACK position
    G4double extension_back_shift = this->air_box_front_length
	- (this->hevimet_tip_thickness +shell_suppressor_extension_length
	+ (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle))
	* cos(this->bent_end_angle);
    G4double extension_radial_shift = extension_back_shift*tan(this->bent_end_angle);

    // the placement of the extensions matches the placement of the side_suppressor pieces
    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(M_PI/2.0);
    rotate_extension1->rotateY(this->bent_end_angle);
    rotate_extension1->rotateX(M_PI);
    
    G4ThreeVector move_extension1(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	- (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift);    
    
    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension1, rotate_extension1);
      
    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(M_PI/2.0);
    rotate_extension2->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension2->rotateX(M_PI); 

    G4ThreeVector move_extension2(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	(this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
  	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0) *sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift, (shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);
	
    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension2, rotate_extension2);

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateZ(M_PI/2.0);
    rotate_extension3->rotateY(this->bent_end_angle);
    rotate_extension3->rotateX(M_PI/2.0);
    
    G4ThreeVector move_extension3(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
        / cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
        + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness 
	+ this->suppressor_shell_thickness*2.0)*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift, (shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension3, rotate_extension3);

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateY(M_PI/2.0);
    rotate_extension4->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension4->rotateX(M_PI/2.0);        

    G4ThreeVector move_extension4(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0
	+ (this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift);  

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension4, rotate_extension4);

    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateY(this->bent_end_angle);
    
    G4ThreeVector move_extension5(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	-(this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius 
	+ extension_back_shift, 
	(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0
	+ this->suppressor_shell_thickness)/cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension5, rotate_extension5);

    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateZ(M_PI/2.0 + this->bent_end_angle);    

    G4ThreeVector move_extension6(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
        / cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
        + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift, -(shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension6, rotate_extension6);
         
    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateZ(M_PI/2.0);
    rotate_extension7->rotateY(this->bent_end_angle);
    rotate_extension7->rotateX(-M_PI/2.0);

    G4ThreeVector move_extension7(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)	   
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius +extension_back_shift, 
	(this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
  	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0) *sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift, -(shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension7, rotate_extension7);
    
    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI/2.0);
    rotate_extension8->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension8->rotateX(-M_PI/2.0); 
    
    G4ThreeVector move_extension8(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	-(this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius 
	+ extension_back_shift, 
	- (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0
	+ this->suppressor_shell_thickness)/cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension8, rotate_extension8);
    
  }//end if(detectors forward) statement

  // Otherwise, put them forward
  else if(this->applied_back_shift == this->back_inner_radius - this->forward_inner_radius)
  {
    // the placement of the extensions matches the placement of the side_suppressor pieces
    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(M_PI/2.0);
    rotate_extension1->rotateY(this->bent_end_angle);
    rotate_extension1->rotateX(M_PI);  
    
    G4ThreeVector move_extension1(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	- (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle)
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)));	

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension1, rotate_extension1);
    
    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(M_PI/2.0);
    rotate_extension2->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension2->rotateX(M_PI);

    G4ThreeVector move_extension2(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	(this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle), (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);
	
    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension2, rotate_extension2);

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateZ(M_PI/2.0);
    rotate_extension3->rotateY(this->bent_end_angle);
    rotate_extension3->rotateX(M_PI/2.0);    
    
    G4ThreeVector move_extension3(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
        + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)), (shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension3, rotate_extension3);

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateY(M_PI/2.0);
    rotate_extension4->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension4->rotateX(M_PI/2.0);
    
    G4ThreeVector move_extension4(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)));

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension4, rotate_extension4);
 
    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateY(this->bent_end_angle);
    
    G4ThreeVector move_extension5(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0
    - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
    * tan(this->bent_end_angle)/2.0)
    * cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation
    + this->side_BGO_thickness
    + this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle)
    - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
    * sin(this->bent_end_angle) +this->shift +this->back_inner_radius
    - this->forward_inner_radius,
    (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius
    + this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0,
    ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
    / cos(this->bent_end_angle)
    + (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
    + this->suppressor_shell_thickness*2.0)
    * tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius
    + this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
    + this->suppressor_shell_thickness*2.0)
    / tan(this->bent_end_angle) -(this->suppressor_extension_thickness
    + this->suppressor_shell_thickness*2.0)
    * sin(this->bent_end_angle))
    * tan(this->bent_end_angle)));


    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension5, rotate_extension5);
    
    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    
    G4ThreeVector move_extension6(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle)
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	- ((this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle)), -(shell_suppressor_extension_length
	* tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension6, rotate_extension6);

    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateZ(M_PI/2.0);
    rotate_extension7->rotateY(this->bent_end_angle);
    rotate_extension7->rotateX(-M_PI/2.0);  

    G4ThreeVector move_extension7(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	(this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
        + this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle), -(shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_right_suppressor_extension_log, move_extension7, rotate_extension7);
    
    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI/2.0);
    rotate_extension8->rotateZ(M_PI/2.0 + this->bent_end_angle);  
    rotate_extension8->rotateX(-M_PI/2.0);

    G4ThreeVector move_extension8(-this->can_face_thickness/2.0 -(shell_suppressor_extension_length/2.0 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)/tan(this->bent_end_angle) 
	- (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius 
	- this->forward_inner_radius, 
	- (shell_suppressor_extension_length*tan(shell_suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0 + this->suppressor_shell_thickness)
	/ cos(this->bent_end_angle) 
	+ (shell_suppressor_extension_length/2.0 -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness
	+ this->suppressor_shell_thickness*2.0)
	/ tan(this->bent_end_angle) -(this->suppressor_extension_thickness
	+ this->suppressor_shell_thickness*2.0)
	* sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    this->suppressorShellAssembly->AddPlacedVolume(shell_for_left_suppressor_extension_log, move_extension8, rotate_extension8);

  }//end if(detectors back) statement

}//end ::ConstructNewSuppressorCasingWithShells


///////////////////////////////////////////////////////////////////////
//methods used in ConstructNewSuppressorCasing
///////////////////////////////////////////////////////////////////////
G4SubtractionSolid* DetectionSystemGriffin::shellForBackSuppressorQuarter()
{
  //G4double half_thickness_x = this->back_BGO_thickness/2.0 + this->suppressor_shell_thickness;
  //G4double half_length_y = this->detector_total_width/4.0 + this->suppressor_shell_thickness;
  G4double half_thickness_x = this->back_BGO_thickness/2.0 + this->suppressor_shell_thickness;
  G4double half_length_y = this->detector_total_width/4.0;
  G4double half_length_z = half_length_y;
  G4Box* quarter_suppressor_shell = new G4Box("quarter_suppressor_shell", half_thickness_x, half_length_y, half_length_z);

  G4double shell_hole_radius = this->cold_finger_outer_shell_radius;
  G4Tubs* shell_hole = new G4Tubs("shell_hole", 0, shell_hole_radius, half_thickness_x +1.0*cm, 0.0*M_PI, 2.0*M_PI);

  G4RotationMatrix* rotate_shell_hole = new G4RotationMatrix;
  rotate_shell_hole->rotateY(M_PI/2.0);
  G4ThreeVector move_shell_hole(0, -half_length_y, half_length_z);

  G4SubtractionSolid* quarter_suppressor_shell_with_hole = new G4SubtractionSolid("quarter_suppressor_shell_with_hole",
    quarter_suppressor_shell, shell_hole, rotate_shell_hole, move_shell_hole);

  //now we need to cut out inner cavity, first we define the cavity
  half_thickness_x = this->back_BGO_thickness/2.0;
  half_length_y = this->detector_total_width/4.0 - this->suppressor_shell_thickness;
  half_length_z = half_length_y;
  G4Box* quarter_suppressor = new G4Box("quarter_suppressor", half_thickness_x, half_length_y, half_length_z);

  G4ThreeVector move_cut(0,0,0);

  // cut
  G4SubtractionSolid* quarter_suppressor_shell_with_hole_and_cavity = new G4SubtractionSolid("quarter_suppressor_shell_with_hole_and_cavity",
    quarter_suppressor_shell_with_hole, quarter_suppressor, 0, move_cut);

  return quarter_suppressor_shell_with_hole_and_cavity;

}//end ::shellForBackSuppressorQuarter


G4SubtractionSolid* DetectionSystemGriffin::shellForFrontRightSlantSuppressor()
{
  // Change some values to accomodate the shells
  // Replacement for this->side_suppressor_length
  G4double shell_side_suppressor_shell_length = this->side_suppressor_length + (this->suppressor_shell_thickness*2.0);

  G4double length_z 		= shell_side_suppressor_shell_length;
  G4double length_y 		= this->side_BGO_thickness + this->suppressor_shell_thickness*2.0;
  G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation
                + this->side_BGO_thickness + this->suppressor_shell_thickness*2.0;
  G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness
                    - this->suppressor_shell_thickness*2.0;

  G4Trap* suppressor_shell = new G4Trap("suppressor_shell", length_z, length_y, length_longer_x, length_shorter_x);

  G4double half_length_x 	= length_longer_x/2.0 +1.0*cm;
  G4double half_thickness_y 	= this->side_BGO_thickness/2.0
         			+ this->suppressor_shell_thickness;
  G4double half_length_z 	= ((this->side_BGO_thickness + this->suppressor_shell_thickness*2.0
         			- this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

  G4Box* chopping_shell_box = new G4Box("chopping_shell_box", half_length_x, half_thickness_y, half_length_z);

  G4RotationMatrix* rotate_chopping_shell_box = new G4RotationMatrix;
  rotate_chopping_shell_box->rotateX(-this->bent_end_angle);
  G4ThreeVector move_chopping_shell_box(0, this->side_BGO_thickness/2.0 - this->suppressor_shell_thickness*sin(this->bent_end_angle)
	- this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
	+ pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
	- length_z/2.0 +0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0
	- this->bent_end_angle -atan(half_thickness_y/half_length_z)));

  G4SubtractionSolid* side_suppressor_shell = new G4SubtractionSolid("side_suppressor_shell", 
  	suppressor_shell, chopping_shell_box, rotate_chopping_shell_box, move_chopping_shell_box);
  
  // cut out cavity  
  G4ThreeVector move_cut(-(this->suppressor_shell_thickness + ( this->extra_cut_length/2.0 - this->suppressor_shell_thickness)/2.0), 0, -(this->suppressor_shell_thickness/2.0) );
  	
  G4SubtractionSolid* side_suppressor_shell_with_cavity = new G4SubtractionSolid("side_suppressor_shell_with_cavity", 
    side_suppressor_shell, this->choppingFrontRightSlantSuppressor(), 0, move_cut);

  return side_suppressor_shell_with_cavity;

}//end ::shellForFrontRightSlantSuppressor

G4SubtractionSolid* DetectionSystemGriffin::shellForFrontLeftSlantSuppressor()
{
  // Change some values to accomodate the shells
  // Replacement for this->side_suppressor_length
  G4double shell_side_suppressor_shell_length = this->side_suppressor_length + (this->suppressor_shell_thickness*2.0);

  G4double length_z 		= shell_side_suppressor_shell_length;
  G4double length_y 		= this->side_BGO_thickness + this->suppressor_shell_thickness*2.0;
  G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation
                + this->side_BGO_thickness + this->suppressor_shell_thickness*2.0;
  G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness
                    - this->suppressor_shell_thickness*2.0;

  G4Trap* suppressor_shell = new G4Trap("suppressor_shell", length_z, length_y, length_longer_x, length_shorter_x);

  G4double half_length_x 	= length_longer_x/2.0 +1.0*cm;
  G4double half_thickness_y 	= this->side_BGO_thickness/2.0 + this->suppressor_shell_thickness;
  G4double half_length_z	= ((this->side_BGO_thickness + this->suppressor_shell_thickness*2.0
         			- this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

  G4Box* chopping_shell_box = new G4Box("chopping_shell_box", half_length_x, half_thickness_y, half_length_z);

  G4RotationMatrix* rotate_chopping_shell_box = new G4RotationMatrix;
  rotate_chopping_shell_box->rotateX(this->bent_end_angle);
  G4ThreeVector move_chopping_shell_box(0, this->side_BGO_thickness/2.0 - this->suppressor_shell_thickness*sin(this->bent_end_angle)
	- this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
	+ pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
	length_z/2.0 -0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0
	- this->bent_end_angle -atan(half_thickness_y/half_length_z)));

  G4SubtractionSolid* side_suppressor_shell = new G4SubtractionSolid("side_suppressor_shell", 
  	suppressor_shell, chopping_shell_box, rotate_chopping_shell_box, move_chopping_shell_box);

  // cut out cavity  
  G4ThreeVector move_cut(-(this->suppressor_shell_thickness + ( this->extra_cut_length/2.0 - this->suppressor_shell_thickness)/2.0 ), 0, (this->suppressor_shell_thickness/2.0) );
  	
  G4SubtractionSolid* side_suppressor_shell_with_cavity = new G4SubtractionSolid("side_suppressor_shell_with_cavity", 
    side_suppressor_shell, this->choppingFrontLeftSlantSuppressor(), 0, move_cut);

  return side_suppressor_shell_with_cavity;

}//end ::shellForFrontLeftSlantSuppressor


G4SubtractionSolid* DetectionSystemGriffin::shellForRightSuppressorExtension()
{
    // Replacement for this->suppressor_extension_length
    G4double shell_suppressor_shell_extension_length = this->suppressor_extension_length
      + (this->suppressor_shell_thickness*2.0)*(1.0/tan(this->bent_end_angle)
      - tan(this->bent_end_angle));

    G4double thickness_z	= this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0;
    G4double length_y 	= shell_suppressor_shell_extension_length;

    G4double longer_length_x = (this->back_inner_radius +this->bent_end_length +(this->BGO_can_seperation
      + this->side_BGO_thickness
      + this->suppressor_shell_thickness*2.0)
          / tan(this->bent_end_angle)
      - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
          * sin(this->bent_end_angle))*tan(this->bent_end_angle);

    G4double shorter_length_x = (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle);

    G4Trap* uncut_extension_shell = new G4Trap("uncut_extension_shell", thickness_z, length_y, longer_length_x, shorter_length_x);

    // because these pieces are rotated in two planes, there are multiple angles that need to be calculated to make sure
    // all of the extensions join up

    G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
    G4double phi 	= atan(1/cos(this->bent_end_angle));

    G4double chopping_half_length_x 	= (thickness_z/sin(phi))/2.0;
    G4double chopping_half_length_y 	= length_y/(2.0*cos(beta));
    G4double chopping_half_length_z 	= chopping_half_length_x;
    G4double y_angle 			= -beta;
    G4double z_angle 			= phi - M_PI/2.0;
    G4double x_angle 			= 0.0;

    G4Para* chopping_para_shell = new G4Para("chopping_para_shell", chopping_half_length_x,
      chopping_half_length_y, chopping_half_length_z, y_angle, z_angle, x_angle);

    G4ThreeVector move_para_shell(((longer_length_x -shorter_length_x)/2.0
      + shorter_length_x)/2.0 +chopping_half_length_x
      - chopping_half_length_x*cos(phi), 0.0, 0.0*m);

    G4SubtractionSolid* right_extension_shell = new G4SubtractionSolid("right_extension_shell",
      uncut_extension_shell, chopping_para_shell, 0, move_para_shell);

    // cut out cavity
//    G4ThreeVector move_cut(-(this->suppressor_shell_thickness + extraCutLength/2.0), this->suppressor_shell_thickness/2.0, -this->suppressor_shell_thickness/4.0);
    G4ThreeVector move_cut(this->suppressorExtRightX , this->suppressorExtRightY, this->suppressorExtRightZ);

    G4SubtractionSolid* extension_suppressor_shell_with_cavity = new G4SubtractionSolid("extension_suppressor_shell_with_cavity",
       right_extension_shell, this->choppingRightSuppressorExtension(), 0, move_cut);

    return extension_suppressor_shell_with_cavity;
}//end ::shellForRightSuppressorExtension


G4SubtractionSolid* DetectionSystemGriffin::shellForLeftSuppressorExtension()
{
    // Replacement for this->suppressor_extension_length
    G4double shell_suppressor_shell_extension_length = this->suppressor_extension_length
      + (this->suppressor_shell_thickness*2.0)*(1.0/tan(this->bent_end_angle)
      - tan(this->bent_end_angle));

    G4double thickness_z = this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0;
    G4double length_y = shell_suppressor_shell_extension_length;

    G4double longer_length_x = (this->back_inner_radius +this->bent_end_length +(this->BGO_can_seperation
      + this->side_BGO_thickness
      + this->suppressor_shell_thickness*2.0)
          / tan(this->bent_end_angle)
      - (this->suppressor_extension_thickness + this->suppressor_shell_thickness*2.0)
      * sin(this->bent_end_angle))*tan(this->bent_end_angle);

    G4double shorter_length_x = (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle);

    G4Trap* uncut_extension_shell = new G4Trap("uncut_extension_shell", thickness_z, length_y, longer_length_x, shorter_length_x);

    // because these pieces are rotated in two planes, there are multiple angles that need to be calculated to make sure
    // all of the extensions join up

    G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
    G4double phi 	= atan(1/cos(this->bent_end_angle));

    G4double chopping_half_length_x = (thickness_z/sin(phi))/2.0;
    G4double chopping_half_length_y = length_y/(2.0*cos(beta));
    G4double chopping_half_length_z = chopping_half_length_x;
    G4double y_angle = -beta;
    G4double z_angle = M_PI/2.0 -phi;
    G4double x_angle = 0.0;

    G4Para* chopping_para_shell = new G4Para("chopping_para_shell", chopping_half_length_x,
      chopping_half_length_y, chopping_half_length_z, y_angle, z_angle, x_angle);

    G4ThreeVector move_para_shell(((longer_length_x -shorter_length_x)/2.0 +shorter_length_x)/2.0
      + chopping_half_length_x -chopping_half_length_x*cos(phi), 0.0, 0.0);

    G4SubtractionSolid* right_extension_shell = new G4SubtractionSolid("right_extension_shell",
      uncut_extension_shell, chopping_para_shell, 0, move_para_shell);

    // cut out cavity
//    G4ThreeVector move_cut(-(this->suppressor_shell_thickness + extraCutLength/2.0), this->suppressor_shell_thickness/2.0, this->suppressor_shell_thickness/4.0);
    G4ThreeVector move_cut(this->suppressorExtLeftX, this->suppressorExtLeftY, this->suppressorExtLeftZ);

    G4SubtractionSolid* extension_suppressor_shell_with_cavity = new G4SubtractionSolid("extension_suppressor_shell_with_cavity",
       right_extension_shell, this->choppingLeftSuppressorExtension(), 0, move_cut);

    return extension_suppressor_shell_with_cavity;
}//end ::shellForLeftSuppressorExtension



///////////////////////////////////////////////////////////////////////
// This is a messy place to put it, but these are the methods to make
// the electrodeMat layers between the crystals
///////////////////////////////////////////////////////////////////////
G4UnionSolid* DetectionSystemGriffin::interCrystalelectrodeMatBack()
{
  G4double distance_of_the_triangle_tips = this->germanium_separation/2.0 
	+ (this->germanium_width/2.0 - this->germanium_shift) //centre of the middle hole
	+ sqrt( pow((this->germanium_outer_radius 
	+ this->triangle_posts_distance_from_crystals), 2.0)
	- pow(this->germanium_width/2.0 - this->germanium_shift, 2.0) );

  G4double extent_of_the_electrodeMat_pieces = distance_of_the_triangle_tips
        - this->triangle_posts_distance_from_crystals;
	
  G4Box* electrodeMat_piece1 = new G4Box("electrodeMat_piece1", extent_of_the_electrodeMat_pieces,
        (this->germanium_length-this->germanium_bent_length)/2.0, 
	this->inter_crystal_electrodeMat_thickness/2.0); 
  G4Box* electrodeMat_piece2 = new G4Box("electrodeMat_piece2", extent_of_the_electrodeMat_pieces, 
        (this->germanium_length-this->germanium_bent_length)/2.0, 
	this->inter_crystal_electrodeMat_thickness/2.0);

  G4RotationMatrix* rotate_piece2 = new G4RotationMatrix;
  rotate_piece2->rotateY(M_PI/2.0);

  G4ThreeVector move_zero(0,0,0);

  G4UnionSolid* inter_crystal_electrodeMat_back = new G4UnionSolid(
	"inter_crystal_electrodeMat_back", electrodeMat_piece1, electrodeMat_piece2, 
	rotate_piece2, move_zero);

  return inter_crystal_electrodeMat_back;
} //end ::interCrystalelectrodeMatBack

G4UnionSolid* DetectionSystemGriffin::interCrystalelectrodeMatFront()
{
   G4double distance_of_the_triangle_tips = this->germanium_separation/2.0 
        + (this->germanium_width/2.0 - this->germanium_shift) //centre of the middle hole
	+ sqrt( pow((this->germanium_outer_radius 
	+ this->triangle_posts_distance_from_crystals), 2.0)
	- pow(this->germanium_width/2.0 - this->germanium_shift, 2.0) );
   G4Trd* electrodeMat_piece1 = new G4Trd("electrodeMat_piece1", distance_of_the_triangle_tips
        - this->triangle_posts_distance_from_crystals, 
        this->germanium_width - this->germanium_bent_length
	*tan(this->bent_end_angle), 
        this->germanium_separation/2.0,
	this->germanium_separation/2.0, (this->germanium_bent_length
        - this->electrodeMat_starting_depth)/2.0);
   G4Trd* electrodeMat_piece2 = new G4Trd("electrodeMat_piece2", distance_of_the_triangle_tips
        - this->triangle_posts_distance_from_crystals, 
        this->germanium_width - this->germanium_bent_length
	*tan(this->bent_end_angle), 
        this->germanium_separation/2.0,
	this->germanium_separation/2.0, (this->germanium_bent_length
        - this->electrodeMat_starting_depth)/2.0);
   
   G4RotationMatrix* rotate_piece2 = new G4RotationMatrix;
   rotate_piece2->rotateZ(M_PI/2.0);

   G4ThreeVector move_zero(0,0,0);

   G4UnionSolid* inter_crystal_electrodeMat_front = new G4UnionSolid(
        "inter_crystal_electrodeMat_front", electrodeMat_piece1, electrodeMat_piece2, 
	rotate_piece2, move_zero);

   return inter_crystal_electrodeMat_front;
} //end ::interCrystalelectrodeMatFront


///////////////////////////////////////////////////////////////////////
// ConstructNewSuppressorCasing builds the suppressor that surrounds 
// the can. It tapers off at the front, and there is a thick piece  
// covering the back of the can, which is divided into quarters.   
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructNewSuppressorCasing()
{
  G4Material* materialBGO = G4Material::GetMaterial(this->BGO_material);
  if( !materialBGO ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* Suppressor_vis_att = new G4VisAttributes(G4Colour(0.75,0.75,0.75));
  Suppressor_vis_att->SetVisibility(true);

  // first we add the four pieces of back suppressor

  G4SubtractionSolid* back_quarter_suppressor = this->backSuppressorQuarter();


  // this one goes in the upper-left position at the back
   
  G4ThreeVector move_back_quarter_suppressor1((this->back_BGO_thickness -this->can_face_thickness)/2.0 
	+ this->detector_total_length +this->BGO_can_seperation +this->shift 
	+ this->applied_back_shift, this->detector_total_width/4.0, 
	- this->detector_total_width/4.0);

  G4Material* back_material = G4Material::GetMaterial(this->back_suppressor_material);
  if( !back_material ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  back_quarter_suppressor_log = new G4LogicalVolume(back_quarter_suppressor, back_material, "back_quarter_suppressor_log", 0, 0, 0);

  back_quarter_suppressor_log->SetVisAttributes(Suppressor_vis_att);

  //this->assembly->AddPlacedVolume(back_quarter_suppressor_log, move_back_quarter_suppressor1, 0); 

  this->suppressorBackAssembly->AddPlacedVolume(back_quarter_suppressor_log, this->move_null, this->rotate_null);

  // this one goes in the upper-right position at the back

  G4RotationMatrix* rotate_back_quarter_suppressor2 = new G4RotationMatrix;
  rotate_back_quarter_suppressor2->rotateX(-M_PI/2.0);
  G4ThreeVector move_back_quarter_suppressor2((this->back_BGO_thickness -this->can_face_thickness)/2.0 
	+ this->detector_total_length +this->BGO_can_seperation +this->shift 
	+ this->applied_back_shift, this->detector_total_width/4.0, 
	this->detector_total_width/4.0);

  //this->assembly->AddPlacedVolume(back_quarter_suppressor_log, move_back_quarter_suppressor2, rotate_back_quarter_suppressor2); 

  // this one goes in the lower-right position at the back

  G4RotationMatrix* rotate_back_quarter_suppressor3 = new G4RotationMatrix;
  rotate_back_quarter_suppressor3->rotateX(M_PI);
  G4ThreeVector move_back_quarter_suppressor3((this->back_BGO_thickness -this->can_face_thickness)/2.0 
	+this->detector_total_length +this->BGO_can_seperation +this->shift 
	+this->applied_back_shift, -this->detector_total_width/4.0, 
	this->detector_total_width/4.0);
    
  //this->assembly->AddPlacedVolume(back_quarter_suppressor_log, move_back_quarter_suppressor3, rotate_back_quarter_suppressor3); 

  // this one goes in the lower-left position at the back

  G4RotationMatrix* rotate_back_quarter_suppressor4 = new G4RotationMatrix;
  rotate_back_quarter_suppressor4->rotateX(M_PI/2.0);
  G4ThreeVector move_back_quarter_suppressor4((this->back_BGO_thickness -this->can_face_thickness)/2.0 
	+ this->detector_total_length +this->BGO_can_seperation +this->shift 
	+ this->applied_back_shift, -this->detector_total_width/4.0, 
	- this->detector_total_width/4.0);

  //this->assembly->AddPlacedVolume(back_quarter_suppressor_log, move_back_quarter_suppressor4, rotate_back_quarter_suppressor4); 

  // now we add the side pieces of suppressor that taper off towards the front of the can

  G4SubtractionSolid* right_suppressor = this->frontRightSlantSuppressor();

  right_suppressor_log = new G4LogicalVolume(right_suppressor, materialBGO, "right_suppressor_log", 0, 0, 0);

  right_suppressor_log->SetVisAttributes(Suppressor_vis_att);


  G4SubtractionSolid* left_suppressor = this->frontLeftSlantSuppressor();

  left_suppressor_log = new G4LogicalVolume(left_suppressor, materialBGO, "left_suppressor_log", 0, 0, 0);

  left_suppressor_log->SetVisAttributes(Suppressor_vis_att);

  this->rightSuppressorCasingAssembly->AddPlacedVolume(right_suppressor_log, this->move_null, this->rotate_null);
  this->leftSuppressorCasingAssembly->AddPlacedVolume(left_suppressor_log, this->move_null, this->rotate_null);  

  G4RotationMatrix* rotate_suppressor1 = new G4RotationMatrix;
  rotate_suppressor1->rotateX(-M_PI/2.0);
  rotate_suppressor1->rotateY(-M_PI/2.0);
  G4ThreeVector move_suppressor1(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0, -(this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation));

  //this->assembly->AddPlacedVolume(right_suppressor_log, move_suppressor1, rotate_suppressor1); 

  G4RotationMatrix* rotate_suppressor2 = new G4RotationMatrix;
  rotate_suppressor2->rotateY(M_PI/2.0);
  rotate_suppressor2->rotateZ(M_PI);
  G4ThreeVector move_suppressor2(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation, 
	- (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0);

  //this->assembly->AddPlacedVolume(left_suppressor_log, move_suppressor2, rotate_suppressor2); 

  G4RotationMatrix* rotate_suppressor3 = new G4RotationMatrix;
  rotate_suppressor3->rotateX(M_PI);
  rotate_suppressor3->rotateY(-M_PI/2.0);
  G4ThreeVector move_suppressor3(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation, 
	(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0);
	
  //this->assembly->AddPlacedVolume(right_suppressor_log, move_suppressor3, rotate_suppressor3); 


  G4RotationMatrix* rotate_suppressor4 = new G4RotationMatrix;
  rotate_suppressor4->rotateX(M_PI/2.0);
  rotate_suppressor4->rotateY(M_PI/2.0);
  G4ThreeVector move_suppressor4(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0, this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation);

  //this->assembly->AddPlacedVolume(left_suppressor_log, move_suppressor4, rotate_suppressor4); 

  G4RotationMatrix* rotate_suppressor5 = new G4RotationMatrix;
  rotate_suppressor5->rotateZ(M_PI/2.0);
  rotate_suppressor5->rotateX(M_PI/2.0);
  G4ThreeVector move_suppressor5(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0, this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation);
 
  //this->assembly->AddPlacedVolume(right_suppressor_log, move_suppressor5, rotate_suppressor5); 

  G4RotationMatrix* rotate_suppressor6 = new G4RotationMatrix;
  rotate_suppressor6->rotateY(M_PI/2.0);
  G4ThreeVector move_suppressor6(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation), 
	(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0);

  //this->assembly->AddPlacedVolume(left_suppressor_log, move_suppressor6, rotate_suppressor6); 

  G4RotationMatrix* rotate_suppressor7 = new G4RotationMatrix;
  rotate_suppressor7->rotateY(-M_PI/2.0);
  G4ThreeVector move_suppressor7(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation), 
	- (this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0);

  //this->assembly->AddPlacedVolume(right_suppressor_log, move_suppressor7, rotate_suppressor7); 

  G4RotationMatrix* rotate_suppressor8 = new G4RotationMatrix;
  rotate_suppressor8->rotateY(M_PI/2.0);
  rotate_suppressor8->rotateZ(M_PI/2.0);
  G4ThreeVector move_suppressor8(this->side_suppressor_length/2.0 -this->can_face_thickness/2.0
	+ this->bent_end_length +(this->BGO_can_seperation 
	+ this->BGO_chopped_tip)/tan(this->bent_end_angle) +this->shift 
	+ this->applied_back_shift, -(this->detector_total_width/2.0 +this->BGO_can_seperation 
	+ this->side_BGO_thickness/2.0)/2.0, -(this->side_BGO_thickness/2.0
	+ this->detector_total_width/2.0 +this->BGO_can_seperation));

  //this->assembly->AddPlacedVolume(left_suppressor_log, move_suppressor8, rotate_suppressor8); 

  // now we add the side pieces of suppressor that extend out in front of the can when it's in the back position

  G4SubtractionSolid* right_suppressor_extension = this->rightSuppressorExtension();

  right_suppressor_extension_log = new G4LogicalVolume(right_suppressor_extension, materialBGO, "right_suppressor_extension_log", 0, 0, 0);

  right_suppressor_extension_log->SetVisAttributes(Suppressor_vis_att);


  G4SubtractionSolid* left_suppressor_extension = this->leftSuppressorExtension();

  left_suppressor_extension_log = new G4LogicalVolume(left_suppressor_extension, materialBGO, "left_suppressor_extension_log", 0, 0, 0);

  left_suppressor_extension_log->SetVisAttributes(Suppressor_vis_att);

  this->rightSuppressorExtensionAssembly->AddPlacedVolume(right_suppressor_extension_log, this->move_null, this->rotate_null);
  this->leftSuppressorExtensionAssembly->AddPlacedVolume(left_suppressor_extension_log, this->move_null, this->rotate_null); 

  if(this->applied_back_shift == 0.0)		// If the detectors are forward, put the extensions in the back position
  {
    // these two parameters are for shifting the extensions back and out when in their BACK position

    G4double extension_back_shift = this->air_box_front_length -(this->hevimet_tip_thickness +this->suppressor_extension_length
	+ this->suppressor_extension_thickness*tan(this->bent_end_angle))*cos(this->bent_end_angle);

    G4double extension_radial_shift = extension_back_shift*tan(this->bent_end_angle);

    // the placement of the extensions matches the placement of the side_suppressor pieces

    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(-M_PI/2.0);
    rotate_extension1->rotateX(-this->bent_end_angle);
    
    G4ThreeVector move_extension1(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius 
	+ extension_back_shift, 
	(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift);
 
    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension1, rotate_extension1); 

    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(-M_PI/2.0);
    rotate_extension2->rotateX(M_PI/2.0 +this->bent_end_angle);
    
    G4ThreeVector move_extension2(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift, -(this->suppressor_extension_length
	* tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension2, rotate_extension2); 

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateY(M_PI/2.0);
    rotate_extension3->rotateX(-(M_PI/2.0 +this->bent_end_angle));
    
    G4ThreeVector move_extension3(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift, (this->suppressor_extension_length
	* tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension3, rotate_extension3); 

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateZ(-M_PI/2.0);
    rotate_extension4->rotateX(this->bent_end_angle);
    
    G4ThreeVector move_extension4(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension4, rotate_extension4); 

    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateY(M_PI);
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateX(-this->bent_end_angle);
    
    G4ThreeVector move_extension5(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	- (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)) +extension_radial_shift);

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension5, rotate_extension5); 


    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateX(this->bent_end_angle -M_PI/2.0);

    G4ThreeVector move_extension6(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift, (this->suppressor_extension_length
	* tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension6, rotate_extension6); 


    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateY(-M_PI/2.0);
    rotate_extension7->rotateX(M_PI/2.0 -this->bent_end_angle);

    G4ThreeVector move_extension7(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift, -(this->suppressor_extension_length
	* tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius +this->hevimet_tip_thickness)
	* sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension7, rotate_extension7); 


    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI);
    rotate_extension8->rotateZ(M_PI/2.0);
    rotate_extension8->rotateX(this->bent_end_angle);
    
    G4ThreeVector move_extension8(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius +extension_back_shift, 
	- (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle) -extension_radial_shift);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension8, rotate_extension8); 

  }//end if(detectors forward) statement

  // Otherwise, put them forward
  else if(this->applied_back_shift == this->back_inner_radius - this->forward_inner_radius)
  {

    // the placement of the extensions matches the placement of the side_suppressor pieces
    G4RotationMatrix* rotate_extension1 = new G4RotationMatrix;
    rotate_extension1->rotateZ(-M_PI/2.0);
    rotate_extension1->rotateX(-this->bent_end_angle);
    
    G4ThreeVector move_extension1(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension1, rotate_extension1); 

    G4RotationMatrix* rotate_extension2 = new G4RotationMatrix;
    rotate_extension2->rotateY(-M_PI/2.0);
    rotate_extension2->rotateX(M_PI/2.0 +this->bent_end_angle);
    
    G4ThreeVector move_extension2(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)), -(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension2, rotate_extension2); 

    G4RotationMatrix* rotate_extension3 = new G4RotationMatrix;
    rotate_extension3->rotateY(M_PI/2.0);
    rotate_extension3->rotateX(-(M_PI/2.0 +this->bent_end_angle));
    
    G4ThreeVector move_extension3(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)), (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension3, rotate_extension3); 

    G4RotationMatrix* rotate_extension4 = new G4RotationMatrix;
    rotate_extension4->rotateZ(-M_PI/2.0);
    rotate_extension4->rotateX(this->bent_end_angle);
    
    G4ThreeVector move_extension4(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)));

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension4, rotate_extension4); 

    G4RotationMatrix* rotate_extension5 = new G4RotationMatrix;
    rotate_extension5->rotateY(M_PI);
    rotate_extension5->rotateZ(M_PI/2.0);
    rotate_extension5->rotateX(-this->bent_end_angle);
    
    G4ThreeVector move_extension5(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	- (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	- ((this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle)));

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension5, rotate_extension5); 

    G4RotationMatrix* rotate_extension6 = new G4RotationMatrix;
    rotate_extension6->rotateY(M_PI/2.0);
    rotate_extension6->rotateX(this->bent_end_angle -M_PI/2.0);

    G4ThreeVector move_extension6(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle), (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension6, rotate_extension6); 

    G4RotationMatrix* rotate_extension7 = new G4RotationMatrix;
    rotate_extension7->rotateY(-M_PI/2.0);
    rotate_extension7->rotateX(M_PI/2.0 -this->bent_end_angle);

    G4ThreeVector move_extension7(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle), -(this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 
	+ (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0);

    //this->assembly->AddPlacedVolume(right_suppressor_extension_log, move_extension7, rotate_extension7); 

    G4RotationMatrix* rotate_extension8 = new G4RotationMatrix;
    rotate_extension8->rotateY(M_PI);
    rotate_extension8->rotateZ(M_PI/2.0);
    rotate_extension8->rotateX(this->bent_end_angle);
    
    G4ThreeVector move_extension8(-this->can_face_thickness/2.0 -(this->suppressor_extension_length/2.0 
	- this->suppressor_extension_thickness*tan(this->bent_end_angle)/2.0)
	* cos(this->bent_end_angle) +this->bent_end_length +(this->BGO_can_seperation 
	+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
	* sin(this->bent_end_angle) +this->shift +this->back_inner_radius - this->forward_inner_radius, 
	- (this->suppressor_extension_length*tan(this->suppressor_extension_angle)/2.0 +(this->forward_inner_radius 
	+ this->hevimet_tip_thickness)*sin(this->bent_end_angle))/2.0, 
	(this->suppressor_extension_thickness/2.0)/cos(this->bent_end_angle) 
	+ (this->suppressor_extension_length/2.0 -this->suppressor_extension_thickness
	* tan(this->bent_end_angle)/2.0)*sin(this->bent_end_angle) -(this->back_inner_radius 
	+ this->bent_end_length +(this->BGO_can_seperation +this->side_BGO_thickness)
	/ tan(this->bent_end_angle) -this->suppressor_extension_thickness*sin(this->bent_end_angle))
	* tan(this->bent_end_angle));

    //this->assembly->AddPlacedVolume(left_suppressor_extension_log, move_extension8, rotate_extension8); 

  }//end if(detectors back) statement

}//end ::ConstructNewSuppressorCasing


///////////////////////////////////////////////////////////////////////
// ConstructNewHeavyMet builds the heavy metal that goes on the front 
// of the Suppressor. It only goes on when the extensions are in their 
// forward position   
///////////////////////////////////////////////////////////////////////
void DetectionSystemGriffin::ConstructNewHeavyMet()
{
  G4Material* materialHevimetal = G4Material::GetMaterial("Hevimetal");
  if( !materialHevimetal ) {
    G4cout << " ----> Material " << this->crystal_material << " not found, cannot build the detector shell! " << G4endl;
    exit(1);
  }

  G4VisAttributes* HeviMet_vis_att = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  HeviMet_vis_att->SetVisibility(true);

  G4SubtractionSolid* hevimet = this->newHeavyMet();
    
  G4RotationMatrix* rotate_hevimet = new G4RotationMatrix;
  rotate_hevimet->rotateY(M_PI/2.0);
  G4ThreeVector move_hevimet(((this->hevimet_tip_thickness/cos(this->hevimet_tip_angle))*cos(this->bent_end_angle -this->hevimet_tip_angle) 
	+ this->forward_inner_radius*tan(this->hevimet_tip_angle)*sin(this->bent_end_angle))/2.0 
	- this->air_box_back_length/2.0 -this->air_box_front_length, 0.0, 0.0);

  // NOTE** The hevimet does not require the "shift" parameter, as the air_box has been designed 
  // so that the hevimet sits right at the front, and has been moved accordingly. Also, the
  // "applied_back_shift" parameter is missing, as the hevimet only goes on in the "detector back" position 

  hevimet_log = new G4LogicalVolume(hevimet, materialHevimetal, "hevimet_log", 0, 0, 0); 

  hevimet_log->SetVisAttributes(HeviMet_vis_att);

  this->assembly->AddPlacedVolume(hevimet_log, move_hevimet, rotate_hevimet); 

}//end ::ConstructHeavyMet


///////////////////////////////////////////////////////////////////////
// methods used in ConstructDetector()
///////////////////////////////////////////////////////////////////////
G4Box* DetectionSystemGriffin::squareFrontFace()
{
  G4double half_thickness_x = this->can_face_thickness/2.0;
    
  G4double half_length_y = (this->detector_total_width/2.0) -(this->bent_end_length 
	*tan(this->bent_end_angle));
    
  G4double half_length_z = half_length_y;
  
  G4Box* front_face = new G4Box("front_face", half_thickness_x, half_length_y, half_length_z);
 		                  
  return front_face;

}//end ::squareFrontFace


G4Trap* DetectionSystemGriffin::cornerWedge()
{
  G4double height_y = this->can_face_thickness; 
  G4double length_z = this->detector_total_width -2.0*(this->bent_end_length *tan(this->bent_end_angle));   
  G4double extension_length_x = height_y *tan(this->bent_end_angle);

  G4Trap* wedge = new G4Trap("wedge", length_z, height_y, extension_length_x, this->wedgeDim);

  return wedge;
}//end ::cornerWedge


///////////////////////////////////////////////////////////////////////
// The bent side pieces attach to the front faceface they angle
// outwards, so the face is smaller than the main part of 
// the can four are needed for a square-faced detector
///////////////////////////////////////////////////////////////////////
G4Para* DetectionSystemGriffin::bentSidePiece()
{

  G4double half_length_x = ((this->bent_end_length -this->can_face_thickness)
	/cos(this->bent_end_angle))/2.0;

  G4double half_length_y = (this->detector_total_width/2.0) -(this->bent_end_length 
	*tan(this->bent_end_angle)) -this->can_face_thickness 
	*(1.0 -tan(this->bent_end_angle));    

  // Last two operations in "half_length_y" are correction 
  // factor to keep the bent pieces from overlapping                     
  G4double half_length_z = this->can_face_thickness *cos(this->bent_end_angle)/2.0; 
  G4double alpha_angle = 0.0*M_PI; 
  G4double polar_angle = this->bent_end_angle;
  G4double azimuthal_angle = 0.0*M_PI; 
 
  G4Para* bent_side_piece = new G4Para("bent_side_piece", half_length_x, 
 	half_length_y, half_length_z, alpha_angle, polar_angle, azimuthal_angle);

  return bent_side_piece;

}//end ::bentSidePiece


// this is a conic piece that attaches two bent end pieces
G4Cons* DetectionSystemGriffin::roundedEndEdge()
{
  G4double hole_eliminator = this->can_face_thickness *(1.0 -tan(this->bent_end_angle));	

  // this is correction factor to avoid holes caused by bent pieces  
  G4double half_length_z = (this->bent_end_length -this->can_face_thickness)/2.0;
  G4double inside_radius_at_apex = 0.0;
  G4double outside_radius_at_apex = this->can_face_thickness *tan(this->bent_end_angle) +hole_eliminator;   
  G4double outside_radius_at_base = this->bent_end_length *tan(this->bent_end_angle) +hole_eliminator;
  G4double inside_radius_at_base = outside_radius_at_base -this->can_face_thickness; 
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 0.5*M_PI;

  G4Cons* rounded_end_edge = new G4Cons("rounded_end_edge", inside_radius_at_apex, 
 	outside_radius_at_apex, inside_radius_at_base, outside_radius_at_base, 
 	half_length_z, start_angle, final_angle);

  return rounded_end_edge;

}//end ::roundedEndEdge


G4Tubs* DetectionSystemGriffin::cornerTube()
{
  G4double outer_radius 	= this->bent_end_length *tan(this->bent_end_angle);
  G4double inner_radius 	= outer_radius - this->can_side_thickness; 
  G4double half_height_z 	= (this->detector_total_length -this->bent_end_length 
				-this->rear_plate_thickness)/2.0;

  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 0.5*M_PI;    

  G4Tubs* corner_tube = new G4Tubs("corner_tube", inner_radius, outer_radius, half_height_z, start_angle, final_angle);

  return corner_tube;

}//end ::cornerTube


G4Box* DetectionSystemGriffin::sidePanel()
{
  G4double half_length_x = (this->detector_total_length -this->bent_end_length -this->rear_plate_thickness)/2.0;
  G4double half_thickness_y = (this->can_side_thickness)/2.0;
  G4double half_width_z = (this->detector_total_width)/2.0 -(this->bent_end_length *tan(this->bent_end_angle));

  G4Box* side_panel = new G4Box("side_panel", half_length_x, half_thickness_y, half_width_z);    

  return side_panel;

}//end ::sidePanel


G4SubtractionSolid* DetectionSystemGriffin::rearPlate()
{

  G4double half_width_x = this->detector_total_width/2.0;
  G4double half_width_y = half_width_x;  
  G4double half_thickness_z = this->rear_plate_thickness/2.0;

  G4Box* plate = new G4Box("plate", half_width_x, half_width_y, half_thickness_z);

  G4double inner_radius = 0.0;  
  G4double outer_radius = this->cold_finger_outer_shell_radius;  
  G4double half_height_z = half_thickness_z +1.0*cm;  // +1.0*cm just to make sure the hole goes completely through the plate
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI;

  G4Tubs* hole = new G4Tubs("hole", inner_radius, outer_radius, half_height_z, start_angle, final_angle);     

  G4SubtractionSolid* rear_plate = new G4SubtractionSolid("rear_plate", plate, hole);

  return rear_plate;

}//end ::rearPlate


G4Tubs* DetectionSystemGriffin::coldFingerShell()
{
  G4double outer_radius = this->cold_finger_outer_shell_radius;
  G4double inner_radius = outer_radius - this->cold_finger_shell_thickness;
  G4double half_length_z = this->cold_finger_shell_length/2.0;
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI;
  
  G4Tubs* cold_finger_shell = new G4Tubs("cold_finger_shell", inner_radius, 
  	outer_radius, half_length_z, start_angle, final_angle);
  	
  return cold_finger_shell;  

}//end ::coldFingerShell


G4Tubs* DetectionSystemGriffin::liquidNitrogenTank()
{
  G4double inner_radius = 0.0;
  G4double outer_radius = this->coolant_radius;
  G4double half_length_z = this->coolant_length/2.0;
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI;

  G4Tubs* tank = new G4Tubs("tank", inner_radius, outer_radius, half_length_z, start_angle, final_angle);

  return tank;   

}//end ::liquidNitrogenTank


///////////////////////////////////////////////////////////////////////
// methods used in ConstructBasicDetectorBlock()
// Both the rectangularSegment and the trapezoidalSegment join
// to form a UnionSolid
///////////////////////////////////////////////////////////////////////
G4Box* DetectionSystemGriffin::rectangularSegment()
{
  G4double half_length_x = this->detector_block_height/2.0;
  G4double half_length_y = this->detector_block_length/2.0;
  G4double half_length_z = half_length_y; 	// Since it is symmetric

  G4Box* rectangular_segment = new G4Box("rectangular_segment", half_length_x, half_length_y, half_length_z);    

  return rectangular_segment;

}// end ::rectangularSegment


G4Trd* DetectionSystemGriffin::trapezoidalSegment()
{
  G4double half_base_x 		= this->detector_block_length/2.0;
  G4double half_top_x 		= half_base_x - (this->detector_block_trapezoidal_height *tan(this->bent_end_angle));
  G4double half_base_y 		= half_base_x; 	// Since it is symmetric
  G4double half_top_y 		= half_top_x;
  G4double half_height_z 	= this->detector_block_trapezoidal_height/2.0;
  
  G4Trd* trapezoidal_segment = new G4Trd("trapezoidal_segment", 
  	half_base_x, half_top_x, half_base_y, half_top_y, half_height_z);

  return trapezoidal_segment;

}//end ::trapezoidalSegment

///////////////////////////////////////////////////////////////////////
// methods used in ConstructComplexDetectorBlock()
// Starting with a rectangle, it gets chopped by an off-centered
// cylinder, and then the two edges are chopped diagonaly
///////////////////////////////////////////////////////////////////////
G4SubtractionSolid* DetectionSystemGriffin::quarterDetector()
{

  G4double half_width_x 	= this->germanium_width/2.0;
  G4double half_width_y 	= half_width_x;
  G4double half_length_z 	= this->germanium_length/2.0;

  G4Box* rectangular_germanium 	= new G4Box("rectangular_germanium", half_width_x, half_width_y, half_length_z);
 
  G4double outer_radius = ((this->germanium_width +this->germanium_shift)*1.5)/2.0;   

  // 1.5 is almost root 2, so this makes sure the corners are chopped off

  G4double inner_radius = this->germanium_outer_radius;     	
  G4double start_angle = 0.0*M_PI;
  G4double final_angle = 2.0*M_PI; 

  G4ThreeVector move_chopping_cylinder(this->germanium_shift, -(this->germanium_shift), 0);
  G4Tubs* chopping_cylinder = new G4Tubs("chopping_cylinder", inner_radius, 
  	outer_radius, half_length_z+1.0*cm, start_angle, final_angle);

  G4SubtractionSolid* germanium_rounded_corners = new G4SubtractionSolid("germanium_rounded_corners", 
  	rectangular_germanium, chopping_cylinder, 0, move_chopping_cylinder);

  G4double base_inner_radius = this->germanium_outer_radius;
  G4double base_outer_radius = 2.0*base_inner_radius;

  // G4double tip_inner_radius = base_inner_radius - this->germanium_bent_length*tan(this->bent_end_angle);
  G4double tip_inner_radius = base_inner_radius 
  	- this->germanium_corner_cone_end_length*tan(this->bent_end_angle);
  G4double tip_outer_radius = base_outer_radius;

  // G4double con_half_length_z = this->germanium_bent_length/2.0;
  G4double con_half_length_z = this->germanium_corner_cone_end_length/2.0 + this->quarterDetectorCxn;
  
  G4double initial_angle = acos((this->germanium_width/2.0 +this->germanium_shift) 
  	/this->germanium_outer_radius);
  G4double total_angle = M_PI/2.0 -2.0*initial_angle;

  G4Cons* rounded_edge = new G4Cons("rounded_edge", base_inner_radius, 
  	base_outer_radius, tip_inner_radius, tip_outer_radius, 
  	con_half_length_z, initial_angle, total_angle);


  G4RotationMatrix* rotate_rounded_edge = new G4RotationMatrix;
  rotate_rounded_edge->rotateZ(-M_PI/2.0);

  G4ThreeVector move_rounded_edge(this->germanium_shift, -this->germanium_shift, 
  	this->germanium_length/2.0 -this->germanium_corner_cone_end_length/2.0 
  	+ this->quarterDetectorCxnB);

  G4SubtractionSolid* germanium_rounded_edge = new G4SubtractionSolid("germanium_rounded_edge", 
  	germanium_rounded_corners, rounded_edge, rotate_rounded_edge, 
  	move_rounded_edge);

  // now we make the diagonal slices  
  G4double half_chop_piece_width_x 	= this->germanium_width/2.0;
  G4double half_chop_piece_width_y 	= this->germanium_width/2.0;
  G4double half_chop_piece_length_z 	= (this->germanium_bent_length/cos(this->bent_end_angle))/2.0;
  G4Box* chop_piece = new G4Box("chop_piece", half_chop_piece_width_x, 
  	half_chop_piece_width_y, half_chop_piece_length_z);

  G4RotationMatrix* rotate_chop_piece1 = new G4RotationMatrix;
  rotate_chop_piece1->rotateX(-(this->bent_end_angle));

  G4ThreeVector move_chop_piece1(0, this->germanium_width/2.0 +(this->germanium_bent_length 
  	/tan(this->bent_end_angle) +this->germanium_width 
  	*cos(this->bent_end_angle))/2.0 -((this->germanium_bent_length
  	/cos(this->bent_end_angle))/2.0) /sin(this->bent_end_angle), 
  	this->germanium_length/2.0 -this->germanium_bent_length
  	+(this->germanium_bent_length +this->germanium_width
  	*sin(this->bent_end_angle))/2.0);

  G4SubtractionSolid* chopped_germanium1 = new G4SubtractionSolid("chopped_germanium1", 
  	germanium_rounded_edge, chop_piece, rotate_chop_piece1, move_chop_piece1);


  G4RotationMatrix* rotate_chop_piece2 = new G4RotationMatrix;
  rotate_chop_piece2->rotateY(-(this->bent_end_angle));

  G4ThreeVector move_chop_piece2(-(this->germanium_width/2.0 +(this->germanium_bent_length 
	/tan(this->bent_end_angle) +this->germanium_width 
	*cos(this->bent_end_angle))/2.0 -((this->germanium_bent_length
	/cos(this->bent_end_angle))/2.0) /sin(this->bent_end_angle)), 0,
	this->germanium_length/2.0 -this->germanium_bent_length
	+(this->germanium_bent_length +this->germanium_width
	*sin(this->bent_end_angle))/2.0);

  G4SubtractionSolid* chopped_germanium2 = new G4SubtractionSolid("chopped_germanium2", 
  	chopped_germanium1, chop_piece, rotate_chop_piece2, move_chop_piece2);


  // now we make the hole that will go in the back of each quarter detector, /////////////////////////////////////////////////////////
  start_angle = 0.0*M_PI;
  final_angle = 2.0*M_PI;
  G4double hole_radius = this->germanium_hole_radius;
  G4double hole_half_length_z = (this->germanium_length -this->germanium_hole_dist_from_face)/2.0;

  G4Tubs* hole_tubs = new G4Tubs("hole_tubs", 0.0, hole_radius, hole_half_length_z, start_angle, final_angle);
  G4ThreeVector move_hole(this->germanium_shift, -(this->germanium_shift),-((this->germanium_hole_dist_from_face)));

  G4SubtractionSolid* chopped_germanium3 = new G4SubtractionSolid("chopped_germanium3", 
  	chopped_germanium2, hole_tubs, 0, move_hole);

  start_angle = 0.0*M_PI;
  final_angle = 2.0*M_PI;
  G4double dead_layer_radius = this->germanium_hole_radius + this->inner_dead_layer_thickness;
  G4double dead_layer_half_length_z = (this->germanium_length 
  	- this->germanium_hole_dist_from_face
	+ this->inner_dead_layer_thickness)/2.0;

  // now dead layer
  G4Tubs* dead_layer_tubs = new G4Tubs("dead_layer_tubs", 0.0,
	dead_layer_radius, dead_layer_half_length_z, start_angle,final_angle);
  
  G4ThreeVector move_dead_layer(this->germanium_shift, -(this->germanium_shift),
	-((this->germanium_hole_dist_from_face
	- this->inner_dead_layer_thickness)/2.0));   

  G4SubtractionSolid* chopped_germanium4 = new G4SubtractionSolid("chopped_germanium4", 
  	chopped_germanium3, dead_layer_tubs, 0, move_dead_layer);

   return chopped_germanium4;

}//end ::quarterDetector


///////////////////////////////////////////////////////////////////////
// methods used in ConstructNewSuppressorCasing
///////////////////////////////////////////////////////////////////////

// Back to making the suppressor volumes themselves
G4SubtractionSolid* DetectionSystemGriffin::backSuppressorQuarter()
{
  G4double half_thickness_x = this->back_BGO_thickness/2.0;
  G4double half_length_y = this->detector_total_width/4.0 - this->suppressor_shell_thickness;
  G4double half_length_z = half_length_y;
  G4Box* quarter_suppressor = new G4Box("quarter_suppressor", half_thickness_x, half_length_y, half_length_z);

  G4double hole_radius = this->cold_finger_outer_shell_radius;
  G4Tubs* hole = new G4Tubs("hole", 0, hole_radius, half_thickness_x +1.0*cm, 0.0*M_PI, 2.0*M_PI);

  G4RotationMatrix* rotate_hole = new G4RotationMatrix;
  rotate_hole->rotateY(M_PI/2.0);
  G4ThreeVector move_hole(0, -half_length_y, half_length_z);

  G4SubtractionSolid* quarter_suppressor_with_hole = new G4SubtractionSolid("quarter_suppressor_with_hole",
    quarter_suppressor, hole, rotate_hole, move_hole);

  return quarter_suppressor_with_hole;

}//end ::backSuppressorQuarter


G4SubtractionSolid* DetectionSystemGriffin::frontRightSlantSuppressor()
{
  G4double length_z 		= this->side_suppressor_length;
  G4double length_y 		= this->side_BGO_thickness;
  G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation +this->side_BGO_thickness;
  G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness;

  G4Trap* suppressor = new G4Trap("suppressor", length_z, length_y, length_longer_x, length_shorter_x);
  
  G4double half_length_x	= length_longer_x/2.0 +1.0*cm;
  G4double half_thickness_y 	= this->side_BGO_thickness/2.0;
  G4double half_length_z 	= ((this->side_BGO_thickness 
  				- this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

  G4Box* chopping_box = new G4Box("chopping_box", half_length_x, half_thickness_y, half_length_z);

  G4RotationMatrix* rotate_chopping_box = new G4RotationMatrix;
  rotate_chopping_box->rotateX(-this->bent_end_angle);
  G4ThreeVector move_chopping_box(0, this->side_BGO_thickness/2.0 -this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
	+ pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
	- length_z/2.0 +0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0 
	- this->bent_end_angle -atan(half_thickness_y/half_length_z)));


  G4SubtractionSolid* side_suppressor = new G4SubtractionSolid("side_suppressor", 
  	suppressor, chopping_box, rotate_chopping_box, move_chopping_box);

  return side_suppressor;

}//end ::frontRightSlantSuppressor

G4SubtractionSolid* DetectionSystemGriffin::choppingFrontRightSlantSuppressor()
{
    G4double length_z 		= this->side_suppressor_length;
    G4double length_y 		= this->side_BGO_thickness;
    G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation +this->side_BGO_thickness + this->extra_cut_length/2.0;
    G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness;

    G4Trap* suppressor = new G4Trap("suppressor", length_z, length_y, length_longer_x, length_shorter_x);

    G4double half_length_x	= length_longer_x/2.0 +1.0*cm;
    G4double half_thickness_y 	= this->side_BGO_thickness/2.0;
    G4double half_length_z 	= ((this->side_BGO_thickness
                  - this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

    G4Box* chopping_box = new G4Box("chopping_box", half_length_x, half_thickness_y, half_length_z);

    G4RotationMatrix* rotate_chopping_box = new G4RotationMatrix;
    rotate_chopping_box->rotateX(-this->bent_end_angle);
    G4ThreeVector move_chopping_box(0, this->side_BGO_thickness/2.0 -this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
      + pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
      - length_z/2.0 +0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0
      - this->bent_end_angle -atan(half_thickness_y/half_length_z)));


    G4SubtractionSolid* side_suppressor = new G4SubtractionSolid("side_suppressor",
      suppressor, chopping_box, rotate_chopping_box, move_chopping_box);

    return side_suppressor;

}//end ::choppingFrontRightSlantSuppressor


G4SubtractionSolid* DetectionSystemGriffin::frontLeftSlantSuppressor()
{
  G4double length_z 		= this->side_suppressor_length;
  G4double length_y 		= this->side_BGO_thickness;
  G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation +this->side_BGO_thickness;
  G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness;

  G4Trap* suppressor = new G4Trap("suppressor", length_z, length_y, length_longer_x, length_shorter_x);
  
  G4double half_length_x 	= length_longer_x/2.0 +1.0*cm;
  G4double half_thickness_y 	= this->side_BGO_thickness/2.0;
  G4double half_length_z 	= ((this->side_BGO_thickness -this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

  G4Box* chopping_box = new G4Box("chopping_box", half_length_x, half_thickness_y, half_length_z);

  G4RotationMatrix* rotate_chopping_box = new G4RotationMatrix;
  rotate_chopping_box->rotateX(this->bent_end_angle);
  G4ThreeVector move_chopping_box(0, this->side_BGO_thickness/2.0 -this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
	+ pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
	length_z/2.0 -0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0 
	- this->bent_end_angle -atan(half_thickness_y/half_length_z)));


  G4SubtractionSolid* side_suppressor = new G4SubtractionSolid("side_suppressor", 
  	suppressor, chopping_box, rotate_chopping_box, move_chopping_box);
  	
  return side_suppressor;
}//end ::frontLeftSlantSuppressor

G4SubtractionSolid* DetectionSystemGriffin::choppingFrontLeftSlantSuppressor()
{
    G4double length_z 		= this->side_suppressor_length;
    G4double length_y 		= this->side_BGO_thickness;
    G4double length_longer_x 	= this->detector_total_width/2.0 +this->BGO_can_seperation +this->side_BGO_thickness + this->extra_cut_length/2.0;
    G4double length_shorter_x 	= length_longer_x -this->side_BGO_thickness;

    G4Trap* suppressor = new G4Trap("suppressor", length_z, length_y, length_longer_x, length_shorter_x);

    G4double half_length_x 	= length_longer_x/2.0 +1.0*cm;
    G4double half_thickness_y 	= this->side_BGO_thickness/2.0;
    G4double half_length_z 	= ((this->side_BGO_thickness -this->BGO_chopped_tip)/sin(this->bent_end_angle))/2.0;

    G4Box* chopping_box = new G4Box("chopping_box", half_length_x, half_thickness_y, half_length_z);

    G4RotationMatrix* rotate_chopping_box = new G4RotationMatrix;
    rotate_chopping_box->rotateX(this->bent_end_angle);
    G4ThreeVector move_chopping_box(0, this->side_BGO_thickness/2.0 -this->BGO_chopped_tip -0.5*sqrt(pow((2.0*half_length_z), 2.0)
      + pow((2.0*half_thickness_y), 2.0))*cos(M_PI/2.0 -this->bent_end_angle -atan(half_thickness_y/half_length_z)),
      length_z/2.0 -0.5*sqrt(pow((2.0*half_length_z), 2.0) +pow((2.0*half_thickness_y), 2.0))*sin(M_PI/2.0
      - this->bent_end_angle -atan(half_thickness_y/half_length_z)));


    G4SubtractionSolid* side_suppressor = new G4SubtractionSolid("side_suppressor",
      suppressor, chopping_box, rotate_chopping_box, move_chopping_box);

    return side_suppressor;
}//end ::choppingFrontLeftSlantSuppressor


G4SubtractionSolid* DetectionSystemGriffin::rightSuppressorExtension()  // Modified!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
{
  G4double thickness_z 	= this->suppressor_extension_thickness;
  G4double length_y 	= this->suppressor_extension_length;
  
  G4double longer_length_x 	= (this->back_inner_radius +this->bent_end_length 
  				+ (this->BGO_can_seperation 
				+ this->side_BGO_thickness)/tan(this->bent_end_angle) 
				- this->suppressor_extension_thickness
                * sin(this->bent_end_angle))*tan(this->bent_end_angle) - this->BGO_can_seperation*2.0;  // - this->BGO_can_seperation

  G4double shorter_length_x 	= (this->forward_inner_radius 
  				+ this->hevimet_tip_thickness)
                * sin(this->bent_end_angle) - this->BGO_can_seperation*2.0; // - this->BGO_can_seperation

  G4Trap* uncut_extension 	= new G4Trap("uncut_extension", thickness_z, 
    				length_y, longer_length_x, shorter_length_x);

  // because these pieces are rotated in two planes, there are multiple angles that need to be calculated to make sure
  // all of the extensions join up
  G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
  G4double phi 	= atan(1/cos(this->bent_end_angle));

  G4double chopping_half_length_x = (thickness_z/sin(phi))/2.0;
  G4double chopping_half_length_y = length_y/(2.0*cos(beta));
  G4double chopping_half_length_z = chopping_half_length_x;

  G4double y_angle = -beta;
  G4double z_angle = phi -M_PI/2.0;
  G4double x_angle = 0.0;


  G4Para* chopping_para = new G4Para("chopping_para", chopping_half_length_x, 
  	chopping_half_length_y, chopping_half_length_z, 
  	y_angle, z_angle, x_angle);

  G4ThreeVector move_para(((longer_length_x -shorter_length_x)/2.0 
  	+ shorter_length_x)/2.0 +chopping_half_length_x 
  	- chopping_half_length_x*cos(phi), 0.0, 0.0);

  G4SubtractionSolid* right_extension = new G4SubtractionSolid("right_extension", 
  	uncut_extension, chopping_para, 0, move_para);

  return right_extension;

}//end ::rightSuppressorExtension

G4SubtractionSolid* DetectionSystemGriffin::choppingRightSuppressorExtension()   
{
    G4double thickness_z 	= this->suppressor_extension_thickness + this->cut_clearance;
    G4double length_y 	= this->suppressor_extension_length + this->cut_clearance;

    G4double longer_length_x 	= (this->back_inner_radius +this->bent_end_length + this->cut_clearance + this->extra_cut_length
                  + (this->BGO_can_seperation
                  + this->side_BGO_thickness)/tan(this->bent_end_angle)
                  - (this->suppressor_extension_thickness)
                  * sin(this->bent_end_angle))*tan(this->bent_end_angle) - this->BGO_can_seperation*2.0;  // - this->BGO_can_seperation

    G4double shorter_length_x 	= (this->forward_inner_radius
                  + this->hevimet_tip_thickness + this->cut_clearance + this->extra_cut_length)
                  * sin(this->bent_end_angle) - this->BGO_can_seperation*2.0; // - this->BGO_can_seperation

    G4Trap* uncut_extension 	= new G4Trap("uncut_extension", thickness_z,
                      length_y, longer_length_x, shorter_length_x);

    // because these pieces are rotated in two planes, there are multiple angles that need to be calculated to make sure
    // all of the extensions join up
    G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
    G4double phi 	= atan(1/cos(this->bent_end_angle));

    G4double chopping_half_length_x = (thickness_z/sin(phi))/2.0;
    G4double chopping_half_length_y = length_y/(2.0*cos(beta));
    G4double chopping_half_length_z = chopping_half_length_x;

    G4double y_angle = -beta;
    G4double z_angle = phi -M_PI/2.0;
    G4double x_angle = 0.0;

    G4Para* chopping_para = new G4Para("chopping_para", chopping_half_length_x,
      chopping_half_length_y, chopping_half_length_z,
      y_angle, z_angle, x_angle);

    G4ThreeVector move_para(((longer_length_x -shorter_length_x)/2.0
      + shorter_length_x)/2.0 +chopping_half_length_x
      - chopping_half_length_x*cos(phi), 0.0, 0.0);

    G4SubtractionSolid* right_extension = new G4SubtractionSolid("right_extension",
      uncut_extension, chopping_para, 0, move_para);

    return right_extension;
  
}//end ::choppingRightSuppressorExtension


G4SubtractionSolid* DetectionSystemGriffin::leftSuppressorExtension() // Modified!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
{
  G4double thickness_z 		= this->suppressor_extension_thickness;
  G4double length_y 		= this->suppressor_extension_length;
  G4double longer_length_x 	= (this->back_inner_radius +this->bent_end_length +(this->BGO_can_seperation 
				+ this->side_BGO_thickness)/tan(this->bent_end_angle) -this->suppressor_extension_thickness
                * sin(this->bent_end_angle))*tan(this->bent_end_angle) - this->BGO_can_seperation*2.0;  ////////

  G4double shorter_length_x 	= (this->forward_inner_radius +this->hevimet_tip_thickness)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0; /////

  G4Trap* uncut_extension = new G4Trap("uncut_extension", thickness_z, length_y, longer_length_x, shorter_length_x);

   
  // because these pieces are rotated in two planes, there are multiple angles 
  // that need to be calculated to make sure all of the extensions join up

  G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
  G4double phi 	= atan(1/cos(this->bent_end_angle));

  G4double chopping_half_length_x = (thickness_z/sin(phi))/2.0;
  G4double chopping_half_length_y = length_y/(2.0*cos(beta));
  G4double chopping_half_length_z = chopping_half_length_x;

  G4double y_angle = -beta;
  G4double z_angle = M_PI/2.0 -phi;
  G4double x_angle = 0.0;

  G4Para* chopping_para = new G4Para("chopping_para", chopping_half_length_x, 
  	chopping_half_length_y, chopping_half_length_z, 
	y_angle, z_angle, x_angle);

  G4ThreeVector move_para(((longer_length_x -shorter_length_x)/2.0 +shorter_length_x)/2.0 
  	+ chopping_half_length_x -chopping_half_length_x*cos(phi), 0.0, 0.0);

  G4SubtractionSolid* right_extension = new G4SubtractionSolid("right_extension", 
  	uncut_extension, chopping_para, 0, move_para);

  return right_extension;

}//end ::leftSuppressorExtension


G4SubtractionSolid* DetectionSystemGriffin::choppingLeftSuppressorExtension()
{
  G4double thickness_z 		= this->suppressor_extension_thickness + this->cut_clearance;
  G4double length_y 		= this->suppressor_extension_length + this->cut_clearance;
  G4double longer_length_x 	= (this->back_inner_radius +this->bent_end_length + this->cut_clearance  + this->extra_cut_length +(this->BGO_can_seperation
                  + this->side_BGO_thickness)/tan(this->bent_end_angle) - (this->suppressor_extension_thickness)
                  * sin(this->bent_end_angle))*tan(this->bent_end_angle) - this->BGO_can_seperation*2.0;  ////////

  G4double shorter_length_x 	= (this->forward_inner_radius +this->hevimet_tip_thickness + this->cut_clearance + this->extra_cut_length)*sin(this->bent_end_angle) - this->BGO_can_seperation*2.0; /////

  G4Trap* uncut_extension = new G4Trap("uncut_extension", thickness_z, length_y, longer_length_x, shorter_length_x);


  // because these pieces are rotated in two planes, there are multiple angles
  // that need to be calculated to make sure all of the extensions join up

  G4double beta = atan((longer_length_x -shorter_length_x)/(length_y));
  G4double phi 	= atan(1/cos(this->bent_end_angle));

  G4double chopping_half_length_x = (thickness_z/sin(phi))/2.0;
  G4double chopping_half_length_y = length_y/(2.0*cos(beta));
  G4double chopping_half_length_z = chopping_half_length_x;

  G4double y_angle = -beta;
  G4double z_angle = M_PI/2.0 -phi;
  G4double x_angle = 0.0;

  G4Para* chopping_para = new G4Para("chopping_para", chopping_half_length_x,
    chopping_half_length_y, chopping_half_length_z,
    y_angle, z_angle, x_angle);

  G4ThreeVector move_para(((longer_length_x -shorter_length_x)/2.0 +shorter_length_x)/2.0
    + chopping_half_length_x -chopping_half_length_x*cos(phi), 0.0, 0.0);

  G4SubtractionSolid* right_extension = new G4SubtractionSolid("right_extension",
    uncut_extension, chopping_para, 0, move_para);

  return right_extension;

}//end ::leftSuppressorExtension

///////////////////////////////////////////////////////////////////////
// methods used in ConstructNewHeavyMet
///////////////////////////////////////////////////////////////////////
G4SubtractionSolid* DetectionSystemGriffin::newHeavyMet()
{
  G4double half_thickness_z = ((this->hevimet_tip_thickness/cos(this->hevimet_tip_angle))
  	* cos(this->bent_end_angle -this->hevimet_tip_angle) 
	+ this->forward_inner_radius*tan(this->hevimet_tip_angle)
	* sin(this->bent_end_angle))/2.0; 

  G4double half_length_shorter_x 	= this->forward_inner_radius*sin(this->bent_end_angle);
  G4double half_length_longer_x 	= half_length_shorter_x +2.0*half_thickness_z*tan(this->bent_end_angle);
  G4double  half_length_shorter_y 	= half_length_shorter_x;
  G4double half_length_longer_y 	= half_length_longer_x;

  G4Trd* uncut_hevimet = new G4Trd("uncut_hevimet", half_length_longer_x, 
  	half_length_shorter_x, half_length_longer_y, half_length_shorter_y, half_thickness_z);

  G4double half_shorter_length_x = half_length_shorter_x 
  	- this->forward_inner_radius*tan(this->hevimet_tip_angle)*cos(this->bent_end_angle);

  G4double half_height_z = (2.0*half_thickness_z +(half_length_longer_x 
  	- ((this->forward_inner_radius +this->hevimet_tip_thickness)
	* tan(this->hevimet_tip_angle))/cos(this->bent_end_angle) 
	- half_shorter_length_x)*tan(this->bent_end_angle))/2.0;

  G4double half_longer_length_x 	= half_shorter_length_x +2.0*half_height_z/tan(this->bent_end_angle);
  G4double half_shorter_length_y 	= half_shorter_length_x;
  G4double half_longer_length_y 	= half_longer_length_x;

  G4Trd* intersector = new G4Trd("intersector", half_shorter_length_x, 
  	half_longer_length_x, half_shorter_length_y, half_longer_length_y, half_height_z);

  G4Trd* chopper = new G4Trd("chopper", half_shorter_length_x, half_longer_length_x,
  	half_shorter_length_y, half_longer_length_y, half_height_z);

  G4ThreeVector move_chopper(0.0, 0.0, half_height_z +half_thickness_z 
  	- this->forward_inner_radius*tan(this->hevimet_tip_angle)
	* sin(this->bent_end_angle));
	
  G4SubtractionSolid* chopped_hevimet = new G4SubtractionSolid("chopped_hevimet", 
  	uncut_hevimet, chopper, 0, move_chopper);   

  G4ThreeVector move_intersector(0.0, 0.0, -half_height_z +half_thickness_z);

  G4IntersectionSolid* intersected_hevimet = new G4IntersectionSolid("intersected_hevimet", 
  	chopped_hevimet, intersector, 0, move_intersector);

  G4double longer_half_length = half_length_longer_x -((this->forward_inner_radius 
  	+ this->hevimet_tip_thickness)*tan(this->hevimet_tip_angle))/cos(this->bent_end_angle);   

  G4double shorter_half_length = longer_half_length -2.0*(half_thickness_z +0.1*mm)
  	* tan(this->bent_end_angle -this->hevimet_tip_angle);

  G4Trd* middle_chopper = new G4Trd("middle_chopper", longer_half_length, shorter_half_length, 
  	longer_half_length, shorter_half_length, half_thickness_z +0.1*mm);

  G4SubtractionSolid* hevimet = new G4SubtractionSolid("hevimet", intersected_hevimet, middle_chopper);

  return hevimet;

}//end ::newHeavyMet

