#pragma once
#include "class_feature_extractor.h"
#include <iomanip>
#include <typeinfo>



template <class T>
void class_feature_extractor_tcp_client_print(string par_string, T par_var) {

    
    float type_float;
    double type_double;
    long double type_long_double;
    

    const type_info& t_float = typeid(type_float);
    const type_info& t_double = typeid(type_double);  
    const type_info& t_long_double = typeid(type_long_double);
    const type_info& t_my_type = typeid(par_var);
    
    
    bool print = true;
    if(print)
    {

        //cout << "entering print " << endl;
        char c;
        if(t_my_type == t_float || t_my_type == t_double)
            cout << par_string << fixed << setprecision(5) << par_var << endl;
        else if (t_my_type == t_long_double)
            cout << par_string << fixed << setprecision(10) << par_var << endl;
        else
            cout << par_string << par_var << endl;
        cin >> c;
    }
}



class class_feature_extractor_tcp_client: public class_feature_extractor {


       
    public:

        class_feature_extractor_tcp_client(){}
        ~class_feature_extractor_tcp_client(){}

        virtual bool meth_check_if_parse_dump_file_is_possible();
        bool meth_extract_client_feature(string par_dump_line, 
                                         uint64_t& par_seq,
                                         uint64_t& par_ack_arrival,
                                         uint64_t& par_eco_reply);

        virtual string meth_search_seq_number(string par_dump_line);
        virtual string  meth_search_time_stamp(string par_dump_line);//time stamp da linha
        string meth_search_eco_reply_time_stamp(string par_dump_line);//time stamp do eco reply. ecr na linha    
        //virtual void set_dump_file(string par_dump_file){dump_file = out_dir+ "/" +"clients_data"+ "/" + par_dump_file;};
        virtual void set_SYN_file(string par_SYN_file){SYN_file = par_SYN_file;};
        bool meth_give_time_stamp_to_synchronize_clock(u_int64_t & par_time_stamp,
                                                       u_int64_t &  par_TS_value);
        void meth_update_port_state(string par_dump_line);

        uint32_t get_current_port(){return current_port;};
        bool get_port_changed(){return port_change;};


    protected:
        std::string SYN_file;
        uint32_t current_port = 0;
        bool port_change = false;
        uint64_t extractor_tcp_clinet_work_line=0;
        bool force_print_tcp_extractor_client =false;


};