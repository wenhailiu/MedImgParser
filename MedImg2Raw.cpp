#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "MedImgParser.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[]){

    // int dimX, dimY, dimZ; 
    // float spacingX, spacingY, spacingZ; 
    // float originX, originY, originZ; 

    // std::vector<float> data; 

    // MedImageParser::read_nii("/home/wenhai/img_registration_ws/MNI_BITE/group2/01/01a_us_tal.nii", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    // MedImageParser::read_dicom("/home/wenhai/Dropbox/CMS/Data/KidneyPhantom/CT_CoronalViewFToB.dcm", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    // MedImageParser::read_nrrd("/home/wenhai/img_registration_ws/3d_FUSION_DATA/Origin/Ultrasound.nrrd", dimX, dimY, dimZ, spacingX, spacingY, spacingZ, originX, originY, originZ, data); 

    std::string FilePath; 
    if(argc == 1){
        std::cout << "Usage: MedImg2Raw FILEPATH(*.dcm || *.nii || *.nii.gz || *.nrrd)" << std::endl; 
    }
    else{
        
        FilePath = argv[1]; 

        //Determine if the format is supported: 
        //Medical volumetric image: 
        if(
            (FilePath.find(".nrrd") != std::string::npos) || 
            (FilePath.find(".dcm") != std::string::npos) || 
            (FilePath.find(".nii") != std::string::npos) || 
            (FilePath.find(".nii.gz") != std::string::npos)){
            
            std::cout << "Medical image was detected!" << std::endl; 
            MedicalImageIO imageIO(FilePath); 
            if(imageIO.ReadableCheck()){
                imageIO.Read(); 
                imageIO.DumpInfo(); 
                imageIO.DumpToRaw(); 
            }
        }
        //2D image: 
        else if(
            (FilePath.find(".png") != std::string::npos) || 
            (FilePath.find(".PNG") != std::string::npos) || 
            (FilePath.find(".jpg") != std::string::npos) || 
            (FilePath.find(".JPG") != std::string::npos) || 
            (FilePath.find(".jpeg") != std::string::npos) || 
            (FilePath.find(".JPEG") != std::string::npos) || 
            (FilePath.find(".bmp") != std::string::npos) || 
            (FilePath.find(".BMP") != std::string::npos)){
            std::cout << "2D image was detected!" << std::endl; 

            //implement: 
            int imgWidth, imgHeight, imgChannels; 
            unsigned char *imgData = stbi_load(FilePath.c_str(), &imgWidth, &imgHeight, &imgChannels, 1); 
            std::cout << "Dimension: " << imgWidth << ", " << imgHeight << std::endl; 

            std::string baseName(
                FilePath.begin(), 
                FilePath.begin() + FilePath.find_last_of("/")
            ); 

            std::string fileName(
                FilePath.begin() + FilePath.find_last_of("/"), 
                FilePath.end()
            ); 

            std::string fileBaseName(
                fileName.begin(), 
                fileName.begin() + fileName.find_first_of(".")
            ); 

            std::string rawName = baseName + fileBaseName + ".raw"; 

            std::vector<float> RawData(imgWidth * imgHeight, 0.0f); 
            std::copy(imgData, imgData + imgWidth * imgHeight, RawData.data()); 

            std::ofstream rawFile(rawName, std::ofstream::out | std::ofstream::binary); 
            rawFile.write((char*)RawData.data(), imgWidth * imgHeight * sizeof(float)); 
            std::cout << "Raw image was saved at: " << rawName << std::endl; 
            rawFile.close(); 

            stbi_image_free(imgData); 
        }
        else{
            std::cout << "Format was unsupported!" << std::endl; 
        }
    }

    return 0; 
}