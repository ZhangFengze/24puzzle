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
                    for (int depth = task.depth;;++depth)
                    {
                        auto tempSteps = task.steps;
                        auto steps = puzzle::Solver<5, 5>::Solve(task.board, tempSteps, depth);
                        if (steps)
                        {
                            auto result = ToJson(*steps);
                            response.send(Pistache::Http::Code::Ok, result);
                            return;
                        }
                    }
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