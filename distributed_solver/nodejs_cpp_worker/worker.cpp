#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);
    std::cout << ToJson(steps);
    return 0;
}