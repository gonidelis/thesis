#include <cilk/cilk.h>
// #include <cilk/holder.h>
#include <cilk/reducer_opadd.h>
#include <cilk/cilk_api.h>

#include <stdlib.h>

#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <execution>

int test_count = 1;

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

int partition (std::vector<int> &arr, int low, int high)
{
    auto pivot = arr[high];

    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
 
    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (arr[j] < pivot)
        {
            i++; // increment index of smaller element
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

int par_partition (std::vector<int> &arr, int low, int high)
{
    int pivot = arr[high]; // pivot
    
    std::vector<int> lo_buf(arr.size(), -1);
    std::vector<int> hi_buf(arr.size(), -1);
    std::vector<int> pong(arr.size());

    auto nworkers =  __cilkrts_get_nworkers();
    auto chunk = (high - low) / nworkers;

    auto total_lo = 0;

    cilk_for(int i = 0 ; i < nworkers ; ++i)
    {
        auto wid = __cilkrts_get_worker_number();
        
        int lo = 0;
        int hi = 0;

        int tlow, thigh;
        if(wid != 0)
        {
            tlow = chunk * wid;
        }
        else
        {
            tlow = low;
        }

        if(wid != nworkers - 1)
        {
            thigh = chunk * (wid + 1);  
        }
        else
        {
            thigh = high;
        }

        std:: cout << tlow << " " << thigh << " " << chunk << std::endl;

        for(int i = tlow ; i <= thigh; ++i)
        {
            
            if(arr[i] <= pivot)
            {
                lo_buf[i] = lo++;
            }
            else
            {
                hi_buf[i] = hi++;
            }
        }

        for(int i = tlow ; i <= thigh ; ++i)
        {
            if(lo_buf[i] != -1)
            {
                pong[tlow + lo_buf[i]] = arr[i];
            }
            else if(hi_buf[i] != -1)
            {
                pong[tlow + lo  + hi_buf[i]] = arr[i];
            }
        }

        for(int i = tlow ; i <= thigh ; ++i)
        {
            arr[i] = pong[i];
        }

        total_lo += lo -1;

    }
    // // cilk::holder<std::array<int, 1000000>> hi_buf;

    return low + total_lo;

}

void quickSort_par(std::vector<int> &arr, int low, int high)
{

    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        auto pi = par_partition(arr, low, high);

        cilk_spawn quickSort_par(arr, low, pi - 1);
        // quickSort(arr, low, pi - 1);
        quickSort_par(arr, pi + 1, high);
        cilk_sync;
    }
} 

void quickSort(std::vector<int> &arr, int low, int high)
{
    
    if (low < high)
    {

        /* pi is partitioning index, arr[p] is now
        at right place */
        int pi = partition(arr, low, high);
 
        // cilk_spawn quickSort(arr, low, pi - 1);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
        // cilk_sync;
    }
}

void measure_cilk_qsort(std::vector<int> &vec)
{

    // int sum = 0;
    // // Compute the sum of elements of `vec`
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     sum += vec[i];
    // }

    quickSort_par(vec, 0, vec.size() - 1);
    // quickSort(vec, 0, vec.size() - 1);

}

void measure_std_qsort(std::vector<int> vec)
{
    // std::qsort(&vec[0], vec.size(), sizeof(int), compare);
    quickSort(vec, 0, vec.size() - 1);
}

double averageout_cilk_qsort(std::vector<int> &vec)
{

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        measure_cilk_qsort(vec);
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
    return elapsed_seconds.count() / test_count;
}

double averageout_std_qsort(std::vector<int> const vec)
{

    auto start = std::chrono::high_resolution_clock::now();

    // average out 100 executions to avoid varying results
    for (auto i = 0; i < test_count; i++)
    {
        measure_std_qsort(vec);
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
        n = 10;
    }
    else
    {
        n = std::atoi(argv[1]);
    }

    int numWorkers = __cilkrts_get_nworkers();

    // std::ofstream f;
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);

    // Fill vec with random numbers
    for( int i = 0; i < vec.size(); ++i )
    {
        vec[i] = std::rand() % 100 + 1;
        // std::cout << vec[i] << std::endl;
    }

    auto time = averageout_cilk_qsort(vec);

    std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    std::cout << numWorkers << ", " << time << ',' << std::endl;

    time = averageout_std_qsort(vec);
    std::cout << "[SEQ]: " << n << ", " << time << std::endl;
    // f << time << std::endl;
    

    // VALIDITY CHECK
    // for( int i = 100; i < 200; ++i )
    // {
    //     std::cout << vec[i] << " " << res_seq[i] << std::endl; 
    // }


    // f.close();

    return 0;

}   