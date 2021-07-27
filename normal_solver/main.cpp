#include "solver.hpp"
#include "adapter.h"
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
    std::string in;
    std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(), std::back_inserter(in));

    auto task = ToTask(in);
    auto board = Solver<5, 5>::MakeBoard(task.board);
    auto steps = Solve(board);

    if (!steps)
        std::cout << "null" << std::endl;
    else
        std::cout << ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
    return 0;
}