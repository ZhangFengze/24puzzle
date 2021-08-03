#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

std::optional<std::vector<puzzle::Direction>> Solve(const Task& task)
{
    for (int maxSteps = 0;maxSteps <= task.maxSteps;++maxSteps)
    {
        auto historySteps = task.steps;
        auto steps = puzzle::Solver<5, 5>::Solve(task.board, historySteps, maxSteps);
        if (steps)
            return steps;
    }
    return std::nullopt;
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ? Solve(task) : std::nullopt;
    std::cout << ToJson(steps);
    return 0;
}