#include <iostream>
#include <libudt/udt.h>
#include <arpa/inet.h>
#include <cstring>
#include <VegasMLP13/include/VegasMLP13.h>

#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace UDT;
using namespace std;

#define FILE 0

#define MESSAGE 1

#define HELLO 2

int send_type = FILE;

UDTSOCKET client;
UDTSOCKET serv;
UDTSOCKET recver;
VegasMLP13* cchandle = NULL;


void Set_client_socket()
{
    if(send_type == FILE || send_type == HELLO)
    
        client = UDT::socket(AF_INET, SOCK_STREAM, 0);
    else    
        client = UDT::socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    memset(&(serv_addr.sin_zero), '\0', 8);
    int optResult = UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> )); 
    std::cout << "optResult: " << optResult << std::endl; 
    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(client, (sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        std::cout << "connect: " << UDT::getlasterror().getErrorMessage();
        exit(0);
    }

}

void Set_server_socket()
{
    if(send_type == FILE || send_type == HELLO)
        serv= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else
        serv= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(9000);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);
    
    int optResult = UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> )); 
    std::cout << "optResult Server: " << optResult << std::endl;
    if (UDT::ERROR == UDT::bind(serv, (sockaddr*)&my_addr, sizeof(my_addr)))
    {
        std::cout << "bind: " << UDT::getlasterror().getErrorMessage();
        exit (0);
    }
    UDT::listen(serv, 10);

    int namelen;
    sockaddr_in their_addr;

    recver = UDT::accept(serv, (sockaddr*)&their_addr, &namelen);
    
    char ip[16];
    std::cout << "new connection: " << inet_ntoa(their_addr.sin_addr) << ":" << ntohs(their_addr.sin_port) << std::endl;


}

void ShowCCSignature()
{
    std::cout << "ShowCCSignature" << std::endl;
    int temp;
    UDT::getsockopt(client, 0, UDT_CC, &cchandle, &temp);
    if(cchandle != NULL)
        cchandle->Signature();
    else
         std::cout << "Não acionou o metodo" << std::endl;
}

std::streampos Get_file_size()
{
    //std::ifstream& ifs("//home//ns//ns-share//video_test.mp4");
    std::ifstream ifs("/home/ns/ns-share/test.mp4");
    ifs.seekg(0, std::ios::end);
    std::streampos size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    ifs.close();
    return size;
}

int main(int argc, char**argv){
    printf("Hello, from udpSocket!\n");
    //Socket* ptSocket=0;
    //bool server = true;
    //bool debug = true;
    std::cout << argv[1]<<"\n";

    if(std::string(argv[1]) == "client")
    {

        if(argc < 3)
        {
            printf("Packets missing\n");
            return 0;
        }

        Set_client_socket();


        ShowCCSignature();

        char hello[100] = "hello world!\n";

        int packets_sent = 0;
        int packets_to_send =  std::stoi(std::string(argv[2]));

        if(send_type == MESSAGE)
        {

            while(packets_sent < packets_to_send)
            {
                
                if (UDT::ERROR == UDT::sendmsg(client, hello,100, -1,false))
                //if (UDT::ERROR == UDT::send(client, hello, strlen(hello) + 1, 0))
                {
                    std::cout << "send: " << UDT::getlasterror().getErrorMessage();
                    return 0;
                }
                std::cout << "sending: " << hello << std::endl;
                packets_sent++;
                sleep(3);
            }
        }

        else if (send_type == FILE)
        {
            
            int64_t  init;
            init = 0;
            std::fstream ifs("/home/ns/ns-share/test.mp4");
            ifs.seekg(0, ios::end);
            std::streampos size = ifs.tellg();
            ifs.seekg(0, ios::beg);

            if (UDT::ERROR == UDT::sendfile(client, ifs, init, size))
            {
                std::cout << "sendfile: " << UDT::getlasterror().getErrorMessage();
                ifs.close();
                return 0;
            }
            ifs.close(); 
        }

        else // Just Hello
        {
            
            while(packets_sent < packets_to_send)
            {
                
                
                if (UDT::ERROR == UDT::send(client, hello, strlen(hello) + 1, 0))
                {
                    std::cout << "send: " << UDT::getlasterror().getErrorMessage();
                    return 0;
                }
                std::cout << "sending: " << hello << std::endl;
                packets_sent++;
                sleep(3);
            }

        }

        UDT::close(client);

        return 1;


    }


    if(std::string(argv[1]) == "server" )
    {

        Set_server_socket();

        char data[100];

        if(send_type == MESSAGE)
        {

            while(true)
            {
                
                if (UDT::ERROR == UDT::recvmsg(recver, data, 100))
                //if (UDT::ERROR == UDT::recv(recver, data, 100, 0))
                {
                    std::cout << "recv:" << UDT::getlasterror().getErrorMessage() << std::endl;
                    return 0;
                }

                std::cout << data << std::endl;

                
            }

        }

        else if (send_type == FILE)
        {

            int64_t  init;
            init = 0;
            std::streampos size = Get_file_size();
            fstream ofs;
            ofs.open("largefile.mp4",ios::out | ios::trunc);
            int optResult = UDT::setsockopt(recver, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> ));
            std::cout << "optResult Server Listener: " << optResult << std::endl;
            if (UDT::ERROR == UDT::recvfile(recver, ofs, init, size))
            {
                std::cout << "recvfile: " << UDT::getlasterror().getErrorMessage();
                return 0;
            } 
            ofs.close();
        }

        else //just hello
        {
           while(true)
            {
                
                
                if (UDT::ERROR == UDT::recv(recver, data, 100, 0))
                {
                    std::cout << "recv:" << UDT::getlasterror().getErrorMessage() << std::endl;
                    return 0;
                }

                std::cout << data << std::endl;

                
            }

        }

        UDT::close(recver);
        UDT::close(serv);

        return 1;
    }



}
