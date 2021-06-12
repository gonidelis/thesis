.PHONY: cilk_qsort clean

CXX = clang++
CXXFLAGS = -Wall -fopencilk -O3 -ltbb -std=c++1z


cilk_qsort:
	$(CXX) -o cilk_qsort  cilk_qsort.cpp  $(CXXFLAGS) 

bench:
	$(CXX) cilk_qsort.cpp -std=c++1z \
	-isystem ~/thesis/google_bench/benchmark/include \
	-L~/thesis/google_bench/benchmark/build/src -pthread \
	-o mybenchmark -lbenchmark -lbenchmark_main -fopencilk