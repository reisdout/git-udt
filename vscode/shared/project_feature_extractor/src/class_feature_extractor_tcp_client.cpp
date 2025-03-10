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
    class_mrs_debug::print<char>("entering meth_extract_client_feature",'\n');

    meth_check_if_parse_dump_file_is_possible();
    string eco_reply_time_stamp;
    string seq_number;
    string ack_arrival_time;
    

   
    if (par_dump_line.length() > 0)
    {
        
        class_mrs_debug::print<uint64_t>("tcp_extractor_clinet_work_line, atuando sobre os ack : ", ++extractor_tcp_clinet_work_line);
        //cout << dump_line << endl;
        //cout <<"packet: " << packet <<endl;
        //cin >> c;

        class_mrs_debug::print<string>("par_dump_line: " ,par_dump_line);
        
        if(par_dump_line[0]=='#')//evita comentários e linhas de dados dos pacotes
        {
            return false; //não precisa, mas é só para destacar que vai para o próximo.
        }

        else if(par_dump_line.find("ack")!=std::string::npos &&
                par_dump_line.find("IP 10.0.1.3."+to_string(port)+" > ")!= std::string::npos)
        {
            

            if(!first_packet)
            {
                first_packet=true;
                return false;

            }
            ack_arrival_time = meth_search_time_stamp(par_dump_line);
            seq_number =  meth_search_seq_number(par_dump_line);
            eco_reply_time_stamp = meth_search_eco_reply_time_stamp(par_dump_line);

            par_seq = stoull(seq_number);
            par_ack_arrival = stoull(ack_arrival_time);
            par_eco_reply = stoull(eco_reply_time_stamp);
            return true;
        }
        
            
        return false; //não  extraiu
                    
    }

    else
    {
        cout << "No char Dump line" << endl;
        return false;
    }

   return false;

}

string class_feature_extractor_tcp_client::meth_search_eco_reply_time_stamp(string par_dump_line)
{
    string str_ecr;
    str_ecr.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',17));
    return str_ecr;
}

string class_feature_extractor_tcp_client::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    //VVVVRetorna algo do tipo " 2897:4345,
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number = str_seq_number.substr(0,str_seq_number.length()-1);//retira a vírgula
    //cout << "str_seq_number: " << str_seq_number;
    class_mrs_debug::print<string>("str_seq_number em meth_search_seq_number: ", str_seq_number);
    return str_seq_number;
}


string class_feature_extractor_tcp_client::meth_search_time_stamp(string par_dump_line)
{

    string str_time_stamp_temp;
    str_time_stamp_temp = meth_search_occurence_string_between_delimiter(par_dump_line,' ',1);
    meth_erase_dot_from_time_stamp(str_time_stamp_temp);    
    return str_time_stamp_temp;
    
}
