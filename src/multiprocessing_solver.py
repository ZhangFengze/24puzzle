import os
import sys
import json
import itertools
import concurrent.futures
import solver


def GenerateTasks(task, preferredCount):
    tasks = json.loads(solver.GenerateTasks(json.dumps(task), preferredCount))
    tasks = itertools.product(range(task["maxSteps"]+1), tasks)
    for maxSteps, task in tasks:
        yield {"board": task["board"], "steps": task["steps"], "maxSteps": maxSteps}


def SolveOne(task):
    return json.loads(solver.Solve(json.dumps(task)))


def Solve(task):
    concurrency = os.cpu_count()
    with concurrent.futures.ProcessPoolExecutor() as executor:
        tasks = (executor.submit(SolveOne, task)
                 for task in GenerateTasks(task, concurrency*8))
        futures = concurrent.futures.as_completed(tasks)
        for future in futures:
            result = future.result()
            if result != None:
                executor.shutdown(cancel_futures=True)
                return result


if __name__ == "__main__":
    task = json.loads(sys.stdin.read())
    result = Solve(task)
    print(json.dumps(result))
