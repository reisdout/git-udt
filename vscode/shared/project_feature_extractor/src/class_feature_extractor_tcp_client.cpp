#include "../include/class_feature_extractor_tcp_client.h"

#include "../include/class_feature_extractor.h"



#include "mrs_utils.h"


bool class_feature_extractor_tcp_client::meth_check_if_parse_dump_file_is_possible()
{
    if(!port)
    {
      cout << "From class_feature_extractor_tcp_client:  Please, set port" << endl;
      exit(0);
    }



    return true;

}

bool class_feature_extractor_tcp_client::meth_extract_client_feature(string par_dump_line, 
                                                                     uint64_t& par_seq,
                                                                     uint64_t& par_ack_arrival,
                                                                     uint64_t& par_eco_reply)

{
    class_mrs_debug::print<char>("entering meth_extract_client_feature",'\n',force_print_tcp_extractor_client);

    meth_check_if_parse_dump_file_is_possible();
    string eco_reply_time_stamp;
    string seq_number;
    string ack_arrival_time;
    

   
    if (par_dump_line.length() > 0)
    {
        
        //nesse caso, a work_line é do main
        //class_mrs_debug::print<uint64_t>("tcp_extractor_clinet_work_line, atuando sobre os ack : ", ++extractor_tcp_clinet_work_line);
        //cout << dump_line << endl;
        //cout <<"packet: " << packet <<endl;
        //cin >> c;

        class_mrs_debug::print<string>("par_dump_line: " ,par_dump_line);
        
        if(par_dump_line[0]=='#')//evita comentários e linhas de dados dos pacotes
        {
            return false; //não precisa, mas é só para destacar que vai para o próximo.
        }

        else if(par_dump_line.find("ack")!=std::string::npos &&
                par_dump_line.find("[.]")!=std::string::npos && //inserida, pois no iperf3 há mais de um three way, que faz com que seja ferida as duas outras condições
                par_dump_line.find("IP 10.0.1.3."+to_string(port)+" > ")!= std::string::npos)
        {
            class_mrs_debug::print<char>("intersting line in extractor_tcp_client:",'\n',force_print_tcp_extractor_client);

            //O método a seguir foi para atender o iperf
            //que muitas vezes muda a porta do cliente durante a conexão
            //Isso tirava o sincronismo dos ACK, que mudavam de range com a mudança
            // da porta. O método abaixo controla essa mudança para retomar o novo range 
            //de ACK.
            meth_update_port_state(par_dump_line);


            if(!first_packet_processed)//já foi o first_packet?
            {
                cout << "first_package in extractor_tcp_client"<< endl;
                first_packet_processed=true;
                return false;

            }
            ack_arrival_time = meth_search_time_stamp(par_dump_line);
            seq_number =  meth_search_seq_number(par_dump_line);
            eco_reply_time_stamp = meth_search_eco_reply_time_stamp(par_dump_line);

            par_seq = stoull(seq_number);
            par_ack_arrival = stoull(ack_arrival_time);
            par_eco_reply = stoull(eco_reply_time_stamp);
            class_mrs_debug::print<char>("features extracted  in extractor_tcp_client:",'\n',force_print_tcp_extractor_client);
            class_mrs_debug::print<uint64_t>("par_seq ", par_seq, force_print_tcp_extractor_client);
            class_mrs_debug::print<uint64_t>("par_ack_arrival ", par_ack_arrival, force_print_tcp_extractor_client);
            class_mrs_debug::print<uint64_t>("par_eco_reply ", par_eco_reply, force_print_tcp_extractor_client);
            return true;
        }
        
            
        class_mrs_debug::print<char>("features not extracted  in extractor_tcp_client:",'\n',force_print_tcp_extractor_client);
        return false; //não  extraiu
                    
    }

    else
    {
        cout << "No char Dump line" << endl;
        return false;
    }

   return false;

}

