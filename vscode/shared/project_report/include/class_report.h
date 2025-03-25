#pragma once
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>

using namespace std;

class class_report{
    public:
        class_report(string par_report_dir, string par_report_file_name);
        ~class_report(){};        
        void meth_add_report_field_values(string par_field, string par_value);
        bool meth_already_reported();
        void meth_report();



    private:
        string arr_fields[50];
        string arr_fields_values[50];
        string report_dir;
        string report_file_name;
        int num_fields=0;
};