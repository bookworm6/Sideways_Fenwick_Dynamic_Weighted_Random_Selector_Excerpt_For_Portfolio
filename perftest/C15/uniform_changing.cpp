//HOW TO USE COMPILATION FLAGS:
//Enter the library you want to test after 'DWRSLIB=' and the number of weights after 'DWEIGHTNUM='.

//Ex:
//g++ -I../lib -O3 "-DWRSLIB=nonuniform_int_distribution<int>" "-DWEIGHTNUM=10000000" -o test0 normal_changing.cpp
//g++ -I../lib -O3 "-DWRSLIB=heap_random_selector<int>" "-DWEIGHTNUM=100000" -o test1 normal_changing.cpp

#include "../lib/sideways_fenwick_selector.hpp"
#include "../lib/dynamic_proposal_array.cpp"
#include "../lib/bucket_alias.hpp"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>


int main() {
  std::uniform_real_distribution<double> d(1,10); 
  std::uniform_int_distribution<int> randomIndex(0, WEIGHTNUM - 1);

  std::default_random_engine generator; 
  std::vector<double> weights = {};
  
  for(int i = 0; i < WEIGHTNUM; i++){
    weights.push_back(std::max<double>(0.00001,d(generator)));
  }	      

      

  //start time
  WRSLIB selector(weights.begin(), weights.end());

  auto start = std::chrono::high_resolution_clock::now();  
  for (int i = 0; i < 1000000; i++) {
    int index = selector(generator);
    selector.update_weight(index, std::max<double>(0.00001,d(generator)));
    for(int j = 0; j < 14; j++){
      selector.update_weight(randomIndex(generator), std::max<double>(0.00001,d(generator)));    
    }
  }
  
  // end time
  auto end = std::chrono::high_resolution_clock::now();  
  std::chrono::duration<double> elapsedtime_sec = end - start;
  std::cout << elapsedtime_sec.count() << std::endl;
  
}
