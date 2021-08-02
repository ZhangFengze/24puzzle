#include "solver.hpp"
#include "adapter.hpp"

extern "C"
void Solve(const char* rawTask, char* resultBuffer, size_t resultBufferLen)
{
    auto task = ToTask(rawTask);
    auto steps = puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.maxSteps);
    auto result = ToJson(steps);
    strncpy((char*)resultBuffer, result.c_str(), std::min(resultBufferLen, result.size()));
}