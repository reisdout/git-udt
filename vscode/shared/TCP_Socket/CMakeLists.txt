cmake_minimum_required(VERSION 3.10.0)
project(tcpsocket VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)

include_directories(. include ${CMAKE_SOURCE_DIR}/defines ${CMAKE_SOURCE_DIR}/project_feature_saver/include) #evita ter que dar o caminho dos .h nos includes


find_library(FEATURE_SAVER 
             NAMES feature_saver 
             PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})



add_library(${PROJECT_NAME} ${ProjectType} src/tcp_server.cpp src/tcp_client.cpp) #cria a lib
target_link_libraries(${PROJECT_NAME} ${FEATURE_SAVER})
target_include_directories(${PROJECT_NAME} PRIVATE ../project_feature_saver/include) 

