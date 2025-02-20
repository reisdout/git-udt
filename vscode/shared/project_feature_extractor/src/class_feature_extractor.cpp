#include "../include/class_feature_extractor.h"
//#include <bits/stdc++.h>

# define FIRST_PACKET_LINE 1
# define SECOND_PACKET_LINE 2
# define THIRD_PACKET_LINE 3
# define FORTH_PACKET_LINE 4
# define DATA_PACKET_LINE 5


class_feature_extractor::class_feature_extractor()
{


}

void class_feature_extractor::meth_adjust_file_path()
{
   
    meth_check_parameters();
    //ajusta para o tempo em que os fluxos foram estabelecidos
    //obtendo esse tempo do out_dir
    meth_adjus_starting_time();
    int client_id = port - 9000;
    seq_metrics_file_name = std::string("10_0_") + 
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("to") +
                            std::string("10_1_") +
                            std::to_string(client_id) +
                            std::string("_2") +
                            str_starting_time +
                            std::string(".csv");
    
    
    std::replace(seq_metrics_file_name.begin(), seq_metrics_file_name.end(),':','_');

    //cin >> c;

     std::cout << seq_metrics_file_name << std::endl;     

}


void class_feature_extractor::meth_adjus_starting_time()
{
    auto pos = out_dir.find("bps");
    str_starting_time = out_dir.substr(pos+4,string::npos);
    cout << "str_starting_time: "<< str_starting_time << endl;

}


void class_feature_extractor::meth_check_parameters()
{

    if(!port)
    {
      cout << "Please, set port" << endl;
      exit(0);
    }

    if(out_dir == "/home/ns/Desktop/output")
    {
      cout << "Please, set out_dir" << endl;
      exit(0);
    }


}


void class_feature_extractor::get_sequence_number(string par_dump_line)
{
    uint32_t seq_number;
    stringstream ss(par_dump_line);
    string t;
    char delimiter = ' ';
    int group = 0; //we neeed group 8 e 9
    while (getline(ss, t, delimiter))
        cout << "\"" << t << "\"" << " ";
    return;
}


void class_feature_extractor::get_time_stamp(string par_dump_line)
{

    return;

}

void class_feature_extractor::set_out_dir(string par_out_dir)
{
    //par_out_dir deve ser o diretório em que foi
    //armazenado os dados dos fluxos.
    out_dir = out_dir+"/"+par_out_dir;
    struct stat sb;
    if (stat(out_dir.c_str(), &sb) == 0)
        cout << "There is previou flow data"<<endl;
    else
    {
        cout << "No previous flow data" << endl;
        exit(0);
    }

    
}