bool class_feature_extractor_tcp_client::meth_give_time_stamp_to_synchronize_clock(u_int64_t & par_time_stamp,
                                                                                   u_int64_t &  par_TS_value)
{
    class_mrs_debug::print<char>("meth_give_time_stamp_to_synchronize_clock", '\n',force_print_tcp_extractor_client);
 

    if(!SYN_file.length())
    {
        cout << "No SYNC file" << endl;
        return false;
    }

    class_mrs_debug::print<string>("SYN_file", SYN_file,force_print_tcp_extractor_client);

    string SYN_line;
    bool extracted_clock_marks = false;

    ifstream stream_SYN_file (SYN_file);
    if (stream_SYN_file.is_open())
    {
        
        cout << "stream opend" << endl;
        
        while (getline (stream_SYN_file ,SYN_line) && !extracted_clock_marks)
        {
            //A condição abaixo independe da porta, pois a origem de tempo é única
            //Ou seja, procura-se um SYN qualquer, que já fornece a origem.
            if(SYN_line.find("[S],")!=std::string::npos &&
            SYN_line.find(string(" > ") + "10.0.1.3.")!= std::string::npos)
            {
                 class_mrs_debug::print<char>("Getting virtual clock origin in extractor_tcp_client ", '\n',force_print_tcp_extractor_client);
                 string syn_time_stamp = class_feature_extractor::meth_search_occurence_string_between_delimiter(SYN_line,' ',1);
                 class_feature_extractor::meth_erase_dot_from_time_stamp(syn_time_stamp);
                 string syn_TS_val = class_feature_extractor::meth_search_occurence_string_between_delimiter(SYN_line,' ',16);
                 class_mrs_debug::print<string>("syn_time_stamp: ", syn_time_stamp,force_print_tcp_extractor_client);
                 class_mrs_debug::print<string>("sys_TS_val: ", syn_TS_val,force_print_tcp_extractor_client);
                 par_time_stamp = stoull(syn_time_stamp);
                 par_TS_value = stoull(syn_TS_val);
                 extracted_clock_marks = true;
            }
        
            if(extracted_clock_marks)
            {
                cout << "Clock Marks successfully extracted" << endl;
                return true;
            }
            else
            {
                cout << "Clock Marks not extracted" << endl;
                return false;

            }

        }
    }
    else
    {
        cout << "Can't oppen SYN file" << endl;
        return false;
    }

    return extracted_clock_marks;

}

string class_feature_extractor_tcp_client::meth_search_eco_reply_time_stamp(string par_dump_line)
{
    string str_ecr;
    str_ecr.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',17));
    str_ecr = str_ecr.substr(0,str_ecr.length()-2);//retirar "],"
    class_mrs_debug::print<string>("str_ecr em meth_search_eco_reply_time_stamp: ", str_ecr);
    return str_ecr;
}

string class_feature_extractor_tcp_client::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    //VVVVRetorna algo do tipo " 3309731283,
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number = str_seq_number.substr(0,str_seq_number.length()-1);//retira a vírgula
    //cout << "str_seq_number: " << str_seq_number;
    class_mrs_debug::print<string>("str_seq_number em meth_search_seq_number: ", str_seq_number,force_print_tcp_extractor_client);
    return str_seq_number;
}


string class_feature_extractor_tcp_client::meth_search_time_stamp(string par_dump_line)
{

    string str_time_stamp_temp;
    str_time_stamp_temp = meth_search_occurence_string_between_delimiter(par_dump_line,' ',1);
    meth_erase_dot_from_time_stamp(str_time_stamp_temp);    
    return str_time_stamp_temp;
    
}

void class_feature_extractor_tcp_client::meth_update_port_state(string par_dump_line)
{
    
    class_mrs_debug::print<char>("class_feature_extractor_tcp_client::meth_update_port_state", '\n', force_print_tcp_extractor_client);
    class_mrs_debug::print<uint32_t>("current_port: ", current_port, force_print_tcp_extractor_client);
    
    string str_port = class_feature_extractor::meth_search_occurence_string_between_delimiter(par_dump_line,' ',5);//10.0.0.3.60434:
    class_mrs_debug::print<string>("brut str_port: ", str_port + "|", force_print_tcp_extractor_client);
    
    str_port = class_feature_extractor::meth_search_occurence_string_between_delimiter(str_port,'.',5);//60434:
    
    str_port = str_port.substr(0,str_port.length()-1);
    class_mrs_debug::print<string>("str_port: ", str_port, force_print_tcp_extractor_client); 
    
    uint32_t temp_port = stoul(str_port);
    class_mrs_debug::print<uint32_t>("temp_port: ", temp_port, force_print_tcp_extractor_client);
    class_mrs_debug::print<bool>("first_packet_processed: ", first_packet_processed, force_print_tcp_extractor_client);

    if(current_port == temp_port || !first_packet_processed)
    {
        port_change = false;
    }
    else
        port_change = true;

    current_port = temp_port;

    class_mrs_debug::print<bool>("port_change: ", port_change, force_print_tcp_extractor_client);

    class_mrs_debug::print<uint32_t>("current_port: ", current_port, force_print_tcp_extractor_client);

    
}
