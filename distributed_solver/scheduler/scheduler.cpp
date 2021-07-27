#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.hpp"
#include "async_http_client.hpp"
#include "adapter.h"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"

using namespace n_puzzle_solver::impl;
namespace ba = boost::asio;

class Producer
{
public:
    Producer(const n_puzzle_solver::impl::Solver<5, 5>::Board& board, int preferedCount, int startDepth)
    {
        for (const auto& rawTask : Solver<5, 5>::GenerateTasks(board, preferedCount))
        {
            SemiTask t;
            t.board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index;});
            t.steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir;});
            tasks_.push_back(t);
        }
        index = startDepth * tasks_.size();
    }

    auto operator()()
    {
        auto curIndex = ++index;

        auto depth = curIndex / tasks_.size();
        auto index = curIndex % tasks_.size();

        return Task{ tasks_[index].board, tasks_[index].steps, (int)depth };
    }

private:
    struct SemiTask
    {
        std::array<int, 25> board;
        std::vector<int> steps;
    };
    std::vector<SemiTask> tasks_;
    std::atomic_int index = 0;
};

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
    Producer producer(board, 290, 60);

    std::string result;

    std::function<void(const Task&)> ContinuouslyAsyncSolve;
    ContinuouslyAsyncSolve = [&](const Task& task)
    {
        AsyncSolve(task, ios).then(
            [&, task](auto f)
            {
                auto _ = f.get();
                auto output = std::get_if<std::string>(&_);
                if (!output || *output == "error")
                    return ContinuouslyAsyncSolve(task);
                else if (*output == "null")
                    return ContinuouslyAsyncSolve(producer());

                result = *output;
                ios.stop();
            });
    };

    for (int i = 0, concurrency = 12; i < concurrency; ++i)
        ContinuouslyAsyncSolve(producer());

	ios.run();
    std::cout << result;
}