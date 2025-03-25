#include "../include/tcp_client.h"
#include "../defines/defines.h"
#include "mrs_utils.h"

#include <cstdlib>
#include <algorithm>
#include <chrono>

using namespace std::chrono;


TCP_Client::TCP_Client(uint32_t par_server_port,
                       uint64_t par_clients_to_fill_band,
                       string par_data_rate,
                       string par_congestion_control,
                       uint64_t par_mss,
                       string par_num_flows,
                       uint64_t par_simul_time,
                       string par_tipo_dado,
                       string par_simu_start_time)
                       

{
    bool tcp_client_constructor_force_print =  false;
    class_mrs_debug::print<char>("Entrando TCPClient constructor", '\n', tcp_client_constructor_force_print);
   
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(0);
    }


    obj_saver.set_port(par_server_port);
    class_mrs_debug::print<char>("ajustou porta do obj saver", '\n', tcp_client_constructor_force_print);
    obj_saver.set_bottleneck_datarate(par_data_rate);
    obj_saver.set_default_congestion(par_congestion_control);
    obj_saver.set_num_flows(par_num_flows);
    obj_saver.set_tipo_dado(par_tipo_dado);
    obj_saver.set_starting_time(par_simu_start_time);
    obj_saver.meth_adjust_file_path();
    class_mrs_debug::print<char>("obj_saver instanciado no TcpClinet constructor", '\n', tcp_client_constructor_force_print);
    mss = par_mss;
    data = new char[mss];
    clients_to_fill_band = par_clients_to_fill_band;
    simul_time = par_simul_time;
    experiemnt_dir = obj_saver.get_out_dir();
    transmission_rate_mbps = stoull(par_data_rate);
    period_to_transmit_micro_seconds = clients_to_fill_band*((8*mss)/transmission_rate_mbps);
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

    if(!clients_to_fill_band || !mss ||!transmission_rate_mbps||!port)
    {
        cout <<" Not enought parameters to transmit" << endl;
        exit(0);
    }

    //std::cout << "Testando digitação...: ";
    //getline(cin, message_to_server);
    //std::cout << message_to_server<< endl;
    
    char temp;


    auto simulation_start = high_resolution_clock::now();
    auto simulation_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(simulation_time - simulation_start);
    for(;;) //Se quiser mandar apenas uma mensagem
            //pode tirar o for.
    {
        if(duration.count() <= simul_time)
        {
            //temp = cin.get();
            usleep (period_to_transmit_micro_seconds);
            delete [] data;
            //std::cout << "Pronto! Pode digitar...: "<< endl;
            //message_to_server.clear();
            //getline(cin, message_to_server);
            //std::cout << message_to_server << endl;
            data = new char [mss];            
            fill_of(data,'a',mss);
            std::cout << "data: " << data[0] << std::endl;
            if(data)
            {      
                std::cout << "Enviando...: "<< endl;
                send(sock, data, mss, 0);
                //std::cout << "cwnd: " << get_cwnd() << std::endl;
                //if(get_cwnd() == 65)
                    //set_cwnd(100);
                std::cout << "message sent: " << data[0] << std::endl;
                //ssize_t valread = read(sock, buffer, CLIENT_BUFFER_SIZE);
                //std::cout << "Server Received: " << buffer[0] << std::endl;
                std::cout << "last ack: " << get_last_ack() << std::endl;
                //if ((strncmp(buffer, "exit", 4)) == 0) 
                //{
                    //printf("Client Exit...\n");
                    //break;
                //}

            }
        
            simulation_time = high_resolution_clock::now();
            duration = duration_cast<microseconds>(simulation_time - simulation_start);
        }

        else
            break;
       
    }
    send(sock, "exit", 4, 0);
    Close();
}

void TCP_Client::Close()
{
    // Close the socket
    cout << "Closing connection in port " << port<< "!" << endl;
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


TCP_Client::~TCP_Client()
{
    if(data) 
        delete data;
};