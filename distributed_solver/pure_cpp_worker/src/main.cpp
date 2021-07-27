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
                    auto board = puzzle::impl::Solver<5, 5>::MakeBoard(task.board);
                    auto tempSteps = Map(task.steps, [](int dir) {return puzzle::Direction(dir); });
                    auto steps = puzzle::impl::Solver<5, 5>::Solve(board, tempSteps, task.depth);
                    response.send(Pistache::Http::Code::Ok, ToJson(steps));
                }
                void OnInitialize(const FcContext& context) override {}
            };
        }    
}
}

#include "entrypoint.h"
using namespace Aliyun::FC::Handlers;
void SetInvokeAndInitHander()
{
    CustomRuntimeHandler::httpHandler = new PuzzleHandler();
}