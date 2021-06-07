.PHONY: cilk_sum clean

CXX = clang++
CXXFLAGS = -Wall -fopencilk -O3 -ltbb -std=c++1z


cilk_sum:
	$(CXX) -o cilk_sum  cilk_sum.cpp  $(CXXFLAGS) 
