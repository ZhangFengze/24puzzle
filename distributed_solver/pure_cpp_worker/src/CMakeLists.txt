cmake_minimum_required(VERSION 3.7)

include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../third-party/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/../../..)
link_directories(${CMAKE_SOURCE_DIR}/../bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(bootstrap)

set(RUNTIME_SOURCE_FILES ${CMAKE_SOURCE_DIR}/main.cpp)

add_executable(bootstrap ${RUNTIME_SOURCE_FILES})
set_target_properties(bootstrap PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ../../bin/)
target_link_libraries(bootstrap pthread cppruntime)
