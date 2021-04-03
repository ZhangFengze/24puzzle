#include "solver.h"
#include "adapter.h"
#include <iostream>

int main()
{
    std::string in;
    std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(), std::back_inserter(in));

    auto task = ToTask(in);
    auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);

    if (!steps)
    {
        std::cout << "null";
        return 0;
    }
    std::cout << ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
    return 0;
}