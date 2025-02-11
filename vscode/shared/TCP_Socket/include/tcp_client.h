#ifndef TCP_CLIENT
#define TCP_CLIENT

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tcp.h"

#define CLIENT_BUFFER_SIZE  1024
#define CLIENT_PORT  9090

using namespace std;

class TCP_Client{

    public:
        TCP_Client();
        void SetServerAddr();
        void ConnectToServer();
        void Send();
        void Close();

    private:
 
        int sock = 0;
        struct sockaddr_in serv_addr;
        char buffer[CLIENT_BUFFER_SIZE] = {0};
        std::string  message_to_server;

};

#endif