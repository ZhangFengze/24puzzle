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
    std::array<int, 25> board;
    std::vector<int> steps;
    int depth = 0;
};

inline Task ToTask(const std::string& json)
{
    using namespace impl;

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json.c_str());

    Task t;
    FromJson(t.board, doc["board"]);
    FromJson(t.steps, doc["steps"]);
    t.depth = doc["depth"].GetInt();
    return t;
}

inline std::string ToJson(const Task& task)
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

template<typename Container>
std::string ToJson(const Container& c)
{
	rapidjson::Document doc;
	doc.SetArray();
    for (auto i : c)
        doc.PushBack(i, doc.GetAllocator());
    return impl::ToString(doc);
}

std::string ToJson(const std::vector<n_puzzle_solver::Direction>& v)
{
    return ToJson(Map(v, [](const auto& dir) {return (int)dir; }));
}

std::string ToJson(const std::optional<std::vector<n_puzzle_solver::Direction>>& option)
{
    return option ? ToJson(*option) : "null";
}

inline std::string ReadAll(std::istream& in)
{
    std::string str;
    std::copy(std::istream_iterator<char>(in), std::istream_iterator<char>(), std::back_inserter(str));
    return str;
}