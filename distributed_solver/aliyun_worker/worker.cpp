#include "solver.hpp"
#include "adapter.hpp"

extern "C"
void Solve(const char* rawTask, char* resultBuffer, size_t resultBufferLen)
{
    auto task = ToTask(rawTask);
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ?
        puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.maxSteps) :
        std::nullopt;
    auto result = ToJson(steps);
    strncpy((char*)resultBuffer, result.c_str(), std::min(resultBufferLen, result.size()));
}