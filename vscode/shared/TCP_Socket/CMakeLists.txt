cmake_minimum_required(VERSION 3.10.0)
project(tcpsocket VERSION 2.8.0)

set(ProjectType STATIC) #pode ser SHARED ou STATIC. Essa variavel sera passada
                        #no add_library para dizer se dinamica (shared/.so) 
                        #ou estática (.a)

include_directories(. include) #evita ter que dar o caminho dos .h nos includes


add_library(${PROJECT_NAME} ${ProjectType} src/tcp_server.cpp src/tcp_client.cpp) #cria a lib

