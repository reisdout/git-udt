#include "../include/class_feature_saver_tcp.h"
#include "../include/class_feature_saver.h"
#include "mrs_utils.h"


uint64_t class_feature_saver_tcp::virtual_clock_origin = 0;


void class_feature_saver_tcp::meth_set_virtual_clock_origin(uint64_t par_1970_ts, uint64_t par_virtual_clock_ts)
{

/*

Observe a tabela abaixo. O cálculo do RTT é feito considerando o primeiro pacote
SYN. No caso considerado, a primeira coluna traz a marca de tempo desde 01/01/1970
(1741093101433) e a marca de temp TS (1773487458), que é a obtida no relógio virtual.
Podemos considerar que essas marcas de tempo caem em pontos iguais no eixo do tempo, mas 
têm valor distintos por estarem sendo contadas de origens diferentes. Ou seja, o 
"eixo dos tempos virtuais" foi transladado. Daí para se obter a origem do tempo virtual
em relação à 01/01/70, basta subtrair  1741093101433 de 1773487458 e encontraremos
o ponto Ov(Origem virtual), a partir do qual 1741093101433 tem valor 1773487458. Depois
sempre que quisermos achar o valor do virtual em relação a 01/01/1970, basta somarmos
Ov. 
Esse processo foi feito com os acks de um experimento, cujos valores estão
tabulados abaixo. Veja que o valor do RTT é exatamente o esperado para um
delay de 40s, adotado no enlace do experimento.



TS SYN 1       Origem Virtual Clock(Ov)	    ack_arr	   ecr	       RTT
1741093101433	   1739319613975	1741093101534	1773487517	42
1773487458	   1739319613975	1741093101534	1773487517	42
	           1739319613975	1741093101534	1773487517	42
	           1739319613975	1741093101534	1773487517	42
	           1739319613975	1741093103472	1773489451	46


*/

    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    class_mrs_debug::print<uint64_t>("par_1970_ts: ", par_1970_ts, feature_saver_tcp_force_print); 
    class_mrs_debug::print<uint64_t>("par_virtual_clock_ts: ", par_virtual_clock_ts, feature_saver_tcp_force_print ); 


    //tem que dividir por 1000 o desde de 1970, pois o dump traz em microssegundos e o virtual
    //é milissegundos.
    
    virtual_clock_origin = (par_1970_ts/1000) - par_virtual_clock_ts;

    class_mrs_debug::print<uint64_t>("virtual_clock_origin: ", virtual_clock_origin, feature_saver_tcp_force_print ); 

}



void class_feature_saver_tcp::meth_deal_ack_arrival(uint32_t par_seq, 
                                                     uint64_t par_ack_arrival_time,
                                                     uint64_t par_packet_eco_reply)
{

        class_mrs_debug::print<char>("Entering meth_deal_ack_arrival: ", '\n',feature_saver_tcp_force_print);


        meth_calculate_ack_ewma(par_ack_arrival_time);
        meth_calculate_send_ewma(par_packet_eco_reply);
        meth_calculate_rtt(par_ack_arrival_time,par_packet_eco_reply);

        meth_save_training_data(par_seq,//par_seq-1, //numAckReceived[idAckReceiver][idAckSender], 
        ack_ewma, 
        send_ewma,
        rtt,
        0,
        0,
        0,
        0,
        0,
        par_ack_arrival_time, 
        par_packet_eco_reply,//ts->GetEcho(),
        0.0,
        0,
        0,
        "L",
        port,
        port,
        false);


        first_ack_process = true;




    return;
}


void class_feature_saver_tcp::meth_calculate_ack_ewma(uint64_t par_ack_arrival_time)
{

    numAckReceived=(numAckReceived+1);
    
    
    if(first_ack_process)    
    {

        long double ack_arrival_micro = par_ack_arrival_time/1000;
        long double marcaTempoChegadaAckAnterior_micro = marcaTempoChegadaAckAnterior/1000;
        long double delta_t = ack_arrival_micro - marcaTempoChegadaAckAnterior_micro;
        long double intervalFromPreviousAck = delta_t;
        ack_ewma = ((1.0-expWeightExpon )*ack_ewma) + (expWeightExpon *intervalFromPreviousAck);
        //cout << "ack_ewma: " << ack_ewma<< "; "<<"Dt: " <<intervalFromPreviousAck <<  endl;
        class_mrs_debug::print<long double>("ack_ewma: ", ack_ewma,feature_saver_tcp_force_print);
    }
    
    
    marcaTempoChegadaAckAnterior = par_ack_arrival_time;
    
    
    


}

void class_feature_saver_tcp::meth_calculate_rtt(uint64_t  par_ack_arrival_time, uint64_t par_packet_eco_reply)
{
    long double ack_arrival_mili_sec = static_cast<long double>(par_ack_arrival_time/1000.0);

    class_mrs_debug::print<long double>("ack_arrival_mili_sec: ", ack_arrival_mili_sec,feature_saver_tcp_force_print);

    class_mrs_debug::print<long double>("virtual_clock_origin: ", virtual_clock_origin ,feature_saver_tcp_force_print );

    class_mrs_debug::print<long double>("par_packet_eco_reply: ", par_packet_eco_reply ,feature_saver_tcp_force_print );
    
    long double ecr_from_1970 =  static_cast<long double>(virtual_clock_origin + par_packet_eco_reply);

    class_mrs_debug::print<long double>("ecr_from_1970: ", ecr_from_1970, feature_saver_tcp_force_print);
    
    rtt = (ack_arrival_mili_sec - ecr_from_1970);  //(double)pre_rtt.count();
    rtt = rtt*1000; // microsseconds
    
    class_mrs_debug::print<float>("rtt in meth_calculate_rtt: ", rtt,feature_saver_tcp_force_print );
}
void class_feature_saver_tcp::meth_calculate_send_ewma(uint64_t  par_eco_reply)
{
    if(first_ack_process)
    {
        auto delta_t = par_eco_reply - marcaTempoAnterior; //duration_cast<microseconds>(marcaTempoAtual - marcaTempoAnterior);
        float intervalbetweenTS = (float) delta_t; //(float) delta_t.count();
        send_ewma = (float)((1.0-expWeightExpon )*send_ewma + (expWeightExpon*intervalbetweenTS));
        //cout << "send_ewma: " << send_ewma<< "; "<<"Dt: " << intervalbetweenTS <<  endl;
        class_mrs_debug::print<float>("send_ewma: ", send_ewma);

    }

 
    marcaTempoAnterior = par_eco_reply;

}

