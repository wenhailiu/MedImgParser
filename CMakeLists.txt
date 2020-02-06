cmake_minimum_required(VERSION 3.8)
project(MedImgParser)

include_directories(nifti/)
include_directories(DICOMParser/src/)

file(GLOB_RECURSE NIFTI_READER_SOURCES "nifti/*.c*")
file(GLOB_RECURSE ZLIB_SOURCES "zlib/*.c*")
file(GLOB_RECURSE DICOMParser_SOURCES "DICOMParser/src/*.c*")
set(DICOMPARSER_NAMESPACE dicomparser)

add_compile_definitions(HAVE_ZLIB)

#Nrrd: 
file(GLOB_RECURSE NRRD_SOURCES "NrrdIO/*.c*") 

ADD_DEFINITIONS(-DTEEM_BUILD=1)
ADD_DEFINITIONS(-DTEEM_DIO=0)
ADD_DEFINITIONS(-DTEEM_ZLIB=1)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/NrrdIO/NrrdIO.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/NrrdIO/NrrdIO.h)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}/NrrdIO/
    ${CMAKE_CURRENT_SOURCE_DIR}/NrrdIO/
)

include(${CMAKE_CURRENT_SOURCE_DIR}/NrrdIO/CMake/TestQnanhibit.cmake)
test_qnanhibit(QNANHIBIT_VALUE ${CMAKE_CURRENT_SOURCE_DIR}/NrrdIO/CMake)
if(QNANHIBIT_VALUE)
   set(QNANHIBIT 0 CACHE INTERNAL "The 22nd bit of 32-bit floating-point quiet NaN.")
else(QNANHIBIT_VALUE)
   set(QNANHIBIT 1 CACHE INTERNAL "The 22nd bit of 32-bit floating-point quiet NaN.")
endif(QNANHIBIT_VALUE)

IF(QNANHIBIT)
  ADD_DEFINITIONS(-DTEEM_QNANHIBIT=0)
ELSE(QNANHIBIT)
  ADD_DEFINITIONS(-DTEEM_QNANHIBIT=1)
ENDIF(QNANHIBIT)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#end NRRD

add_library(
    MedImgParser STATIC 
    MedImgParser.cpp 
    utilities.cpp
    ${NIFTI_READER_SOURCES} 
    ${ZLIB_SOURCES} 
    ${DICOMParser_SOURCES}
    ${NRRD_SOURCES}
)

add_executable(MedImg2Raw MedImg2Raw.cpp)
target_link_libraries(MedImg2Raw MedImgParser)
