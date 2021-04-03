#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.h"
#include "async_http_client.hpp"
#include "adapter.h"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"

using namespace n_puzzle_solver::impl;
namespace ba = boost::asio;

auto AsyncSolve(const Task& task, ba::io_service& ios)
{
    return AsyncHttpRequest(ios, "1045481767726147.cn-beijing.fc.aliyuncs.com", "80", "/2016-08-15/proxy/puzzle/test/", ToJson(task));
}

int main()
{
    ba::io_service ios;

    auto board = Solver<5, 5>::MakeBoard
    ({
        0,  24, 2,  9,  4,
        5,  6,  7,  3,  8,
        10, 11, 12, 18, 14,
        15, 21, 22, 13, 19,
        20, 16, 23, 17, 1
        });
    auto rawTasks = Solver<5, 5>::GenerateTasks(board, 290);
    auto tasks = Map(rawTasks,
        [](auto rawTask)
    {
        auto board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index; });
        auto steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir; });
        return Task{ board, steps, 0 };
    });

    std::string result;
    for (const auto& task : tasks)
    {
        AsyncSolve(task, ios).then(
            [&, task](auto f)
            {
                auto _ = f.get();
                auto output = std::get_if<std::string>(&_);
                if (!output || *output == "error")
                    return;
                else if (*output == "null")
                    return;

                result = *output;
                ios.stop();
            });
    }

	ios.run();
	std::cout << result;
}