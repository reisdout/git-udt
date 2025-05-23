#ifndef CLASS_MRS_UTILS
#define CLASS_MRS_UTILS

#include <iomanip>
#include <cstring> 
#include <typeinfo>
#include <iostream>

using namespace std;


#define APP_RELEASE_PROFILE 0

#define APP_DEBUG_PROFILE 1

class class_mrs_debug{

public:


    inline static int print_counter = 0;

    inline static int par_print_from = 0; //200000000; // // //1836 para Treino_TcpVegas_60Fluxos_100Mbps_Tue_Mar_11_12_15_58

    template <typename T>
    static void print(string par_string, T par_var, bool par_force_print=false) {

        //int profile = APP_DEBUG_PROFILE;
        int profile = APP_RELEASE_PROFILE;

        //if(profile ==  APP_RELEASE_PROFILE)
            //return;

        if(profile == APP_DEBUG_PROFILE || par_force_print)
        {
            print_counter++;
            float type_float;
            double type_double;
            long double type_long_double;
        

            const type_info& t_float = typeid(type_float);
            const type_info& t_double = typeid(type_double);  
            const type_info& t_long_double = typeid(type_long_double);
            const type_info& t_my_type = typeid(par_var);
        
    

            //cout << "entering print " << endl;
            //char c;
            cout << print_counter<<": ";
            if(t_my_type == t_float || t_my_type == t_double)
                cout << par_string << fixed << setprecision(5) << par_var << endl;
            else if (t_my_type == t_long_double)
                cout << par_string << fixed << setprecision(10) << par_var << endl;
            else
                cout << par_string << par_var << endl;
            
            if(print_counter >= par_print_from)
                cin.ignore();
                //cin >> c;
        }
    }

   
    static int get_print_counter(){return print_counter;};

    static int get_port_range(int par_port)
    {
        int mil = par_port/100;
        return mil*100;
    
    }
    



};


#endif