cmake_minimum_required(VERSION 3.14)

project(scheduler)

aux_source_directory(. source)
add_executable(${PROJECT_NAME} ${source})

target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/rapidjson/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/boost")
target_include_directories(${PROJECT_NAME} PRIVATE "../..")
target_link_directories(${PROJECT_NAME} PRIVATE "../../submodules/boost/stage/lib")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME})