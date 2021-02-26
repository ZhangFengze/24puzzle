#include "solver.h"
#include <string>

using namespace n_puzzle_solver;
using namespace n_puzzle_solver::impl;

std::string ToString(Direction d)
{
    static const std::array<std::string, 4> strings =
    { "up","right","down","left" };
    return strings[(int)d];
}

std::string ToString(int i)
{
    char buf[4];
    snprintf(buf, sizeof buf, "%2d", i);
    return buf;
}

template<size_t rows, size_t cols>
std::string ToString(const typename Solver<rows, cols>::Board& board)
{
    std::string out;
    for (int y = 0;y < rows;++y)
    {
        std::string line = "|";
        for (int x = 0;x < cols;++x)
        {
            int grid = board.board[Solver<rows, cols>::Position{ x, y }.index].index;
            if (grid == rows * cols - 1)
                line += "  |";
            else
                line += ToString(grid) + "|";
        }
        out += line + "\n";
    }
    return out;
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
    auto board = boards[4];
    std::cout << ToString<5, 5>(board) << std::endl;

    if (!Solver<5, 5>::Solvable(board))
    {
        std::cout << "not solable";
        return 0;
    }
    std::cout << "sovable" << std::endl;

    Solver<5, 5>::Solver2(board);
    return 0;
}