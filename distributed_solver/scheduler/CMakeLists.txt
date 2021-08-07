cmake_minimum_required(VERSION 3.14)
project(scheduler)
add_subdirectory(../../common common)
install(FILES scheduler.py DESTINATION bin)