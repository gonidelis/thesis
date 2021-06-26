.PHONY: std_qsort clean

CXX = g++
CXXFLAGS = -Wall -O3 -std=c++1z


std_qsort:
	$(CXX) -o std_qsort  std_qsort.cpp  $(CXXFLAGS) 

bench:
	$(CXX) cilk_qsort.cpp -std=c++1z \
	-isystem ~/thesis/google_bench/benchmark/include \
	-L~/thesis/google_bench/benchmark/build/src -pthread \
	-o mybenchmark -lbenchmark -lbenchmark_main -fopencilk