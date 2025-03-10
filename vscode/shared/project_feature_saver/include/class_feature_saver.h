#ifndef CLASS_FEATURE_SAVER
#define CLASS_FEATURE_SAVER

#include <ctime>
#include <algorithm> //para usar std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>
//#include <unordered_map>
#include <map>
//#include <unistd.h>
#include <sys/stat.h>

#include <typeinfo> 
#include <iomanip>


using namespace std;
using namespace std::chrono;



#define FIRST_ACK 1

#define MAX_PACKETS_ROUTER_BUFFERSIZE 300


template <class T>
void class_feature_saver_print(string par_string, T par_var) {
 
    
    float type_float;
    double type_double;
    long double type_long_double;
    

    const type_info& t_float = typeid(type_float);
    const type_info& t_double = typeid(type_double);  
    const type_info& t_long_double = typeid(type_long_double);
    const type_info& t_my_type = typeid(par_var);
    
    
    bool print = true;
    if(print)
    {

        //cout << "entering print " << endl;
        char c;
        if(t_my_type == t_float || t_my_type == t_double)
            cout << par_string << fixed << setprecision(5) << par_var << endl;
        else if (t_my_type == t_long_double)
            cout << par_string << fixed << setprecision(10) << par_var << endl;
        else
            cout << par_string << par_var << endl;
        cin >> c;
    }
}




class class_feature_saver{
    public:
        class_feature_saver();
        ~class_feature_saver(){};
        
        void meth_add_file_header();
        void meth_adjust_file_path(string par_out_dir="");

        //Este método guarda o timestamp, pois não é retornado com o ack
        virtual void meth_calculate_ack_ewma(uint32_t par_seq, 
                                    high_resolution_clock::time_point par_ack_arrival_time){};
        
        virtual void meth_calculate_rtt(uint32_t par_seq, 
                                high_resolution_clock::time_point par_ack_arrival_time){};

        virtual void meth_calculate_send_ewma(uint32_t par_seq){};

        virtual void meth_calculate_ack_ewma(uint64_t par_ack_arrival_time){};

        virtual void meth_calculate_rtt(uint64_t  par_ack_arrival_time, uint64_t  par_eco_reply){};

        virtual void meth_calculate_send_ewma(uint64_t  par_eco_reply){};


        void meth_check_parameters();
        
        virtual void meth_deal_packet_send(uint32_t par_seq, 
                                   high_resolution_clock::time_point par_send_time){};
        
        virtual void meth_deal_ack_arrival(uint32_t par_seq, 
                                   high_resolution_clock::time_point par_ack_arrival_time){};
        virtual void meth_deal_ack_arrival(uint32_t par_seq, 
                                           uint64_t par_ack_arrival,
                                           uint64_t par_packet_eco_reply){};
       
        
        void meth_save_training_data(   uint64_t parNumAckFlow, 
                                        float parAck_ewma, 
                                        float parSend_ewma, 
                                        float parRtt_ratio,
                                        uint32_t parCwnd,
                                        int parBuffeSizeVariation,
                                        float parRouterQueue_ewma,
                                        uint32_t parRouterBufferSizeValueAckArrival,
                                        uint32_t parRouterBufferSizeValuePacketSent,
                                        uint64_t parTSAckArrival,
                                        uint64_t parTSInsideAck,
                                        double parMimRTTAck,
                                        uint8_t* parBufEndSrc, 
                                        uint8_t* parBufEndDest,
                                        string parFlowType,
                                        int parIdSrc,
                                        int parIdDest,
                                        bool parUnicFile);
        
        float get_ack_ewma(){return ack_ewma;};
        string get_bottleneck_datarate(){return bottleneck_datarate;};
        string get_default_congestion(){return default_congestion;};
        string get_num_flows(){return num_flows;};
        string get_out_dir(){return out_dir;};
        float get_send_ewma(){return send_ewma;};
        //O ideal é que tenha o formato do comando date no linux
        // antes do AM/PM.
        string get_starting_time(){return str_starting_time;};
        string get_tipo_dado(){return tipo_dado;};        
        
        
        void set_tipo_dado(string par_tipo_dado){tipo_dado = par_tipo_dado;};
        void set_default_congestion(string par_default_congestion){default_congestion = par_default_congestion;};
        void set_num_flows(string par_num_flows){num_flows = par_num_flows;};
        void set_bottleneck_datarate(string par_data_rate){bottleneck_datarate = par_data_rate;};
        void set_port(uint32_t par_port){port=par_port;};
        void set_starting_time(string par_starting_time){str_starting_time = par_starting_time;};

        

    protected:

    string out_dir = "/home/ns/Desktop/output";
    uint32_t port = 0;
    std::string seq_metrics_file_name;
    
    std::string str_starting_time;
    float lastAck_ewma=0;
    float expWeightExpon = 0.8;
    std::timespec *time_ack_arrival;
    //int a = timespec_get(time_ack_arrival,);
    //high_resolution_clock::time_point t1 = high_resolution_clock::now();

    uint64_t numAckReceived = 0;
    bool first_ack_process = false;
    float ack_ewma = 0;
    float send_ewma = 0;
    float rtt;
    string tipo_dado;
    string default_congestion;
    string num_flows;
    string bottleneck_datarate;    


};

#endif