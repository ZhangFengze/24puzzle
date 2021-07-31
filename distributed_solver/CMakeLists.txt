cmake_minimum_required(VERSION 3.14)

project(distributed_solver)
add_subdirectory(aliyun/pure_cpp_worker)
add_subdirectory(aliyun/python_cpp_worker)
add_subdirectory(scheduler)
add_subdirectory(scheduler2)