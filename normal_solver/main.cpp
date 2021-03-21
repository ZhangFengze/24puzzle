#include "solver.h"
#include "adapter.h"
#include <string>

using namespace n_puzzle_solver;
using namespace n_puzzle_solver::impl;

std::optional<std::vector<Direction>> Solve(const Solver<5,5>::Board& board)
{
    for (int depth = 0;;++depth)
    {
        std::vector<Direction> steps_;
        auto steps = Solver<5,5>::Solve(board, steps_, depth);
        if (steps)
            return steps;
    }
}

std::vector<Solver<5, 5>::Board> boards =
{
    Solver<5,5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19,
        20, 21, 22, 23, 24
    }),
    Solver<5,5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  6,  7,  8,  9,
        10, 11, 12, 24, 14,
        15, 16, 17, 18, 19,
        20, 21, 22, 23, 13
    }),
    Solver<5,5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  11, 7,  8,  9,
        10, 6,  12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13
    }),
    Solver<5,5>::MakeBoard
    ({
        0,  6,  2,  3,  4,
        5,  1,  7,  8,  9,
        10, 11, 12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13
    }),
    Solver<5,5>::MakeBoard
    ({
        0,  24, 2,  9,  4,
        5,  6,  7,  3,  8,
        10, 11, 12, 18, 14,
        15, 21, 22, 13, 19,
        20, 16, 23, 17, 1
    })
};

int main()
{
    auto board = boards[3];
    auto steps = Solve(board);
    if (!steps)
        std::cout<<"null"<<std::endl;
    else
        std::cout << ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
    return 0;
}