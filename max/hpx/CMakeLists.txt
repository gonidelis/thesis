cmake_minimum_required(VERSION 3.13)
project(hpx_max CXX)

find_package(HPX REQUIRED)
add_executable(hpx_max hpx_max.cpp)
target_link_libraries(hpx_max HPX::hpx HPX::wrap_main)
