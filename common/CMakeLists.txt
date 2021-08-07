cmake_minimum_required(VERSION 3.14)

set(name solver)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(Python_USE_STATIC_LIBS TRUE)

find_package (Python3 COMPONENTS Interpreter Development)
Python3_add_library(${name} SHARED python.cpp)

target_include_directories(${name} PRIVATE "../submodules/json/single_include")

if(NOT WIN32)
    target_link_libraries(${name} PRIVATE -static-libgcc -static-libstdc++)
endif()

set_property(TARGET ${name} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${name} PROPERTIES PREFIX "")
if(WIN32)
    set_target_properties(${name} PROPERTIES SUFFIX ".pyd")
endif()
set_target_properties(${name} PROPERTIES OUTPUT_NAME "solver")
install(TARGETS ${name} DESTINATION bin)