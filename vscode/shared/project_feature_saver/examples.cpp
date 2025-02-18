/*
// high_resolution_clock example
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

int main ()
{
  using namespace std::chrono;

  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  std::cout << "printing out 1000 stars...\n";
  for (int i=0; i<1000; ++i) std::cout << "*";
  std::cout << std::endl;

  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  std::cout << "It took me " << time_span.count() << " seconds.";
  std::cout << std::endl;

  return 0;
}

*/

/*


// unordered_map::erase
#include <iostream>
#include <string>
#include <unordered_map>

int main ()
{
  std::unordered_map<std::string,std::string> mymap;

  // populating container:
  mymap["U.S."] = "Washington";
  mymap["U.K."] = "London";
  mymap["France"] = "Paris";
  mymap["Russia"] = "Moscow";
  mymap["China"] = "Beijing";
  mymap["Germany"] = "Berlin";
  mymap["Japan"] = "Tokyo";

  // erase examples:
  mymap.erase ( mymap.begin() );      // erasing by iterator
  mymap.erase ("France");             // erasing by key
  mymap.erase ( mymap.find("China"), mymap.end() ); // erasing by range

  // show content:
  for ( auto& x: mymap )
    std::cout << x.first << ": " << x.second << std::endl;

  return 0;
}


*/