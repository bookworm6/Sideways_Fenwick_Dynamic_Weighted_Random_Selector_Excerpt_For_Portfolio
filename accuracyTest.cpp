//Compile with g++ -std=c++20 -O3 accuracyTest.cpp
#include "sideways_fenwick_selector.hpp"
#include <iostream>
#include <fstream>
#include <random>


int main(){
    int numWeights = 10;
    int numOperations=10;
    double percentWeightMin=.8;
    double percentWeightMax=1.2;

    std::ofstream outputFile;
 

    std::default_random_engine generator(10);
    std::normal_distribution<double> distribution(5.0,2.0);
    std::vector<double> weights;

    outputFile.open("accuracyTestOutput.csv");
    //creating list to use as distribution. every element has a weight equal to its index
    for (int i=0;i<numWeights;i++){
        weights.push_back(i);
    }
    //constructing selector
    sideways_fenwick_selector<> selector(weights.begin(), weights.end());
    //performing 100000 selections
    for(int i=0;i<100000;i++){
        outputFile<<selector(generator)<<std::endl;
    }
    outputFile.close();

}