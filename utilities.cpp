/*
    utilities.cpp
    Purpose: A collection of utility functions for C++ programming. 

    Content: 
    1. Binary file read and write: readFromBin, writeToBin. 
    2. Matrix 4X4 operations: Invert, M-M-Multiply, M-P-Multiply, Identity, 2DVetorDeserializer, Print. 
    3. Matrix 3X3 operations: Invert, Print. 
    4. Timer: class MyTimer, to get elapstime, FPS.  
    5. String operations: Split, GetFullFileName(including extension, get: "aaa.bbb" ), GetFileExtension(get: ".xxx" )
    6. ROS related operations: RosGeoMsgToMatrixS4X4(geometry_msgs::PoseStamped TO 4X4 transform matrix). 

    @author: Wenhai Liu
    @version: 1.1 06/02/2020
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <functional>
#include <numeric>
#ifdef ROS_VERSION_MAJOR
#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"
#endif

#include "utilities.h"

namespace Utilities{

/* ------------------------------------Basic binary file IOs------------------------------------ */
//Read binary files from disk, with Number of elements.
template<typename T> 
void readFromBin(T *Output, int Num_Elements, const std::string FILENAME) {
	std::ifstream InputStream;
	InputStream.open(FILENAME, std::ios::in | std::ios::binary);

	if (!InputStream.good()) {
		std::cout << "Failed to open " << FILENAME << std::endl;
		exit(0);
	}

	InputStream.read(reinterpret_cast<char*>(Output), sizeof(T) * Num_Elements);
	Output = reinterpret_cast<T*>(Output);

	InputStream.close();
}
template void readFromBin<float>(float *Output, int Num_Elements, const std::string FILENAME);
template void readFromBin<int>(int *Output, int Num_Elements, const std::string FILENAME);
template void readFromBin<uint8_t>(uint8_t *Output, int Num_Elements, const std::string FILENAME);

//Write binary files to disk, with Number of elements.
template<typename T>
void writeToBin(T *Output, int Num_Elements, const std::string FILENAME) {
	std::ofstream OutputStream;
	OutputStream.open(FILENAME, std::ios::out | std::ios::binary);

	if (!OutputStream.good()) {
		std::cout << "Failed to open " << FILENAME << std::endl;
		exit(0);
	}

	OutputStream.write(reinterpret_cast<char*>(Output), sizeof(T) * Num_Elements);

	OutputStream.close();
}
template void writeToBin<float>(float *Output, int Num_Elements, const std::string FILENAME);
template void writeToBin<int>(int *Output, int Num_Elements, const std::string FILENAME);
template void writeToBin<uint8_t>(uint8_t *Output, int Num_Elements, const std::string FILENAME);

/* ------------------------------------ basic Math functions ------------------------------------ */
bool MatrixS4X4Invert(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    invOut[12] = 0.0f;
    invOut[13] = 0.0f;
    invOut[14] = 0.0f;
    invOut[15] = 1.0f;

    return true;
}

void MatrixMatrixS4X4Multiply(const float m_i_1[16], const float m_i_2[16], float m_o[16]){
    float accumulator = 0.0f;
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int col_it = 0; col_it < 4; ++col_it){
            for(int ele_it = 0; ele_it < 4; ++ele_it){
                accumulator += m_i_1[row_it * 4 + ele_it] * m_i_2[ele_it * 4 + col_it];
            }
            m_o[col_it + row_it * 4] = accumulator;
            accumulator = 0.0f;
        }
    }
}

void MatrixPointS4X4Multiply(const float m_i[16], const float p_i[4], float p_o[4]){
    float accumulator = 0.0f;
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int ele_it = 0; ele_it < 4; ++ele_it){
            accumulator += m_i[ele_it + row_it * 4] * p_i[ele_it];
        }
        p_o[row_it] = accumulator;
        accumulator = 0.0f;
    }
}

void MatrixS4X4Identity(std::vector<float>& InputMatrix){
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int col_it = 0; col_it < 4; ++col_it){
            if(row_it == col_it){
                InputMatrix[col_it + row_it * 4] = 1.0f;
            }
            else{
                InputMatrix[col_it + row_it * 4] = 0.0f;
            }
        }
    }
}

void MatrixS4X4Identity(std::vector<std::vector<float>>& InputMatrix){
    for(int Matrix_it = 0; Matrix_it < InputMatrix.size(); ++Matrix_it){
        for(int row_it = 0; row_it < 4; ++row_it){
            for(int col_it = 0; col_it < 4; ++col_it){
                if(row_it == col_it){
                    InputMatrix[Matrix_it][col_it + row_it * 4] = 1.0f;
                }
                else{
                    InputMatrix[Matrix_it][col_it + row_it * 4] = 0.0f;
                }
            }
        }
    }
}

//Matrix 2D-Vector deserializer:
std::vector<float> MatrixVector2D_Deserializer(const std::vector<std::vector<float>>& inputMatrix){
    std::vector<float> outputMatrix(16, 0.0f);
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int col_it = 0; col_it < 4; ++col_it){
            outputMatrix[col_it + row_it * 4] = inputMatrix[row_it][col_it];
        }
    }
    return outputMatrix;
}

//Display 2d matrix:
void MatrixS4X4Print(const std::vector<float> InputMatrix){
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int col_it = 0; col_it < 4; ++col_it){
            std::cout << std::setw(15) << InputMatrix[col_it + row_it * 4] << std::setw(15);
        }
        std::cout << std::endl;
    }
}

void MatrixS4X4Print(const float *InputMatrix){
    for(int row_it = 0; row_it < 4; ++row_it){
        for(int col_it = 0; col_it < 4; ++col_it){
            std::cout << std::setw(15) << InputMatrix[col_it + row_it * 4] << std::setw(15);
        }
        std::cout << std::endl;
    }
}

