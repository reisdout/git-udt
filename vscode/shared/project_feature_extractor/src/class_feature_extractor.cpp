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
    std::ofstream file;
    file.open(seq_metrics_file_name, std::ios::out | std::ios::app);
    if (file.fail())
    {
        std::cout << "Error opening seq buffer file" << endl;
        exit(0);
    }
    file <<"#Ack" << "," 
        << "Last Router Ocupation Router Arrival" << "(" <<"Packets"<< ")"<<"," 
        << "Last Router Ocupation Router Arrival_ewma" << "(" <<"Packets"<< ")"<<","         
        << "Network Situation Router Arrival" <<  ","
        "Measure Time"<<","
        <<"Queue_Router2" << "\n";

        file.close();
     

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
    uint64_t queue_now;
    uint64_t seq_in_decimal;

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
                queue_now =  meth_search_best_queue_size_by_time_stamp(time_stamp);
                seq_in_decimal = std::stoull("0x"+seq_number,0,16);
                float bytes_in_queue = (float) queue_now;
                router_queue_ewma = ((1- exp_weight_expon_queue)*router_queue_ewma + (exp_weight_expon_queue)*bytes_in_queue); 
                meth_update_seq_queue_file(seq_in_decimal, router_queue_ewma);
                

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


uint64_t class_feature_extractor:: meth_search_best_queue_size_by_time_stamp(string par_time_stamp)
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
    
    if(!stream_queue_size_along_time_file.is_open())
        stream_queue_size_along_time_file.open(queue_size_along_time_file);
    
    if(stream_queue_size_along_time_file.is_open())
    {
        
        uint64_t packet_arrival_time;
        par_time_stamp.erase(std::remove(par_time_stamp.begin(), par_time_stamp.end(), '.'), par_time_stamp.end());
        packet_arrival_time = (uint64_t)stoull(par_time_stamp);

        if(!prior_packet_arrival_time)
        {
            //Lendo as duas primeiras linhas
            for(int i = 0; i < 2; i++)
            {               

                if(i==0) //preenchenco o prioritario com os dados a primeira linha
                    meth_take_and_store_line_values(prior_packet_arrival_time, buffer_at_prior_packet_arrival_time);
            
                else//prrenchendo o secundário com os dados da segunda linha
                    meth_take_and_store_line_values(secondary_packet_arrival_time,buffer_at_secondary_packet_arrival_time);
                 
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
                meth_take_and_store_line_values(secondary_packet_arrival_time,buffer_at_secondary_packet_arrival_time);
            }

            

        }


        return buffer_at_prior_packet_arrival_time;
        




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


void class_feature_extractor:: meth_take_and_store_line_values(uint64_t & par_time_stamp, uint64_t & par_queue_buffer)
{
    
    string queue_along_time_file_line;
    string str_line_time_stamp;
    string str_line_queue;


    getline(stream_queue_size_along_time_file, queue_along_time_file_line);
    str_line_time_stamp = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',1);
    
    //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    //Retorna algo do tipo 23456b
    //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
    str_line_queue = meth_search_occurence_string_between_delimiter(queue_along_time_file_line,',',2);//2345
    str_line_queue = str_line_queue.substr(0,str_line_queue.find("b"));
    str_line_queue = meth_deal_with_K_occurence(str_line_queue);

    par_time_stamp = (uint64_t)stoull(str_line_time_stamp);
    par_queue_buffer = (uint64_t)stoull(str_line_queue);

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


void class_feature_extractor::set_queue_size_along_time_file(string par_queue_size_along_time_file)
{
    string line;
    queue_size_along_time_file = par_queue_size_along_time_file;
    queue_size_along_time_file_ewma = queue_size_along_time_file;
    auto pos = queue_size_along_time_file_ewma.find(".");
    queue_size_along_time_file_ewma.replace(pos,1,"_wema.");
    
    ifstream streaam_queue_size (out_dir+"router_data/"+queue_size_along_time_file);

    if(!streaam_queue_size.is_open())
    {
        cout << "Error oppening  queue size  along time file" << endl;
        exit(0);

    }

    std::ofstream file;

    file.open(out_dir+"router_data/"+queue_size_along_time_file_ewma, std::ios::out | std::ios::app);
    if (file.fail())
    {
        std::cout << "Router queue along time ewma error" << endl;
        exit(0);
    }
    if (streaam_queue_size.is_open())
    {
        uint64_t time;
        uint64_t queue=0;
        float ewma=0.0;
        
        while ( getline (streaam_queue_size,line) )
        {
            meth_take_and_store_line_values(time, queue);
            float bytes_in_queue = (float) queue;
            ewma = ((1- exp_weight_expon_queue)*ewma + (exp_weight_expon_queue)*bytes_in_queue);

            file <<time << "," << ewma <<"b"<<endl; 
        }
        file.close();
        streaam_queue_size.close();

    }
}


void class_feature_extractor:: meth_update_seq_queue_file(uint64_t par_seq, float par_queue_ewma)
{

  /*  
  file.open(filepath, std::ios::out | std::ios::app);
  if (file.fail())
  {
    std::cout << "Erro ao registrar Dados de Treinamento no Seq " << parSeq << "\n";
    exit(0);
  }
  //make sure write fails with exception if something is wrong
  file << parSeq << ","
      << (float) queued_packages/(MAXROUTERBUFFERSIZE) << "," 
      << lastRouterQueue_router_arrival_ewma[parIdSrc][parIdDest]/(MAXROUTERBUFFERSIZE) << ","
      << networkSituation << ","
      << routerTime << "\n";
        //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();
  
  file.close();

*/

}
