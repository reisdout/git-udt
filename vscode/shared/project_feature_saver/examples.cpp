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

/*
// C++ program to find out execution time of
// of functions
#include <algorithm>
#include <chrono>
#include <iostream>
#include<vector>
using namespace std;
using namespace std::chrono;

// For demonstration purpose, we will fill up
// a vector with random integers and then sort
// them using sort function. We fill record
// and print the time required by sort function
int main()
{

    vector<int> values(10000);

    // Generate Random values
    auto f = []() -> int { return rand() % 10000; };

    // Fill up the vector
    generate(values.begin(), values.end(), f);

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    // Call the function, here sort()
    sort(values.begin(), values.end());

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    // Get duration. Substart timepoints to 
    // get duration. To cast it to proper unit
    // use duration cast method
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}

*/

/*
// C++ Program to split a string by a delimiter
// using stringstream
#include <bits/stdc++.h>
using namespace std;

int main() {
    string str = "geeks,for,geeks";

    // Create a stringstream object to str
    stringstream ss(str);
	
  	// Temporary object to store the splitted
  	// string
    string t;
  
  	// Delimiter
    char del = ',';

   	// Splitting the str string by delimiter
    while (getline(ss, t, del))
        cout << "\"" << t << "\"" << " ";
    return 0;
}
*/