#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <random>
#include <execution>
#include <numeric>
#include <thread>

// #define n 160'000'000

int test_count = 100;

unsigned int seed = std::random_device{}();
std::mt19937 gen(seed);


int measure_std_sum(std::vector<int> const& vec)
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

double averageout_std_sum(std::vector<int> const& vec)
{
    int res;

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        res = measure_std_sum(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    // Use that for validity check
    std::cout << "STD SUM: " << res << std::endl;

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

    unsigned int workers = std::thread::hardware_concurrency();

    // std::ofstream f;
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::fill(
        std::begin(vec), std::end(vec), gen() % 1000);

    auto time = averageout_std_sum(vec);
    std::cout << "[STD]: " << n << ", " << workers << ", " << time << std::endl;
    // f << time << std::endl;
    

    // f.close();

    return 0;

}   



