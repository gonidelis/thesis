#include <hpx/hpx_main.hpp>
#include <hpx/modules/algorithms.hpp>
// #include <hpx/execution.hpp>
// #include <hpx/runtime.hpp>

#include <vector>
#include <random>
#include <chrono>


int test_count = 100;


void measure_hpx_copy(std::vector<int> const& vec, std::vector<int> &res)
{

    hpx::copy(hpx::execution::par, vec.begin(), vec.end(), res.begin());

}

double averageout_hpx_copy(std::vector<int> const& vec, std::vector<int> &res)
{
 
    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; ++i)
    {
        measure_hpx_copy(vec, res);
    }

    auto end = std::chrono::high_resolution_clock::now();

 
    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}


int main(int argc, char* argv[])
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

    std::vector<int> vec(n);
    std::vector<int> res(n);

    // Fill vec with random numbers
    for( int i = 0; i < vec.size(); ++i )
    {
        vec[i] = std::rand() % 1000 + 1;
    }

    auto time = averageout_hpx_copy(vec, res);

    std::cout << "[HPX]: " << n << ", " << hpx::get_num_worker_threads() << ", " << time << std::endl;

    // VALIDITY CHECK
    // for( int i = 100; i < 200; ++i )
    // {
    //     std::cout << vec[i] << " " << res[i] << std::endl; 
    // }

    return 0;
}
