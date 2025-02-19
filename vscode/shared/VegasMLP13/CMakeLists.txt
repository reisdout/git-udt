cmake_minimum_required(VERSION 3.10.0)
project(VegasMLP13 VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)


set (FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static libs") #static libs
                        
include_directories(. include ${CMAKE_SOURCE_DIR}/project_feature_saver/include) #evita ter que dar o caminho dos .h nos includes

find_library(UDT_LIB 
             NAMES udt 
             PATHS ${CMAKE_SOURCE_DIR}/libudt)


#Precisando da lib de mesmo nível, manda achar
find_library(FEATURE_SAVER 
             NAMES feature_saver 
             PATHS ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

#As duas linhas seguintes incluem oss elementos da lib de 
#memso nível

 
add_library(${PROJECT_NAME} ${ProjectType} src/VegasMLP13.cpp) #cria a lib

target_link_libraries(${PROJECT_NAME} ${FEATURE_SAVER})
target_include_directories(${PROJECT_NAME} PRIVATE ../project_feature_saver/include)             
