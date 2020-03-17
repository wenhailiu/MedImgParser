#include "MedImgParser.h"

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "utilities.h"

//includes: 
#include "nifti2_io.h"
#include "DICOMParser/src/DICOMAppHelper.h"
#include "NrrdIO.h"

namespace MedImageParser
{

/* ------------------------------ IO routine for NIfTI (Neuroimaging Informatics Technology Initiative) ---------------------------- */ 
bool read_nii
(   
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, 
    std::vector<float>& ImageBuff
)
{
    
    nifti_image* niiImage = new nifti_image;
    niiImage = nifti_image_read(filename, true);

    if(niiImage == NULL){
        std::cout << "File: " << filename << ", failed to open. " << std::endl; 
        return false; 
    }

    //dimension: 
    dimX = niiImage->nx; dimY = niiImage->ny; dimZ = niiImage->nz; 

    //spacing: 
    spacingX = niiImage->dx; spacingY = niiImage->dy; spacingZ = niiImage->dz; 

    //origin: 
    originX = niiImage->sto_xyz.m[0][3]; originY = niiImage->sto_xyz.m[1][3]; originZ = niiImage->sto_xyz.m[2][3]; 

    //ImageData: 
    ImageBuff.resize(dimX * dimY * dimZ, 0.0f); 

    switch (niiImage->datatype)
    {
    case DT_UINT8:
        std::copy((uint8_t*)niiImage->data, (uint8_t*)niiImage->data + niiImage->nvox, ImageBuff.begin()); 
        break;

    case DT_INT16:
        std::copy((short*)niiImage->data, (short*)niiImage->data + niiImage->nvox, ImageBuff.begin()); 
        break;

    case DT_UINT16:
        std::copy((uint16_t*)niiImage->data, (uint16_t*)niiImage->data + niiImage->nvox, ImageBuff.begin()); 
        break;

    case DT_INT32:
        std::copy((int*)niiImage->data, (int*)niiImage->data + niiImage->nvox, ImageBuff.begin()); 
        break;

    case DT_FLOAT32: 
        std::copy((float*)niiImage->data, (float*)niiImage->data + niiImage->nvox, ImageBuff.begin()); 
        break; 
    
    default:
        std::cout << "Data type cannot be recongnized! " << std::endl; 
        break;
    }

    delete niiImage; 

    //correct the vertical convention: 
    std::vector<float> ImageBuff_Correct(dimX * dimY * dimZ, 0.0f); 
    for(int idxZ = 0; idxZ < dimZ; ++idxZ){
        for(int idxY = 0; idxY < dimY; ++idxY){
            std::copy(
                ImageBuff.begin() + (dimX * dimY * idxZ + (dimX * dimY) - (idxY + 1) * dimX),  
                ImageBuff.begin() + (dimX * dimY * idxZ + (dimX * dimY) - (idxY) * dimX), 
                ImageBuff_Correct.begin() + (dimX * dimY * idxZ + idxY * dimX)
            ); 
        }
    }

    std::copy(ImageBuff_Correct.begin(), ImageBuff_Correct.end(), ImageBuff.begin()); 

    return true; 
}

/* ------------------------------ IO routine for DICOM ---------------------------- */ 
bool read_dicom
(   
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, 
    std::vector<float>& ImageBuff
)
{

    std::unique_ptr<DICOMPARSER_NAMESPACE::DICOMParser> dicomHandle(new DICOMPARSER_NAMESPACE::DICOMParser); 
    std::unique_ptr<DICOMPARSER_NAMESPACE::DICOMAppHelper> dicomReader(new DICOMPARSER_NAMESPACE::DICOMAppHelper); 

    dicomHandle->ClearAllDICOMTagCallbacks(); 
    dicomReader->RegisterCallbacks(dicomHandle.get()); 
    dicomReader->RegisterPixelDataCallback(dicomHandle.get()); 

    bool isOpen = dicomHandle->OpenFile(filename); 
    if(!isOpen){
        std::cout << "File: " << filename << ", failed to open. " << std::endl; 
        return false; 
    }

    dicomHandle->ReadHeader(); 

    spacingX = dicomReader->GetPixelSpacing()[0]; 
    spacingY = dicomReader->GetPixelSpacing()[1]; 
    spacingZ = dicomReader->GetPixelSpacing()[2]; 

    dimX = dicomReader->GetDimensions()[0]; 
    dimY = dicomReader->GetDimensions()[0]; 
    dimZ = dicomReader->GetSliceNumber(); 

    originX = dicomReader->GetImagePositionPatient()[0]; 
    originY = dicomReader->GetImagePositionPatient()[1]; 
    originZ = dicomReader->GetImagePositionPatient()[2]; 
    
    void *dataBuffer = NULL; 
    DICOMPARSER_NAMESPACE::DICOMParser::VRTypes dataType; 
    unsigned long dataLength = 0;
    dicomReader->GetImageData(dataBuffer, dataType, dataLength); 

    ImageBuff.resize(dimX * dimY * dimZ, 0.0f); 
    
    switch (dataType)
    {
    case DICOMPARSER_NAMESPACE::DICOMParser::VR_OW: 
        {
            int16_t* castedInt16Buffer = static_cast<int16_t*>(dataBuffer); 
            std::copy(castedInt16Buffer, castedInt16Buffer + dataLength / 2, ImageBuff.begin()); 
        }
        
        break;

    case DICOMPARSER_NAMESPACE::DICOMParser::VR_FL: 
        {
            float* castedFPBuffer = static_cast<float*>(dataBuffer); 
            std::copy(castedFPBuffer, castedFPBuffer + dataLength / 4, ImageBuff.begin()); 
        }
        
        break;
    
    default:
        break;
    }
    
}

bool read_nrrd
(   
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, 
    std::vector<float>& ImageBuff
)
{
    Nrrd *nrrdReader = nrrdNew(); 
    
    //read file: 
    int stat = nrrdLoad(nrrdReader, filename, NULL); 
    if(stat != 0){
        std::cout << "File: " << filename << ", failed to open. " << std::endl; 
        return false; 
    }

    dimX = static_cast<int>(nrrdReader->axis[0].size); 
    dimY = static_cast<int>(nrrdReader->axis[1].size); 
    dimZ = static_cast<int>(nrrdReader->axis[2].size); 

    int vectorIncrement=0;
    if(nrrdReader->axis[0].kind==nrrdKindVector){
        vectorIncrement=1;
    }

    double spaceDir[NRRD_SPACE_DIM_MAX], spacing;

    nrrdSpacingCalculate(nrrdReader, vectorIncrement, &spacing, spaceDir); 
    spacingX = spacing; 

    nrrdSpacingCalculate(nrrdReader, 1 + vectorIncrement, &spacing, spaceDir); 
    spacingY = spacing; 

    nrrdSpacingCalculate(nrrdReader, 2 + vectorIncrement, &spacing, spaceDir); 
    spacingZ = spacing; 

    originX = static_cast<float>(nrrdReader->spaceOrigin[0]); 
    originY = static_cast<float>(nrrdReader->spaceOrigin[1]); 
    originZ = static_cast<float>(nrrdReader->spaceOrigin[2]); 

    //parse raw data: 
    ImageBuff.resize(dimX * dimY * dimZ, 0.0f); 
    switch(nrrdReader->type)
    {
        case nrrdTypeUChar:
            std::copy((u_char*)nrrdReader->data, (u_char*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeChar:
            std::copy((char*)nrrdReader->data, (char*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeUShort:
            std::copy((u_short*)nrrdReader->data, (u_short*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeShort:
            std::copy((short*)nrrdReader->data, (short*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeUInt:
            std::copy((uint*)nrrdReader->data, (uint*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeInt:
            std::copy((int*)nrrdReader->data, (int*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeFloat:
            std::copy((float*)nrrdReader->data, (float*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        case nrrdTypeDouble:
            std::copy((double*)nrrdReader->data, (double*)nrrdReader->data + (dimX * dimY * dimZ), ImageBuff.begin()); 
            break;
        default:
            std::cout << "ERROR: The data type is not supported. " << std::endl; 
            return false; 
    }

    nrrdNuke(nrrdReader); 
    return true; 
}

}

MedicalImageIO::MedicalImageIO(){
    dimension[0] = 0; dimension[1] = 0; dimension[2] = 0; 
    spacing[0] = 0.0f; spacing[1] = 0.0f; spacing[2] = 0.0f; 
    origin[0] = 0.0f; origin[1] = 0.0f; origin[2] = 0.0f; 

    filePath = ""; 
    fileName = ""; 
    fileExtension = ""; 
    readableExtensions = {".nii", ".nii.gz", ".dcm", ".nrrd"}; 

    dataBuffer.clear(); 

    isReadable = false; 
    isParsed = false; 
    isBufferAvailable = false; 
    isHeaderAvailable = false; 
}

MedicalImageIO::MedicalImageIO(std::string _filePath){
    dimension[0] = 0; dimension[1] = 0; dimension[2] = 0; 
    spacing[0] = 0.0f; spacing[1] = 0.0f; spacing[2] = 0.0f; 
    origin[0] = 0.0f; origin[1] = 0.0f; origin[2] = 0.0f; 

    filePath = _filePath; 
    fileName = Utilities::GetFullFileName(filePath); 
    fileExtension = Utilities::GetFileExtension(filePath); 
    readableExtensions = {".nii", ".nii.gz", ".dcm", ".nrrd"}; 

    dataBuffer.clear(); 

    isReadable = false; 
    isParsed = false; 
    isBufferAvailable = false; 
    isHeaderAvailable = false; 
}

MedicalImageIO::MedicalImageIO(const char *_filePath){
    dimension[0] = 0; dimension[1] = 0; dimension[2] = 0; 
    spacing[0] = 0.0f; spacing[1] = 0.0f; spacing[2] = 0.0f; 
    origin[0] = 0.0f; origin[1] = 0.0f; origin[2] = 0.0f; 

    filePath = std::string(_filePath); 
    fileName = Utilities::GetFullFileName(filePath); 
    fileExtension = Utilities::GetFileExtension(filePath); 
    readableExtensions = {".nii", ".nii.gz", ".dcm", ".nrrd"}; 

    dataBuffer.clear(); 

    isReadable = false; 
    isParsed = false; 
    isBufferAvailable = false; 
    isHeaderAvailable = false; 
}

bool MedicalImageIO::ReadableCheck(){

    if(std::find(readableExtensions.begin(), readableExtensions.end(), fileExtension) == readableExtensions.end()){
        std::cout << "File extension: " << fileExtension << " cannot be read. " << std::endl; 
        isReadable = false; 
        return false; 
    }

    isReadable = true; 
    return true; 
}

bool MedicalImageIO::BufferAvailable(){
    return isBufferAvailable; 
}

bool MedicalImageIO::HeaderAvailable(){
    return isHeaderAvailable; 
}

bool MedicalImageIO::Read(){
    if(fileExtension == ".nii" || fileExtension == ".nii.gz"){
        std::cout << "NIfTI file was parsed. " << std::endl; 
        isParsed = MedImageParser::read_nii( 
            filePath.c_str(), 
            dimension[0], dimension[1], dimension[2], 
            spacing[0], spacing[1], spacing[2], 
            origin[0], origin[1], origin[2], 
            dataBuffer); 

        isBufferAvailable = true;
        isHeaderAvailable = true; 
    }
    else if(fileExtension == ".dcm"){
        std::cout << "Dicom file was parsed. " << std::endl; 
        isParsed = MedImageParser::read_dicom( 
            filePath.c_str(), 
            dimension[0], dimension[1], dimension[2], 
            spacing[0], spacing[1], spacing[2], 
            origin[0], origin[1], origin[2], 
            dataBuffer);  

        isBufferAvailable = true;
        isHeaderAvailable = true; 
    }
    else if(fileExtension == ".nrrd"){
        std::cout << "Nrrd file was parsed. " << std::endl; 
        isParsed = MedImageParser::read_nrrd( 
            filePath.c_str(), 
            dimension[0], dimension[1], dimension[2], 
            spacing[0], spacing[1], spacing[2], 
            origin[0], origin[1], origin[2], 
            dataBuffer); 
        
        isBufferAvailable = true; 
        isHeaderAvailable = true; 
    }
    else{
        std::cout << fileName << " was not supported. " << std::endl; 
        isParsed = false; 
    }
}

void MedicalImageIO::DumpBufferOut(std::vector<float>& output){
    if(isParsed){
        output.resize(dataBuffer.size(), 0.0f); 
        std::copy(dataBuffer.begin(), dataBuffer.end(), output.begin()); 

        //clear local memory: 
        dataBuffer.clear(); 
        isBufferAvailable = false; 
    }
    else{
        std::cout << "File was not parsed. " << std::endl; 
    }
}

void MedicalImageIO::DumpInfo(){
    if(isParsed){
        std::cout << "Dimension: " << dimension[0] << ", " << dimension[1] << ", " << dimension[2] << std::endl; 
        std::cout << "Spacing: " << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << std::endl; 
        std::cout << "Origin: " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl; 
    }
    else{
        std::cout << "File was not parsed. " << std::endl; 
    }
}

void MedicalImageIO::DumpToRaw(){
    if(isParsed){
        std::string basePath(
            filePath.begin(), 
            filePath.begin() + filePath.find_last_of("/")
        ); 
        
        std::string baseName(
            fileName.begin(), 
            fileName.begin() + fileName.find_first_of(".")
        ); 

        std::string rawFilePath = basePath + "/" + baseName + ".raw"; 

        Utilities::writeToBin(dataBuffer.data(), dimension[0] * dimension[1] * dimension[2], rawFilePath); 
        std::cout << "Image file is written to " << rawFilePath << std::endl; 
    }
    else{
        std::cout << "File was not parsed. " << std::endl; 
    }
}

void MedicalImageIO::DumpToRaw(std::string raw_path){
    if(isParsed){
        size_t found = raw_path.find(".raw"); 
        if(found != std::string::npos){
            Utilities::writeToBin(dataBuffer.data(), dimension[0] * dimension[1] * dimension[2], raw_path); 
            std::cout << "Image file is written to " << raw_path << std::endl; 
        }
        else{
            std::cout << "Output file extension does not match .raw " << std::endl; 
        }
    }
    else{
        std::cout << "File was not parsed. " << std::endl; 
    }
}

void MedicalImageIO::GetDimension(int& dimX, int& dimY, int& dimZ){
    dimX = dimension[0]; 
    dimY = dimension[1]; 
    dimZ = dimension[2]; 
}

void MedicalImageIO::GetDimension(int _dim[3]){
    _dim[0] = dimension[0]; 
    _dim[1] = dimension[1]; 
    _dim[2] = dimension[2]; 
}

void MedicalImageIO::GetSpacing(float& spacingX, float& spacingY, float& spacingZ){
    spacingX = spacing[0]; 
    spacingY = spacing[1]; 
    spacingZ = spacing[2]; 
}

void MedicalImageIO::GetSpacing(float _spacing[3]){
    _spacing[0] = origin[0]; 
    _spacing[1] = origin[1]; 
    _spacing[2] = origin[2]; 
}

void MedicalImageIO::GetOrigin(float& originX, float& originY, float& originZ){
    originX = origin[0]; 
    originY = origin[1]; 
    originZ = origin[2]; 
}

void MedicalImageIO::GetOrigin(float _origin[3]){
    _origin[0] = origin[0]; 
    _origin[1] = origin[1]; 
    _origin[2] = origin[2]; 
}

float* MedicalImageIO::GetRawBuffer(){
    return dataBuffer.data(); 
}

std::string MedicalImageIO::GetFileExtension(){
    return fileExtension; 
}

std::string MedicalImageIO::GetFileName(){
    return fileName; 
}