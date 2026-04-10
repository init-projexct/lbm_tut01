cmake_minimum_required(VERSION 3.10)
project(lbm_tut01)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PALABOS_ROOT /home/researchx/apps/palabos)

# Use SYSTEM for libraries to hide their internal warnings
include_directories(SYSTEM ${PALABOS_ROOT}/src)
include_directories(SYSTEM ${PALABOS_ROOT}/externalLibraries)

# 1. COLLECT ALL SOURCES
file(GLOB_RECURSE PALABOS_SOURCES 
    "${PALABOS_ROOT}/src/*.cpp"
    "${PALABOS_ROOT}/externalLibraries/*.cpp"
)

# 2. CREATE EXECUTABLE
add_executable(main main.cpp ${PALABOS_SOURCES})

# 3. COMPILER OPTIONS - The "Victory" Flags
target_compile_options(main PRIVATE 
    -O3 
    -march=native
    -fpermissive
    -Wno-narrowing
    -Wno-ignored-attributes
    -Wno-deprecated-enum-enum-conversion
)

# 4. LINKING
find_package(MPI REQUIRED)
target_link_libraries(main PRIVATE MPI::MPI_CXX)

# TinyXML Linkage
target_link_libraries(main PRIVATE tinyxml)
