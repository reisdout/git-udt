/*
Esta classe é equivalente ao que esta no kernel,
a TCP-Cubic, reno, vegas,etc
*/

#include <udt.h>
#include <ccc.h> //para procurar local
#include <iostream>
#include <unistd.h>
#include "defines.h"

extern bool send_lock;
extern uint32_t  last_ack;
extern int send_type;
int dif_unlock = 10;

class VegasMLP13: public CCC
{
public:

   void Signature()
   {
      std::cout << "VegasMLP13" << std::endl;
   }

   void init()
   {
      m_bSlowStart = true;
      m_issthresh = 83333;

      m_dPktSndPeriod = 0.0; //1.0;//sera que evita travamento?
      m_dCWndSize = 2.0;

      setACKInterval(5);
      setRTO(5000000);
   }
   /*Não estava sendo acionado esse callback pelo fato de a assinatura do método
   estar com parametro de entrada diferente no CCC é "int32_t"*/
   void onACK(int32_t ack)
   {
      last_ack = ack;
      std::cout << "m_dCWndSize: "<< m_dCWndSize << std::endl;
      if(send_type == PCC)
      {
         if(!ack_lock)
               ack_lock = ack;
         /*
            O Próximo if visa garantir que so havera novas transmissoes
            se houver escoamento minimo de dados, percervido pela diferenca,
            que indica novos ack. Caso contrario o looping infinito no main.cpp
            vai estourar qualquer buffer.
         */      
         
         if((ack - ack_lock) > dif_unlock )
         {
            
            std::cout << "(ack, ack_lock, ack_dif, diff_unlock) " <<"("<<  
            ack << ", "
            << ack_lock<<", "
            << ack - ack_lock<<", "
            << dif_unlock
            <<")" << std::endl;
            send_lock = false;
            ack_lock = ack;
         }
         
         //send_lock = false;
      }
      char c;
      std::cout << "Acionou onACK!! " <<  ack << std::endl;
      //std::cin >> c;
      if (ack == m_iLastACK)
      {
         if (3 == ++ m_iDupACKCount)
            DupACKAction();
         else if (m_iDupACKCount > 3)
            m_dCWndSize += 1.0;
         else
            ACKAction();
      }
      else
      {
         if (m_iDupACKCount >= 3)
            m_dCWndSize = m_issthresh;

         m_iLastACK = ack;
         m_iDupACKCount = 1;

         ACKAction();
      }
   }

   /*virtual void onPktSent(const CPacket*pkt)
   {
      //std::cout << "Pacote: "<< pkt->getAckSeqNo() <<" Enviado" << std::endl;
      //std::cout << "Pacote: "<< pkt->m_iSeqNo <<" Enviado" << std::endl;
      
      //std::cout << "m_dCWndSize: "<< m_dCWndSize << std::endl;

      //if(!ack_lock)
         //ack_lock = pkt->m_iSeqNo;

   }*/

   virtual void onPktReceived(const CPacket*pkt)
   {
      //std::cout << "Pacote getAckSeqNo: "<< (unsigned)pkt->getAckSeqNo() <<" Recebido" << std::endl;
      std::cout << "Pacote m_iSeqNo: "<< pkt->m_iSeqNo <<" Recebido" << std::endl;
     
   
   }

   virtual void onTimeout()
   {
      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_bSlowStart = true;
      std::cout << "timeout" << "\n";
   }

protected:
   virtual void ACKAction()
   {
      
      if (m_bSlowStart)
      {
         m_dCWndSize += 1.0;

         if (m_dCWndSize >= m_issthresh)
            m_bSlowStart = false;
      }
      else
         m_dCWndSize += 1.0/m_dCWndSize;

      //if(dif_unlock <5)
         //dif_unlock++;
      //else
         //dif_unlock = 5;

      //if(m_dCWndSize > 3000)
         //m_dCWndSize = 200;
   }

   virtual void DupACKAction()
   {
      m_bSlowStart = false;

      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_dCWndSize = m_issthresh + 3;
      
   }

protected:
   int m_issthresh;
   bool m_bSlowStart;

   int m_iDupACKCount;
   int32_t m_iLastACK;
   int32_t ack_lock = 0;
   
};


class CUDPBlast: public CCC
{
public:
   CUDPBlast()
   {
      m_dPktSndPeriod = 1000000; 
      m_dCWndSize = 83333.0; 
   }

public:

   void Signature()
   {
      std::cout << "CUDPBlast" << std::endl;
   }
   void setRate(double mbps)
   {
      m_dPktSndPeriod = (m_iMSS * 8.0) / mbps;
   }
};