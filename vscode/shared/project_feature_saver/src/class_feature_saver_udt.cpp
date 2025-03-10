#include "../include/class_feature_saver_udt.h"
#include "../include/class_feature_saver.h"



void class_feature_saver_udt::meth_deal_ack_arrival(uint32_t par_seq, 
    high_resolution_clock::time_point par_ack_arrival_time)
{

    //cout << "======Receiving seq "<< par_seq <<"======"<<endl;

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
    par_ack_arrival_time.time_since_epoch().count(), 
    marcaTempoAtual.time_since_epoch().count(),//ts->GetEcho(),
    0,
    0,
    0,
    "L",
    port,
    port,
    false);


    first_ack_process = true;

}



void class_feature_saver_udt::meth_amortize_map(int32_t par_seq)
{
    
    if(map_seq_timestamp.empty())
    {
        cout << "não amortizou.... " << par_seq << endl;
        return;
    }    

    
    
    cout << "Entering amortization. The atual map elements are : \n"; 
    for (auto it1 = map_seq_timestamp.begin(); it1 != map_seq_timestamp.end(); ++it1) 
    cout << it1->first << "->" << endl; 
    map <uint32_t, high_resolution_clock::time_point>::iterator it;

    

    //cout << "amortiz.... " << par_seq << endl;

    
    bool done = false;
    uint32_t erased;
    while (!done && !map_seq_timestamp.empty())
    {
        //cout << "Há " <<  map_seq_timestamp.count(par_seq) << " com seq == " <<par_seq<<endl;
        done=true;
        
        //este for é devido a desestabilização do iterador
        //quando algum elemento do map é apagado
        //por isso, quando apaga um há um continie, para se renovar o
        //iterador
        for(auto it = map_seq_timestamp.begin(),next_it = it; it != map_seq_timestamp.end(); it=next_it)
        {
        
            ++next_it;
            if(it->first < par_seq )
            {
                
                //cout << "amortiz  " << it->first << endl;
                erased = map_seq_timestamp.erase(it->first);
                //cout << "erased:  " << erased << endl;
                done=false;
                continue;
                        
            }
            else
            {
                done=true;

            }
                
        }
    }

        //cout << "The atual map elements are : \n"; 
        //for (auto it1 = map_seq_timestamp.begin(); it1 != map_seq_timestamp.end(); ++it1) 
        //cout << it1->first << "->" << endl; 
}


void class_feature_saver_udt::meth_calculate_ack_ewma(uint32_t par_seq, 
    high_resolution_clock::time_point par_ack_arrival_time)
{

numAckReceived=(numAckReceived+1);
marcaTempoChegadaAck = par_ack_arrival_time;

if(first_ack_process)
{
auto delta_t = duration_cast<microseconds>(marcaTempoChegadaAck - marcaTempoChegadaAckAnterior);
float intervalFromPreviousAck = (float) delta_t.count();
ack_ewma = (float)(((1.0-expWeightExpon )*ack_ewma) + (expWeightExpon *intervalFromPreviousAck));
//cout << "ack_ewma: " << ack_ewma<< "; "<<"Dt: " <<intervalFromPreviousAck <<  endl;

}


marcaTempoChegadaAckAnterior = marcaTempoChegadaAck;



}

void class_feature_saver_udt::meth_calculate_send_ewma(uint32_t par_seq)
{

    //seq-1, pois o ack é a espera do próximio byte ou segmento
    if(map_seq_timestamp.find(par_seq-1) != map_seq_timestamp.end())
    {
        //cout << "found seq: " << par_seq-1 << endl;
        marcaTempoAtual = map_seq_timestamp.at(par_seq-1);
    }
    else
    {
        //cout << "NOT found seq: " << par_seq-1 << endl;
        return;
    }
    if(first_ack_process)
    {
        auto delta_t = duration_cast<microseconds>(marcaTempoAtual - marcaTempoAnterior);
        float intervalbetweenTS = (float) delta_t.count();
        send_ewma = (float)((1-expWeightExpon )*send_ewma + (expWeightExpon )*intervalbetweenTS);
        //cout << "send_ewma: " << send_ewma<< "; "<<"Dt: " << intervalbetweenTS <<  endl;

    }

 
    marcaTempoAnterior = marcaTempoAtual;
    
}

void class_feature_saver_udt::meth_calculate_rtt(uint32_t par_seq, 
    high_resolution_clock::time_point par_ack_arrival_time)
{

auto pre_rtt = duration_cast<microseconds>(par_ack_arrival_time - map_seq_timestamp.at(par_seq-1));
rtt = (double)pre_rtt.count();
//cout << "rtt: " << rtt << "; Marca ACK: " << par_ack_arrival_time.time_since_epoch().count()<<"; Marca Send: "<< map_seq_timestamp.at(par_seq-1).time_since_epoch().count() << endl;

}


void class_feature_saver_udt::meth_deal_packet_send(uint32_t par_seq, 
    high_resolution_clock::time_point par_send_time)
{

    map_seq_timestamp.insert({par_seq, par_send_time});

}



