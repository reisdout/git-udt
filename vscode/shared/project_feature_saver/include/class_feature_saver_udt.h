#pragma once
#include "class_feature_saver.h"

extern map<uint32_t, high_resolution_clock::time_point> map_seq_timestamp;


class class_feature_saver_udt: public class_feature_saver{

    public:
        void meth_amortize_map(int32_t par_seq);
        virtual void meth_deal_ack_arrival(uint32_t par_seq, 
            high_resolution_clock::time_point par_ack_arrival_time);

        virtual void meth_calculate_ack_ewma(uint32_t par_seq, 
            high_resolution_clock::time_point par_ack_arrival_time);

        virtual void meth_calculate_rtt(uint32_t par_seq, 
                high_resolution_clock::time_point par_ack_arrival_time);

        virtual void meth_calculate_send_ewma(uint32_t par_seq);
        virtual void meth_deal_packet_send(uint32_t par_seq, 
            high_resolution_clock::time_point par_send_time);


    protected:

        high_resolution_clock::time_point  marcaTempoChegadaAckAnterior;
        high_resolution_clock::time_point  marcaTempoChegadaAck;

        high_resolution_clock::time_point  marcaTempoAnterior;
        high_resolution_clock::time_point  marcaTempoAtual;

};