#include "../include/class_report.h"


class_report::class_report (string par_report_dir, string par_report_file_name)
{

    report_dir = par_report_dir;
    report_file_name = par_report_file_name;

}

bool class_report::meth_already_reported()
{
    string filepath = report_dir +"/" + report_file_name;
    ifstream f(filepath.c_str());
    return f.good();

}


void class_report::meth_add_report_field_values(string par_field, string par_value)
{

    arr_fields[num_fields] = par_field;
    arr_fields_values [num_fields] = par_value;
    num_fields++;

}

void class_report::meth_report()
{
    std::ofstream file;

    string filepath = report_dir +"/" + report_file_name;
    
    ifstream f(filepath.c_str());
    if (f.good())
    {
        cout << "already reported." << endl;
        return;
    }



    file.open(filepath, std::ios::out | std::ios::app);
    if (file.fail())
    {
        std::cout << "Report File Error";
        exit(0);
    }
  
    for (int i =0; i < num_fields; i++ )
    {
        file << arr_fields[i] << ": "
             << arr_fields_values[i] << endl;
    }
  
     file.close();
 
}