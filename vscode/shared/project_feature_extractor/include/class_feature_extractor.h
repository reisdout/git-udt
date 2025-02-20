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
    
    void meth_adjust_file_path();
    void meth_adjus_starting_time();
    void meth_extract_router_features(string par_path_router_file);
    void meth_check_parameters();

    string get_out_dir(){return out_dir;};
    void get_sequence_number(string par_dump_line);
    void get_time_stamp(string par_dump_line);
    void set_out_dir(string par_out_dir);
    void set_port(uint32_t par_port){port=par_port;};

 
    private:

        string out_dir = "/home/ns/Desktop/output";
        uint32_t port=0;
        std::string seq_metrics_file_name; 
        std::string str_starting_time;



    
};