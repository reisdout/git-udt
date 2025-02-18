#ifndef CLASS_FEATURE_SAVER
#define CLASS_FEATURE_SAVER

#include <ctime>
#include <algorithm> //para usar std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>
#include <unordered_map>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define FIRST_ACK 1

#define MAX_PACKETS_ROUTER_BUFFERSIZE 300


class class_feature_saver{
    public:
        class_feature_saver(uint32_t par_port);
        ~class_feature_saver(){};
        
        void meth_add_file_header();
        void meth_amortize_map(int32_t par_seq);
        void meth_update_seq_metrics_file(int32_t seq, unsigned );
        //Este método guarda o timestamp, pois não é retornado com o ack
        void meth_calculate_ack_ewma(uint32_t par_seq, 
                                    high_resolution_clock::time_point par_ack_arrival_time);
        
        void meth_calculate_rtt(uint32_t par_seq, 
                                high_resolution_clock::time_point par_ack_arrival_time);

        void meth_calculate_send_ewma(uint32_t par_seq);
        
        void meth_deal_packet_send(uint32_t par_seq, 
                                   high_resolution_clock::time_point par_send_time);
        
        void meth_deal_ack_arrival(uint32_t par_seq, 
                                   high_resolution_clock::time_point par_ack_arrival_time);
       
        
        void meth_save_training_data(   unsigned int parNumAckFlow, 
                                        float parAck_ewma, 
                                        float parSend_ewma, 
                                        float parRtt_ratio,
                                        uint32_t parCwnd,
                                        int parBuffeSizeVariation,
                                        float parRouterQueue_ewma,
                                        uint32_t parRouterBufferSizeValueAckArrival,
                                        uint32_t parRouterBufferSizeValuePacketSent,
                                        high_resolution_clock::time_point parTSAckArrival,
                                        high_resolution_clock::time_point parTSInsideAck,
                                        double parMimRTTAck,
                                        uint8_t* parBufEndSrc, 
                                        uint8_t* parBufEndDest,
                                        string parFlowType,
                                        int parIdSrc,
                                        int parIdDest,
                                        bool parUnicFile);
        void meth_start_simulation_time();
        float get_ack_ewma(){return ack_ewma;};
        float get_send_ewma(){return send_ewma;};

        unordered_map<uint32_t, high_resolution_clock::time_point> umap_seq_timestamp;

    protected:
        //static const int BUFFER_SIZE = 1024;
        //static const int PORT = 8080;

    
    
    private:

    string outDir = "/home/ns/Desktop";
    uint32_t port;
    std::string path_seq_metrics_file; 
    std::string str_starting_time;
    float lastAck_ewma=0;
    float expWeightExpon = 0.8;
    std::timespec *time_ack_arrival;
    //int a = timespec_get(time_ack_arrival,);
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    
    long long unsigned numAckReceived = 0;
    bool first_ack_process = false;
    float ack_ewma = 0;
    float send_ewma = 0;
    duration<double> intervalbetweenTS;
    double rtt;

    high_resolution_clock::time_point  marcaTempoChegadaAckAnterior;
    high_resolution_clock::time_point  marcaTempoChegadaAck;

    high_resolution_clock::time_point  marcaTempoAnterior;
    high_resolution_clock::time_point  marcaTempoAtual;
};

#endif