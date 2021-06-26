#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <execution>
#include <algorithm>
#include <ctime>
#include <thread>

// #define n 160'000'000

int test_count = 100;

void measure_std_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    // int sum = 0;
    // // Compute the sum of elements of `vec`
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     sum += vec[i];
    // }

    std::copy(std::execution::par, vec.begin(), vec.end(), res.begin());

}

double averageout_std_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        measure_std_copy(vec, res);
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

    // std::ofstream f;
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::vector<int> res(n);

    // Fill vec with random numbers
    for( int i = 0; i < vec.size(); ++i )
    {
        vec[i] = std::rand() % 1000 + 1;
    }

    unsigned int workers = std::thread::hardware_concurrency();

    std::vector<int> res_seq(n);
    auto time = averageout_std_copy(vec, res_seq);
    std::cout << "[STD]: " << n << ", " << workers << ", " << time << std::endl;
    // f << time << std::endl;
    
    // VALIDITY CHECK
    // for( int i = 100; i < 200; ++i )
    // {
    //     std::cout << vec[i] << " " << res_seq[i] << std::endl; 
    // }


    // f.close();

    return 0;

}   



