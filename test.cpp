#include <iostream>
#include <vector>

#include "MedImgParser.h"

int main(int argc, char *argv[]){

    // int dimX, dimY, dimZ; 
    // float spacingX, spacingY, spacingZ; 
    // float originX, originY, originZ; 

    // std::vector<float> data; 

    // MedImageParser::read_nii("/home/wenhai/img_registration_ws/MNI_BITE/group2/01/01a_us_tal.nii", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    // MedImageParser::read_dicom("/home/wenhai/Dropbox/CMS/Data/KidneyPhantom/CT_CoronalViewFToB.dcm", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    // MedImageParser::read_nrrd("/home/wenhai/img_registration_ws/3d_FUSION_DATA/Origin/Ultrasound.nrrd", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    MedicalImageIO imageIO("/home/wenhai/data/Registration/ImplementedResults/06a_us_tal_origin.nrrd"); 
    if(imageIO.ReadableCheck()){
        imageIO.Read(); 
        imageIO.DumpInfo(); 
        imageIO.DumpToRaw(); 
    }

    return 0; 
}