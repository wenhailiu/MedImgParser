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

#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

#ifdef ROS_VERSION_MAJOR
#include "ros/ros.h"
#include "geometry_msgs/PoseStamped.h"
#endif

namespace Utilities{

template<typename T> extern void readFromBin(T *Output, int Num_Elements, const std::string FILENAME);
template<typename T> extern void writeToBin(T *Output, int Num_Elements, const std::string FILENAME);

class Matrix{

public: 
    Matrix(const int def_rows, const int def_cols); 

private: 
    std::vector<float> InternalData; 
    int Rows; 
    int Cols; 

}; 

//Matrix 4 X 4: 
extern bool MatrixS4X4Invert(const float m[16], float invOut[16]);
extern void MatrixMatrixS4X4Multiply(const float m_i_1[16], const float m_i_2[16], float m_o[16]);
extern void MatrixPointS4X4Multiply(const float m_i[16], const float p_i[4], float p_o[4]);
extern void MatrixS4X4Identity(std::vector<float>& InputMatrix);
extern void MatrixS4X4Identity(std::vector<std::vector<float>>& InputMatrix);
extern std::vector<float> MatrixVector2D_Deserializer(const std::vector<std::vector<float>>& inputMatrix);
extern void MatrixS4X4Print(const std::vector<float> InputMatrix);
extern void MatrixS4X4Print(const float *InputMatrix);

//Matrix 3 X 3: 
extern bool MatrixS3X3Invert(const float m[9], float invOut[9]);
extern void MatrixS3X3Print(const std::vector<float> InputMatrix);
extern void MatrixS3X3Print(const float *InputMatrix);
template<typename T> extern void MatrixDeserialization(const std::vector<std::vector<T>>& InputMatrix, T* OutputArray); 

class MyTimer{
public:
    MyTimer();
    void tic();
    void toc();
    double Duration(std::ostream& os);
    double Duration();
    double GetFPS(); 

private:
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> begin;
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> end;

    bool BeginSet;
    bool EndSet;
    bool DurationSet; 
    double duration;
    double FPS; 
};

//Script for string: 
extern std::vector<std::string> Split(const std::string &s, char delim); 
extern std::string GetFullFileName(const std::string &Path); 
extern std::string GetFileExtension(const std::string &Path); 

#ifdef ROS_VERSION_MAJOR

extern void RosGeoMsgToMatrixS4X4(const geometry_msgs::PoseStampedConstPtr& RosGeoMsg, std::vector<float>& OutputVector);

#endif

}//End namespace: Utilities



#endif