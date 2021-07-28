#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.hpp"
#include "async_http_client.hpp"
#include "adapter.hpp"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"

namespace ba = boost::asio;

auto AsyncSolve(const PlaneTask& task, ba::io_service& ios)
{
    return AsyncHttpRequest(ios, "1045481767726147.cn-beijing.fc.aliyuncs.com", "80", "/2016-08-15/proxy/puzzle/test/", ToJson(task));
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto tasks = puzzle::Solver<5, 5>::GenerateTasks(task.board, task.steps, 290);

    ba::io_service ios;

    std::string result;
    for (const auto& task : tasks)
    {
        AsyncSolve(ToPlaneTask(Task{ task.board,task.steps,0 }), ios).then(
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