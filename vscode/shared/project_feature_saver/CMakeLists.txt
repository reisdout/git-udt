cmake_minimum_required(VERSION 3.10.0)
project(feature_saver VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)

include_directories(. include ${CMAKE_SOURCE_DIR}/defines ${CMAKE_SOURCE_DIR}/project_mrs_utils/include) #evita ter que dar o caminho dos .h nos includes

add_library(${PROJECT_NAME} ${ProjectType} src/class_feature_saver.cpp
                                           src/class_feature_saver_udt.cpp
                                           src/class_feature_saver_tcp.cpp) #cria a lib

target_include_directories(${PROJECT_NAME} PRIVATE include)