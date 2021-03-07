#include "solver.h"
#include "../common.h"
#include "rapidjson/document.h"

using namespace n_puzzle_solver::impl;

void ToTasks(const n_puzzle_solver::impl::Solver<5, 5>::Board& board, int prefered)
{
    rapidjson::Document d;
    for (const auto& rawTask : n_puzzle_solver::impl::Solver<5, 5>::GenerateTasks(board, 3))
    {
        Task t;
        t.board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index;});
        t.steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir;});
        t.depth = 30;

        auto s = ToString(ToJson(t, d.GetAllocator()));

        std::cout << s << std::endl;
    }
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main()
{
    auto board = Solver<5, 5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19,
        20, 21, 22, 23, 24
        });

    std::vector<Task> tasks;
    for (const auto& rawTask : Solver<5, 5>::GenerateTasks(board, 100))
    {
        Task t;
        t.board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index;});
        t.steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir;});
        tasks.push_back(t);
    }

    for (int depth = 0;;++depth)
    {
        for (auto task : tasks)
        {
            task.depth = depth;
            rapidjson::Document d;
            std::string input = ToString(ToJson(task, d.GetAllocator()));
            // TODO
        }
    }
}