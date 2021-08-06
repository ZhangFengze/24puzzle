#pragma once
#include "solver.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

template<typename T, typename Func>
auto Map(const std::vector<T>& in, Func func)
{
    using ResultType = std::vector<std::invoke_result_t<Func, T>>;
    ResultType out;
    std::transform(std::begin(in), std::end(in), std::back_inserter(out), func);
    return out;
}

template<typename T, typename Func, size_t size>
auto Map(const std::array<T, size>& in, Func func)
{
    using ResultType = std::array<std::invoke_result_t<Func, T>, size>;
    ResultType out;
    std::transform(std::begin(in), std::end(in), std::begin(out), func);
    return out;
}

template<typename T, typename Func>
auto Map(const std::list<T>& in, Func func)
{
    using ResultType = std::list<std::invoke_result_t<Func, T>>;
    ResultType out;
    std::transform(std::begin(in), std::end(in), std::back_inserter(out), func);
    return out;
}

struct Task
{
    puzzle::Solver<5, 5>::Board board;
    std::vector<puzzle::Direction> steps;
    int maxSteps = 0;
};

struct PlaneTask
{
    std::array<int, 25> board;
    std::vector<int> steps;
    int maxSteps = 0;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlaneTask, board, steps, maxSteps)

inline PlaneTask ToPlaneTask(const Task& task)
{
    PlaneTask planeTask;
    planeTask.board = Map(task.board.board, [](const auto& position) {return (int)position.index; });
    planeTask.steps = Map(task.steps, [](const auto& dir) {return (int)dir; });
    planeTask.maxSteps = task.maxSteps;
    return planeTask;
}

inline Task ToTask(const PlaneTask& planeTask)
{
    Task task;
    task.board = puzzle::Solver<5, 5>::MakeBoard(planeTask.board);
    task.steps = Map(planeTask.steps, [](int dir) {return puzzle::Direction(dir); });
    task.maxSteps = planeTask.maxSteps;
    return task;
}

inline Task ToTask(const std::string& str)
{
    auto planeTask = json::parse(str).get<PlaneTask>();
    return ToTask(planeTask);
}

inline std::string ToJson(const Task& task)
{
    auto planeTask = ToPlaneTask(task);
    return json(planeTask).dump();
}

inline std::string ToJson(const std::vector<puzzle::Direction>& v)
{
    return json(v).dump();
}

inline std::string ToJson(const std::optional<std::vector<puzzle::Direction>>& option)
{
    return option ? ToJson(*option) : "null";
}

inline std::string ReadAll(std::istream& in)
{
    std::string str;
    std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(str));
    return str;
}

template<typename T>
std::vector<T> ToVector(const std::list<T>& list)
{
    return std::vector<T>{ list.begin(), list.end() };
}