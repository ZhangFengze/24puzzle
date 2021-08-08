cmake_minimum_required(VERSION 3.14)
project(puzzle)

function (cpp_executable name sources)
    add_executable(${name} ${sources})

    target_include_directories(${name} PRIVATE "../3rd/json/single_include")
    target_include_directories(${name} PRIVATE "../3rd/concurrentqueue")
    target_include_directories(${name} PRIVATE "common")

    find_package(Threads REQUIRED)
    target_link_libraries(${name} PRIVATE Threads::Threads)

    if(NOT WIN32)
        target_link_libraries(${name} PRIVATE -static-libgcc -static-libstdc++)
    endif()

    set_property(TARGET ${name} PROPERTY CXX_STANDARD 20)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED ON)

    install(TARGETS ${name} DESTINATION .)
endfunction()

function (python_project name scripts)
    install(FILES ${scripts} DESTINATION .)
endfunction()

add_subdirectory(common)

cpp_executable(normal_solver_cpp normal_solver.cpp)
python_project(normal_solver_py normal_solver.py)
cpp_executable(multithreading_solver_cpp multithreading_solver.cpp)
python_project(multiprocessing_solver_py multiprocessing_solver.py)
python_project(multiprocessing_solver_py_2 multiprocessing_solver2.py)
python_project(multiprocessing_solver_py_3 multiprocessing_solver3.py)
python_project(multiprocessing_solver_py_4 multiprocessing_solver4.py)
python_project(distributed_solver_scheduler distributed_solver_scheduler.py)
python_project(distributed_solver_worker distributed_solver_worker.py)
set(test_scripts test.py common.py)
python_project(test "${test_scripts}")
