#include <cilk/cilk.h>
#include <ios>
// #include "opencilk_reducer.hpp"
#include <cilk/reducer_max.h>
#include <cilk/cilk_api.h>

// #include <benchmark/benchmark.h>

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <exception>
#include <exception>

// #define n 160'000'000

int test_count = 100;

unsigned int seed = std::random_device{}();
std::mt19937 gen(seed);

int measure_std_max(std::vector<int> const& vec)
{

    // int max = vec[0];
    // // Compute the max of integers 1..n
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     if(vec[i] > max)
    //     {
    //         max = vec[i];
    //     }
    // }
    // return max;

    auto max = std::max_element(vec.begin(), vec.end());
    return *max;

}

int measure_cilk_max(std::vector<int> const& vec)
{

    // __cilkrts_set_param("nworkers","2");

    cilk::reducer<cilk::op_max<int> > rm;
    cilk_for (int i = 0; i < vec.size(); ++i)
    {
        rm->calc_max(vec[i]);    // *rm = cilk::max_of(*max, vec[i])
    }
    
    return rm.get_value();

    // // TESTING. DON'T ENABLE IF BENCHMARKING RUNTIME
    // if(measure_seq_max(vec) !=  rm.get_value())
    // {
    //     std::cout << "Error!" << rm.get_value() << "!=" << measure_seq_max(vec) << std::endl;
    //     throw std::exception();
    // }


}

double averageout_cilk_max(std::vector<int> const& vec)
{
    
    int res;
    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        res = measure_cilk_max(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    // std::cout << "Cilk max: " << res << std::endl;;

    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

double averageout_std_max(std::vector<int> const& vec)
{
    int res;

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        res = measure_std_max(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    // Use that for validity check
    std::cout << "STD MAX: " << res << std::endl;

    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

int  main(int argc, char* argv[])
{
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
    // f.open("max_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::fill(
        std::begin(vec), std::end(vec), gen() % 1000);

    auto time = averageout_cilk_max(vec);
    // // f << numWorkers << ", " << time << ',';
    std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    
    time = averageout_std_max(vec);
    // f << time << std::endl;
    std::cout << "[STD]: " << n <<  ", " << time << std::endl;


    // f.close();

    return 0;

}   

