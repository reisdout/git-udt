/*
Esta classe é equivalente ao que esta no kernel,
a TCP-Cubic, reno, vegas,etc
*/

#include <udt.h>
#include <ccc.h> //para procurar local
#include <iostream>
#include <unistd.h>
#include <chrono>
#include "../../project_feature_saver/include/class_feature_saver_udt.h"
#include "../../defines/defines.h"


extern int server_port;
extern uint32_t  last_ack;
extern string data_rate;
extern string num_flows;
extern string tipo_dado;
extern string simu_start_time;
extern string terminal_type;
extern int send_type;

class class_pure_udt: public CCC
{
public:

   void Signature()
   {
      std::cout << "class_pure_udt" << std::endl;
   }

   void init()
   {

        //m_dCWndSize = 10.0; 
        m_dPktSndPeriod = (30*m_iMSS * 8.0) / 100;//Vai atingir 100MB quando a metade dos fluxos subirem
        m_dCWndSize = 83333.0;
      
      save_start = high_resolution_clock::now();
      if(terminal_type == "udt_client")
      {
         obj_saver.set_port(server_port);
         obj_saver.set_bottleneck_datarate(data_rate);
         obj_saver.set_default_congestion("udt");
         obj_saver.set_num_flows(num_flows);
         obj_saver.set_tipo_dado(tipo_dado);
         obj_saver.set_starting_time(simu_start_time);
         obj_saver.meth_adjust_file_path();
      }
   }
   /*Não estava sendo acionado esse callback pelo fato de a assinatura do método
   estar com parametro de entrada diferente no CCC é "int32_t"*/

   void onACK(int32_t ack)
   {
      auto ack_arrival = high_resolution_clock::now();

      save_time = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(save_time - save_start);
      if(duration.count() <= SIMUL_TIME_UDT )
         obj_saver.meth_deal_ack_arrival(ack,ack_arrival);
      else
         cout << "Save stoped. See duration: " <<duration.count()<< endl;
   }

   virtual void onPktSent(const CPacket*pkt)
   {
      std::cout << "Packat m_iSeqNo: "<< (int32_t) pkt->m_iSeqNo <<" sent" << std::endl;
      //time stamp é do início da conexão e é em microsec (proc-092.pdf)
      std::cout << "Packat TimeStamp: "<< pkt->m_iTimeStamp << " sent" << std::endl;
      auto snd_time = high_resolution_clock::now();
      save_time = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(save_time - save_start);
      
      if(duration.count() < SIMUL_TIME_UDT)
         obj_saver.meth_deal_packet_send(pkt->m_iSeqNo,snd_time);

      else
         cout << "Save stoped. See duration: " <<duration.count()<< endl;

      
      std::cout << "m_dCWndSize: "<< m_dCWndSize << std::endl;

      //if(!ack_lock)
         //ack_lock = pkt->m_iSeqNo;

   }

   virtual void onPktReceived(const CPacket*pkt)
   {
      //std::cout << "Pacote getAckSeqNo: "<< (unsigned)pkt->getAckSeqNo() <<" Recebido" << std::endl;
      std::cout << "Pacote m_iSeqNo: "<< pkt->m_iSeqNo <<" Recebido" << std::endl;
      
     
   
   }



protected:

   class_feature_saver_udt obj_saver;
   high_resolution_clock::time_point save_start;
   high_resolution_clock::time_point save_time = high_resolution_clock::now();

   
};

