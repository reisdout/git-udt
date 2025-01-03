cmake_minimum_required(VERSION 3.10.0)
project(VegasMLP13 VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)

include_directories(. include) #evita ter que dar o caminho dos .h nos includes

find_library(UDT_LIB 
             NAMES udt 
             PATHS ${CMAKE_SOURCE_DIR}/libudt)


add_library(${PROJECT_NAME} ${ProjectType} src/VegasMLP13.cpp) #cria a lib

