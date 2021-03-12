#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#include "solver.h"
#include "../common.h"
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
    Producer(const n_puzzle_solver::impl::Solver<5, 5>::Board& board, int preferedCount)
    {
		for (const auto& rawTask : Solver<5, 5>::GenerateTasks(board, preferedCount))
		{
			SemiTask t;
			t.board = Map(rawTask.board.board, [](const auto& position) {return (int)position.index;});
			t.steps = Map(rawTask.steps, [](const auto& dir) {return (int)dir;});
            tasks_.push_back(t);
		}
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


    async_out_future(boost::shared_future<Type>& fut)
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

boost::shared_future<std::string> AsyncSolve(const Task& task, ba::io_service& ios)
{
    std::string input = ToJson(task);

    bp::opstream in;
    boost::shared_future<std::string> out;
    auto child = bp::child("worker.exe", bp::std_in < in, async_out_future<1, -1, std::string>(out), ios);
    in << input << std::endl << std::endl;
    child.detach();
    return out;
}

int main()
{
    ba::io_service ios;

    auto board = Solver<5, 5>::MakeBoard
    ({
        0,  1,  2,  3,  4,
        5,  11, 7,  8,  9,
        10, 6,  12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13
        });
    Producer producer(board, 100);

    std::atomic<std::shared_ptr<std::string>> result;

    std::function<void()> ContinuouslyAsyncSolve;
    ContinuouslyAsyncSolve= [&]()
	{
		AsyncSolve(producer(), ios).then(
			[&](boost::shared_future<std::string> f)
		{
            auto output = f.get();
            if (output != "null")
            {
                result.store(std::make_shared<std::string>(output));
                ios.stop();
                return;
            }
			ContinuouslyAsyncSolve();
		});
	};

    for (int i = 0, concurrency = 12; i < concurrency; ++i)
        ContinuouslyAsyncSolve();

    while (!result.load())
        ios.run();
    std::cout << *result.load();
}