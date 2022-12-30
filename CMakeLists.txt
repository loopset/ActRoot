#-----ActRoot-----

# Setup the project
cmake_minimum_required(VERSION 3.20 FATAL_ERROR) #minimum version of CMAKE required
project(ActRoot)

# stablish c++ version
set(CMAKE_CXX_STANDARD 17) # same as ROOT built for Arch Linux
set(CMAKE_CXX_STANDARD_REQUIRED TRUE) # force require this standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ROOT 6.20 CONFIG REQUIRED)

## because it is located outside this project! keep in mind directory structure so this can be feasible!!
include_directories(${CMAKE_SOURCE_DIR}/../MyActarEvents/inc)
include_directories(${CMAKE_SOURCE_DIR}/../../NFS-Simulations/ActSim/inc)
## for ROOT includes in Ubuntu
include_directories(${ROOT_INCLUDE_DIRS})

## set files to compile
file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${CMAKE_SOURCE_DIR}/inc/*.h)

# ActRoot LinkDef and dict generation
set(LINKDEF_LOCATION ${CMAKE_SOURCE_DIR}/ActRootLinkDef.h)
set(DICTIONARY_NAME G__${PROJECT_NAME})

ROOT_GENERATE_DICTIONARY(${DICTIONARY_NAME} ${headers} LINKDEF  ${LINKDEF_LOCATION})

##find libMyActarEvents.so
find_library(MYACTARTEVENTS NAMES MyActarEvents PATHS ${CMAKE_SOURCE_DIR}/../MyActarEvents/build)

##find ActSim for Geomatry facilities
find_library(ACTSIM NAMES ActSim PATHS ${CMAKE_SOURCE_DIR}/../../NFS-Simulations/ActSim/build)

# add ActRoot library
set(TARGET_NAME ${PROJECT_NAME})
add_library(${TARGET_NAME} SHARED ${sources} ${headers} ${DICTIONARY_NAME}.cxx)
## add ActRoot headers so g++ knows where to locate them
target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/inc)
# link to MyActarEvents library and ROOT
target_link_libraries(${TARGET_NAME} ${ROOT_LIBRARIES} ${MYACTAREVENTS} ${ACTSIM})
