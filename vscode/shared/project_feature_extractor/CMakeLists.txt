cmake_minimum_required(VERSION 3.10.0)
project(feature_extractor VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)

include_directories(. include) #evita ter que dar o caminho dos .h nos includes

add_library(${PROJECT_NAME} ${ProjectType} src/class_feature_extractor.cpp
                                           src/class_feature_extractor_udt.cpp) #cria a lib

target_include_directories(${PROJECT_NAME} PRIVATE include)