#pragma once

#include <ctime>
#include <algorithm> //para usar std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>
#include <sys/stat.h>

using namespace std;

#define SEGMENT_SIZE 1500

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
    ~class_feature_extractor(){}; //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    
    void meth_adjust_seq_metrics_file_path();
    void meth_adjus_starting_time();
    string meth_deal_with_K_occurence(string par_queue_along_time_file_line);
    void meth_extract_router_features();//scan dump file
    void meth_check_parameters();
    string meth_search_seq_number(string par_dump_line);
    string  meth_search_time_stamp(string par_dump_line);
    void meth_search_best_queue_size_by_time_stamp(string dump_seq_time_stamp);
    string meth_search_occurence_string_between_delimiter(string par_string_to_search,
                                                          char par_delimiter, 
                                                          int par_occruence_pos);

    string get_out_dir(){return out_dir;};
    string get_seq_metrics_file_name(){return seq_metrics_file_name;};
    string get_dump_file(){return dump_file;};
    string get_queue_size_along_time_file(){return queue_size_along_time_file;};
  
    void set_dump_file(string par_dump_file){dump_file = par_dump_file;};
    void set_out_dir(string par_out_dir);
    void set_port(uint32_t par_port){port=par_port;};
    void set_queue_size_along_time_file(string par_queue_size_along_time_file){queue_size_along_time_file = par_queue_size_along_time_file;};
    //void set_seq_metrics_file_name(string par_seq_metrics_file_name){seq_metrics_file_name = par_seq_metrics_file_name;}; 

 
    private:

        string out_dir = "/home/ns/Desktop/output";
        uint32_t port=0;
        std::string dump_file;
        uint64_t prior_packet_arrival_time=0;
        uint64_t secondary_packet_arrival_time=0;
        uint64_t buffer_at_prior_packet_arrival_time=0;
        uint64_t buffer_at_secondary_packet_arrival_time=0;

        ifstream stream_queue_size_along_time_file;
        std::string queue_size_along_time_file;
        std::string seq_metrics_file_name; 
        std::string str_starting_time;



    
};