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
#include </usr/include/linux/tcp.h>


#define CLIENT_BUFFER_SIZE  1500000
#define CLIENT_PORT  9090

using namespace std;

class TCP_Client{

    public:
        TCP_Client();
        void SetPort(u_int32_t par_port){port = par_port;};
        void SetServerAddr();
        void ConnectToServer();
        void Send();
        void Close();
        void fill_of(char* par_buf, char par_value_to_fill, int par_size);
        u_int32_t get_cwnd();
        u_int32_t get_last_ack();

        void set_cwnd(u_int32_t par_cwnd);

    private:

        u_int32_t port=0;
        int sock = 0;
        struct sockaddr_in serv_addr;
        char buffer[CLIENT_BUFFER_SIZE] = {0};
        std::string  message_to_server;


};

#endif