#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.h"
#include "async_http_client.h"
#include "adapter.h"
#include "boost/process.hpp"
#include "boost/asio.hpp"
#include "boost/asio/use_future.hpp"
#include "boost/thread/future.hpp"
#include <semaphore>
#include <atomic>

using namespace n_puzzle_solver::impl;
namespace bp = boost::process;
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

template<int p1, int p2, typename Type>
struct async_out_future : ::boost::process::detail::windows::handler_base_ext,
    ::boost::process::detail::windows::require_io_context,
    ::boost::process::detail::uses_handles
{
    std::shared_ptr<boost::process::async_pipe> pipe;
    std::shared_ptr<boost::promise<Type>> promise = std::make_shared<boost::promise<Type>>();
    std::shared_ptr<boost::asio::streambuf> buffer = std::make_shared<boost::asio::streambuf>();

    ::boost::winapi::HANDLE_ get_used_handles() const
    {
        return std::move(*pipe).sink().native_handle();
    }


    async_out_future(boost::unique_future<Type>& fut)
    {
        fut = promise->get_future();
    }
    template <typename Executor>
    inline void on_success(Executor&)
    {
        auto pipe_ = this->pipe;
        auto buffer_ = this->buffer;
        auto promise_ = this->promise;
        std::move(*pipe_).sink().close();
        boost::asio::async_read(*pipe_, *buffer_,
            [pipe_, buffer_, promise_](const boost::system::error_code& ec, std::size_t)
            {
                if (ec && (ec.value() != ::boost::winapi::ERROR_BROKEN_PIPE_))
                {
                    std::error_code e(ec.value(), std::system_category());
                    //promise_->set_exception(std::make_exception_ptr(process_error(e)));
                }
                else
                {
                    std::istream is(buffer_.get());
                    Type arg;
                    if (buffer_->size() > 0)
                    {
                        arg.resize(buffer_->size());
                        is.read(&*arg.begin(), buffer_->size());
                    }

                    promise_->set_value(std::move(arg));


                }
            });
        this->pipe = nullptr;
        this->buffer = nullptr;
        this->promise = nullptr;


    }

    template<typename Executor>
    void on_error(Executor&, const std::error_code&) const
    {
        std::move(*pipe).sink().close();
    }

    template <typename WindowsExecutor>
    void on_setup(WindowsExecutor& exec)
    {
        if (!pipe)
            pipe = std::make_shared<boost::process::async_pipe>(get_io_context(exec.seq));

        apply_out_handles(exec, std::move(*pipe).sink().native_handle(),
            std::integral_constant<int, p1>(), std::integral_constant<int, p2>());
    }
};

boost::unique_future<std::string>
AsyncSolve_Local(const Task& task, ba::io_service& ios)
{
    std::string input = ToJson(task);

    bp::opstream in;
    boost::unique_future<std::string> out;
    auto child = bp::child("worker.exe", bp::std_in < in, async_out_future<1, -1, std::string>(out), ios);
    in << input << std::endl << std::endl;
    child.detach();
    return out;
}

auto AsyncSolve_Http(const Task& task, ba::io_service& ios)
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
        AsyncSolve_Http(task, ios).then(
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