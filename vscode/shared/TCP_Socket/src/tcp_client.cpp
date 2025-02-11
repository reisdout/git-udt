#include "../include/tcp_client.h"


TCP_Client::TCP_Client()
{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(0);
    }
}

void TCP_Client::SetServerAddr()
{

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(CLIENT_PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        exit(0);
    }

}
void TCP_Client::ConnectToServer()
{
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        exit(0);
    }
}
void TCP_Client::Send()
{

    //std::cout << "Testando digitação...: ";
    //getline(cin, message_to_server);
    //std::cout << message_to_server<< endl;
    
    for(;;) //Se quiser mandar apenas uma mensagem
            //pode tirar o for.
    {
        std::cout << "Pronto! Pode digitar...: "<< endl;
        message_to_server.clear();
        getline(cin, message_to_server);
        //std::cout << message_to_server << endl;
        if(message_to_server.size())
        {      
            std::cout << "Enviando...: "<< endl;
            send(sock, message_to_server.c_str(),CLIENT_BUFFER_SIZE, 0);
            //std::cout << "Hello message sent" << std::endl;
            ssize_t valread = read(sock, buffer, CLIENT_BUFFER_SIZE);
            std::cout << "Server Received: " << buffer << std::endl;
            if ((strncmp(buffer, "exit", 4)) == 0) 
            {
                printf("Client Exit...\n");
                break;
            }
        }
        
    }
    Close();
}

void TCP_Client::Close()
{
    // Close the socket
    close(sock);

}