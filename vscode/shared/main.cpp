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

#define PCC 3

#define MESSAGE_SIZE 3000

char filePath[] = "/home/ns/UDT-workspace/git-udt/vscode/shared/main.txt";

int send_type = PCC;

UDTSOCKET client;
UDTSOCKET serv;
UDTSOCKET recver;
UDTSOCKET recent;//receptor do cliente

sockaddr_in serv_addr; //endereço do servidor, no cliente
int namelen_server_addr_in_client; //endereco do servidor capturado no cliente
sockaddr_in server_addr_in_client; //endereco ip do servidor capturado no cliente

sockaddr_in my_addr;//endereco do servidor, no servidor

VegasMLP13* cchandle = NULL;


void Set_client_socket()
{
    if(send_type == FILE || send_type == HELLO)
        client= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == PCC)
        client= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == MESSAGE )
        client= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    else
    {
        cout << "Tipo de envio invalido" << endl;
        exit(0);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    //inet_pton(AF_INET, "192.168.15.200", &serv_addr.sin_addr);
    memset(&(serv_addr.sin_zero), '\0', 8);
    int optResult = UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<VegasMLP13> , sizeof(CCCFactory<VegasMLP13> )); 
    std::cout << "optResult: " << optResult << std::endl; 
    // connect to the server, implict bind
    
    /*Aparentemente, quando do conect, há um bind(reserva) tácito na máquiana que 
    está solicitando a conexao*/
    if (UDT::ERROR == UDT::connect(client, (sockaddr*)&serv_addr, sizeof(serv_addr)))
    {
        std::cout << "connect: " << UDT::getlasterror().getErrorMessage();
        exit(0);
    }
    UDT::listen(client, 10);
    recent = UDT::accept(client, (sockaddr*)&server_addr_in_client, &namelen_server_addr_in_client);
}

void Set_server_socket()
{
    if(send_type == FILE || send_type == HELLO)
        serv= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == PCC)
        serv= UDT::socket(AF_INET, SOCK_STREAM, 0);
    else if (send_type == MESSAGE )
        serv= UDT::socket(AF_INET, SOCK_DGRAM, 0);
    else
    {
        cout << "Tipo de envio invalido" << endl;
        exit(0);
    }
    
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

    //if (UDT::ERROR == UDT::connect(serv, (sockaddr*)&their_addr, sizeof(their_addr)))
    //{
        //std::cout << "connect send server to cleint: " << UDT::getlasterror().getErrorMessage();
        //exit(0);
    //}

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
    std::ifstream ifs(filePath);
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

        char hello[3000];// = "hello world!\n";
        int packets_sent = 0;
        int packets_to_send =  std::stoi(std::string(argv[2]));
        
        if(send_type == PCC)
        {
            int size = 100000;
            char* data = new char[size];
            bzero(data, size);
            int ssize = 0;
            int ss;
            char ack[100];
            unsigned numPack = 0;

            while (true) 
            {
                ssize = 0;
                //int ss;
                while (ssize < size) 
                {
                    if (UDT::ERROR ==(ss = UDT::send(client, data + ssize, size - ssize, 0))) 
                    {
                        cout << "send:" << UDT::getlasterror().getErrorMessage() << endl;
                        break;
                    }

                    ssize += ss;
                    //if (UDT::ERROR == UDT::recv(client, ack, 100, 0))
                    //{
                        //cout << "recv ack:" << UDT::getlasterror().getErrorMessage() << endl;
                        //return 0;
                    //}
                }
                numPack++;
                cout << "numPack: " << numPack << endl;
                if (ssize < size) 
                {
                    break;
                }
            }

          UDT::close(client);
          delete [] data;

        }

        else if(send_type == MESSAGE)
        {

            while(packets_sent < packets_to_send)
            {
                for(int i=0; i < MESSAGE_SIZE-1; i++)
                    hello[i] = 'a';
                hello[MESSAGE_SIZE -1] ='\0';


                if (UDT::ERROR == UDT::sendmsg(client, hello,MESSAGE_SIZE, -1,false))
                //if (UDT::ERROR == UDT::send(client, hello, strlen(hello) + 1, 0))
                {
                    std::cout << "send: " << UDT::getlasterror().getErrorMessage();
                    return 0;
                }
                std::cout << "sending: " << hello << std::endl;
                packets_sent++;
                int rsize = UDT::recvmsg(client, hello, MESSAGE_SIZE);

                //sleep(3);
                
            }
        }

        else if (send_type == FILE)
        {
            
            int64_t  init;
            init = 0;
            std::fstream ifs(filePath);
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

        if(send_type == PCC)
        {
            char* data;
            int size = 100000000;
            data = new char[size];
            int rsize = 0;
            int rs;
            int rcv_size;
            int var_size = sizeof(int);


            while (true) 
            {
                while (rsize < size) 
                {
                // UDT::getsockopt(recver, 0, UDT_RCVDATA, &rcv_size, &var_size);
                    if (UDT::ERROR == (rs = UDT::recv(recver, data + rsize, size - rsize, 0))) 
                    {
                        cout << "recv:" << UDT::getlasterror().getErrorMessage() << endl;
                        break;
                    }

                    rsize += rs;
                }
            

                if (rsize < size) 
                {
                    break;
                }
            }
        

            delete [] data;
            UDT::close(recver);
        }

    
        else if(send_type == MESSAGE)
        {

            char messa_data[MESSAGE_SIZE];
            while(true)
            {
                
                if (UDT::ERROR == UDT::recvmsg(recver, data, MESSAGE_SIZE))
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
