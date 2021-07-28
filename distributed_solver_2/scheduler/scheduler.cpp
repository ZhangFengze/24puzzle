#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.hpp"
#include "async_http_client.hpp"
#include "adapter.hpp"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"

namespace ba = boost::asio;

auto AsyncSolve(const Task& task, ba::io_service& ios)
{
    return AsyncHttpRequest(ios, "1045481767726147.cn-beijing.fc.aliyuncs.com", "80", "/2016-08-15/proxy/puzzle/test/", ToJson(task));
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = puzzle::Solver<5, 5>::MakeBoard(task.board);
    auto historySteps = Map(task.steps, [](int dir) {return puzzle::Direction(dir); });
    auto rawTasks = puzzle::Solver<5, 5>::GenerateTasks(board, historySteps, 290);
    auto tasks = Map(rawTasks,
        [](auto rawTask)
        {
            auto board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index; });
            auto steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir; });
            return Task{ board, steps, 0 };
        });

    ba::io_service ios;

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