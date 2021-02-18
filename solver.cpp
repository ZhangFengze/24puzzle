#include "solver.h"
#include <string>

std::string ToString(Direction d)
{
    static const std::array<std::string, 4> strings =
    { "¡ü","¡ú","¡ý","¡û" };
    return strings[(int)d];
}

std::string ToString(int i)
{
    char buf[4];
    snprintf(buf, sizeof buf, "%2d", i);
    return buf;
}

template<size_t rows, size_t cols>
std::string ToString(const Board<rows, cols>& board)
{
    std::string out;
    for (int y = 0;y < rows;++y)
    {
        std::string line = "|";
        for (int x = 0;x < cols;++x)
        {
            int grid = board.board[Position<rows, cols>{ x, y }.index].index;
            if (grid == rows * cols - 1)
                line += "  |";
            else
                line += ToString(grid) + "|";
        }
        out += line + "\n";
    }
    return out;
}

int main()
{
    auto board = MakeBoard<5,5>(
        {
            0,  24,  2,  9,  4,
            5,  6,  7,  3,  8,
            10, 11, 12, 13, 14,
            15, 16, 22, 18, 19,
            20, 21, 23, 17, 1
        });
    std::cout << ToString(board) << std::endl;

    if (!Solvable(board))
    {
        std::cout << "not solable";
        return 0;
    }
    std::cout << "sovable" << std::endl;

    for (int depth = 0;;++depth)
    {
        std::cout << "depth: " << depth << "\n";

        std::vector<Direction> steps_;
        auto steps = Solve(board, steps_, depth);

        if (steps)
        {
            std::cout << "got steps:";
            for (auto step : *steps)
                std::cout << ToString(step) << "\t";
            break;
        }
    }
    return 0;
}