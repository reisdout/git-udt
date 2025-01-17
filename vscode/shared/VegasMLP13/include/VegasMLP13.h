#include <udt.h>
#include <ccc.h> //para procurar local
#include <iostream>

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

      m_dPktSndPeriod = 0.0;
      m_dCWndSize = 2.0;

      setACKInterval(2);
      setRTO(1000000);
   }
   /*Não estava sendo acionado esse callback pelo fato de a assinatura do método
   estar com parametro de entrada diferente no CCC é "int32_t"*/
   void onACK(int32_t ack)
   {
      //char c;
      int32_t clean_ack = ack&0xFFFFFFFF;
      std::cout << "Acionou onACK!!" << (unsigned) clean_ack << std::endl;
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

   virtual void onPktSent(const CPacket*pkt)
   {
      std::cout << "Pacote: "<< (unsigned)pkt->getAckSeqNo() <<" Enviado" << std::endl;
      
      std::cout << "m_dCWndSize: "<< m_dCWndSize << std::endl;
   }

   virtual void onPktReceived(const CPacket*pkt)
   {
      std::cout << "Pacote "<< (unsigned)pkt->getAckSeqNo() <<" Recebido" << std::endl;
     
   
   }

   virtual void onTimeout()
   {
      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_bSlowStart = true;
      m_dCWndSize = 2.0;
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
   int m_iLastACK;
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