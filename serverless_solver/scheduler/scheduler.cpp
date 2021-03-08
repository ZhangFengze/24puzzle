#include "solver.h"
#include "../common.h"
#include "rapidjson/document.h"
#include "boost/process.hpp"
#include "boost/asio.hpp"

using namespace n_puzzle_solver::impl;
namespace bp = boost::process;
namespace ba = boost::asio;

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

std::future<std::string> AsyncSolve(const Task& task, ba::io_service& ios)
{
	rapidjson::Document d;
	std::string input = ToString(ToJson(task, d.GetAllocator()));

	bp::opstream in;
	std::future<std::string> out;
	auto child = bp::child("worker.exe", bp::std_in < in, bp::std_out > out, ios);
	in << input << std::endl << std::endl;
    child.detach();
    return out;
}

int main()
{
    auto board = Solver<5, 5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  11, 7,  8,  9,
        10, 6,  12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13
        });
    std::vector<Task> tasks;
    for (const auto& rawTask : Solver<5, 5>::GenerateTasks(board, 100))
    {
        Task t;
        t.board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index;});
        t.steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir;});
        tasks.push_back(t);
    }

    ba::io_service ios;

    for (int depth = 0;;++depth)
    {
        std::vector<std::future<std::string>> results;
        for (auto task : tasks)
        {
            task.depth = depth;
            results.push_back(AsyncSolve(task, ios));
        }

        ios.restart();
        ios.run();

        for (auto& _ : results)
        {
            auto result = _.get();
            if (result != "null")
            {
                std::cout << result;
                return 0;
            }
        }
    }
}