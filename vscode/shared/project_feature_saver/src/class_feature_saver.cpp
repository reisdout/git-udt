#include "../include/class_feature_saver.h"
#include <iomanip>


class_feature_saver::class_feature_saver()
{

}  


void class_feature_saver::meth_adjust_file_path(string par_out_dir)
{
   
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

    //completando o out_dir com os dados do experimento.
    if(!par_out_dir.length())//diretorio inedito
      out_dir = out_dir + "/" +tipo_dado+"_"+default_congestion+"_"+num_flows+"Fluxos_"+ bottleneck_datarate+"Mbps_"+str_starting_time;
    else //já há um diretório
      out_dir = out_dir + "/" + par_out_dir;
    cout << "out_dir: " << out_dir << endl;
    //char c;
    //cin >> c;

    struct stat sb;
    if (stat(out_dir.c_str(), &sb) == 0)
        cout << "out_dir already created";
    else
    {
        int status = mkdir(out_dir.c_str(),0777);
        if(status < 0)
        {
          cout << "Impossível criar pai do diretório de saída";
          exit(0);
        }
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


void class_feature_saver::meth_check_parameters()
{
    if(!port)
    {
      cout << "From class_feature_saver: Please, set port" << endl;
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


void class_feature_saver::meth_save_training_data(uint64_t parNumAckFlow, 
                                                    float parAck_ewma, 
                                                    float parSend_ewma, 
                                                    float parRtt_ratio,
                                                    uint32_t parCwnd,
                                                    int parBuffeSizeVariation,
                                                    float parRouterQueue_ewma,
                                                    uint32_t parRouterBufferSizeValueAckArrival,
                                                    uint32_t parRouterBufferSizeValuePacketSent,
                                                    uint64_t parTSAckArrival,
                                                    uint64_t parTSInsideAck,
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
       << fixed << setprecision(5) << parAck_ewma << "," 
       << fixed << setprecision(5) << parSend_ewma << "," 
       << fixed << setprecision(3) << parRtt_ratio << ","
       << parCwnd << ","
       //<< (float)parBuffeSizeVariation/MAXROUTERBUFFERSIZE << ","
       << parRouterQueue_ewma/MAX_PACKETS_ROUTER_BUFFERSIZE << ","
       //<< (float)parRouterBufferSizeValueAckArrival/MAXROUTERBUFFERSIZE << ","
       << parRouterBufferSizeValuePacketSent/MAX_PACKETS_ROUTER_BUFFERSIZE << ","
       << networkSituation << ","
       << parTSAckArrival << ","
       << parTSInsideAck  << "," 
       << parMimRTTAck << "\n";
         //<< " port " << InetSocketAddress::ConvertFrom (srcAddress).GetPort ();
  
  file.close();
  //return true;


}




