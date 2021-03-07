#pragma once
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <string>
#include <vector>
#include <array>
#include <algorithm>

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

struct Task
{
    std::array<int, 25> board;
    std::vector<int> steps;
    int depth = 0;
};

inline Task ToTask(const rapidjson::Value& json)
{
    Task t;
    FromJson(t.board, json["board"]);
    FromJson(t.steps, json["steps"]);
    t.depth = json["depth"].GetInt();
    return t;
}

template<typename Allocator>
rapidjson::Value ToJson(const Task& task, Allocator& alloc)
{
    rapidjson::Value v;
    v.SetObject();
    v.AddMember("board", ToJson(task.board, alloc), alloc);
    v.AddMember("steps", ToJson(task.steps, alloc), alloc);
    v.AddMember("depth", task.depth, alloc);
    return v;
}
