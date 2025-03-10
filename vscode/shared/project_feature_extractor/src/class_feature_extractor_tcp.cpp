#include "../include/class_feature_extractor_tcp.h"

#include "../include/class_feature_extractor.h"
#include "mrs_utils.h"

void class_feature_extractor_tcp::meth_extract_router_features()
{
    
    meth_check_if_parse_dump_file_is_possible();

    std::string dump_line;
    float queue_now;
    //char c;
    //cout << "dump_file: " << dump_file << endl;
    stream_dump_file.open(dump_file);
    string time_stamp;
    string seq_number;
    string seq_number_inf;
    string seq_number_sup;
    uint64_t seq_number_inf_in_decimal;
    uint64_t seq_number_sup_in_decimal;

    uint64_t work_line=0;

    

    if (stream_dump_file.is_open())
    {
        while ( getline (stream_dump_file,dump_line) )
        {

            
            class_mrs_debug::print<uint64_t>("workline: ", ++work_line);
            class_mrs_debug::print<string>("dump_line: ", dump_line);
            //cout << "work line: " << ++work_line<< endl;
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
                class_mrs_debug::print<string>("dump_line: ", dump_line);
                if(!first_packet)
                {
                    first_packet=true;
                    continue;
                }
                time_stamp = meth_search_time_stamp(dump_line);                
                seq_number =  meth_search_seq_number(dump_line);// 2345:3459
                seq_number_inf = meth_search_occurence_string_between_delimiter(seq_number,':',1);//2345                
                seq_number_sup = meth_search_occurence_string_between_delimiter(seq_number,':',2);//3459
                seq_number_inf_in_decimal = stoull(seq_number_inf);
                seq_number_sup_in_decimal = stoull(seq_number_sup);
                if(meth_search_best_queue_size_by_time_stamp(time_stamp,queue_now))
                {
                    //essa expansão contempla ack com seq no meio dos bytes de um segmento.
                    //ver notes_to_consider.txt.
                    for (uint64_t i = seq_number_inf_in_decimal; i <= seq_number_sup_in_decimal;i++)
                        meth_update_seq_queue_file(i, queue_now);
                    //continue;
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

bool class_feature_extractor_tcp::meth_extract_router_features(uint64_t par_ack_seq)
{
    if(last_ack_processed >= par_ack_seq)//Ack anacronico
        return false;
    
    last_ack_processed = par_ack_seq;
    meth_check_if_parse_dump_file_is_possible();

    std::string dump_line;
    
    //char c;
    //cout << "dump_file: " << dump_file << endl;
    if(!stream_dump_file.is_open())
        stream_dump_file.open(dump_file);
    string time_stamp;

       

    if (stream_dump_file.is_open())
    {
            
        
        while (!stream_dump_file.eof())
        {

            //tratar mais de um ack na mesma em um mesmo segmento, já
            //que a coisa é por bytes. Enquanto par_ack_seq-1 > seq_number_sup_in_decimal
            //ainda permaneço no pacote correpondente à linha anterior             
            if(!first_packet || par_ack_seq-1 > seq_number_sup_in_decimal)
            {
                class_mrs_debug::print<char>("Ack siginifcativo, lendo nova linha ",'\n');
                getline (stream_dump_file,dump_line);
            }
            else
            {
                class_mrs_debug::print<char>("Ack referente a pacote anterior ou corrente",'\n');
                meth_update_seq_queue_file(par_ack_seq-1, queue_now);
                //Só se calcula novos limites se for obtida uma nova linha.
                return true;
            }
            
            class_mrs_debug::print<uint64_t>("feature_extrator_work_line, nos colhidos pelo roteador: ", ++feature_extrator_work_line);
            class_mrs_debug::print<string>("dump_line, nos colhidos pelo roteador: ", dump_line);
            //cout << "work line: " << ++work_line<< endl;
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
                
                if(!first_packet)
                {
                    first_packet=true;
                    continue;
                }
                time_stamp = meth_search_time_stamp(dump_line);                
                seq_number =  meth_search_seq_number(dump_line);// 2345:3459
                seq_number_inf = meth_search_occurence_string_between_delimiter(seq_number,':',1);//2345                
                seq_number_sup = meth_search_occurence_string_between_delimiter(seq_number,':',2);//3459
                seq_number_inf_in_decimal = stoull(seq_number_inf);
                seq_number_sup_in_decimal = stoull(seq_number_sup);
                if(seq_number_sup_in_decimal < (par_ack_seq-1))//seqs reconhecidos no futuro. 
                {
                    class_mrs_debug::print<char> ("Ack referente a linha à frente",'\n');
                    continue;
                }
                if (seq_number_inf_in_decimal > (par_ack_seq-1)) //Ack anacronico
                {    
                    class_mrs_debug::print<char> ("Precisa de ack maior",'\n'); 
                    return false; //preciso de outro ack, maior. A princípio não acontece, pois o
                
                }          // teste inicial com last_ack_processed bloqueia.
                if(meth_search_best_queue_size_by_time_stamp(time_stamp,queue_now))
                {
                    //essa expansão contempla ack com seq no meio dos bytes de um segmento.
                    //ver notes_to_consider.txt. Se chegou aqui o ack-1 está entre inf e sup.
                 
                        meth_update_seq_queue_file(par_ack_seq-1, queue_now);
                        return true;
                }
                   
                

                else
                {
                    cout << "Can't update seq_queue file" << endl;
                    exit(0);
                }



            }
            else
            {
                cout << "Unusual line" << endl;   
                return false;
            }     
                
                
                      
        }

        
       
    
    }
    else
    {
        cout << "Dump file oppening error" << endl;
        exit(0);
    }

    return false;

}




string class_feature_extractor_tcp::meth_search_seq_number(string par_dump_line)
{
    //uint32_t seq_number;
    string str_seq_number;
    //VVVVRetorna algo do tipo " ack 2897:4345,
    str_seq_number.append(meth_search_occurence_string_between_delimiter(par_dump_line,' ',9));
    str_seq_number = str_seq_number.substr(0,str_seq_number.length()-1);//retira a vírgula
    //str_seq_number = meth_search_occurence_string_between_delimiter(str_seq_number,':',2);//pega o ultimo numero, apos o : 
    //cout << "str_seq_number: " << str_seq_number;
    return str_seq_number;
}


string class_feature_extractor_tcp::meth_search_time_stamp(string par_dump_line)
{

    string time_stamp_temp;
    time_stamp_temp = meth_search_occurence_string_between_delimiter(par_dump_line,' ',1);
    meth_erase_dot_from_time_stamp(time_stamp_temp);
    return time_stamp_temp;
    
}

