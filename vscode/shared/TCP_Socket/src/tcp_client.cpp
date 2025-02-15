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
    serv_addr.sin_port = htons(port);
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
    
    char* data = new char[CLIENT_BUFFER_SIZE];
    char temp;

    for(;;) //Se quiser mandar apenas uma mensagem
            //pode tirar o for.
    {
        //temp = cin.get();
        usleep (10000);
        delete [] data;
        //std::cout << "Pronto! Pode digitar...: "<< endl;
        //message_to_server.clear();
        //getline(cin, message_to_server);
        //std::cout << message_to_server << endl;
        data = new char [CLIENT_BUFFER_SIZE];            
        fill_of(data,'a',CLIENT_BUFFER_SIZE);
        std::cout << "data: " << data[0] << std::endl;
        if(data)
        {      
            std::cout << "Enviando...: "<< endl;
            send(sock, data, CLIENT_BUFFER_SIZE, 0);
            std::cout << "cwnd: " << get_cwnd() << std::endl;
            if(get_cwnd() == 65)
                set_cwnd(100);
            std::cout << "message sent: " << data[0] << std::endl;
            //ssize_t valread = read(sock, buffer, CLIENT_BUFFER_SIZE);
            //std::cout << "Server Received: " << buffer[0] << std::endl;
            std::cout << "last ack: " << get_last_ack() << std::endl;
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


u_int32_t TCP_Client::get_cwnd()
{
    tcp_info tcpi;
    socklen_t len = sizeof(tcp_info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tcpi, &len);
    return tcpi.tcpi_snd_cwnd;  // <-
}

void TCP_Client::set_cwnd(u_int32_t par_cwnd)
{
    tcp_info tcpi;
    socklen_t len = sizeof(tcp_info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tcpi, &len);
    tcpi.tcpi_snd_cwnd = par_cwnd;
    int success = setsockopt(sock, IPPROTO_TCP, TCP_INFO, &tcpi, len);
    cout << "Success in setsockopt: " << success <<endl;    
}

u_int32_t TCP_Client::get_last_ack()
{
    tcp_info tcpi;
    socklen_t len = sizeof(tcp_info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tcpi, &len);
    return tcpi.tcpi_last_ack_recv;  // <-
}

void TCP_Client::fill_of(char* par_buf, char par_value_to_fill, int par_size)
{
    for (int i=0; i<par_size; i++)
        par_buf[i]=par_value_to_fill;
}