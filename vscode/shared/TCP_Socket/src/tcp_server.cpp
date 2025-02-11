#include "../include/tcp_server.h"


TCP_Server::TCP_Server()
{
    std::cout << "Creating socket" << std::endl;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
}
void TCP_Server::SetOptions()
{
    std::cout << "Seting socket" << std::endl;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}
void TCP_Server::Bind()
{
    std::cout << "Biding socket" << std::endl;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PORT);
    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}
void TCP_Server::Listen()
{
    std::cout << "Listening socket" << std::endl;
    if (listen(server_fd, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }

    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

}
void TCP_Server::Accept()
{
    //O accept também trava a execução,
    //que é liberada após o cliente conectar
    std::cout << "Acepting..." << std::endl;
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }    
}

void TCP_Server:: Read()
{
    ssize_t valread = 0;
    std::cout << "Lendo..." << std::endl;
    for(;;)//Se quiser receber apenas uma mensagem, pode tirar o for.
    {
        valread = 0;
        // O read não precisa de looping ele para 
        // a execução quando é chamado.
        valread = read(new_socket, buffer, SERVER_BUFFER_SIZE);
        if(valread > 0)
        {
            std::cout << "Received: " << buffer << std::endl;
            send(new_socket, buffer, valread, 0);
            //std::cout << "Echo message sent" << std::endl;
            if (strncmp("exit", buffer, 4) == 0) 
            { 
                printf("Server Exit...\n"); 
                break; 
            }
        }
    } 
    Close();
}

void TCP_Server::Close()
{
    std::cout << "Closing socket" << std::endl;
    close(new_socket);
    close(server_fd);
}