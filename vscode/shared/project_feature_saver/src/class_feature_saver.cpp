#include "../include/class_feature_saver.h"


class_feature_saver::class_feature_saver()
{

}  


void class_feature_saver::meth_adjust_file_path()
{
   
    char c;
    meth_check_parameters();
    int client_id = port - 9000;
    seq_metrics_file_name = std::string("10_0_") + 
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("to") +
                            std::string("10_1_") +
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("_L_") +
                            str_starting_time +
                            std::string(".csv");
                            std::replace(seq_metrics_file_name.begin(), seq_metrics_file_name.end(),':','_');

    out_dir = out_dir+"/"+tipo_dado+"_"+default_congestion+"_"+num_flows+"Fluxos_"+ bottleneck_datarate+"Mbps_"+str_starting_time;
    
    cout << "out_dir: " << out_dir << endl;
    cin >> c;

    
    int status = mkdir(out_dir.c_str(),0777);
    if(status < 0)
    {
      cout << "Impossível criar pai do diretório de saída";
      exit(0);
    }

    
    std::cout << seq_metrics_file_name << std::endl;     
}



void class_feature_saver::meth_add_file_header()
{

    std::string filepath;
    std::string flowType = "L";
    std::ofstream file;

    /*
      string strSrcAddres = to_string(parBufEndSrc[0])+"_"+
                      to_string(parBufEndSrc[1])+"_"+
                      to_string(parBufEndSrc[2])+"_"+
                      to_string(parBufEndSrc[3]);

  
      string strDestAddres = to_string(parBufEndDest[0])+"_"+
                      to_string(parBufEndDest[1])+"_"+
                      to_string(parBufEndDest[2])+"_"+
                      to_string(parBufEndDest[3]);
      */
    //cout << strDestAddres;
    //deletar rtt.substr(0,rtt.length()-3)
  filepath = out_dir+"/"+seq_metrics_file_name;
  file.open(filepath, std::ios::out | std::ios::app);
  if (file.fail())
  {
    std::cout << "Erro ao registrar Dados de Treinamento";
    exit(0);
  }
  file <<"#Ack" << "," 
         << "ack_ewma(us)" << ","
         << "send_ewma(us)" << ","
         << "rtt_ratio" << ","
         << "cwnd (Bytes)" << ","
         //<< "Buffer variation(%)" << ","
         //<< "queue_ewma" << ","
         << "Last Router Ocupation Ack Arriaval" << "(" <<"Packets"<< ")"<<  ","
         << "Last Router Ocupation Packet Sent" << "(" <<"Packets"<< ")"<<  ","
         << "Network Situation" <<  ","
         << "AckArrival(us)" << ","
         << "TSInsideAck(us)" << "," 
         << "RTTAck(us)" << "\n";
  
  file.close();
  
}


void class_feature_saver::meth_amortize_map(int32_t par_seq)
{
    
    if(map_seq_timestamp.empty())
    {
        cout << "não amortizou.... " << par_seq << endl;
        return;
    }    

    
    
    cout << "Entering amortization. The atual map elements are : \n"; 
    for (auto it1 = map_seq_timestamp.begin(); it1 != map_seq_timestamp.end(); ++it1) 
    cout << it1->first << "->" << endl; 
    //map <uint32_t, high_resolution_clock::time_point>::iterator it;

    

    //cout << "amortiz.... " << par_seq << endl;

    
    bool done = false;
    uint32_t erased;
    while (!done)
    {
        cout << "Há " <<  map_seq_timestamp.count(par_seq) << " com seq == " <<par_seq<<endl;
        done=true;
        for(auto it = map_seq_timestamp.begin(),next_it = it; it != map_seq_timestamp.end(); it=next_it)
        {
        
            ++next_it;
            if(it->first < par_seq && !map_seq_timestamp.empty())
            {
                
                cout << "amortiz  " << it->first << endl;
                erased = map_seq_timestamp.erase(it->first);
                cout << "erased:  " << erased << endl;
                done=false;
                break;
                        
            }
            else
            {
                done=true;
                break;
            }
                
        }
    }

        cout << "The atual map elements are : \n"; 
        for (auto it1 = map_seq_timestamp.begin(); it1 != map_seq_timestamp.end(); ++it1) 
        cout << it1->first << "->" << endl; 
}


 void class_feature_saver::meth_calculate_ack_ewma(uint32_t par_seq, 
                                                   high_resolution_clock::time_point par_ack_arrival_time)
 {

    numAckReceived=(numAckReceived+1);
    marcaTempoChegadaAck = par_ack_arrival_time;

    if(first_ack_process)
    {
        auto delta_t = duration_cast<microseconds>(marcaTempoChegadaAck - marcaTempoChegadaAckAnterior);
        float intervalFromPreviousAck = (float) delta_t.count();
        ack_ewma = (float)(((1.0-expWeightExpon )*ack_ewma) + (expWeightExpon *intervalFromPreviousAck));
        cout << "ack_ewma: " << ack_ewma<< "; "<<"Dt: " <<intervalFromPreviousAck <<  endl;

    }


    marcaTempoChegadaAckAnterior = marcaTempoChegadaAck;



 }



 void class_feature_saver::meth_calculate_rtt(uint32_t par_seq, 
                         high_resolution_clock::time_point par_ack_arrival_time)
{
    
    auto pre_rtt = duration_cast<microseconds>(par_ack_arrival_time - map_seq_timestamp.at(par_seq-1));
    rtt = (double)pre_rtt.count();
    cout << "rtt: " << rtt << "; Marca ACK: " << par_ack_arrival_time.time_since_epoch().count()<<"; Marca Send: "<< map_seq_timestamp.at(par_seq-1).time_since_epoch().count() << endl;

}

