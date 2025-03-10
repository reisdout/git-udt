#ifndef CLASS_MRS_UTILS
#define CLASS_MRS_UTILS

#include <iomanip>
#include <cstring> 
#include <typeinfo>
#include <iostream>

using namespace std;


class class_mrs_debug{

public:


    inline static int print_counter = 0;

    inline static int par_print_from = 0;

    template <typename T>
    static void print(string par_string, T par_var, bool par_print=true) {

        print_counter++;
        if(par_print)
        {

            float type_float;
            double type_double;
            long double type_long_double;
        

            const type_info& t_float = typeid(type_float);
            const type_info& t_double = typeid(type_double);  
            const type_info& t_long_double = typeid(type_long_double);
            const type_info& t_my_type = typeid(par_var);
        
    

            //cout << "entering print " << endl;
            char c;
            cout << print_counter<<": ";
            if(t_my_type == t_float || t_my_type == t_double)
                cout << par_string << fixed << setprecision(5) << par_var << endl;
            else if (t_my_type == t_long_double)
                cout << par_string << fixed << setprecision(10) << par_var << endl;
            else
                cout << par_string << par_var << endl;
            
            if(print_counter >= par_print_from)
                cin >> c;
        }
    }

   
    static int get_print_counter(){return print_counter;};
    



};


#endif