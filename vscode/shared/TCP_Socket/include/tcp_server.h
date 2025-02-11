#ifndef TCP_SERVER
#define TCP_SERVER

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "tcp.h"

#define SERVER_BUFFER_SIZE  1024
#define SERVER_PORT  9090

using namespace std;

class TCP_Server{

public:
    TCP_Server();
    ~TCP_Server(){};
    void SetOptions();
    void Bind();
    void Listen();
    void Accept();
    void Read();
    void Close();


private:
    
    
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[SERVER_BUFFER_SIZE] = {0};

};

#endif

