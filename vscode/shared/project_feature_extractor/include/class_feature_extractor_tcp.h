#pragma once
#include "class_feature_extractor.h"
#include <iomanip> 
#include <typeinfo>



template <class T>
void class_feature_extractor_tcp_print(string par_string, T par_var,bool par_pause=true) {
    
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



class class_feature_extractor_tcp: public class_feature_extractor {

protected:
   
public:
    class_feature_extractor_tcp(){};//Lembrando que os constructors são da classe Base (Pai)
               //é autmaticamnete chamda quando se instancia a classe filha
               //se for herança múltipla, os constructors são chamados na ordem
               //de leituraem que foram declaradas as heranças. ver 
               //https://www.geeksforgeeks.org/multiple-inheritance-in-c/
               // 
    ~class_feature_extractor_tcp(){stream_dump_file.close();}; //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    
    virtual void meth_extract_router_features();//scan dump file
    //retorna true se o seq foi efetivamente a um ewma no cvs.
    bool meth_extract_router_features(uint64_t par_ack_seq);//scan dump file
    virtual string meth_search_seq_number(string par_dump_line);
    virtual string  meth_search_time_stamp(string par_dump_line);

   
    

    protected:
       
        
        uint64_t last_ack_processed = 0;
        ifstream stream_dump_file;
        string seq_number;
        string seq_number_inf;
        string seq_number_sup;
        uint64_t seq_number_inf_in_decimal=0;
        uint64_t seq_number_sup_in_decimal=0;
        float queue_now;
    
    
};