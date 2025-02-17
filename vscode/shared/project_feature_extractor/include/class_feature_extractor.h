#pragma once

#define SEGMENT_SIZE 1500

class class_feature_extractor {

protected:
    unsigned segmentsReceived=0;
public:
    class_feature_extractor(){};//Lembrando que os constructors são da classe Base (Pai)
               //é autmaticamnete chamda quando se instancia a classe filha
               //se for herança múltipla, os constructors são chamados na ordem
               //de leituraem que foram declaradas as heranças. ver 
               //https://www.geeksforgeeks.org/multiple-inheritance-in-c/
               // 
    ~class_feature_extractor(){}; //precisa ser virtual para ser chamado quando ser deleta 
                        //a classe base ver https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    // pure virtual functions
    void meth_extract_client_features(char* par_path_client_file);
    void meth_extract_router_features(char* par_path_router_file);
 
    private:


    
};