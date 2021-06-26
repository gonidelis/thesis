#include <cilk/cilk.h>
// #include <cilk/holder.h>
#include <cilk/reducer_opadd.h>
#include <cilk/cilk_api.h>

#include <stdlib.h>

#include <iostream>
#include <random>
#include <vector>
#include <chrono>

int GRANULARITY_THRESHOLD = 10000;
int test_count = 1;


int partition(std::vector<int> &arr, int low, int high)
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


void quickSort_par(std::vector<int> &arr, int low, int high)
{

    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
        auto pi = partition(arr, low, high);

        if(abs(high - low) > GRANULARITY_THRESHOLD)
        {
            cilk_spawn quickSort_par(arr, low, pi - 1);
            // quickSort(arr, low, pi - 1);
            quickSort_par(arr, pi + 1, high);
            cilk_sync;
        }
        else
        {
            quickSort(arr, low, pi - 1);
            // quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
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


int  main(int argc, char* argv[])
{
    std::srand(std::time(nullptr)); // use current time as seed for random generator

    int n;
    if(argv[1] == NULL)
    {
        n = 100000;
    }
    else
    {
        n = std::atoi(argv[1]);
    }

    int numWorkers = __cilkrts_get_nworkers();

    // std::ofstream f;
    // f.open("sum_times.csv", std::ios_base::app);

    std::vector<int> vec(n);
    std::vector<int> seq_vec(n);

    // Fill vec with random numbers
    for( int i = 0; i < vec.size(); ++i )
    {
        vec[i] = std::rand() % 100 + 1;
        // std::cout << vec[i] << std::endl;
    }

    // copy vector
    for( int i = 0; i < vec.size(); ++i )
    {
        seq_vec[i] = vec[i];
        // std::cout << vec[i] << std::endl;
    }    

    auto time = averageout_cilk_qsort(vec);
    std::cout << "[Cilk]: " << n << " ," << numWorkers << ", " << time << std::endl;
    std::cout << numWorkers << ", " << time << ',' << std::endl;

    // f << time << std::endl;
    

    // VALIDITY CHECK
    // for( int i = 0 ; i < vec.size(); ++i )
    // {
    //     std::cout << seq_vec[i] << std::endl; 
    // }


    // f.close();

    return 0;

}   
