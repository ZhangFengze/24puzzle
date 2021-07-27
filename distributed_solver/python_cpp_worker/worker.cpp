#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

std::string Solve(const std::string& in)
{
    auto task = ToTask(in);
    auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);

    if (!steps)
        return "null";

    return ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
}

extern "C"
const char* Solve(const char* in)
{
    thread_local std::string result;
    result = Solve(std::string(in));
    return result.c_str();
}