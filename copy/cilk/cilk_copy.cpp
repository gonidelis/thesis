#include <cilk/cilk.h>
// #include "opencilk_reducer.hpp"
#include <cilk/reducer_vector.h>
#include <cilk/cilk_api.h>

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <ctime>

// #define n 160'000'000

int test_count = 100;


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

    auto time = averageout_cilk_copy(vec, res);
    std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    // std::cout << numWorkers << ", " << time << ',' << std::endl;

    // VALIDITY CHECK
    // for( int i = 100; i < 200; ++i )
    // {
    //     std::cout << vec[i] << " " << res_seq[i] << std::endl; 
    // }

    // f.close();

    return 0;

}   



