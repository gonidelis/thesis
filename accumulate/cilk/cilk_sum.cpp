#include <cilk/cilk.h>
#include <ios>
// #include "opencilk_reducer.hpp"
#include <cilk/reducer_opadd.h>
#include <cilk/cilk_api.h>

#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <execution>
#include <numeric>

// #define n 160'000'000

int test_count = 100;

unsigned int seed = std::random_device{}();
std::mt19937 gen(seed);


int measure_seq_sum(std::vector<int> const& vec)
{

    // int sum = 0;
    // // Compute the sum of elements of `vec`
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     sum += vec[i];
    // }

    auto sum = std::reduce(std::execution::par, vec.begin(), vec.end(), int(0));

    return sum;
}


int measure_cilk_sum(std::vector<int> const& vec)
{

    // __cilkrts_set_param("nworkers","2");

    cilk::reducer< cilk::op_add<int> > sum;
    cilk_for (int i = 0; i < vec.size(); ++i)
    {
        *sum += vec[i];
    }
    
    return sum.get_value();

    // // TESTING. DON'T ENABLE IF BENCHMARKING RUNTIME
    // if(measure_seq_sum(vec) !=  rm.get_value())
    // {
    //     std::cout << "Error!" << rm.get_value() << "!=" << measure_seq_max(vec) << std::endl;
    //     throw std::exception();
    // }

}

double averageout_cilk_sum(std::vector<int> const& vec)
{
    
    int res;
    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        res = measure_cilk_sum(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    // std::cout << "Cilk SUM: " << res << std::endl;;

    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

double averageout_seq_sum(std::vector<int> const& vec)
{
    int res;

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        res = measure_seq_sum(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    // Use that for validity check
    std::cout << "Real SUM: " << res << std::endl;

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
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::fill(
        std::begin(vec), std::end(vec), gen() % 1000);

    auto time = averageout_cilk_sum(vec);
    std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    // f << numWorkers << ", " << time << ',';
    
    time = averageout_seq_sum(vec);
    std::cout << "[STD]: " << n << ", " << time << std::endl;
    // f << time << std::endl;
    

    // f.close();

    return 0;

}   



