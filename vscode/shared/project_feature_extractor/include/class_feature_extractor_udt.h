#pragma once
#include "class_feature_extractor.h"

class class_feature_extractor_udt: public class_feature_extractor {

protected:
   
public:
    class_feature_extractor_udt(){};//Lembrando que os constructors são da classe Base (Pai)
               //é autmaticamnete chamda quando se instancia a classe filha
               //se for herança múltipla, os constructors são chamados na ordem
               //de leituraem que foram declaradas as heranças. ver 
               //https://www.geeksforgeeks.org/multiple-inheritance-in-c/
               // 
    ~class_feature_extractor_udt(){}; //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    
    virtual void meth_extract_router_features();//scan dump file
    virtual string meth_search_seq_number(string par_dump_line);
    virtual string  meth_search_time_stamp(string par_dump_line);

    
};