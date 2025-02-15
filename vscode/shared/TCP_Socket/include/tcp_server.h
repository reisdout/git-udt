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
//#include "tcp.h"

#define SERVER_BUFFER_SIZE  1500000
#define SERVER_PORT  9090

using namespace std;

class TCP_Server{

public:
    TCP_Server();
    ~TCP_Server(){};
    void SetPort(u_int32_t par_port){port = par_port;};
    void SetOptions();
    void Bind();
    void Listen();
    void Accept();
    void Read();
    void Close();


private:
    
    u_int32_t port=0;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[SERVER_BUFFER_SIZE] = {0};

};

#endif

