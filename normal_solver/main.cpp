#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ?
        puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.maxSteps) :
        std::nullopt;
    std::cout << ToJson(steps);
    return 0;
}