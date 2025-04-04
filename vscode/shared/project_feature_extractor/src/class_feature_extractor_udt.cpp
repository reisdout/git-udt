#include "../include/class_feature_extractor_udt.h"

#include "../include/class_feature_extractor.h"

void class_feature_extractor_udt::meth_extract_router_features()
{
    
    this->meth_check_if_parse_dump_file_is_possible();

    std::string dump_line;

    uint64_t seq_in_decimal;
    //char c;
    //cout << "dump_file: " << dump_file << endl;

    ifstream stream_dump_file (dump_file);
    int packet = 0;
    string time_stamp;
    string seq_number;

    uint64_t work_line=0;

    if (stream_dump_file.is_open())
    {
        while ( getline (stream_dump_file,dump_line) )
        {
            
            cout << "work line: " << ++work_line<< endl;
            //cout << dump_line << endl;
            //cout <<"packet: " << packet <<endl;
            //cin >> c;
            
            if(dump_line[0]=='#')//evita comentários e linhas de dados dos pacotes
            {
                packet = 0;
                continue; //não precisa, mas é só para destacar que vai para o próximo.
            }

            else if(dump_line.find("10.0.0.3")!=std::string::npos &&
                    dump_line.find("UDP, length 1472")!=std::string::npos &&
                    dump_line.find(to_string(port)+":") != string::npos)
                     
            {
                time_stamp = meth_search_time_stamp(dump_line);
                cout << "time_stamp: " << time_stamp<< "\n";
                //char c;
                //cin >> c;                
                packet = FIRST_PACKET_LINE;
                continue;
            }


            else if(packet == FIRST_PACKET_LINE)
            {
      
                packet = SECOND_PACKET_LINE;
                continue;

            }

            else if(packet == SECOND_PACKET_LINE)
            {
                seq_number =  meth_search_seq_number(dump_line);
                packet = THIRD_PACKET_LINE;
                continue;//não precisa, mas é só pra marcar que vai para outra linha
            }
            else if(packet == THIRD_PACKET_LINE)
            {
                
                if(meth_search_best_queue_size_by_time_stamp(time_stamp,queue_now))
                {
                    seq_in_decimal = std::stoull("0x"+seq_number,0,16);
                    meth_update_seq_queue_file(seq_in_decimal, queue_now);
                    packet = 0;
                    continue;
                }
                else
                {
                    stream_dump_file.close();
                    return;
                }                
                
            }

            else
                packet = 0;          
                      
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



string class_feature_extractor_udt::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',10));
    //cout << "str_seq_number: " << str_seq_number;
    return str_seq_number;
}


string class_feature_extractor_udt::meth_search_time_stamp(string par_dump_line)
{

    string time_stamp_temp;
    time_stamp_temp = meth_search_occurence_string_between_delimiter(par_dump_line,' ',1);
    meth_erase_dot_from_time_stamp(time_stamp_temp);
    return time_stamp_temp;
    
}

