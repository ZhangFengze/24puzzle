#include "solver.h"
#include "../common.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <iostream>

int main()
{
    rapidjson::Document inDoc;
    rapidjson::IStreamWrapper in(std::cin);
    inDoc.ParseStream<rapidjson::kParseStopWhenDoneFlag>(in);

    auto task = ToTask(inDoc);
    auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);

    if (!steps)
    {
        std::cout << ToString(rapidjson::Value());
        return 0;
    }
    rapidjson::Document outDoc;
    auto out = ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }), outDoc.GetAllocator());
    std::cout << ToString(out);
    return 0;
}