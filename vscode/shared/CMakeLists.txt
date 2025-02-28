cmake_minimum_required(VERSION 3.10.0)
project(communicator)

set(OSBitness 32)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()

set (FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs") #static libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}") #shared libs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}") #executable files

include_directories(. ./libudt 
                    #./Vegas_MLP_13
                    ./project_pure_udt 
                    ./TCP_Socket 
                    ./project_feature_saver
                    ./project_feature_extractor)

#link_directories(${CMAKE_SOURCE_DIR}/lib)

find_library(UDT_LIB 
             NAMES udt 
             PATHS ${CMAKE_SOURCE_DIR}/libudt)


#Veja que temos que ter um find_library para as
#libs que são construídas por nós. No caso aqui 
# correspondem aos diretorios TCP_Socket e VegasMLP13
#cuidado com tab e espaços.
#find_library(VEGAS_MLP_13 
#            NAMES VegasMLP13 
#            PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

find_library(PURE_UDT 
            NAMES tcpsocket 
            PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

find_library(TCP_SOCKET 
            NAMES tcpsocket 
            PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

find_library(FEATURE_EXTRACTOR 
            NAMES feature_extractor 
            PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

#find_library(FEATURE_SAVER 
#            NAMES feature_saver 
#            PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})


#add_subdirectory(VegasMLP13) #necessário se tiver que gerar a lib, como nesse caso
add_subdirectory(project_pure_udt)
add_subdirectory(TCP_Socket)
add_subdirectory(project_feature_saver)
add_subdirectory(project_feature_extractor)


#add_library(console SHARED console.cpp)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} 
                      ${UDT_LIB} 
                      ${PURE_UDT} 
                      ${TCP_SOCKET}
                      ${FEATURE_SAVER} 
                      ${FEATURE_EXTRACTOR})