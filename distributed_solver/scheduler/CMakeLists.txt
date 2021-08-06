cmake_minimum_required(VERSION 3.14)

project(scheduler)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(Python_USE_STATIC_LIBS TRUE)

aux_source_directory(. source)
find_package (Python3 COMPONENTS Interpreter Development)
Python3_add_library(${PROJECT_NAME} SHARED ${source})

target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/rapidjson/include")
target_include_directories(${PROJECT_NAME} PRIVATE "../../submodules/json/single_include")
target_include_directories(${PROJECT_NAME} PRIVATE "../../common")

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "func")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES scheduler.py DESTINATION bin)