void class_feature_saver::meth_calculate_send_ewma(uint32_t par_seq)
{

    //seq-1, pois o ack é a espera do próximio byte ou segmento
    if(map_seq_timestamp.find(par_seq-1) != map_seq_timestamp.end())
    {
        cout << "found seq: " << par_seq-1 << endl;
        marcaTempoAtual = map_seq_timestamp.at(par_seq-1);
    }
    else
    {
        cout << "NOT found seq: " << par_seq-1 << endl;
        return;
    }
    if(first_ack_process)
    {
        auto delta_t = duration_cast<microseconds>(marcaTempoAtual - marcaTempoAnterior);
        intervalbetweenTS = (float) delta_t.count();
        send_ewma = (float)((1-expWeightExpon )*send_ewma + (expWeightExpon )*intervalbetweenTS);
        cout << "send_ewma: " << send_ewma<< "; "<<"Dt: " << intervalbetweenTS <<  endl;

    }

 
    marcaTempoAnterior = marcaTempoAtual;
    
}


void class_feature_saver::meth_check_parameters()
{
    if(!port)
    {
      cout << "Please, set port" << endl;
      exit(0);
    }
    if(!tipo_dado.length())
    {
      cout << "Please, set data type" << endl;
      exit(0);
    }
    
    if(!default_congestion.length())
    {
      cout << "Please, set default congestion" << endl;
      exit(0);
    }
    if(!num_flows.length())
    {
      cout << "Please, set num flows" << endl;
      exit(0);
    }
    if(!bottleneck_datarate.length())
    {
      cout << "Please, set data rate" << endl;
      exit(0);
    }

    if(!str_starting_time.length())
    {

      cout << "Please, set starting time" << endl;
      exit(0);


    }
}


void class_feature_saver::meth_deal_ack_arrival(uint32_t par_seq, 
                                                high_resolution_clock::time_point par_ack_arrival_time)
{

    cout << "======Receiving seq "<< par_seq <<"======"<<endl;

    meth_calculate_ack_ewma(par_seq, par_ack_arrival_time);
    meth_calculate_send_ewma(par_seq);
    meth_calculate_rtt(par_seq, par_ack_arrival_time);
    meth_amortize_map(par_seq);
    
    meth_save_training_data(par_seq-1, //numAckReceived[idAckReceiver][idAckSender], 
                         ack_ewma, 
                         send_ewma,
                         rtt,
                         0,
                         0,
                         0,
                         0,
                         0,
                         par_ack_arrival_time, 
                         marcaTempoAtual,//ts->GetEcho(),
                         intervalbetweenTS,
                         0,
                         0,
                         "L",
                         port,
                         port,
                        false);
    
    
    first_ack_process = true;

}

void class_feature_saver::meth_deal_packet_send(uint32_t par_seq, 
                                                high_resolution_clock::time_point par_send_time)
{

    map_seq_timestamp.insert({par_seq, par_send_time});

}




