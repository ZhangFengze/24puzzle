#pragma once
#include "solver.hpp"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

namespace impl
{
    inline std::string ToString(const rapidjson::Value& doc)
    {
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        doc.Accept(writer);
        return sb.GetString();
    }

    template<typename Container, typename Allocator>
    rapidjson::Value ToJson(const Container& c, Allocator& alloc)
    {
        rapidjson::Value v;
        v.SetArray();
        for (auto i : c)
            v.PushBack(i, alloc);
        return v;
    }

    template<typename T>
    void FromJson(std::vector<T>& to, const rapidjson::Value& from)
    {
        for (const auto& v : from.GetArray())
            to.push_back(v.Get<T>());
    }

    template<typename T, size_t size>
    void FromJson(std::array<T, size>& to, const rapidjson::Value& from)
    {
        assert(from.GetArray().Size() == size);
        for (size_t i = 0;i < size;++i)
            to[i] = from.GetArray()[i].Get<T>();
    }
}

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
    int depth = 0;
};

struct PlaneTask
{
    std::array<int, 25> board;
    std::vector<int> steps;
    int depth = 0;
};

inline PlaneTask ToPlaneTask(const Task& task)
{
    PlaneTask planeTask;
    planeTask.board = Map(task.board.board, [](const auto& position) {return (int)position.index; });
    planeTask.steps = Map(task.steps, [](const auto& dir) {return (int)dir; });
    planeTask.depth = task.depth;
    return planeTask;
}

inline Task ToTask(const PlaneTask& planeTask)
{
    Task task;
    task.board = puzzle::Solver<5, 5>::MakeBoard(planeTask.board);
    task.steps = Map(planeTask.steps, [](int dir) {return puzzle::Direction(dir); });
    task.depth = planeTask.depth;
    return task;
}

inline PlaneTask ToPlaneTask(const std::string& json)
{
    using namespace impl;

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json.c_str());

    PlaneTask t;
    FromJson(t.board, doc["board"]);
    FromJson(t.steps, doc["steps"]);
    t.depth = doc["depth"].GetInt();
    return t;
}

inline Task ToTask(const std::string& json)
{
    return ToTask(ToPlaneTask(json));
}

inline std::string ToJson(const PlaneTask& task)
{
    using namespace impl;

    rapidjson::Document v;
    auto& alloc = v.GetAllocator();

    v.SetObject();
    v.AddMember("board", ToJson(task.board, alloc), alloc);
    v.AddMember("steps", ToJson(task.steps, alloc), alloc);
    v.AddMember("depth", task.depth, alloc);

    return ToString(v);
}

inline std::string ToJson(const Task& task)
{
    return ToJson(ToPlaneTask(task));
}

template<typename Container>
std::string ToJson(const Container& c)
{
    rapidjson::Document doc;
    doc.SetArray();
    for (auto i : c)
        doc.PushBack(i, doc.GetAllocator());
    return impl::ToString(doc);
}

inline std::string ToJson(const std::vector<puzzle::Direction>& v)
{
    return ToJson(Map(v, [](const auto& dir) {return (int)dir; }));
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