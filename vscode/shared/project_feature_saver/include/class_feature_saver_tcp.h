#pragma once
#include "class_feature_saver.h"


class class_feature_saver_tcp: public class_feature_saver{

    public:
        virtual void meth_deal_ack_arrival(uint32_t par_seq, 
                                           uint64_t par_ack_arrival_time,
                                           uint64_t par_packet_eco_reply);
    
    

        virtual void meth_calculate_ack_ewma(uint64_t par_ack_arrival_time);

        virtual void meth_calculate_rtt(uint64_t  par_ack_arrival_time, uint64_t par_packet_eco_reply);
        virtual void meth_calculate_send_ewma(uint64_t  par_eco_reply);
        void meth_set_virtual_clock_origin(uint64_t par_1970_ts, uint64_t par_virtual_clock_ts);


    protected:
        uint64_t marcaTempoChegadaAckAnterior;
        uint64_t marcaTempoAnterior;
        u_int64_t virtual_clock_origin=0;

        bool feature_saver_tcp_force_print = false;
    };