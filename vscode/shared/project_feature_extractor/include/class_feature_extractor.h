#pragma once

#include <ctime>
#include <algorithm> //para usar std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>
#include <sys/stat.h>
#include <bits/stdc++.h>

using namespace std;

#define SEGMENT_SIZE 1500


#define MAX_PACKETS_ROUTER_BUFFERSIZE_100Mbps 300 //300//100
#define MAX_BYTES_ROUTER_BUFFERSIZE_100Mbps  MAX_PACKETS_ROUTER_BUFFERSIZE_100Mbps*1500 //2795084


class class_feature_extractor {

protected:
    unsigned segmentsReceived=0;
public:
    class_feature_extractor();//Lembrando que os constructors são da classe Base (Pai)
               //é autmaticamnete chamda quando se instancia a classe filha
               //se for herança múltipla, os constructors são chamados na ordem
               //de leituraem que foram declaradas as heranças. ver 
               //https://www.geeksforgeeks.org/multiple-inheritance-in-c/
               // 
    ~class_feature_extractor(); //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    
    void meth_adjust_seq_metrics_file_path();
    void meth_adjus_starting_time();
    void meth_construct_queue_along_time_file_ewma();
    static string meth_deal_with_K_occurence(string par_queue_along_time_file_line);
    void meth_extract_router_features();//scan dump file
    void meth_check_parameters();
    static bool meth_generate_queue_ewma_along_time_file(string par_queue_along_time_file, string par_queue_along_time_file_ewma);

    string meth_search_seq_number(string par_dump_line);
    string  meth_search_time_stamp(string par_dump_line);
    bool meth_search_best_queue_size_by_time_stamp(string dump_seq_time_stamp, uint64_t& par_best_queue_size);
    static string meth_search_occurence_string_between_delimiter(string par_string_to_search,
                                                          char par_delimiter, 
                                                          int par_occruence_pos);

    static bool meth_take_and_store_line_values(ifstream &par_stream_of_lines, uint64_t & par_time_stamp, uint64_t & par_queue_buffer);

    void meth_update_seq_queue_file(uint64_t par_seq, float par_queue_ewma);

    string get_out_dir(){return out_dir;};
    string get_seq_metrics_file_name(){return seq_metrics_file_name;};
    string get_dump_file(){return dump_file;};
    string get_queue_size_along_time_file(){return queue_size_along_time_file;};
  
    void set_dump_file(string par_dump_file){dump_file = out_dir+ "/" + par_dump_file;};
    void set_out_dir(string par_out_dir); //Será acrescentado ao /home/ns/Desktop/output
    void set_port(uint32_t par_port){port=par_port;};
    void set_queue_size_along_time_file(string par_queue_size_along_time_file);
    //void set_seq_metrics_file_name(string par_seq_metrics_file_name){seq_metrics_file_name = par_seq_metrics_file_name;}; 

 
    private:
        static inline bool queue_ewma_generated = false;
        string out_dir = "/home/ns/Desktop/output";
        uint32_t port=0;
        std::string dump_file;
        uint64_t prior_packet_arrival_time=0;
        uint64_t secondary_packet_arrival_time=0;
        uint64_t buffer_at_prior_packet_arrival_time=0;
        uint64_t buffer_at_secondary_packet_arrival_time=0;
        static inline float exp_weight_expon_queue = 0.8;
        float router_queue_ewma = 0.0;
        ifstream stream_queue_size_along_time_file;
        std::string queue_size_along_time_file;
        std::string queue_size_along_time_file_ewma;
        std::string seq_metrics_file_name; 
        std::string str_starting_time;



    
};