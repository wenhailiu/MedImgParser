#ifndef MEDIMGPARSER
#define MEDIMGPARSER

#include <iostream>
#include <vector>
#include <string>

namespace MedImageParser
{


/* ------------------------------ IO routine for NIfTI (Neuroimaging Informatics Technology Initiative) ---------------------------- */ 
bool read_nii( 
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, std::vector<float>& ImageBuff); 


/* ------------------------------ IO routine for DICOM ---------------------------- */ 
bool read_dicom( 
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, std::vector<float>& ImageBuff); 


/* ------------------------------ IO routine for nrrd ---------------------------- */ 
bool read_nrrd( 
    const char *filename, 
    int& dimX, int& dimY, int& dimZ, 
    float& spacingX, float& spacingY, float& spacingZ, 
    float& originX, float& originY, float& originZ, std::vector<float>& ImageBuff); 

}



class MedicalImageIO{

public: 
    MedicalImageIO(); 
    MedicalImageIO(std::string _filePath); 
    MedicalImageIO(const char *_filePath); 

    bool ReadableCheck(); 

    bool BufferAvailable(); 
    bool HeaderAvailable(); 

    bool Read(); 

    void DumpBufferOut(std::vector<float>& output); 
    void DumpInfo(); 
    void DumpToRaw(); 
    void DumpToRaw(std::string raw_path); 

    void GetDimension(int& dimX, int& dimY, int& dimZ); 
    void GetDimension(int _dim[3]); 
    void GetSpacing(float& spacingX, float& spacingY, float& spacingZ); 
    void GetSpacing(float _spacing[3]); 
    void GetOrigin(float& originX, float& originY, float& originZ); 
    void GetOrigin(float _origin[3]); 

private: 
    //geometry parameter: 
    int dimension[3]; 
    float spacing[3]; 
    float origin[3]; 

    //path: 
    std::string filePath; 
    std::string fileName; 
    std::string fileExtension; 
    std::vector<std::string> readableExtensions; 

    //data buffer: 
    std::vector<float> dataBuffer; 

    //flags: 
    bool isReadable; 
    bool isParsed; 
    bool isBufferAvailable; 
    bool isHeaderAvailable; 
}; 

#endif