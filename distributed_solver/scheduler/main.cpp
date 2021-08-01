#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.hpp"
#include "async_http_client.hpp"
#include "adapter.hpp"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"

namespace ba = boost::asio;

class Producer
{
public:
    Producer(const puzzle::Solver<5, 5>::Board& board,
        const std::vector<puzzle::Direction>& historySteps, int preferredCount, int startDepth)
    {
        for (const auto& rawTask : puzzle::Solver<5, 5>::GenerateTasks(board, historySteps, preferredCount))
            tasks_.push_back(ToPlaneTask(Task{ rawTask.board,rawTask.steps,0 }));
        index_ = startDepth * tasks_.size();
    }

    auto operator()()
    {
        auto curIndex = ++index_;

        auto depth = curIndex / tasks_.size();
        auto index = curIndex % tasks_.size();

        auto task = tasks_[index];
        task.depth = (int)depth;
        return task;
    }

private:
    std::vector<PlaneTask> tasks_;
    std::atomic_size_t index_ = 0;
};

auto AsyncSolve(const PlaneTask& task, ba::io_service& ios)
{
    return AsyncHttpRequest(ios, "1045481767726147.cn-beijing.fc.aliyuncs.com", "80", "/2016-08-15/proxy/puzzle/test/", ToJson(task));
}

int main()
{
    rapidjson::Document config;
    config.Parse<rapidjson::kParseStopWhenDoneFlag>(ReadAll(std::cin).c_str());

    std::string url = config["url"].GetString();
    int concurrency = config["concurrency"].GetInt();
    int taskPreferredCount = config["taskPreferredCount"].GetInt();
    auto task = ToTask(ToPlaneTask(config["task"]));

    Producer producer(task.board, task.steps, taskPreferredCount, 0);

    ba::io_service ios;
    std::string result;

    std::function<void(const PlaneTask&)> ContinuouslyAsyncSolve;
    ContinuouslyAsyncSolve = [&](const PlaneTask& task)
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

    for (int i = 0; i < concurrency; ++i)
        ContinuouslyAsyncSolve(producer());

    ios.run();
    std::cout << result;
}