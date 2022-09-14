#-----ActRoot-----

# Setup the project
cmake_minimum_required(VERSION 3.20 FATAL_ERROR) #need to look at this
project(ActRoot)

# stablish c++ version
set(CMAKE_CXX_STANDARD 17) # same as ROOT built for Arch Linux
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Include cmake macros to, i.e., find ROOT installation
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include("${CMAKE_MODULE_PATH}/FindROOT.cmake")
#find_package(ROOT REQUIRED) #does not work in Arch Linux bc we don't have sourced thisroot.sh

# ActRoot inc directories
include_directories(${PROJECT_SOURCE_DIR}/inc)
# for MEvent and so on
# add_subdirectory(${PROJECT_SOURCE_DIR}/cobo_libs)
include_directories(${PROJECT_SOURCE_DIR}/cobo_libs/inc)
include_directories(${PROJECT_SOURCE_DIR}/cobo_libs/libMFM)
# ROOT directories
include_directories(${ROOT_INCLUDE_DIR})
# ROOT linking directories
link_directories(${ROOT_LIBRARY_DIR})

# locate files
#file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
#file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.h)
set(sources
  ${PROJECT_SOURCE_DIR}/src/ActHit.cpp
  ${PROJECT_SOURCE_DIR}/src/ActTrack.cpp
  ${PROJECT_SOURCE_DIR}/src/ActClusteringResults.cpp
  ${PROJECT_SOURCE_DIR}/src/ActLine.cpp
  ${PROJECT_SOURCE_DIR}/src/ActSample.cpp
  ${PROJECT_SOURCE_DIR}/src/ActRANSAC.cpp
  ${PROJECT_SOURCE_DIR}/src/ActDraw.cpp
  ${PROJECT_SOURCE_DIR}/src/ActCalibrations.cpp
  ${PROJECT_SOURCE_DIR}/src/ActStructs.cpp
  ${PROJECT_SOURCE_DIR}/src/ActEvent.cpp
)

set(headers
  ${PROJECT_SOURCE_DIR}/inc/ActParameters.h
  ${PROJECT_SOURCE_DIR}/inc/ActHit.h
  ${PROJECT_SOURCE_DIR}/inc/ActTrack.h
  ${PROJECT_SOURCE_DIR}/inc/ActClusteringResults.h
  ${PROJECT_SOURCE_DIR}/inc/ActLine.h
  ${PROJECT_SOURCE_DIR}/inc/ActSample.h
  ${PROJECT_SOURCE_DIR}/inc/ActRANSAC.h
  ${PROJECT_SOURCE_DIR}/inc/ActDraw.h
  ${PROJECT_SOURCE_DIR}/inc/ActCalibrations.h
  ${PROJECT_SOURCE_DIR}/inc/ActStructs.h
  ${PROJECT_SOURCE_DIR}/inc/ActEvent.h
)
# Set everything needed for the root dictonary and create the
# dictionary = headers already defined
# set(ActRoot_HEADERS
#   ${PROJECT_SOURCE_DIR}/inc/ActParameters.h
#   ${PROJECT_SOURCE_DIR}/inc/ActHit.h
#   ${PROJECT_SOURCE_DIR}/inc/ActTrack.h
#   ${PROJECT_SOURCE_DIR}/inc/ActClusteringResults.h
#   ${PROJECT_SOURCE_DIR}/inc/ActLine.h
#   ${PROJECT_SOURCE_DIR}/inc/ActRANSAC.h
#   ${PROJECT_SOURCE_DIR}/inc/ActDraw.h
# )

# ActRoot LinkDef and dict name
set(ActRoot_LINKDEF ${PROJECT_SOURCE_DIR}/src/ActRootLinkDef.h )
set(ActRoot_DICTIONARY ActRootDict.cc)

# function to generate dictonary imported from FindROOT.cmake
ROOT_GENERATE_DICTIONARY("${headers}" "${ActRoot_LINKDEF}" "${ActRoot_DICTIONARY}" "${ROOT_INCLUDE_DIR}")
# below using built-in macro (not working here as mentioned)
#ROOT_GENERATE_DICTIONARY(${ActRoot_DICTIONARY} ${headers} LINKDEF ${ActRoot_LINKDEF})

# and add library
set(TARGET_NAME ActRoot)
add_library(${TARGET_NAME} SHARED ${sources} ${headers} ${ActRoot_DICTIONARY})
# setup linking libraries
target_link_libraries(${TARGET_NAME} ${ROOT_LIBRARIES})
