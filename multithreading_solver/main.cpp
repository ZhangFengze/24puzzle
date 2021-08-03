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
    Producer(const Task& task, int preferredCount)
    {
        auto taskList = puzzle::Solver<5, 5>::GenerateTasks(task.board, task.steps, preferredCount);
        tasks_ = ToVector(taskList);
        maxIndex_ = (task.maxSteps + 1) * tasks_.size();
    }

    std::optional<Task> operator()()
    {
        auto curIndex = ++index_;
        if (curIndex >= maxIndex_)
            return std::nullopt;

        auto maxSteps = curIndex / tasks_.size();
        auto index = curIndex % tasks_.size();

        return Task{ tasks_[index].board, tasks_[index].steps, (int)maxSteps };
    }

private:
    std::vector<puzzle::Solver<5, 5>::Task> tasks_;
    std::atomic_size_t index_ = 0;
    size_t maxIndex_ = 0;
};

class Consumer
{
public:
    Consumer(Producer& producer, std::atomic_flag& exit,
        std::atomic<std::shared_ptr<std::vector<puzzle::Direction>>>& output)
        :producer_(producer), exit_(exit), output_(output) {}

    void operator()()
    {
        while (!exit_.test())
        {
            auto task = producer_();
            if (!task)
                return;
            auto steps = puzzle::Solver<5, 5>::Solve(task->board, task->steps, task->maxSteps);
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
    Producer producer{ task, (int)std::thread::hardware_concurrency() * 8 };
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
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ? Solve(task) : std::nullopt;
    std::cout << ToJson(steps);
    return 0;
}