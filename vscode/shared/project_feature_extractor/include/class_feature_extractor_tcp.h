#pragma once
#include "class_feature_extractor.h"



template <class T>
void class_feature_extractor_tcp_print(string par_string, T par_var) {
    
    bool print = true;

    cout << "entering print " << endl;

    if(print)
    {
        char c;
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
    ~class_feature_extractor_tcp(){}; //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    
    virtual void meth_extract_router_features();//scan dump file
    virtual string meth_search_seq_number(string par_dump_line);
    virtual string  meth_search_time_stamp(string par_dump_line);
    void meth_extract_virtual_clock_origin(string par_syn_line);
   
    

    protected:

        u_int64_t virtual_clock_origin=0;
        bool first_packet = false;
    
};