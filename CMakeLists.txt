cmake_minimum_required(VERSION 3.10)
project(lbm_tut01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PALABOS_ROOT /home/researchx/apps/palabos)

# Find Dependencies
find_package(MPI REQUIRED)
find_package(TBB REQUIRED)

# Essential Palabos Definitions
add_definitions(-DPLB_MPI_PARALLEL)
add_definitions(-DPLB_USE_POSIX)
add_definitions(-DPLB_USE_CPP20)

# Include Paths - CRITICAL FIX HERE
include_directories(SYSTEM "${PALABOS_ROOT}/src")
include_directories(SYSTEM "${PALABOS_ROOT}/externalLibraries")
include_directories(SYSTEM "${PALABOS_ROOT}/externalLibraries/Eigen3")
# Tell Palabos where to find mpi.h
include_directories(SYSTEM ${MPI_CXX_INCLUDE_DIRS}) 

# Collect Sources
file(GLOB_RECURSE PALABOS_SRC "${PALABOS_ROOT}/src/*.cpp")
file(GLOB_RECURSE EXT_SRC "${PALABOS_ROOT}/externalLibraries/tinyxml/*.cpp")

# Build Palabos Library
add_library(palabos STATIC ${PALABOS_SRC} ${EXT_SRC})

# Compiler Flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -fpermissive")
endif()

# Build Executable
add_executable(test01 test01.cpp)

# Linking sequence
target_link_libraries(test01 
    palabos 
    ${MPI_CXX_LIBRARIES} 
    TBB::tbb 
    pthread
)
