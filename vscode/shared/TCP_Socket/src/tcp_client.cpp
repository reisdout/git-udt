#include "../include/tcp_client.h"
#include "../defines/defines.h"

#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std::chrono;


TCP_Client::TCP_Client(uint32_t par_server_port,
                       string par_data_rate,
                       string par_congestion_control,
                       string par_num_flows,
                       string par_tipo_dado,
                       string par_simu_start_time)
                       

{
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(0);
    }


    obj_saver.set_port(par_server_port);
    obj_saver.set_bottleneck_datarate(par_data_rate);
    obj_saver.set_default_congestion(par_congestion_control);
    obj_saver.set_num_flows(par_num_flows);
    obj_saver.set_tipo_dado(par_tipo_dado);
    obj_saver.set_starting_time(par_simu_start_time);
    obj_saver.meth_adjust_file_path();
}

void TCP_Client::SetServerAddr()
{

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "10.0.1.3", &serv_addr.sin_addr) <= 0) {
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
    
    char* data = new char[MSS];
    char temp;


    auto simulation_start = high_resolution_clock::now();
    auto simulation_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(simulation_time - simulation_start);
    for(;;) //Se quiser mandar apenas uma mensagem
            //pode tirar o for.
    {
        if(duration.count() <= SIMUL_TIME)
        {
            //temp = cin.get();
            usleep (period_to_transmit_micro_seconds);
            delete [] data;
            //std::cout << "Pronto! Pode digitar...: "<< endl;
            //message_to_server.clear();
            //getline(cin, message_to_server);
            //std::cout << message_to_server << endl;
            data = new char [MSS];            
            fill_of(data,'a',MSS);
            std::cout << "data: " << data[0] << std::endl;
            if(data)
            {      
                std::cout << "Enviando...: "<< endl;
                send(sock, data, MSS, 0);
                //std::cout << "cwnd: " << get_cwnd() << std::endl;
                //if(get_cwnd() == 65)
                    //set_cwnd(100);
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
        
            simulation_time = high_resolution_clock::now();
            duration = duration_cast<microseconds>(simulation_time - simulation_start);
        }

        else
            break;
       
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