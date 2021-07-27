#include "solver.hpp"
#include "adapter.hpp"
#include <string>

std::optional<std::vector<puzzle::Direction>> Solve(const puzzle::Solver<5, 5>::Board& board)
{
    for (int depth = 0;;++depth)
    {
        std::vector<puzzle::Direction> steps_;
        auto steps = puzzle::Solver<5, 5>::Solve(board, steps_, depth);
        if (steps)
            return steps;
    }
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = puzzle::Solver<5, 5>::MakeBoard(task.board);
    auto steps = Solve(board);
    std::cout << ToJson(steps);
    return 0;
}