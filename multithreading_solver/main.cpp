#include "solver.h"
#include "adapter.h"
#include <ppl.h>
#include <concurrent_queue.h>
#include <string>
#include <atomic>
#include <vector>
#include <tuple>
#include <iostream>

using namespace n_puzzle_solver;
using namespace n_puzzle_solver::impl;

class Producer
{
public:
    Producer(const Solver<5,5>::Board& board)
    {
        auto taskList = Solver<5,5>::GenerateTasks(board, std::thread::hardware_concurrency() * 8);
        tasks = std::vector<Solver<5,5>::Task>(taskList.begin(), taskList.end());
    }

    auto operator()()
    {
        auto curIndex = ++index;

        auto depth = curIndex / tasks.size();
        auto index = curIndex % tasks.size();

        return std::make_tuple(tasks[index].board, tasks[index].steps, (int)depth);
    }

private:
    std::vector<Solver<5,5>::Task> tasks;
    std::atomic_int index = 0;
};

class Consumer
{
public:
    Consumer(Producer& producer, std::atomic_flag& found, Concurrency::concurrent_queue<std::vector<Direction>>& output)
        :producer_(producer), found_(found), output_(output) {}

    void operator()()
    {
        while (!found_.test())
        {
            auto task = producer_();
            auto steps = Solver<5,5>::Solve(std::get<0>(task), std::get<1>(task), std::get<2>(task));
            if (!steps)
                continue;
            output_.push(*steps);
            found_.test_and_set();
            found_.notify_all();
        }
    }

private:
    Producer& producer_;
    std::atomic_flag& found_;
    Concurrency::concurrent_queue<std::vector<Direction>>& output_;
};

class _Solver : public std::enable_shared_from_this<_Solver>
{
public:
    _Solver(const Solver<5,5>::Board& board)
        :producer(board) {}

    std::optional<std::vector<Direction>> operator()()
    {
        for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
        {
            std::thread t{ [self = shared_from_this(), this] { Consumer{ producer,found,output }(); } };
            t.detach();
        }
        found.wait(false);
        std::vector<Direction> steps;
        bool got = output.try_pop(steps);
        assert(got);
        return steps;
    }

private:
    Producer producer;
    std::atomic_flag found;// no need to init since C++20
    Concurrency::concurrent_queue<std::vector<Direction>> output;
};

std::optional<std::vector<Direction>> Solve(const Solver<5,5>::Board& board)
{
    auto solver = std::make_shared<_Solver>(board);
    return (*solver)();
}

int main()
{
    std::string in;
    std::copy(std::istream_iterator<char>(std::cin), std::istream_iterator<char>(), std::back_inserter(in));

    auto task = ToTask(in);
    auto board = Solver<5, 5>::MakeBoard(task.board);
    auto steps = Solve(board);

    if (!steps)
        std::cout<<"null"<<std::endl;
    else
        std::cout << ToJson(Map(*steps, [](const auto& dir) {return (int)dir; }));
    return 0;
}