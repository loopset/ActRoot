#-----ActRoot-----

# Setup the project
cmake_minimum_required(VERSION 3.20 FATAL_ERROR) #need to look at this
project(ActRoot)

# stablish c++ version
set(CMAKE_CXX_STANDARD 17) # same as ROOT built for Arch Linux
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(ROOT 6.20 CONFIG REQUIRED)

## because it is located outside this project
include_directories(/media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/MyEvents/inc)

set(sources
  ${CMAKE_SOURCE_DIR}/src/ActHit.cpp
  ${CMAKE_SOURCE_DIR}/src/ActTrack.cpp
  ${CMAKE_SOURCE_DIR}/src/ActClusteringResults.cpp
  ${CMAKE_SOURCE_DIR}/src/ActLine.cpp
  ${CMAKE_SOURCE_DIR}/src/ActSample.cpp
  ${CMAKE_SOURCE_DIR}/src/ActRANSAC.cpp
  ${CMAKE_SOURCE_DIR}/src/ActDraw.cpp
  ${CMAKE_SOURCE_DIR}/src/ActCalibrations.cpp
  ${CMAKE_SOURCE_DIR}/src/Silicons.cpp
  #${CMAKE_SOURCE_DIR}/src/ActTrackPhysics.cpp
  ${CMAKE_SOURCE_DIR}/src/ActEvent.cpp
)

set(headers
  ${CMAKE_SOURCE_DIR}/inc/ActParameters.h
  ${CMAKE_SOURCE_DIR}/inc/ActHit.h
  ${CMAKE_SOURCE_DIR}/inc/ActTrack.h
  ${CMAKE_SOURCE_DIR}/inc/ActClusteringResults.h
  ${CMAKE_SOURCE_DIR}/inc/ActLine.h
  ${CMAKE_SOURCE_DIR}/inc/ActSample.h
  ${CMAKE_SOURCE_DIR}/inc/ActRANSAC.h
  ${CMAKE_SOURCE_DIR}/inc/ActDraw.h
  ${CMAKE_SOURCE_DIR}/inc/ActCalibrations.h
  ${CMAKE_SOURCE_DIR}/inc/Silicons.h
  ${CMAKE_SOURCE_DIR}/inc/TriggersAndGates.h
  ${CMAKE_SOURCE_DIR}/inc/TrackPhysics.h
  ${CMAKE_SOURCE_DIR}/inc/ActEvent.h
)

# ActRoot LinkDef and dict name
set(LINKDEF_LOCATION ${CMAKE_SOURCE_DIR}/src/ActRootLinkDef.h )
set(DICTIONARY_NAME G__${PROJECT_NAME})

ROOT_GENERATE_DICTIONARY(${DICTIONARY_NAME} ${headers} LINKDEF  ${LINKDEF_LOCATION})

##link with MyActEvents
find_library(MYACTARTEVENTS NAMES MyActarEvents PATHS /media/Datos/ApuntesUSC/TESE/ACTAR_tracking/analysis_2022/ACTAR_ANALYSIS_LIGHT_root6/MyEvents/build)
# and add library
set(TARGET_NAME ${PROJECT_NAME})
add_library(${TARGET_NAME} SHARED ${sources} ${headers} ${DICTIONARY_NAME}.cxx)
## add ActRoot headers
target_include_directories(${TARGET_NAME} BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/inc)
# setup linking libraries
target_link_libraries(${TARGET_NAME} ${ROOT_LIBRARIES} ${MYACTAREVENTS})
