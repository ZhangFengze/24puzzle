#include <array>
#include <list>
#include <vector>
#include <numeric>
#include <optional>
#include <cassert>
#include <ranges>
#include <iostream>
#include <algorithm>
#include <execution>

enum class Direction
{
	Up, Right, Down, Left
};
constexpr std::array<Direction, 4> Directions =
{ Direction::Up,Direction::Right,Direction::Down,Direction::Left };

constexpr Direction Opposite(Direction direction)
{
    int i = int(direction);
    return Direction((i + 2) % 4);
    //static constexpr std::array<Direction, 4> opposites =
    //{ Direction::Down,Direction::Left,Direction::Up,Direction::Right };
    //return opposites[(int)direction];
}

template<size_t rows, size_t cols>
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

template<typename Position>
constexpr std::array<Position, 4> offsets =
{ Position{0,-1},Position{1,0},Position{0,1},Position{-1,0} };

template<typename Position>
constexpr Position Move(const Position& position, Direction direction)
{
	return position + offsets<Position>[(int)direction];
}

template<size_t rows, size_t cols>
struct Board
{
    std::array<Position<rows,cols>, rows* cols> board;
    Position<rows,cols> emptyPosition;
    constexpr bool operator==(const Board&) const = default;
};

template<size_t rows, size_t cols>
Board<rows, cols> MakeBoard(const std::array<int, rows* cols>& raw)
{
    Board<rows, cols> board;
    for (int i = 0; i < raw.size(); ++i)
        board.board[i] = raw[i];
    board.emptyPosition = (int)std::distance(raw.begin(), std::find(raw.begin(), raw.end(), rows * cols - 1));
    return board;
}

template<size_t rows, size_t cols>
Board<rows, cols> FinishedBoard()
{
	std::array<int, rows* cols> board;
	std::iota(board.begin(), board.end(), 0);
	return MakeBoard(board);
}

template<size_t rows, size_t cols>
constexpr bool Finished(const Board<rows, cols>& board)
{
    static constexpr auto finished = FinishedBoard<rows,cols>();
    return board.board == finished;
}

// http://blog.csdn.net/hnust_xiehonghao/article/details/7951173
template<size_t rows, size_t cols>
bool Solvable(const Board<rows, cols>& board)
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

    if constexpr(cols % 2 == 1)
        return reverseOrder % 2 == 0;

    return ((rows - 1) - board.emptyPosition.y) % 2 == reverseOrder % 2;
}

template<size_t rows,size_t cols>
constexpr std::array<Position<rows, cols>, rows* cols> PossiblePositions = []() constexpr
{
    std::array<Position<rows, cols>, rows* cols> positions;
	for (int index = 0; index < rows * cols;++index)
        positions[index] = Position<rows, cols>(index);
    return positions;
}();

template<size_t rows,size_t cols>
int ManhattanDistance(const Board<rows, cols>& board)
{
    return std::accumulate(PossiblePositions<rows, cols>.begin(), PossiblePositions<rows, cols>.end(), 0,
        [&](int now, auto nowPosition)
    {
        if (nowPosition == board.emptyPosition)
            return now;
        const auto& targetPosition = board.board[nowPosition.index];
        return now + abs(targetPosition.x - nowPosition.x) + abs(targetPosition.y - nowPosition.y);
    });
}

template<size_t rows, size_t cols>
bool ValidPosition(const Position<rows, cols>& position)
{
    return position.x >= 0 && position.x < cols&& position.y >= 0 && position.y < rows;
}

template<bool check, size_t rows, size_t cols>
Board<rows, cols> Moved(const Board<rows, cols>& board, Direction direction)
{
    auto copy = board;
    Move<check>(copy, direction);
    return copy;
}

template<bool check, size_t rows, size_t cols>
void Move(Board<rows, cols>& board, Direction direction)
{
    auto now = board.emptyPosition;
    auto target = Move(now, direction);
    if constexpr (check)
    {
		if (!ValidPosition<rows, cols>(target))
			return;
    }
    std::swap(board.board[now.index], board.board[target.index]);
    board.emptyPosition = target;
}

template<size_t rows, size_t cols>
std::optional<std::vector<Direction>>
Solve(Board<rows, cols> board, std::vector<Direction>& steps, int maxDepth)
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
		if (!ValidPosition<rows, cols>(target))
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
