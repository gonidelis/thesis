#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include <stdlib.h>

#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <execution>

int test_count = 5;

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

// Alternate Function
// void quickSort(std::vector<int> arr, int left, int right) {
//       int i = left, j = right;
//       int tmp;
//       int pivot = arr[(left + right) / 2];
 
//       /* partition */
//       while (i <= j) {
//             while (arr[i] < pivot)
//                   i++;
//             while (arr[j] > pivot)
//                   j--;
//             if (i <= j) {
//                   tmp = arr[i];
//                   arr[i] = arr[j];
//                   arr[j] = tmp;
//                   i++;
//                   j--;
//             }
//       };
 
//       /* recursion  */

// 	  if (left < j)
//       {
        
//         // std::cout << __cilkrts_get_nworkers() << std::endl;
//         cilk_spawn quickSort(arr, left, j);
        
//       }
//       if (i < right)
//             quickSort(arr, i, right);
// 		cilk_sync;
// 		//*/
// }

template<class T>
void quick_sort(std::vector<T> list, std::true_type);
template<class T>
void quick_sort(std::vector<T> list);

template<class T>
void quick_sort(std::vector<T>& list, int start, int end, std::true_type);
template<class T>
void quick_sort(std::vector<T>& list, int start, int end);

template<class T>
int partition(std::vector<T>& list, int start, int end, int pivot);


// SEQUENTIAL CALLER
template<class T>
void quick_sort(std::vector<T> list) {
  quick_sort(list, 0, list.size() - 1);
}

// PARALLEL CALLER
template<class T>
void quick_sort(std::vector<T> list, std::true_type) {
  quick_sort(list, 0, list.size() - 1, std::true_type{});
}


// SEQUENTIAL VERSION
template<class T>
void quick_sort(std::vector<T>& list, int start, int end) {

    if(start < end)
    {
        int pivot = list[(start + end) / 2];
        int index = partition(list, start, end, pivot);
        quick_sort(list, start, index - 1);
        // quick_sort(list, start, index - 1);
        quick_sort(list, index, end);
    }
}

// PARALLEL VERSION (std::true_type)
template<class T>
void quick_sort(std::vector<T>& list, int start, int end, std::true_type) {

    if(start < end)
    {
        int pivot = list[(start + end) / 2];
        int index = partition(list, start, end, pivot);
        if (abs(end-start) > list.size()/16)                  // CONTROL GRANULARITY!!!
        {
            if(abs(start - index) > abs(end - index))          // SPAWN EXTRA WORKER FOR THE LARGER CHUNK
            {
                cilk_spawn quick_sort(list, start, index - 1);
                quick_sort(list, index, end);
                cilk_sync;
            }
            else
            {
                quick_sort(list, start, index - 1);
                cilk_spawn quick_sort(list, index, end);
                cilk_sync;
            }
        }
        else
        {
            quick_sort(list, start, index - 1);
            // quick_sort(list, start, index - 1);
            quick_sort(list, index, end);

        }

    }
}

template<class T>
int partition(std::vector<T>& list, int start, int end, int pivot) {
  int left = start, right = end;
  
  while(left <= right) {
    while (list[left] < pivot) left++;
    while (list[right] > pivot) right--;
    if (left <= right) {
      std::swap(list[left], list[right]);
      left++, right--;
    }
  }
  return left;
}

void measure_cilk_qsort(std::vector<int> vec)
{

    // int sum = 0;
    // // Compute the sum of elements of `vec`
    // for(unsigned int i = 1; i <= vec.size(); ++i)
    // {
    //     sum += vec[i];
    // }

    quick_sort(vec, std::true_type{});
    // quickSort(vec, 0, vec.size() - 1);

}

void measure_std_qsort(std::vector<int> vec)
{
    // std::qsort(&vec[0], vec.size(), sizeof(int), compare);
    quick_sort(vec);
}

double averageout_cilk_qsort(std::vector<int> vec)
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