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

    if(!stream_dump_file.is_open())
        stream_dump_file.open(dump_file);
    

    bool done = false;
    bool considered_ack = false;
   

    if (stream_dump_file.is_open())
    {
            
        
        while (!stream_dump_file.eof() && !done)
        {
            class_mrs_debug::print<uint64_t>("\n\nAck to analize: ",par_ack_seq,force_print_in_meth_extract_router_features_ack);
            class_mrs_debug::print<string>("Current dump Line: ", current_dump_line,force_print_in_meth_extract_router_features_ack);
            class_mrs_debug::print<uint64_t>("Current Seq Inf: ", current_seq_number_inf_in_decimal ,force_print_in_meth_extract_router_features_ack); 
            class_mrs_debug::print<uint64_t>("Current Seq Sup: ", current_seq_number_sup_in_decimal,force_print_in_meth_extract_router_features_ack);
    

            //tratar mais de um ack na mesma em um mesmo segmento, já
            //que a coisa é por bytes. Enquanto par_ack_seq-1 > seq_number_sup_in_decimal
            //ainda permaneço no pacote correpondente à linha anterior
            
            //Para escapar do primeiro pacote absurdo e posicionar no primeiro significativo
            if(!first_packet)
            {
                class_mrs_debug::print<char>("Dealing with first packet",'\n',force_print_in_meth_extract_router_features_ack);
                bool resultado_leitura_linha1;// resultado_leitura_linha1;
                bool resultado_leitura_linha2;// resultado_leitura_linha2;
                resultado_leitura_linha1 = meth_get_one_of_my_lines(current_dump_line);//Obteve a estranha linha seq 3807052485:3807055381, ack 4019792363,
                resultado_leitura_linha2 = meth_get_one_of_my_lines(current_dump_line);
                if(!resultado_leitura_linha1 || !resultado_leitura_linha2)
                {
                    cout << "Can't start first router dump readind." << endl;
                    done = true;
                    considered_ack =  false;
                }
               
                
 
                else
                {
                    class_mrs_debug::print<uint64_t>("First packet in extractor_tcp com Ack ", par_ack_seq,force_print_in_meth_extract_router_features_ack);
                    class_mrs_debug::print<string>("First packet in extractor_tcp com linha ", current_dump_line ,force_print_in_meth_extract_router_features_ack);
                    meth_update_seq_numbers(current_dump_line);
                    first_packet=true;
                    done = true;
                    considered_ack = false;

                } 
                
            }

            
            else if(par_ack_seq-1 >= current_seq_number_inf_in_decimal && par_ack_seq-1 <= current_seq_number_sup_in_decimal)
            {
                class_mrs_debug::print<char>("Ack refers to current packet",'\n',force_print_in_meth_extract_router_features_ack);

                if(meth_search_best_queue_size_by_time_stamp(current_time_stamp,queue_now))
                {
                    //essa expansão contempla ack com seq no meio dos bytes de um segmento.
                    //ver notes_to_consider.txt. Se chegou aqui o ack-1 está entre inf e sup.
                 
                        meth_update_seq_queue_file(par_ack_seq-1, queue_now);
                        done = true;
                        considered_ack = true;
                }
                else
                {
                    cout << "Can't associate Ack seq to queue level." << endl;
                    done = true;
                    considered_ack = false;

                }    


            }

            
            else if(par_ack_seq-1 > current_seq_number_sup_in_decimal)//seqs da linha reconhecidos no futuro.
            {
                class_mrs_debug::print<char>("Ack refers to future packet. Reading new line ",'\n',force_print_in_meth_extract_router_features_ack);
                //getline (stream_dump_file,dump_line);
                if(meth_get_one_of_my_lines(current_dump_line))
                {
                    //Atializando a "agulha"
                    meth_update_seq_numbers(current_dump_line);
                    done = false; //continua e ainda nao se sabe se o ack sera considerado
                }
                else
                {
                    cout << "There is no more data of port "<< port << "." << endl;
                    done = true;
                    considered_ack =  false;
                }
            }
 
            else if ((par_ack_seq-1) < current_seq_number_inf_in_decimal) //Ack anacronico
            {
                //O ACK é menor do o limite superior do pacote corrente
                //Assim, ou ele está reconhecendo uma parte dos bytes do
                //pacote corrente (primeiro if) ou está completamente perdido,
                //se é inferior ao limite inferior do seq do pacote (else)
                class_mrs_debug::print<char>("Anacronic Ack of previous packet",'\n',force_print_in_meth_extract_router_features_ack);    
                done = true;
                considered_ack = false; //preciso de outro ack, maior. A princípio não acontece, pois o
            
            }          

            else
            {
                cout << "Can't sync clients and router files" << endl;
                done = true;
                considered_ack =  false;
            }
                                           
                      
        }

  
    }

    else
    {
        cout << "Dump file oppening error" << endl;
        exit(0);
    }

    return considered_ack;

}

bool class_feature_extractor_tcp::meth_get_one_of_my_lines(string& par_dump_line)
{
    
    while (!stream_dump_file.eof())
    {
        getline (stream_dump_file, par_dump_line);
        ++feature_extrator_work_line;
        if(par_dump_line[0]=='#')//evita comentários e linhas de dados dos pacotes
        {
            continue; //não precisa, mas é só para destacar que vai para o próximo.
        }

        else if(par_dump_line.find("[P.],")!=std::string::npos &&
                par_dump_line.find(" > 10.0.1.3."+to_string(port)+ ": Flags [P.], seq")!= std::string::npos)
            {
                class_mrs_debug::print<string>("current port:", to_string(port)+":",force_print_meth_get_one_of_my_lines);
                class_mrs_debug::print<string>("**read line:", par_dump_line,force_print_meth_get_one_of_my_lines);
                
                return true; //found line
            }

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

void class_feature_extractor_tcp::meth_update_seq_numbers(string par_dump_line)
{
    current_time_stamp = meth_search_time_stamp(par_dump_line);                
    current_seq_number_interval  =  meth_search_seq_number(par_dump_line);// 2345:3459
    current_seq_number_inf = meth_search_occurence_string_between_delimiter(current_seq_number_interval ,':',1);//2345                
    current_seq_number_sup = meth_search_occurence_string_between_delimiter(current_seq_number_interval ,':',2);//3459
    current_seq_number_inf_in_decimal = stoull(current_seq_number_inf);
    current_seq_number_sup_in_decimal = stoull(current_seq_number_sup);
}

