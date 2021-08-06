#pragma once
#include "solver.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <optional>

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

inline void to_json(json& j, const Task& task)
{
    auto board = Map(task.board.board, [](const auto& position) {return (int)position.index; });
    j = json{ {"board", board}, {"steps", task.steps}, {"maxSteps", task.maxSteps} };
}

inline void from_json(const json& j, Task& task)
{
    auto board = j.at("board").get<std::array<int, 25>>();
    task.board = puzzle::Solver<5, 5>::MakeBoard(board);
    j.at("steps").get_to(task.steps);
    j.at("maxSteps").get_to(task.maxSteps);
}

namespace nlohmann
{
    template<>
    struct adl_serializer<puzzle::Solver<5, 5>::Task>
    {
        static void to_json(json& j, const puzzle::Solver<5, 5>::Task& task)
        {
            auto board = Map(task.board.board, [](const auto& position) {return (int)position.index; });
            j = json{ {"board", board}, {"steps", task.steps} };
        }

        static void from_json(const json& j, puzzle::Solver<5, 5>::Task& task)
        {
            auto board = j.at("board").get<std::array<int, 25>>();
            task.board = puzzle::Solver<5, 5>::MakeBoard(board);
            j.at("steps").get_to(task.steps);
        }
    };

    template <typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void to_json(json& j, const std::optional<T>& opt)
        {
            if (opt)
                j = *opt;
            else
                j = nullptr;
        }

        static void from_json(const json& j, std::optional<T>& opt)
        {
            opt = j.is_null() ? std::nullopt : j.get<T>();
        }
    };
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