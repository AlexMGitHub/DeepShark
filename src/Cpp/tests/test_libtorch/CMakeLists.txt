cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(custom_ops)

find_package(Torch REQUIRED)

add_executable(test_libtorch test_libtorch.cpp)
target_link_libraries(test_libtorch "${TORCH_LIBRARIES}")
set_property(TARGET test_libtorch PROPERTY CXX_STANDARD 17)