#include "echo_handler.h"
#include "solver.h"
#include "adapter.h"
using namespace std;
using namespace Pistache;

namespace Aliyun {
namespace FC {
namespace Handlers {
std::string EchoHandler::mInitHandler;
void EchoHandler::OnInvoke(const string& payload, const FcContext& context, string& response)
{
    response = EchoHandler::mInitHandler + payload;
}

void EchoHandler::OnInitialize(const FcContext& context)
{
    EchoHandler::mInitHandler = context.initializer;
}

std::string EchoHttpHandler::mInitHandler;
void EchoHttpHandler::OnInvoke(const FcContext& context, const Pistache::Http::Request& req,
            Pistache::Http::ResponseWriter& response)
{
    auto task = ToTask(req.body());
    auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
    auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);
    auto result = steps? ToJson(Map(*steps, [](const auto& dir) {return (int)dir; })) : "null";
    response.send(Http::Code::Ok, EchoHttpHandler::mInitHandler + result);
} 

void EchoHttpHandler::OnInitialize(const FcContext& context)
{
    EchoHttpHandler::mInitHandler = context.initializer;
}
}}}
