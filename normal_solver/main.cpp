#include "solver.hpp"
#include "adapter.hpp"
#include <string>

using namespace n_puzzle_solver;
using namespace n_puzzle_solver::impl;

std::optional<std::vector<Direction>> Solve(const Solver<5, 5>::Board& board)
{
    for (int depth = 0;;++depth)
    {
        std::vector<Direction> steps_;
        auto steps = Solver<5, 5>::Solve(board, steps_, depth);
        if (steps)
            return steps;
    }
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = Solver<5, 5>::MakeBoard(task.board);
    auto steps = Solve(board);

    if (!steps)
        std::cout << "null" << std::endl;
    else
        std::cout << ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
    return 0;
}