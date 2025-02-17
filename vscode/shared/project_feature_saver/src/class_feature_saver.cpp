#include "../include/class_feature_saver.h"
#include <ctime>
#include <algorithm>
#include <iostream>

using namespace std;


class_feature_saver::class_feature_saver(unsigned par_port)
{

    meth_start_simulation_time();
    int client_id = par_port - 9000;
    path_seq_metrics_file = std::string("./10_0_") + 
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("to") +
                            std::string("10_1_") +
                            std::to_string(client_id) +
                            std::string("_2") +
                            std::string("_L_") +
                            str_starting_time.substr(0,str_starting_time.length()-6) +
                            std::string(".csv");
                            std::replace(path_seq_metrics_file.begin(), path_seq_metrics_file.end(),':','_');

    std::cout << path_seq_metrics_file << std::endl;     
} 



void class_feature_saver::meth_start_simulation_time()
{
  
  //startingHour = simulationStart.GetHours();
  //startingminute = simulationStart.GetMinutes();
  time_t now = time(0);
  this->str_starting_time.append(ctime(&now));
 }