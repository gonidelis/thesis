.PHONY: std_max clean

CXX = g++
CXXFLAGS = -Wall -O3 -std=c++1z

std_max:
	$(CXX) -o std_max  std_max.cpp  $(CXXFLAGS) 

bench:
	$(CXX) cilk_max.cpp -std=c++1z \
	-isystem ~/thesis/google_bench/benchmark/include \
	-L~/thesis/google_bench/benchmark/build/src -pthread \
	-o mybenchmark -lbenchmark -lbenchmark_main -fopencilk