void class_feature_saver::meth_save_training_data(   unsigned int parNumAckFlow, 
                                        float parAck_ewma, 
                                        float parSend_ewma, 
                                        float parRtt_ratio,
                                        uint32_t parCwnd,
                                        int parBuffeSizeVariation,
                                        float parRouterQueue_ewma,
                                        uint32_t parRouterBufferSizeValueAckArrival,
                                        uint32_t parRouterBufferSizeValuePacketSent,
                                        high_resolution_clock::time_point parTSAckArrival,
                                        high_resolution_clock::time_point parTSInsideAck,
                                        double parMimRTTAck,
                                        uint8_t* parBufEndSrc, 
                                        uint8_t* parBufEndDest,
                                        string parFlowType,
                                        int parIdSrc,
                                        int parIdDest,
                                        bool parUnicFile)
{

  //escapando do slowStart
  //if((Simulator::Now().GetSeconds() - lastTimeCwndIsZero[parIdSrc][parIdDest] <=50) && parFlowType == LONG_FLOW)
    //return;
  
  if(!first_ack_process)
  {
    meth_add_file_header();
    return;
  }


  int networkSituation = 1;
  string filepath;
  string flowType;

  flowType = parFlowType;

  /*
  if (parRouterBufferSizeValueAckArrival <= 60)
    networkSituation = 1;
  else if (parRouterBufferSizeValueAckArrival >= 80 && parRouterBufferSizeValueAckArrival <= 95)
    networkSituation = 2;
  //else if (parRouterBufferSizeValueAckArrival >= 70 && parRouterBufferSizeValueAckArrival <= 95)
  //  networkSituation = 3;
  else
    return;
  */

  /*
  
  if (parRouterQueue_ewma <= 10 || 
     (parRouterQueue_ewma >= 20 && parRouterQueue_ewma <= 30)||  
     (parRouterQueue_ewma >= 40 && parRouterQueue_ewma <= 45))
    networkSituation = 1;
  else if ((parRouterQueue_ewma >=75 && parRouterQueue_ewma <= 90)) //||       (parRouterQueue_ewma >=90 && parRouterQueue_ewma <= 95))
    networkSituation = 2;
  
  //else if (parRouterBufferSizeValueAckArrival >= 70 && parRouterBufferSizeValueAckArrival <= 95)
  //  networkSituation = 3;
  
  //PARA FEEDFOWARD!!!!!!!!!!!!!!!!!!!!!!!!!!!
  else
    return false;
  
  */

  if(parRouterQueue_ewma >= 75)
    networkSituation = 2;

  
  if(parUnicFile)
    filepath = out_dir+"/"+seq_metrics_file_name;

  else
  {
    /*
    string strSrcAddres = to_string(parBufEndSrc[0])+"_"+
                      to_string(parBufEndSrc[1])+"_"+
                      to_string(parBufEndSrc[2])+"_"+
                      to_string(parBufEndSrc[3]);

  
    string strDestAddres = to_string(parBufEndDest[0])+"_"+
                      to_string(parBufEndDest[1])+"_"+
                      to_string(parBufEndDest[2])+"_"+
                      to_string(parBufEndDest[3]);
    */
    //cout << strDestAddres;
    //deletar rtt.substr(0,rtt.length()-3)
    filepath = out_dir+"/"+seq_metrics_file_name;
  }
  //std::replace(filepath.begin(),filepath.end(),':','_');
  std::ofstream file;
  //std::ifstream testfile;
  //can't enable exception now because of gcc bug that raises ios_base::failure with useless message
  //file.exceptions(file.exceptions() | std::ios::failbit);
  /*testfile.open(filepath, std::ios::in);
  if (testfile.fail())
  {
    cabecalho = true;
  }
  else
    testfile.close();
  */

  file.open(filepath, std::ios::out | std::ios::app);
  if (file.fail())
  {
    std::cout << "Erro ao registrar Dados de Treinamento";
    exit(0);
  }
  //make sure write fails with exception if something is wrong
  file << parNumAckFlow << ","
       <<parAck_ewma << "," 
       << parSend_ewma << "," 
       << parRtt_ratio << ","
       << parCwnd << ","
       //<< (float)parBuffeSizeVariation/MAXROUTERBUFFERSIZE << ","
       << parRouterQueue_ewma/MAX_PACKETS_ROUTER_BUFFERSIZE << ","
       //<< (float)parRouterBufferSizeValueAckArrival/MAXROUTERBUFFERSIZE << ","
       << parRouterBufferSizeValuePacketSent/MAX_PACKETS_ROUTER_BUFFERSIZE << ","
       << networkSituation << ","
       << parTSAckArrival.time_since_epoch().count() << ","
       << parTSInsideAck.time_since_epoch().count()  << "," 
       << parMimRTTAck << "\n";
         //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();
  
  file.close();
  //return true;


}




