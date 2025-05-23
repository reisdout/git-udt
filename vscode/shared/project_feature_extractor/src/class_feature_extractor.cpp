#include "../include/class_feature_extractor.h"
//#include <bits/stdc++.h>
#include "mrs_utils.h"

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
    //int client_id = port - 9000;
    int client_id = port - class_mrs_debug::get_port_range(port);
    seq_metrics_file_name = std::string("10_0_") + 
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("to") +
                            std::string("10_1_") +
                            std::to_string(client_id) +
                            std::string("_2_") +
                            str_starting_time +
                            "_training_buffer" +
                            std::string(".csv");
    
    
    std::replace(seq_metrics_file_name.begin(), seq_metrics_file_name.end(),':','_');

    //cin >> c;

    std::cout << seq_metrics_file_name << std::endl;
    cout << "saindo meth_adjust_seq_metrics_file_path" << endl;
}


void class_feature_extractor::meth_adjus_starting_time()
{

    if(out_dir == "/home/ns/Desktop/output")
    {
      cout << "Please, set out_dir" << endl;
      exit(0);
    }
    
    auto pos = out_dir.find("bps");
    str_starting_time = out_dir.substr(pos+4,string::npos);
    string sub_string;
    for (size_t i=str_starting_time.length()-16; i <str_starting_time.length()-8; ++i)
        if(str_starting_time[i] == '_')
            str_starting_time[i] = ' ';

    cout << "str_starting_time: "<< str_starting_time << endl;

}

bool class_feature_extractor::meth_check_if_parse_dump_file_is_possible()
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


    return true;

}



