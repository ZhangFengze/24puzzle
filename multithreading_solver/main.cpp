#include "solver.hpp"
#include "adapter.hpp"
#include <thread>
#include <string>
#include <atomic>
#include <memory>
#include <vector>
#include <tuple>
#include <iostream>

class Producer
{
public:
    Producer(const puzzle::Solver<5, 5>::Board& board)
    {
        auto taskList = puzzle::Solver<5, 5>::GenerateTasks(board, std::thread::hardware_concurrency() * 8);
        tasks = std::vector<puzzle::Solver<5, 5>::Task>(taskList.begin(), taskList.end());
    }

    auto operator()()
    {
        auto curIndex = ++index;

        auto depth = curIndex / tasks.size();
        auto index = curIndex % tasks.size();

        return std::make_tuple(tasks[index].board, tasks[index].steps, (int)depth);
    }

private:
    std::vector<puzzle::Solver<5, 5>::Task> tasks;
    std::atomic_int index = 0;
};

class Consumer
{
public:
    Consumer(Producer& producer, std::atomic_flag& found, std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>>& output)
        :producer_(producer), found_(found), output_(output) {}

    void operator()()
    {
        while (!found_.test())
        {
            auto task = producer_();
            auto steps = puzzle::Solver<5, 5>::Solve(std::get<0>(task), std::get<1>(task), std::get<2>(task));
            if (!steps)
                continue;
            output_.store(std::make_shared<std::vector<puzzle::Direction>>(*steps));
            found_.test_and_set();
            found_.notify_all();
        }
    }

private:
    Producer& producer_;
    std::atomic_flag& found_;
    std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>>& output_;
};

class _Solver : public std::enable_shared_from_this<_Solver>
{
public:
    _Solver(const puzzle::Solver<5, 5>::Board& board)
        :producer(board) {}

    std::optional<std::vector<puzzle::Direction>> operator()()
    {
        for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            std::thread t{ [self = shared_from_this(), this] { Consumer{ producer,found,output }(); } };
            t.detach();
        }
        found.wait(false);
        return *output.load();
    }

private:
    Producer producer;
    std::atomic_flag found;// no need to init since C++20
    std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>> output;
};

std::optional<std::vector<puzzle::Direction>> Solve(const puzzle::Solver<5, 5>::Board& board)
{
    auto solver = std::make_shared<_Solver>(board);
    return (*solver)();
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto board = puzzle::Solver<5, 5>::MakeBoard(task.board);
    auto steps = Solve(board);
    std::cout << ToJson(steps);
    return 0;
}