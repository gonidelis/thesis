.PHONY: cilk_max clean

CXX = clang++
CXXFLAGS = -Wall -fopencilk -O3


cilk_max:
	$(CXX) -o cilk_max  cilk_max.cpp  $(CXXFLAGS) 

bench:
	$(CXX) cilk_max.cpp -std=c++1z \
	-isystem ~/thesis/google_bench/benchmark/include \
	-L~/thesis/google_bench/benchmark/build/src -pthread \
	-o mybenchmark -lbenchmark -lbenchmark_main -fopencilk