#include "solver.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <future>
#include <ppl.h>
#include <concurrent_queue.h>

std::string ToString(Direction d)
{
    static const std::array<std::string, 4> strings =
    { "¡ü","¡ú","¡ý","¡û" };
    return strings[(int)d];
}

std::string ToString(int i)
{
    char buf[4];
    snprintf(buf, sizeof buf, "%2d", i);
    return buf;
}

template<size_t rows, size_t cols>
std::string ToString(const Board<rows, cols>& board)
{
    std::string out;
    for (int y = 0;y < rows;++y)
    {
        std::string line = "|";
        for (int x = 0;x < cols;++x)
        {
            int grid = board.board[Position<rows, cols>{ x, y }.index].index;
            if (grid == rows * cols - 1)
                line += "  |";
            else
                line += ToString(grid) + "|";
        }
        out += line + "\n";
    }
    return out;
}

template<typename Board>
void Solver0(const Board& board)
{
    for (int depth = 0;;++depth)
    {
        std::cout << "depth: " << depth << "\n";

        std::vector<Direction> steps_;
        auto steps = Solve(board, steps_, depth);

        if (steps)
        {
            std::cout << "got steps:";
            for (auto step : *steps)
                std::cout << ToString(step) << "\t";
            break;
        }
    }
}

template<size_t rows, size_t cols>
void Solver1(const Board<rows,cols>& board)
{
	auto tasks = GenerateTasks(board, 60);

    for (int depth = 0;; ++depth)
    {
        std::cout << "depth: " << depth << "\n";

        std::mutex mutex;
        std::optional<std::vector<Direction>> steps;

        Concurrency::parallel_for_each(tasks.begin(), tasks.end(),
            [depth, &mutex, &steps](auto task)
        {
            {
				std::scoped_lock lock{ mutex };
				if (steps)
					return;
            }
            auto thisSteps = Solve(task.board, task.steps, depth);
            if (!thisSteps)
                return;
            std::scoped_lock lock{ mutex };
            if (!steps)
                steps = thisSteps;
        });

		if (steps)
		{
			std::cout << "got steps:";
			for (auto step : *steps)
				std::cout << ToString(step) << "\t";
			return;
		}
    }
}

template<size_t rows, size_t cols>
void Solver2(const Board<rows,cols>& board)
{
    class Producer
    {
    public:
        Producer(const Board<rows,cols>& board)
        {
            auto taskList = GenerateTasks(board, std::thread::hardware_concurrency() * 8);
            tasks = std::vector<Task<rows, cols>>(taskList.begin(), taskList.end());
        }

        auto operator()()
        {
			auto curIndex = ++index;

			auto depth = curIndex / tasks.size();
			auto index = curIndex % tasks.size();

			return std::make_tuple(tasks[index].board, tasks[index].steps, (int)depth);
        }

    private:
        std::vector<Task<rows, cols>> tasks;
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
				auto steps = Solve(std::get<0>(task), std::get<1>(task), std::get<2>(task));
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

    class Solver: public std::enable_shared_from_this<Solver>
    {
    public:
        Solver(const Board<rows, cols>& board)
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

    auto solver = std::make_shared<Solver>(board);
    auto steps = (*solver)();
	if (steps)
	{
		std::cout << "got steps:";
		for (auto step : *steps)
			std::cout << ToString(step) << "\t";
	}
}

std::vector<Board<5,5>> boards =
{
    MakeBoard<5,5>
    ({
        0,  1,  2,  3,  4,
        5,  6,  7,  8,  9,
        10, 11, 12, 13, 14,
        15, 16, 17, 18, 19,
        20, 21, 22, 23, 24
    }),
    MakeBoard<5,5>
    ({
        0,  1,  2,  3,  4,
        5,  6,  7,  8,  9,
        10, 11, 12, 24, 14,
        15, 16, 17, 18, 19,
        20, 21, 22, 23, 13 
    }),
    MakeBoard<5,5>
    ({
        0,  1,  2,  3,  4,
        5,  11,  7,  8,  9,
        10, 6, 12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13 
    }),
    MakeBoard<5,5>
    ({
        0,  6,  2,  3,  4,
        5,  1,  7,  8,  9,
        10, 11, 12, 24, 14,
        15, 21, 17, 18, 19,
        20, 16, 22, 23, 13 
    }),
    MakeBoard<5,5>
    ({
        0,  24,  2,  9,  4,
        5,  6,  7,  3,  8,
        10, 11, 12, 18, 14,
        15, 21, 22, 13, 19,
        20, 16, 23, 17, 1
    })
};

int main()
{
    auto board = boards[4];
    std::cout << ToString(board) << std::endl;

    if (!Solvable(board))
    {
        std::cout << "not solable";
        return 0;
    }
    std::cout << "sovable" << std::endl;

    Solver2(board);
    return 0;
}