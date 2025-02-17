#ifndef CLASS_FEATURE_SAVER
#define CLASS_FEATURE_SAVER

#include <iostream>

class class_feature_saver{
    public:
        class_feature_saver(unsigned par_port);
        ~class_feature_saver(){};
        void meth_update_seq_metrics_file();
        void meth_start_simulation_time();



    protected:
        //static const int BUFFER_SIZE = 1024;
        //static const int PORT = 8080;

    private:

    std::string path_seq_metrics_file; 
    std::string str_starting_time;
};

#endif