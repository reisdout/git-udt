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

void class_feature_extractor::meth_adjust_seq_metrics_file_path()
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
                            "_training_buffer" +
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


void class_feature_extractor::meth_extract_router_features()
{
    
    if (!dump_file.length())
    {
        cout << "Please, set dump file" <<endl;
        exit(0);
    }
    if(!queue_size_along_time_file.length())
    {
        cout << "Please, set router queue file" <<endl;
        exit(0);

    }

    if(!seq_metrics_file_name.length())
    {
        cout << "Please, set seq metrics output file" <<endl;
        exit(0);

    }
    
    std::string dump_line;

    //cout << parPathTopology << "\n";
    //cout << parPathFlow << "\n";
    //myPause();
    ifstream stream_dump_file (dump_file);
    bool packet=false;
    int packet_line;
    string time_stamp;
    string seq_number;

    if (stream_dump_file.is_open())
    {
        while ( getline (stream_dump_file,dump_line) )
        {
            
            
            
            if(dump_line[0]!='#')//evitar comentários
            {
                packet = 0;
                break;
            } 

            if(dump_line.find("10.0.0.3.41708 > 10.0.1.3")!=std::string::npos &&
                     dump_line.find("UDP, length 1472")!=std::string::npos)
            {
                packet = FIRST_PACKET_LINE;
            }


            if(packet == FIRST_PACKET_LINE)
            {
              
                time_stamp = meth_search_time_stamp(dump_line);
                packet_line = SECOND_PACKET_LINE;

            }

            else if(packet == FIRST_PACKET_LINE)
            {
                packet_line = THIRD_PACKET_LINE;
                break;//não precisa, mas é só pra marcar que vai para outra linha
            }
            else if(packet_line == THIRD_PACKET_LINE)
            {
                seq_number =  meth_search_seq_number(dump_line);
                packet_line = FORTH_PACKET_LINE;
                break;
            }

            else if(packet_line == FORTH_PACKET_LINE)
            {
                meth_search_best_queue_size_by_time_stamp(time_stamp);

            }


            


            //if its starting a packet
                      
        }
       
    
    }

    else
    { 
      std::cout << "Unable to open file\n"; 
      exit(0);
    }


    return;

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

    if (!dump_file.length())
    {
        cout << "Please, set dump file" <<endl;
        exit(0);
    }
    if(!queue_size_along_time_file.length())
    {
        cout << "Please, set router queue file" <<endl;
        exit(0);

    }



}

string class_feature_extractor:: meth_deal_with_K_occurence(string par_queue_along_time_file_line)
{

    auto pos = par_queue_along_time_file_line.find("K");
    if(pos != string::npos)
        par_queue_along_time_file_line.replace(pos,1,"000");
    

    return par_queue_along_time_file_line;

}


void class_feature_extractor:: meth_search_best_queue_size_by_time_stamp(string par_time_stamp)
{

    if(!stream_queue_size_along_time_file.is_open())
        stream_queue_size_along_time_file.open(queue_size_along_time_file);
    
    if(stream_queue_size_along_time_file.is_open())
    {
        string queue_along_time_file_line;
        string str_line_time_stamp;
        string str_line_queue;
        uint64_t packet_arrival_time;
        par_time_stamp.erase(std::remove(par_time_stamp.begin(), par_time_stamp.end(), '.'), par_time_stamp.end());
        packet_arrival_time = (uint64_t)stoull(par_time_stamp);

        if(! prior_packet_arrival_time)
        {
            
            getline(stream_queue_size_along_time_file, queue_along_time_file_line);
            str_line_time_stamp = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',1);
            
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            //Retorna algo do tipo 23456b234p
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            str_line_queue = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',2);//2345
            str_line_queue = str_line_queue.substr(0,str_line_queue.find("b"));
            str_line_queue = meth_deal_with_K_occurence(str_line_queue);
        }
        




    }



}


string class_feature_extractor::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',10));
    //cout << "str_seq_number: " << str_seq_number;
    return str_seq_number;
}


string class_feature_extractor::
meth_search_occurence_string_between_delimiter(string par_string_to_search,
                                               char par_delimiter, 
                                               int par_occruence_pos)
{

    int occurence = 0;
    stringstream ss(par_string_to_search);
    string t;
    //we neeed group 1
    //cout << endl;
    while (getline(ss, t, par_delimiter))
    {
        occurence++;
        //cout << "\"" << t << "\"" << endl;
        if(occurence == par_occruence_pos)
            return t;
        
    }
        
    return "";

    
}


string class_feature_extractor::meth_search_time_stamp(string par_dump_line)
{

    string str_time_stamp;
    str_starting_time.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',1));
    return str_starting_time;
}

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
//o out_dir é o diretório onde foram salvos os dados dos
//fluxos, que é o mesmo que deve ser usado para guardar
//os dados do roteador.
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    

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