void class_feature_extractor::meth_check_parameters()
{

    if(!port)
    {
      cout << "From class_feature_extractor: Please, set port" << endl;
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

void class_feature_extractor::meth_erase_dot_from_time_stamp(string &par_time_stamp)
{
    par_time_stamp.erase(std::remove(par_time_stamp.begin(), par_time_stamp.end(), '.'), par_time_stamp.end());
    
    /*int places = par_time_stamp.length();

    if(par_precision == "MILI")
        if(places < PLACES_IN_MILLI)
        {
            for(int i = places; i < PLACES_IN_MILLI; i++)
                par_time_stamp.append("0");

        }
    else if (par_precision == "NANO")
    {
        if(places < PLACES_IN_NANO)
        {
            for(int i = places; i < PLACES_IN_NANO; i++)
                par_time_stamp.append("0");

        }

    }
*/


}


void class_feature_extractor::meth_prepare_n1_n2_file()
{


    if(out_dir == "/home/ns/Desktop/output")
    {
      cout << "Please, set out_dir before n1_n2 file conf." << endl;
      exit(0);
    }

    std::ofstream file;
    file.open(out_dir + "/" + "n1_n2.csv", std::ios::out | std::ios::trunc);
    if (file.fail())
    {
        std::cout << "n1_n2 register error at init. " << "\n";
        exit(0);
    }
    //make sure write fails with exception if something is wrong

    file << "n1"<<","<< "n2" << endl;
        //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();

    file.close();
    
}


string class_feature_extractor:: meth_deal_with_K_occurence(string par_queue_along_time_file_line)
{

    auto pos = par_queue_along_time_file_line.find("K");
    if(pos != string::npos)
        par_queue_along_time_file_line.replace(pos,1,"000");
    

    return par_queue_along_time_file_line;

}

bool class_feature_extractor::meth_drain_dump_file(string par_dump_file)
{


    std::string dump_file_drained = par_dump_file;

    auto pos = dump_file_drained.find(".");
    if(pos == string::npos)
    {
        cout << "Queue size along time file with no extension."<< endl;
        exit(0);
    }
    dump_file_drained.replace(pos,1,"_drained.");

    string dump_line;
      
   
    ifstream stream_dump_file (par_dump_file);
    if (!stream_dump_file.is_open())
    {
        cout << "Can't oppen file to drain" << endl;
        exit(0);
    } 
    
    
    std::ofstream file;

    file.open(dump_file_drained, std::ios::out | std::ios::app);
    if (file.fail())
    {
        std::cout << "Error opening seq buffer file drained" << endl;
        exit(0);
    }
  
    uint64_t work_line=0;

    while (getline (stream_dump_file,dump_line) )
    {
        cout << "work line: " << ++work_line << endl;

        if(dump_line.find("0101 0101 0101 0101 0101 0101 0101 0101  ................") != string::npos)
            continue;
        
        file << dump_line << endl;


    }
    stream_dump_file.close();
    file.close();
    
    
    return true;
    


}


bool class_feature_extractor::meth_search_best_queue_size_by_time_stamp(string par_time_stamp, 
                                                                         long double& par_best_queue_size)
{

    /*
    A ideia dos dados primário e secundário se baseia nos seguintes fatos

    1. O pacote faz a diferença com a timestamp prioritária e depois com a secundária. Se
    a diferenca com a primária for menor, o valor da fila associada com a primária é retornada.
    
    2. Se a diferença com a secundária for menor, a secundária passa a ser primária e a posição 
    secundária é preenchida com uma nova linha da fila. Retorna-se ao passo 1, ou seja, permance na
    busca pela menor diferença.

    3. A estratégia acima só é possível pelo fato de tanto a chegada dos pacotes quanto a leitura dos
    buffer estarem distribuídas sequencialmente.
    
    */

    cout << "queue_size_along_time_file_ewma" <<"\n";
    
    cout << queue_size_along_time_file_ewma <<"\n"; 


    cout << "queue_size_along_time_file" <<"\n";
    
    cout << queue_size_along_time_file <<"\n"; 


    
    if(!stream_queue_size_along_time_file.is_open())
    {
       cout << "Queue ewma along time file not oppened." << endl;
       //Para chavear entre o queue ewma e a pura. Não esqueça de alterar
       //o metodo meth_extract_router_features, para passar o ewma calculado a cada
       //valor de fila levantada.
       
       stream_queue_size_along_time_file.open(queue_size_along_time_file_ewma);
       //stream_queue_size_along_time_file.open(queue_size_along_time_file);
    }
    
    if(stream_queue_size_along_time_file.is_open())
    {
        
        uint64_t packet_arrival_time;
        meth_erase_dot_from_time_stamp(par_time_stamp);
        cout << "par_time stamp: " << par_time_stamp << ".\n";
        //char c;
        //cin >> c;
        packet_arrival_time = (uint64_t)stoull(par_time_stamp);

        //float  buffer_at_prior_packet_arrival_time = 0.0;

        //float  buffer_at_secondary_packet_arrival_time = 0.0;

        if(!prior_packet_arrival_time)//Lendio pela primeira vez. Pegar as duas primeiras linhas
        {
            //Lendo as duas primeiras linhas
            for(int i = 0; i < 2; i++)
            {               

                if(i==0) //preenchenco o prioritario (frente na linha do tempo) com os dados a primeira linha
                {    
                    if(!class_feature_extractor::meth_take_and_store_line_values_from_queue_along_time(stream_queue_size_along_time_file, prior_packet_arrival_time, buffer_at_prior_packet_arrival_time))
                    {    cout << "End of queue along time file";
                         stream_queue_size_along_time_file.close();
                         return false;
                    }
                }
                else//prrenchendo o secundário (imedeiatemente posterior na linha do tempo) com os dados da segunda linha
                {
                    if(!class_feature_extractor::meth_take_and_store_line_values_from_queue_along_time(stream_queue_size_along_time_file,secondary_packet_arrival_time,buffer_at_secondary_packet_arrival_time))
                    {
                        stream_queue_size_along_time_file.close();
                        return false;
 
                    }

                }
            }
        }

        uint64_t diff_prior=0;
        uint64_t diff_secondary=0;
        bool found= false;
        while(!found)
        {

            diff_prior = labs(packet_arrival_time - prior_packet_arrival_time);
            diff_secondary = labs(packet_arrival_time - secondary_packet_arrival_time);

            if(diff_prior < diff_secondary)
                found = true;

            else
            {
                prior_packet_arrival_time = secondary_packet_arrival_time;
                buffer_at_prior_packet_arrival_time = buffer_at_secondary_packet_arrival_time;
                if(!class_feature_extractor::meth_take_and_store_line_values_from_queue_along_time(stream_queue_size_along_time_file, secondary_packet_arrival_time,buffer_at_secondary_packet_arrival_time))
                {
                    cout<< "End of queue along time file" << endl;
                    stream_queue_size_along_time_file.close();
                    return false;
                }   
            }

            

        }

        
        par_best_queue_size = buffer_at_prior_packet_arrival_time;
        return true;



    }

    else
    {
        cout << "Error opening queue ewma along time file" << endl;
        exit(0);
    }
    
    return false;
    



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



//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
//o out_dir é o diretório onde foram salvos os dados dos
//fluxos, que é o mesmo que deve ser usado para guardar
//os dados do roteador.
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

bool class_feature_extractor:: meth_take_and_store_line_values_from_queue_along_time(ifstream &par_stream_of_lines, 
                                                               uint64_t & par_time_stamp, 
                                                               float & par_queue_buffer)
{
    
    cout << "Enter meth_take_and_store_line_values() float\n";

    

    string queue_along_time_file_line;
    string str_line_time_stamp;
    string str_line_queue;


    if(getline(par_stream_of_lines, queue_along_time_file_line))
    {
        cout <<  "queue_along_time_file_line: "<< queue_along_time_file_line<< "\n";
        
        if(queue_along_time_file_line.find("b") != string::npos)
        {
            str_line_time_stamp = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',1);
            
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            //Retorna algo do tipo 23456b
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            str_line_queue = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',2);//2345
            str_line_queue = str_line_queue.substr(0,str_line_queue.find("b"));
            str_line_queue = class_feature_extractor::meth_deal_with_K_occurence(str_line_queue);

            par_time_stamp = (uint64_t)stoull(str_line_time_stamp);
            par_queue_buffer = (float)stof(str_line_queue);
            return true;
        }
        cout<< "sintax error in queueu along time file." <<endl;
        exit(0);
    }

    return false;

}



bool class_feature_extractor:: meth_take_and_store_line_values_from_queue_along_time(ifstream &par_stream_of_lines, 
                                                               uint64_t & par_time_stamp, 
                                                               uint64_t & par_queue_buffer)
{
    
    cout << "Enter meth_take_and_store_line_values() uint_64\n";

    string queue_along_time_file_line;
    string str_line_time_stamp;
    string str_line_queue;


    if(getline(par_stream_of_lines, queue_along_time_file_line))
    {
        cout <<  "queue_along_time_file_line"<< queue_along_time_file_line<< "\n";
        
        if(queue_along_time_file_line.find("b") != string::npos)
        {
            str_line_time_stamp = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',1);
            
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            //Retorna algo do tipo 23456b
            //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
            str_line_queue = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',2);//2345
            str_line_queue = str_line_queue.substr(0,str_line_queue.find("b"));
            str_line_queue = class_feature_extractor::meth_deal_with_K_occurence(str_line_queue);

            par_time_stamp = (uint64_t)stoull(str_line_time_stamp);
            par_queue_buffer = (uint64_t)stoull(str_line_queue);
            return true;
        }
        cout<< "sintax error in quueu along time file." <<endl;
        return false;
    }

    return false;

}


    

void class_feature_extractor::set_out_dir(string par_out_dir)
{
    //par_out_dir deve ser o diretório em que foi
    //armazenado os dados dos fluxos.
    out_dir = out_dir+"/"+par_out_dir;
    class_mrs_debug::print<string>("out_dir: ", out_dir);
    struct stat sb;
    if (stat(out_dir.c_str(), &sb) == 0)
        cout << "There is previous flow data"<<endl;
    else
    {
        cout << "No previous flow data" << endl;
        exit(0);
    }

    
}

bool class_feature_extractor::meth_generate_queue_ewma_along_time_file(string par_queue_along_time_file, string par_queue_along_time_file_ewma)
{
    
    //if(class_feature_extractor::queue_ewma_generated)
    //{
        //cout << "Queue ewma along time file already generated" << "\n";
        //return false;
    //}

    //from https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
    //struct stat sb;
    //if (stat(par_queue_along_time_file_ewma.c_str(), &sb) == 0)
    //{
        //cout << "There is previou flow data"<<endl;
        //return false;
    //}

    
    ifstream stream_queue_size (par_queue_along_time_file);

    if(!stream_queue_size.is_open())
    {
        cout << "Error oppening  queue size  along time file" << endl;
        exit(0);

    }

    std::ofstream file;

    file.open(par_queue_along_time_file_ewma, std::ios::out | std::ios::trunc);
    if (file.fail())
    {
        std::cout << "Router queue along time ewma error" << endl;
        exit(0);
    }
    if (stream_queue_size.is_open())
    {
        cout <<"stream opened" << endl;
        uint64_t time;
        uint64_t queue = 0;
        long double ewma = 0.0;
        uint32_t MAX_BYTES_ROUTER_BUFFERSIZE;
        
        while (meth_take_and_store_line_values_from_queue_along_time(stream_queue_size, time, queue))
        {
           
            long double bytes_in_queue = static_cast<long double> (queue);
    
            ewma = (long double)((1- exp_weight_expon_queue)*ewma + (exp_weight_expon_queue)*bytes_in_queue);
            
            //Nunca divida por define, pois, não sei por que dá errado
            //por isso, armazenamos na variavels MAX_BYTES_ROUTER_BUFFERSIZE
            MAX_BYTES_ROUTER_BUFFERSIZE = MAX_BYTES_ROUTER_BUFFERSIZE_100Mbps; 
            file <<time << "," <<  fixed << setprecision(5) << (long double) (ewma/MAX_BYTES_ROUTER_BUFFERSIZE) <<"b"<< endl; 
        }
        file.close();
        stream_queue_size.close();
        class_feature_extractor::queue_ewma_generated = true;
        return true;

    }

    return false;
}


void class_feature_extractor::set_queue_size_along_time_file(string par_queue_size_along_time_file)
{
   
    if(out_dir == "/home/ns/Desktop/output")
    {
      cout << "Please, set out_dir" << endl;
      exit(0);
    }
    
    queue_size_along_time_file = par_queue_size_along_time_file;
    queue_size_along_time_file_ewma = queue_size_along_time_file;
    auto pos = queue_size_along_time_file_ewma.find(".");
    if(pos == string::npos)
    {
        cout << "Queue size along time file with no extension."<< endl;
        exit(0);
    }

    queue_size_along_time_file = out_dir + "/" + "router_data" + "/" + queue_size_along_time_file;

    queue_size_along_time_file_ewma.replace(pos,1,"_ewma.");
    queue_size_along_time_file_ewma = out_dir + "/" + "router_data" + "/" + queue_size_along_time_file_ewma;
    
    
    cout << "queue_size_along_time_file_ewma......" << queue_size_along_time_file_ewma <<"\n"; 
    //char c;
    //cin >> c;
}

bool  class_feature_extractor::meth_update_seq_queue_file(uint64_t par_seq, long double par_queue_ewma)
{
    

    bool force_print_in_meth_update_seq_queue_file = false;

    //if((par_queue_ewma > 0.1) && (par_queue_ewma < 0.2))
    //force_print_in_meth_update_seq_queue_file=true;

    class_mrs_debug::print<char>("In meth_update_seq_queue_file",'\n',force_print_in_meth_update_seq_queue_file);

    
    long double queue_ewma_cuted = par_queue_ewma;

    class_mrs_debug::print<long double>("queue_ewma_cuted: ",queue_ewma_cuted,force_print_in_meth_update_seq_queue_file);


    if(par_queue_ewma > 1.0)
        queue_ewma_cuted = 1.0;



    if(queue_ewma_cuted <= 0.000001) //para enriquecer a amostra
    {
        cout << "tiny queue buffer." << endl;
        //return;
    }    
    

    int network_situation = 0;

    

    bool p1 = queue_ewma_cuted <= 0.1;
    bool p3 = (queue_ewma_cuted >= 0.2) && 
              (queue_ewma_cuted <= 0.4);

    
    if(p1 || p3)
    {
        
        class_mrs_debug::print<char>("produce n1! ",'\n',force_print_in_meth_update_seq_queue_file);
        network_situation = 1;
        n1++; //vai acumulando e é armazenado no destructor

    }
    else if(queue_ewma_cuted >= 0.70)
    {
        class_mrs_debug::print<char>("produce n2! ",'\n',force_print_in_meth_update_seq_queue_file);
        network_situation = 2;
        n2++; //vai acumulando e é armazenado no destructor
    }
    else 
    {
        cout << "Queue out of range."<<endl;
        return false;
              
    }

    std::ofstream file;
    //make sure write fails with exception if something is wrong

    if(first_ack_seq_queue_association)//precisa intitular as colunas
    {

        file.open(out_dir + "/" + seq_metrics_file_name, std::ios::out | std::ios::trunc);
        if (file.fail())
        {
            std::cout << "Erro ao registrar Dados de Treinamento no Seq " << par_seq << "\n";
            exit(0);
        }

        file <<"#Ack" << "," 
            << "Last Router Ocupation Router Arrival" << "(" <<"Packets"<< ")"<<"," 
            << "Last Router Ocupation Router Arrival_ewma" << "(" <<"Packets"<< ")"<<","         
            << "Network Situation Router Arrival" <<  ","
            "Measure Time"<<","
            <<"Queue_Router2" << "\n";

        first_ack_seq_queue_association = false;
        file.close();
    
    }



    file.open(out_dir + "/" + seq_metrics_file_name, std::ios::out | std::ios::app);
    if (file.fail())
    {
        std::cout << "Erro ao registrar Dados de Treinamento no Seq " << par_seq << "\n";
        exit(0);
    }
    cout << "updating router file..."<<"\n";
    file << par_seq << ","
        << "0" << "," 
        << queue_ewma_cuted << ","
        << network_situation << ","
        << "0" << "," 
        << "0" << "\n";
        //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();

    file.close();

    return true;

}

class_feature_extractor:: ~class_feature_extractor()
{
    if(stream_queue_size_along_time_file.is_open())
        stream_queue_size_along_time_file.close();



        std::ofstream file;
        file.open(out_dir + "/" + "n1_n2.csv", std::ios::out | std::ios::app);
        if (file.fail())
        {
            std::cout << "n1_n2 register error " << "\n";
            exit(0);
        }
        //make sure write fails with exception if something is wrong
    
        file << n1<<","<< n2 << endl;
            //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();
    
        file.close();
    
    
}


