cmake_minimum_required(VERSION 3.7)
project(distributed_solver_pure_cpp_worker)

aux_source_directory(src sources)
add_executable(${PROJECT_NAME} ${sources})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE third-party/rapidjson/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../..)
target_link_directories(${PROJECT_NAME} PRIVATE libs)
target_link_libraries(${PROJECT_NAME} pistache pthread)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME})