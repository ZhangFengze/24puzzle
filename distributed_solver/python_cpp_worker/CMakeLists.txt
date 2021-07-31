cmake_minimum_required(VERSION 3.14)

project(distributed_solver_python_cpp_worker)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

aux_source_directory(. source)
add_library(${PROJECT_NAME} SHARED ${source})

target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/rapidjson/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../..")
target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME})