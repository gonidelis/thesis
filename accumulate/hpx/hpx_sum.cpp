#include <hpx/hpx_main.hpp>
#include <hpx/numeric.hpp>
#include <hpx/execution.hpp>
// #include <hpx/runtime.hpp>

#include <vector>
#include <random>
#include <chrono>


int test_count = 100;

unsigned int seed = std::random_device{}();
std::mt19937 gen(seed);

int measure_hpx_sum(std::vector<int> const& vec)
{

    auto sum = hpx::reduce(hpx::execution::par, vec.begin(), vec.end());

    return sum;

}

double averageout_hpx_sum(std::vector<int> const& vec)
{
    int res;

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; ++i)
    {
        res = measure_hpx_sum(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();

    // Use that for validity check
    // std::cout << "HPX SUM: " << res << std::endl;

    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}


int main(int argc, char* argv[])
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

    std::vector<int> vec(n);
    std::fill(
        std::begin(vec), std::end(vec), gen() % 1000);

    auto time = averageout_hpx_sum(vec);

    std::cout << "[HPX]: " << n << ", " << hpx::get_num_worker_threads() << ", " << time << std::endl;

    return 0;
}