//Matrix 3 X 3: 
bool MatrixS3X3Invert(const float m[9], float invOut[9]){
    float inv[9] = {0.0f}, det;
    inv[0] = m[4] * m[8] - m[7] * m[5];
    inv[1] = m[2] * m[7] - m[8] * m[1];
    inv[2] = m[1] * m[5] - m[2] * m[4];

    inv[3] = m[5] * m[6] - m[3] * m[8];
    inv[4] = m[0] * m[8] - m[2] * m[6];
    inv[5] = m[2] * m[3] - m[0] * m[5];

    inv[6] = m[3] * m[7] - m[6] * m[4];
    inv[7] = m[1] * m[6] - m[0] * m[7];
    inv[8] = m[0] * m[4] - m[1] * m[3];

    det = m[0] * inv[0] + m[1] * inv[3] + m[2] * inv[6];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (int i = 0; i < 9; i++){
        invOut[i] = inv[i] * det;
    }
        
    invOut[6] = 0.0f; 
    invOut[7] = 0.0f; 
    invOut[8] = 1.0f; 

    return true;
}

void MatrixS3X3Print(const std::vector<float> InputMatrix){
    for(int row_it = 0; row_it < 3; ++row_it){
        for(int col_it = 0; col_it < 3; ++col_it){
            std::cout << std::setw(15) << InputMatrix[col_it + row_it * 3] << std::setw(15);
        }
        std::cout << std::endl;
    }
}

void MatrixS3X3Print(const float *InputMatrix){
    for(int row_it = 0; row_it < 3; ++row_it){
        for(int col_it = 0; col_it < 3; ++col_it){
            std::cout << std::setw(15) << InputMatrix[col_it + row_it * 3] << std::setw(15);
        }
        std::cout << std::endl;
    }
}

template<typename T>
void MatrixDeserialization(const std::vector<std::vector<T>>& InputMatrix, T* OutputArray){
	for(int row_it = 0; row_it < InputMatrix.size(); ++row_it){
		for(int col_it = 0; col_it < InputMatrix[0].size(); ++col_it){
			OutputArray[col_it + row_it * InputMatrix[0].size()] = InputMatrix[row_it][col_it];
		}
	}
}
template void MatrixDeserialization<float>(const std::vector<std::vector<float>>& InputMatrix, float* OutputArray); 
template void MatrixDeserialization<double>(const std::vector<std::vector<double>>& InputMatrix, double* OutputArray); 
template void MatrixDeserialization<int>(const std::vector<std::vector<int>>& InputMatrix, int* OutputArray); 

/* ------------------------------------ Utilities functions ------------------------------------ */
MyTimer::MyTimer(){

    BeginSet = false;
    EndSet = false;
    DurationSet = false; 

    duration = 0.0;
}
void MyTimer::tic(){
    BeginSet = true;
    DurationSet = false; 
    begin = std::chrono::high_resolution_clock::now();
}
void MyTimer::toc(){
    end = std::chrono::high_resolution_clock::now();
    EndSet = true;
    DurationSet = false; 
}
double MyTimer::Duration(std::ostream& os){
    if(BeginSet && EndSet){
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1.0e6;
        os << "Elasped time: " << duration << "[ms]. \n";
        BeginSet = false;
        EndSet = false;
        DurationSet = true; 

        return duration;
    }
    else{
        os << "Please set timestamp! \n";
        return 0.0;
    }
}
double MyTimer::Duration(){
    if(BeginSet && EndSet){
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1.0e6;
        BeginSet = false;
        EndSet = false;
        DurationSet = true; 

        return duration;
    }
    else{
        std::cout << "Please set timestamp! \n";
        return 0.0;
    }
}
double MyTimer::GetFPS(){
    if(DurationSet){
        FPS = 1e3 / duration; 
        return FPS; 
    }
    else{
        std::cout << "Please Set Duration! " << std::endl; 
    }
}

template <typename Out>
void Split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> Split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    Split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string GetFullFileName(const std::string &Path){
    std::string FullName( 
        Path.begin() + Path.find_last_of("/") + 1, 
        Path.end()
    ); 

    return FullName; 
}

std::string GetFileExtension(const std::string &Path){
    std::string FullName = GetFullFileName(Path); 

    std::string Extension( 
        FullName.begin() + FullName.find_first_of("."), 
        FullName.end()
    ); 

    return Extension; 
}

#ifdef ROS_VERSION_MAJOR
void RosGeoMsgToMatrixS4X4(const geometry_msgs::PoseStampedConstPtr& RosGeoMsg, std::vector<float>& OutputVector){
	OutputVector.resize(16, 0.0f); 
    
    //Rotation: 
	float Quat[4] = {(float)RosGeoMsg->pose.orientation.w, (float)RosGeoMsg->pose.orientation.x, (float)RosGeoMsg->pose.orientation.y, (float)RosGeoMsg->pose.orientation.z};
	Utilities::QuaternionToMatrixS4X4(Quat, OutputVector.data());

	//Translation: 
	OutputVector[3] = (float)RosGeoMsg->pose.position.x * 1000.0f; 
	OutputVector[7] = (float)RosGeoMsg->pose.position.y * 1000.0f; 
	OutputVector[11] = (float)RosGeoMsg->pose.position.z * 1000.0f; 

	//Regularization:
	OutputVector[12] = 0.0f; OutputVector[13] = 0.0f; OutputVector[14] = 0.0f; OutputVector[15] = 1.0f; 
}
#endif

}//End namespace.