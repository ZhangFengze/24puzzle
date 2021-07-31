cmake_minimum_required(VERSION 3.14)

project(aliyun_python_cpp_worker)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

aux_source_directory(. source)
add_library(${PROJECT_NAME} SHARED ${source})

target_include_directories(${PROJECT_NAME} PRIVATE "../../../submodules/rapidjson/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../../../common")
target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "worker")
install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
install(FILES aliyun_function_compute_shell.py http_shell.py DESTINATION ${PROJECT_NAME})