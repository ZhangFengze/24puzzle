#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

std::string Solve(const std::string& in)
{
    auto task = ToTask(in);
    auto board = puzzle::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return puzzle::Direction(dir); });
    auto steps = puzzle::Solver<5, 5>::Solve(board, tempSteps, task.depth);
    return ToJson(steps);
}

extern "C"
const char* Solve(const char* in)
{
    thread_local std::string result;
    result = Solve(std::string(in));
    return result.c_str();
}