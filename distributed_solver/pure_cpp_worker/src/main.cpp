#include "handler.h"
#include "solver.hpp"
#include "adapter.h"

namespace Aliyun {
namespace FC {
namespace Handlers {
class PuzzleHandler : public FcHttpBaseHandler
{
public:
   void OnInvoke(const FcContext& context, const Pistache::Http::Request& req,
            Pistache::Http::ResponseWriter& response) override
            {
                auto task = ToTask(req.body());
                auto board = n_puzzle_solver::impl::Solver<5, 5>::MakeBoard(task.board);
                auto tempSteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
                auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);
                auto result = steps? ToJson(Map(*steps, [](const auto& dir) {return (int)dir; })) : "null";
                response.send(Pistache::Http::Code::Ok, result);
            }
   void OnInitialize(const FcContext& context) override{}
};
}}}

#include "entrypoint.h"
using namespace Aliyun::FC::Handlers;
void SetInvokeAndInitHander()
{
    CustomRuntimeHandler::httpHandler = new PuzzleHandler();
}