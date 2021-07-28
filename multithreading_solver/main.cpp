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
    Producer(const Task& task)
    {
        auto taskList = puzzle::Solver<5, 5>::GenerateTasks(task.board, task.steps, std::thread::hardware_concurrency() * 8);
        tasks = std::vector<puzzle::Solver<5, 5>::Task>(taskList.begin(), taskList.end());
        maxIndex = (task.depth + 1) * (int)tasks.size();
    }

    std::optional<Task> operator()()
    {
        auto curIndex = ++index;
        if (curIndex >= maxIndex)
            return std::nullopt;

        auto depth = curIndex / tasks.size();
        auto index = curIndex % tasks.size();

        return Task{ tasks[index].board, tasks[index].steps, (int)depth };
    }

private:
    std::vector<puzzle::Solver<5, 5>::Task> tasks;
    std::atomic_int index = 0;
    int maxIndex = 0;
};

class Consumer
{
public:
    Consumer(Producer& producer, std::atomic_flag& found,
        std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>>& output)
        :producer_(producer), exit_(found), output_(output) {}

    void operator()()
    {
        while (!exit_.test())
        {
            auto task = producer_();
            if (!task)
                return;
            auto steps = puzzle::Solver<5, 5>::Solve(task->board, task->steps, task->depth);
            if (!steps)
                continue;
            output_.store(std::make_shared<std::vector<puzzle::Direction>>(*steps));
            exit_.test_and_set();
            exit_.notify_all();
        }
    }

private:
    Producer& producer_;
    std::atomic_flag& exit_;
    std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>>& output_;
};

std::optional<std::vector<puzzle::Direction>> Solve(const Task& task)
{
    Producer producer{ task };
    std::atomic_flag exit;// no need to init since C++20
    std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>> output;

    std::vector<std::thread> consumers;
    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
        consumers.emplace_back(std::thread{ Consumer{ producer,exit,output } });
    for (auto& consumer : consumers)
        consumer.join();

    auto steps = output.load();
    if (steps)
        return *steps;
    return std::nullopt;
}

int main()
{
    auto task = ToTask(ReadAll(std::cin));
    auto steps = Solve(task);
    std::cout << ToJson(steps);
    return 0;
}