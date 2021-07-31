#define CPPHTTPLIB_THREAD_POOL_COUNT 1
#include "httplib.h"
#include "solver.hpp"
#include "adapter.hpp"

int main()
{
    httplib::Server server;
    server.Post("/", [](const httplib::Request &req, httplib::Response &res) {
        auto task = ToTask(req.body);
        for (int depth = task.depth;;++depth)
        {
            auto tempSteps = task.steps;
            auto steps = puzzle::Solver<5, 5>::Solve(task.board, tempSteps, depth);
            if (steps)
            {
                auto result = ToJson(*steps);
                res.set_content(result, "application/json");
                return;
            }
        }
    });
    server.listen("localhost", 8080);
    return 0;
}