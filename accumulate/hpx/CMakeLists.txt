cmake_minimum_required(VERSION 3.13)
project(hpx_sum CXX)


find_package(HPX REQUIRED)
add_executable(hpx_sum hpx_sum.cpp)
target_link_libraries(hpx_sum HPX::hpx HPX::wrap_main)
