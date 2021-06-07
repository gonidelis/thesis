cmake_minimum_required(VERSION 3.13)
project(hpx_copy CXX)


find_package(HPX REQUIRED)
add_executable(hpx_copy hpx_copy.cpp)
target_link_libraries(hpx_copy HPX::hpx HPX::wrap_main)
