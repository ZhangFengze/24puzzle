#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto steps = puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.depth);
    std::cout << ToJson(steps);
    return 0;
}