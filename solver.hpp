#pragma once
#include <optional>
#include <vector>
#include <array>

namespace puzzle
{
    enum class Direction
    {
        Up, Right, Down, Left
    };
    template<size_t rows, size_t cols>
    std::optional<std::vector<Direction>> Solve(const std::array<int, rows* cols>&);
}

// internals

#include <list>
#include <vector>
#include <numeric>
#include <optional>
#include <cassert>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <memory>

namespace puzzle
{
    namespace impl
    {
        constexpr std::array<Direction, 4> Directions =
        { Direction::Up,Direction::Right,Direction::Down,Direction::Left };

        constexpr Direction Opposite(Direction direction)
        {
            int i = int(direction);
            return Direction((i + 2) % 4);
        }

        template<size_t rows, size_t cols>
        struct Solver
        {
            struct Position
            {
                int8_t x;
                int8_t y;
                int16_t index;

                constexpr Position() = default;
                constexpr Position(int x_, int y_) :x(x_), y(y_), index(x + y * cols) {}
                constexpr Position(int index_) : Position(index_% cols, index_ / cols) {}

                constexpr bool operator==(const Position& other) const { return index == other.index; }
                constexpr Position operator+(const Position& other) const { return { x + other.x,y + other.y }; }
            };

            static constexpr std::array<Position, 4> offsets =
            { Position{0,-1},Position{1,0},Position{0,1},Position{-1,0} };

            static constexpr Position Move(const Position& position, Direction direction)
            {
                return position + offsets[(int)direction];
            }

            struct Board
            {
                std::array<Position, rows* cols> board;
                Position emptyPosition;
                constexpr bool operator==(const Board&) const = default;
            };

            static constexpr Board MakeBoard(const std::array<int, rows* cols>& raw)
            {
                Board board;
                for (int i = 0; i < raw.size(); ++i)
                    board.board[i] = raw[i];
                board.emptyPosition = (int)std::distance(raw.begin(), std::find(raw.begin(), raw.end(), rows * cols - 1));
                return board;
            }

            static constexpr Board FinishedBoard()
            {
                std::array<int, rows* cols> board;
                std::iota(board.begin(), board.end(), 0);
                return MakeBoard(board);
            }

            static constexpr auto finished = FinishedBoard();
            static constexpr bool Finished(const Board& board)
            {
                return board == finished;
            }

            // http://blog.csdn.net/hnust_xiehonghao/article/details/7951173
            static bool Solvable(const Board& board)
            {
                int reverseOrder = 0;
                for (int i = 0; i < rows * cols; ++i)
                {
                    if (board.board[i].index == rows * cols - 1)
                        continue;
                    for (int j = i + 1; j < rows * cols; ++j)
                    {
                        if (board.board[i].index > board.board[j].index)
                            ++reverseOrder;
                    }
                }

                if constexpr (cols % 2 == 1)
                    return reverseOrder % 2 == 0;

                return ((rows - 1) - board.emptyPosition.y) % 2 == reverseOrder % 2;
            }

            static constexpr std::array<Position, rows* cols> PossiblePositions = []() constexpr
            {
                std::array<Position, rows* cols> positions;
                for (int index = 0; index < rows * cols;++index)
                    positions[index] = Position(index);
                return positions;
            }();

            static int ManhattanDistance(const Board& board)
            {
                return std::accumulate(PossiblePositions.begin(), PossiblePositions.end(), 0,
                    [&](int now, const auto& nowPosition)
                    {
                        if (nowPosition == board.emptyPosition)
                            return now;
                        const auto& targetPosition = board.board[nowPosition.index];
                        return now + abs(targetPosition.x - nowPosition.x) + abs(targetPosition.y - nowPosition.y);
                    });
            }

            static bool ValidPosition(const Position& position)
            {
                return position.x >= 0 && position.x < cols&& position.y >= 0 && position.y < rows;
            }

            template<bool check>
            static Board Moved(const Board& board, Direction direction)
            {
                auto copy = board;
                Move<check>(copy, direction);
                return copy;
            }

            template<bool check>
            static void Move(Board& board, Direction direction)
            {
                const auto& now = board.emptyPosition;
                auto target = Move(now, direction);
                if constexpr (check)
                {
                    if (!ValidPosition(target))
                        return;
                }
                std::swap(board.board[now.index], board.board[target.index]);
                board.emptyPosition = target;
            }

            static std::optional<std::vector<Direction>>
                Solve(Board board, std::vector<Direction>& steps, int maxDepth)
            {
                int h = ManhattanDistance(board);
                if (h == 0)
                    return steps;
                if (steps.size() + h > maxDepth)
                    return std::nullopt;
                for (auto direction : Directions)
                {
                    if (!steps.empty() && direction == Opposite(steps.back()))
                        continue;

                    auto target = Move(board.emptyPosition, direction);
                    if (!ValidPosition(target))
                        continue;

                    steps.push_back(direction);
                    Move<false>(board, direction);

                    auto result = Solve(board, steps, maxDepth);
                    if (result)
                        return result;

                    Move<false>(board, Opposite(direction));
                    steps.pop_back();
                }
                return std::nullopt;
            }

            struct Task
            {
                Board board;
                std::vector<Direction> steps;
            };

            static std::list<Task> GenerateTasks(const Board& board, int preferedTasks)
            {
                std::list<Task> tasks;
                tasks.push_back({ board,{} });

                while (!tasks.empty() && tasks.size() < preferedTasks)
                {
                    auto task = tasks.front();
                    if (Finished(task.board))
                        return { task };
                    tasks.pop_front();

                    for (auto direction : Directions)
                    {
                        if (!task.steps.empty() && direction == Opposite(task.steps.back()))
                            continue;

                        auto target = Move(task.board.emptyPosition, direction);
                        if (!ValidPosition(target))
                            continue;

                        auto steps = task.steps;
                        steps.push_back(direction);
                        tasks.push_back({ Moved<false>(task.board,direction),steps });
                    }
                }
                return tasks;
            }
        };
    }

    template<size_t rows, size_t cols>
    std::optional<std::vector<Direction>> Solve(const std::array<int, rows* cols>& raw)
    {
        using _ = impl::Solver<rows, cols>;
        auto board = _::MakeBoard(raw);
        if (!_::Solvable(board))
            return std::nullopt;
        return _::Solver2(board);
    }
}