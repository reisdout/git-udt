#include "../include/class_feature_extractor_tcp.h"

#include "../include/class_feature_extractor.h"

void class_feature_extractor_tcp::meth_extract_router_features()
{
    
    this->meth_check_if_parse_dump_file_is_possible();

    std::string dump_line;
    float queue_now;
    uint64_t seq_in_decimal;
    //char c;
    //cout << "dump_file: " << dump_file << endl;
    class_feature_extractor_tcp_print<string>("dump_line: ", dump_file);
    ifstream stream_dump_file (dump_file);
    string time_stamp;
    string seq_number;

    uint64_t work_line=0;

    

    if (stream_dump_file.is_open())
    {
        while ( getline (stream_dump_file,dump_line) )
        {

            
            cout << "work line: " << work_line++<< endl;
            //cout << dump_line << endl;
            //cout <<"packet: " << packet <<endl;
            //cin >> c;

    
            
            if(dump_line[0]=='#')//evita comentários e linhas de dados dos pacotes
            {
                continue; //não precisa, mas é só para destacar que vai para o próximo.
            }

            else if(dump_line.find("[P.],")!=std::string::npos &&
                    dump_line.find("> 10.0.1.3")!= std::string::npos &&
                    dump_line.find(to_string(port)+":") != string::npos)
            {
                class_feature_extractor_tcp_print<string>("dump_line: ", dump_line);
                if(!first_packet)
                {
                    first_packet=true;
                    continue;
                }
                time_stamp = meth_search_time_stamp(dump_line);
                meth_erase_dot_from_time_stamp(time_stamp);
                seq_number =  meth_search_seq_number(dump_line);
                seq_number = meth_search_occurence_string_between_delimiter(seq_number,':',2);
                if(meth_search_best_queue_size_by_time_stamp(time_stamp,queue_now))
                {
                    seq_in_decimal = std::stoull(seq_number);
                    meth_update_seq_queue_file(seq_in_decimal, queue_now);
                    continue;
                }

                else
                {
                    cout << "Can't update seq_queue file" << endl;
                    exit(0);
                }



            }
            
                
                
                      
        }

        stream_dump_file.close();
       
    
    }
    else
    {
        cout << "Dump file oppening error" << endl;
        exit(0);
    }

    return;

}


void class_feature_extractor_tcp::meth_extract_virtual_clock_origin(string par_dump_line)
{
    string str_standart_time_stamp;
    str_standart_time_stamp.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',1));
    meth_prepare_time_stamp(str_standart_time_stamp);
    virtual_clock_origin = (uint64_t)stoull(str_standart_time_stamp);
    

}



string class_feature_extractor_tcp::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    //VVVVRetorna algo do tipo " ack 2897
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number = str_seq_number.substr(0,str_seq_number.length()-1);
    //cout << "str_seq_number: " << str_seq_number;
    return str_seq_number;
}


string class_feature_extractor_tcp::meth_search_time_stamp(string par_dump_line)
{

    
    return meth_search_occurence_string_between_delimiter(par_dump_line,' ',1);
    
}

