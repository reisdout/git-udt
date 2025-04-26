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

#include "../../project_feature_saver/include/class_feature_saver_tcp.h"


//#define MSS 20000
            //3000 - Péssimo
            //15000 - Bom com clients_to_fill_band=30, mas com degradação das CNN
//#define CLIENT_PORT  9090



using namespace std;

class TCP_Client{

    public:

        uint64_t clients_to_fill_band = 0;                                                     
        uint64_t mss = 0;
        uint64_t transmission_rate_mbps = 0;
        u_int32_t port=0;
        u_int64_t simul_time=0;
   

        /*
            A princípio deve-se buscar um periodo menor que o rtt, para ter o 
            efeito da simulação, que sempre que pode transmite. Se o período form
            maior que o rtt pode haver período de ociosidade. sem dado para transmitir.
        */

        uint64_t period_to_transmit_micro_seconds=0;

        TCP_Client(uint32_t par_server_port,
            uint64_t par_clients_to_fill_band,
            string par_data_rate,
            string par_congestion_control,
            uint64_t par_mss,
            string par_num_flows,
            uint64_t par_simul_time,
            string par_tipo_dado,
            string par_simu_start_time);


        ~TCP_Client();
            
        
        string get_experiment_dir(){return experiemnt_dir;};
        uint64_t get_period_to_transmit(){return period_to_transmit_micro_seconds;};
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

        int sock = 0;
        struct sockaddr_in serv_addr;
        //uint64_t period_to_transmit_micro_seconds = clients_to_fill_band*((8*mss)/transmission_rate_mbps);
        char *data;
        char *buffer;
        std::string  message_to_server;
        std::string experiemnt_dir;
        
        class_feature_saver_tcp obj_saver;


};

#endif