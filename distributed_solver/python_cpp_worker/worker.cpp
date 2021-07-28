#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

std::string Solve(const std::string& in)
{
    auto task = ToTask(in);
    auto steps = puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.depth);
    return ToJson(steps);
}

extern "C"
const char* Solve(const char* in)
{
    thread_local std::string result;
    result = Solve(std::string(in));
    return result.c_str();
}