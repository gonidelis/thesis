.PHONY: cilk_copy clean

CXX = clang++
CXXFLAGS = -Wall -fopencilk -O3 -ltbb -std=c++1z


cilk_copy:
	$(CXX) -o cilk_copy  cilk_copy.cpp  $(CXXFLAGS) 
