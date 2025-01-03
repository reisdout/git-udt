cmake_minimum_required(VERSION 3.10.0)
project(udt)

set(OSBitness 32)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()

set (FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs") #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}") #shared libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}") #executable files

include_directories(. ./libudt ./VEGAS_MLP_13)

#link_directories(${CMAKE_SOURCE_DIR}/lib)

find_library(UDT_LIB 
             NAMES udt 
             PATHS ${CMAKE_SOURCE_DIR}/libudt)

add_subdirectory(VegasMLP13) #necessário se tiver que gerar a lib, como nesse caso



#add_library(console SHARED console.cpp)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${UDT_LIB} ${VEGGAS_MLP_13})