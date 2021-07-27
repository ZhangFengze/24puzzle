#include "handler.h"
#include "solver.hpp"
#include "adapter.hpp"

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
                auto historySteps = Map(task.steps, [](int dir) {return n_puzzle_solver::Direction(dir); });
                for(int depth = task.depth;;++depth)
                {
                    auto tempSteps = historySteps;
                    auto steps = n_puzzle_solver::impl::Solver<5, 5>::Solve(board, tempSteps, depth);
                    if(steps)
                    {
                        auto result = ToJson(*steps);
                        response.send(Pistache::Http::Code::Ok, result);
                        return;
                    }
                }
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