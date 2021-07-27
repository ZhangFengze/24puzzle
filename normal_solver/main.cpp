#include "solver.hpp"
#include "adapter.hpp"
#include <iostream>

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = puzzle::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return puzzle::Direction(dir); });
    auto steps = puzzle::Solver<5, 5>::Solve(board, tempSteps, task.depth);
    std::cout << ToJson(steps);
    return 0;
}