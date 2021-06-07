#include <cilk/cilk.h>
#include <ios>
// #include "opencilk_reducer.hpp"
#include <cilk/reducer_vector.h>
#include <cilk/cilk_api.h>

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <execution>
#include <algorithm>
#include <ctime>

// #define n 160'000'000

int test_count = 100;

void measure_seq_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    // int sum = 0;
    // // Compute the sum of elements of `vec`
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     sum += vec[i];
    // }

    std::copy(std::execution::par, vec.begin(), vec.end(), res.begin());

}


void measure_cilk_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    cilk::reducer<cilk::op_vector<int> > r;
    cilk_for (int i = 0; i != vec.size(); ++i) 
    {
        r->push_back(vec[i]);
    }

    r.move_out(res);    // this damages performance
    // cores are not fully utilized (50%-70% each)
    // imposes important sequential step

}

double averageout_cilk_copy(std::vector<int> const& vec, std::vector<int> &res)
{
    
    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        measure_cilk_copy(vec, res);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

double averageout_seq_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        measure_seq_copy(vec, res);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

int  main(int argc, char* argv[])
{
    std::srand(std::time(nullptr)); // use current time as seed for random generator

    int n;
    if(argv[1] == NULL)
    {
        n = 1000000;
    }
    else
    {
        n = std::atoi(argv[1]);
    }

    int numWorkers = __cilkrts_get_nworkers();

    // std::ofstream f;
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::vector<int> res(n);

    // Fill vec with random numbers
    for( int i = 0; i < vec.size(); ++i )
    {
        vec[i] = std::rand() % 1000 + 1;
    }

    // auto time = averageout_cilk_copy(vec, res);
    // std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    // f << numWorkers << ", " << time << ',';

    std::vector<int> res_seq(n);
    auto time = averageout_seq_copy(vec, res_seq);
    std::cout << "[STD]: " << n << ", " << time << std::endl;
    // f << time << std::endl;
    

    // VALIDITY CHECK
    // for( int i = 100; i < 200; ++i )
    // {
    //     std::cout << vec[i] << " " << res_seq[i] << std::endl; 
    // }


    // f.close();

    return 0;

}   



