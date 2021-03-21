cmake_minimum_required(VERSION 3.14)

project(worker)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

aux_source_directory(. source)
add_library(${PROJECT_NAME} SHARED ${source})

set(CMAKE_SHARED_LINKER_FLAGS " -static")

target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/rapidjson/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../..")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME})