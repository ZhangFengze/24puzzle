cmake_minimum_required(VERSION 3.14)

project(normal_solver)

aux_source_directory(. source)
add_executable(${PROJECT_NAME} ${source})

target_include_directories(${PROJECT_NAME} PRIVATE "../submodules/json/single_include")
target_include_directories(${PROJECT_NAME} PRIVATE "../common")

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME})