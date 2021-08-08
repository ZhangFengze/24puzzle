#include "solver.hpp"
#include "adapter.hpp"
#include "blockingconcurrentqueue.h"
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
        auto curIndex = index_++;
        if (curIndex >= maxIndex_)
            return std::nullopt;

        auto maxSteps = curIndex / tasks_.size();
        auto index = curIndex % tasks_.size();

        return Task{ tasks_[index].board, tasks_[index].steps, (int)maxSteps };
    }

    size_t TotalTasks() const { return maxIndex_; }

public:
    std::vector<puzzle::Solver<5, 5>::Task> tasks_;
    std::atomic_size_t index_ = 0;
    size_t maxIndex_ = 0;
};

using Result = std::optional<std::vector<puzzle::Direction>>;

template<typename T>
using Queue = moodycamel::BlockingConcurrentQueue<T>;

class Consumer
{
public:
    Consumer(Producer& producer, Queue<Result>& output)
        :producer_(producer), output_(output) {}

    void operator()()
    {
        for (;auto task = producer_();)
        {
            auto steps = puzzle::Solver<5, 5>::Solve(task->board, task->steps, task->maxSteps);
            output_.enqueue(steps);
        }
    }

private:
    Producer& producer_;
    Queue<Result>& output_;
};

Result Solve(const Task& task)
{
    int concurrency = std::thread::hardware_concurrency();
    Producer producer{ task, (int)concurrency * 8 };
    Queue<Result> results;

    for (unsigned int i = 0; i < concurrency; ++i)
    {
        std::thread consumer{ Consumer{ producer,results} };
        consumer.detach();
    }

    Result result;
    for (size_t i = 0;i < producer.TotalTasks();++i)
    {
        results.wait_dequeue(result);
        if (result)
            return result;
    }
    return std::nullopt;
}

int main()
{
    auto task = json::parse(ReadAll(std::cin)).get<Task>();
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ? Solve(task) : std::nullopt;
    std::cout << json(steps);
    return 0;
}