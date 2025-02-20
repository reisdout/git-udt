#include "../include/class_feature_extractor.h"
//#include <bits/stdc++.h>

# define FIRST_PACKET_LINE 1
# define SECOND_PACKET_LINE 2
# define THIRD_PACKET_LINE 3
# define FORTH_PACKET_LINE 4
# define DATA_PACKET_LINE 5


class_feature_extractor::class_feature_extractor()
{
    if(!port)
    {
        cout << "Please, set port first" << endl;
        exit(0);
    }



}

void class_feature_extractor::meth_adjust_file_path()
{
   
   /*
   if(!port)
    {
      cout << "Porta nao configurada";
      exit(0);
    }
    meth_start_simulation_time();
    int client_id = port - 9000;
    path_seq_metrics_file = std::string("10_0_") + 
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("to") +
                            std::string("10_1_") +
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("_L_") +
                            str_starting_time.substr(0,str_starting_time.length()-6) +
                            std::string(".csv");
                            std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');

    std::cout << path_seq_metrics_file << std::endl;     


void class_feature_extractor::meth_extract_router_features(string par_path_router_file)
{
    std::string dump_line;

    //cout << parPathTopology << "\n";
    //cout << parPathFlow << "\n";
    //myPause();
    ifstream dump_file (par_path_router_file);
    bool packet=false;
    int packet_line=0;

    if (dump_file.is_open())
    {
        while ( getline (dump_file,dump_line) )
        {
            if(dump_line[0]!='#')//evitar comentários
            {
                packet = 0;
                break;
            } 

            else if(packet)
            {
                if (packet_line == SECOND_PACKET_LINE)
                {
                    packet_line++;
                    break;
                }
                else if(packet_line == THIRD_PACKET_LINE)
                {
                    get_sequence_number(dump_line);
                }

            }


            //if its starting a packet
            else if(dump_line.find("10.0.0.3.41708 > 10.0.1.3")!=std::string::npos &&
                     dump_line.find("UDP, length 1472")!=std::string::npos)

            {//10.0.0.3.41708 > 10.0.1.3.9000: UDP, length 1472

                packet = true;
                get_time_stamp(dump_line);
                packet_line = SECOND_PACKET_LINE;

            }          
        }
       
    
    }

    else
    { 
      std::cout << "Unable to open file\n"; 
      exit(0);
    }
*/

    return;

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

